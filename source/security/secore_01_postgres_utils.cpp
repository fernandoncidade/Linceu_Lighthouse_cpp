#include "secore_01_postgres_utils.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>

#include "source/utils/LogManager.h"
#include "source/utils/CaminhoPersistenteUtils.h"

namespace source::security::pg {
namespace {

auto& logger() {
    static auto value = source::utils::LogManager::get_logger();
    return value;
}

struct RuntimeState {
    bool prepared = false;
    bool driver_available = false;
    bool sqlite_available = false;
    bool local_cache_active = false;
    qint64 next_retry_after_ms = 0;
    QString last_error;
};

QMutex& runtime_lock() {
    static QMutex mutex;
    return mutex;
}

RuntimeState& runtime_state() {
    static RuntimeState state;
    return state;
}

QString env_value(const char* primary, const char* fallback, const QString& default_value = QString()) {
    QString value = qEnvironmentVariable(primary);
    if (value.isEmpty() && fallback != nullptr) {
        value = qEnvironmentVariable(fallback);
    }
    return value.isEmpty() ? default_value : value;
}

int env_int_value(const char* primary, const char* fallback, int default_value) {
    bool ok = false;
    const int value = env_value(primary, fallback).toInt(&ok);
    return ok ? value : default_value;
}

QString normalized_directory(const QString& path) {
    return path.isEmpty() ? QString() : QDir::cleanPath(QDir(path).absolutePath());
}

QStringList client_library_candidates() {
    QStringList candidates;

    const QString configured_dir =
        env_value("LINCEU_SECURITY_PGLIBDIR", "PGBINDIR", qEnvironmentVariable("POSTGRES_HOME"));
    if (!configured_dir.isEmpty()) {
        const QFileInfo info(configured_dir);
        const QString dir = info.isDir() ? configured_dir : info.absolutePath();
        if (!dir.isEmpty()) {
            candidates.append(normalized_directory(dir));
            if (QFileInfo(dir).fileName().compare(QStringLiteral("bin"), Qt::CaseInsensitive) != 0) {
                candidates.append(normalized_directory(QDir(dir).filePath(QStringLiteral("bin"))));
            }
        }
    }

    const QString app_dir = QCoreApplication::applicationDirPath();
    if (!app_dir.isEmpty()) {
        candidates.append(normalized_directory(app_dir));
        candidates.append(normalized_directory(QDir(app_dir).filePath(QStringLiteral("pgsql"))));
        candidates.append(normalized_directory(QDir(app_dir).filePath(QStringLiteral("postgres"))));
    }

    candidates.removeDuplicates();
    return candidates;
}

void prepend_to_path(const QString& directory) {
    if (directory.isEmpty() || !QDir(directory).exists()) {
        return;
    }

    const QByteArray current_path = qgetenv("PATH");
    const QString current = QString::fromLocal8Bit(current_path);
    const QString normalized = QDir::toNativeSeparators(directory);
    if (current.contains(normalized, Qt::CaseInsensitive)) {
        return;
    }

    const QString updated =
        normalized + QString(QDir::listSeparator()) + current;
    qputenv("PATH", QDir::toNativeSeparators(updated).toLocal8Bit());
}

QString connection_name(const QString& scope) {
    return QStringLiteral("security_pg_%1_%2")
        .arg(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16),
             scope);
}

QString cache_connection_name(const QString& scope) {
    return QStringLiteral("security_cache_%1_%2")
        .arg(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16),
             scope);
}

QString cache_connection_prefix_for_thread() {
    return QStringLiteral("security_cache_%1_")
        .arg(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16));
}

QString cache_connection_prefix_all() {
    return QStringLiteral("security_cache_");
}

