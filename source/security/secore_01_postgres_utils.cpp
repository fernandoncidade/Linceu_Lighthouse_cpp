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

}  // namespace

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
                QStringLiteral("Driver QSQLITE indisponível para o armazenamento local de segurança.");
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
        record_failure(QStringLiteral("Falha ao inicializar o banco local de segurança."));
        return QSqlDatabase();
    }

    db.setDatabaseName(cache_path);
    if (!db.isOpen() && !db.open()) {
        record_failure(QStringLiteral("Falha ao abrir o banco local de segurança: %1")
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