QString connect_options(const ConnectionConfig& cfg) {
    QStringList options;
    options << QStringLiteral("connect_timeout=%1").arg(qMax(1, cfg.connect_timeout_s));
    if (!cfg.application_name.trimmed().isEmpty()) {
        options << QStringLiteral("application_name=%1").arg(cfg.application_name.trimmed());
    }
    if (!cfg.sslmode.trimmed().isEmpty()) {
        options << QStringLiteral("sslmode=%1").arg(cfg.sslmode.trimmed());
    }
    options << QStringLiteral("keepalives=1");
    options << QStringLiteral("keepalives_idle=30");
    options << QStringLiteral("keepalives_interval=10");
    options << QStringLiteral("keepalives_count=3");
    return options.join(QLatin1Char(';'));
}

void apply_session_settings(QSqlDatabase& db) {
    if (db.driverName().compare(QStringLiteral("QPSQL"), Qt::CaseInsensitive) != 0) {
        return;
    }
    QSqlQuery query(db);
    query.exec(QStringLiteral("SET statement_timeout = '15000ms'"));
    query.exec(QStringLiteral("SET lock_timeout = '5000ms'"));
    query.exec(QStringLiteral("SET idle_in_transaction_session_timeout = '30000ms'"));
}

void apply_local_cache_settings(QSqlDatabase& db) {
    if (db.driverName().compare(QStringLiteral("QSQLITE"), Qt::CaseInsensitive) != 0) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral("PRAGMA journal_mode=WAL"));
    query.exec(QStringLiteral("PRAGMA synchronous=NORMAL"));
    query.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
    query.exec(QStringLiteral("PRAGMA cache_size=-131072"));
    query.exec(QStringLiteral("PRAGMA mmap_size=1073741824"));
    query.exec(QStringLiteral("PRAGMA wal_autocheckpoint=4000"));
    query.exec(QStringLiteral("PRAGMA journal_size_limit=268435456"));
    query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
    query.exec(QStringLiteral("PRAGMA busy_timeout=5000"));
    query.exec(QStringLiteral("PRAGMA optimize"));
}

void record_failure(const QString& message) {
    QMutexLocker locker(&runtime_lock());
    RuntimeState& state = runtime_state();
    state.last_error = message;
    state.next_retry_after_ms = QDateTime::currentMSecsSinceEpoch() + 10000;
}

bool retry_allowed() {
    QMutexLocker locker(&runtime_lock());
    return QDateTime::currentMSecsSinceEpoch() >= runtime_state().next_retry_after_ms;
}

void mark_backend_state(bool local_cache_active) {
    QMutexLocker locker(&runtime_lock());
    runtime_state().local_cache_active = local_cache_active;
}

}  // namespace

bool ConnectionConfig::is_configured() const {
    return !host.trimmed().isEmpty() && port > 0 && !database.trimmed().isEmpty() &&
           !user.trimmed().isEmpty();
}

const ConnectionConfig& config() {
    static const ConnectionConfig cfg = [] {
        ConnectionConfig value;
        value.host = env_value("LINCEU_SECURITY_PGHOST", "PGHOST", QStringLiteral("127.0.0.1"));
        value.port = env_int_value("LINCEU_SECURITY_PGPORT", "PGPORT", 5432);
        value.database =
            env_value("LINCEU_SECURITY_PGDATABASE", "PGDATABASE", QStringLiteral("linceu_security"));
        value.user = env_value("LINCEU_SECURITY_PGUSER", "PGUSER", QStringLiteral("postgres"));
        value.password = env_value("LINCEU_SECURITY_PGPASSWORD", "PGPASSWORD");
        value.sslmode = env_value("LINCEU_SECURITY_PGSSLMODE", "PGSSLMODE", QStringLiteral("prefer"));
        value.connect_timeout_s =
            env_int_value("LINCEU_SECURITY_PGCONNECT_TIMEOUT", "PGCONNECT_TIMEOUT", 2);
        value.application_name = QStringLiteral("LinceuLighthouseSecurity");

        const QStringList candidates = client_library_candidates();
        for (const QString& candidate : candidates) {
            if (QFileInfo(QDir(candidate).filePath(QStringLiteral("libpq.dll"))).exists() ||
                QFileInfo(QDir(candidate).filePath(QStringLiteral("LIBPQ.dll"))).exists()) {
                value.client_library_dir = candidate;
                break;
            }
        }
        if (value.client_library_dir.isEmpty() && !candidates.isEmpty()) {
            value.client_library_dir = candidates.first();
        }
        return value;
    }();
    return cfg;
}

void prepare_runtime() {
    QMutexLocker locker(&runtime_lock());
    RuntimeState& state = runtime_state();
    if (state.prepared) {
        return;
    }
    state.prepared = true;
    locker.unlock();

    const QStringList drivers = QSqlDatabase::drivers();
    const bool sqlite_available = drivers.contains(QStringLiteral("QSQLITE"));

    locker.relock();
    state.driver_available = sqlite_available;
    state.sqlite_available = sqlite_available;
    state.local_cache_active = sqlite_available;
    if (!sqlite_available) {
        state.last_error = QStringLiteral("Driver QSQLITE indisponível para o subsistema de segurança.");
    } else {
        state.last_error.clear();
    }
}

bool is_driver_available() {
    prepare_runtime();
    QMutexLocker locker(&runtime_lock());
    return runtime_state().driver_available;
}

bool is_local_cache_active() {
    QMutexLocker locker(&runtime_lock());
    return runtime_state().local_cache_active;
}

QString last_error() {
    QMutexLocker locker(&runtime_lock());
    return runtime_state().last_error;
}

QString local_cache_path();

QString describe_target() {
    return local_cache_path();
}

QString local_cache_path() {
    return QDir(source::utils::obter_caminho_persistente())
        .filePath(QStringLiteral("security/security.db"));
}

QString effective_backend_name() {
    return QStringLiteral("sqlite");
}

QString effective_target() {
    return local_cache_path();
}

QSqlDatabase open_local_cache_database(const QString& scope) {
    prepare_runtime();

    {
        QMutexLocker locker(&runtime_lock());
        if (!runtime_state().sqlite_available) {
            runtime_state().last_error =
                QStringLiteral("Driver QSQLITE indisponível para o cache local de segurança.");
            return QSqlDatabase();
        }
    }

    const QString cache_path = local_cache_path();
    QDir().mkpath(QFileInfo(cache_path).absolutePath());

    const QString conn_name = cache_connection_name(scope);
    QSqlDatabase db = QSqlDatabase::contains(conn_name)
                          ? QSqlDatabase::database(conn_name, false)
                          : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
    if (!db.isValid()) {
        record_failure(QStringLiteral("Falha ao inicializar o cache local de segurança."));
        return QSqlDatabase();
    }

    db.setDatabaseName(cache_path);
    if (!db.isOpen() && !db.open()) {
        record_failure(QStringLiteral("Falha ao abrir o cache local de segurança: %1")
                           .arg(db.lastError().text()));
        return QSqlDatabase();
    }

    apply_local_cache_settings(db);
    {
        QMutexLocker locker(&runtime_lock());
        runtime_state().local_cache_active = true;
        runtime_state().last_error.clear();
        runtime_state().next_retry_after_ms = 0;
    }
    return db;
}

QSqlDatabase open_database(const QString& scope) {
    prepare_runtime();
    return open_local_cache_database(scope);
}

void close_thread_local_connections() {
    const QString prefix = cache_connection_prefix_for_thread();
    const QStringList names = QSqlDatabase::connectionNames();
    for (const QString& name : names) {
        if (!name.startsWith(prefix)) {
            continue;
        }
        {
            QSqlDatabase db = QSqlDatabase::database(name, false);
            if (db.isValid()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(name);
    }
}

void close_all_local_cache_connections() {
    const QString prefix = cache_connection_prefix_all();
    const QStringList names = QSqlDatabase::connectionNames();
    for (const QString& name : names) {
        if (!name.startsWith(prefix)) {
            continue;
        }
        {
            QSqlDatabase db = QSqlDatabase::database(name, false);
            if (db.isValid()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(name);
    }
}

}  // namespace source::security::pg
