#include "base_01_baseline_criptografico.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QSqlQuery>
#include <QWaitCondition>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"

namespace source::security {
namespace {

constexpr int kMaxBaselineCacheEntries = 32768;

struct BaselineProfileIndex {
    bool loaded = false;
    bool loading = false;
    QHash<QString, QVariantMap> rows;
};

QString event_path(const QVariantMap& evento) {
    const QString atual = evento.value(QStringLiteral("dir_atual")).toString();
    if (!atual.isEmpty() && QFileInfo::exists(atual)) {
        return atual;
    }
    return evento.value(QStringLiteral("dir_anterior")).toString();
}

constexpr qint64 kMaxSyncHashFileSize = 50 * 1024 * 1024; // 50 MB

QString compute_sha256(const QString& path) {
    const QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        return QString();
    }
    if (info.size() > kMaxSyncHashFileSize) {
        return QStringLiteral("HASH_PENDING_LARGE_FILE");
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 1024));
    }
    file.close();
    return QString::fromLatin1(hash.result().toHex());
}

bool is_textual_extension(const QString& path) {
    static const QSet<QString> kTextExts = {
        QStringLiteral("txt"),  QStringLiteral("md"),   QStringLiteral("json"), QStringLiteral("xml"),
        QStringLiteral("yaml"), QStringLiteral("yml"),  QStringLiteral("ini"),  QStringLiteral("cfg"),
        QStringLiteral("cpp"),  QStringLiteral("c"),    QStringLiteral("h"),    QStringLiteral("hpp"),
        QStringLiteral("py"),   QStringLiteral("js"),   QStringLiteral("ts"),   QStringLiteral("tsx"),
        QStringLiteral("java"), QStringLiteral("cs"),   QStringLiteral("sql"),  QStringLiteral("ps1"),
        QStringLiteral("bat"),  QStringLiteral("cmd"),  QStringLiteral("vbs"),  QStringLiteral("html"),
        QStringLiteral("css"),  QStringLiteral("qml"),
    };
    return kTextExts.contains(QFileInfo(path).suffix().toLower());
}

QString snapshot_reference(const QString& profile_id, const QString& path) {
    const QByteArray digest = QCryptographicHash::hash(
        (profile_id + QStringLiteral("::") + path).toUtf8(), QCryptographicHash::Sha1).toHex();
    return QStringLiteral("sqlite://security_baseline/%1").arg(QString::fromLatin1(digest));
}

QString baseline_cache_key(const QString& profile_id, const QString& path) {
    return profile_id + QStringLiteral("::") + QDir::cleanPath(path);
}

QMutex& baseline_cache_lock() {
    static QMutex mutex;
    return mutex;
}

QWaitCondition& baseline_cache_ready() {
    static QWaitCondition condition;
    return condition;
}

QHash<QString, BaselineProfileIndex>& profile_baseline_cache() {
    static QHash<QString, BaselineProfileIndex> cache;
    return cache;
}

QHash<QString, QVariantMap>& file_analysis_cache() {
    static QHash<QString, QVariantMap> cache;
    return cache;
}

QHash<QString, QString>& snapshot_text_cache() {
    static QHash<QString, QString> cache;
    return cache;
}

bool ensure_profile_baseline_loaded(const QString& db_path, const QString& profile_id) {
    if (profile_id.isEmpty()) {
        return false;
    }

    {
        QMutexLocker locker(&baseline_cache_lock());
        BaselineProfileIndex& profile_index = profile_baseline_cache()[profile_id];
        if (profile_index.loaded) {
            return true;
        }
        while (profile_index.loading) {
            baseline_cache_ready().wait(&baseline_cache_lock());
            if (profile_index.loaded) {
                return true;
            }
        }
        profile_index.loading = true;
    }

    QHash<QString, QVariantMap> loaded_rows;
    bool loaded_ok = false;
    QSqlDatabase db = source::security::pg::open_database(
        QStringLiteral("baseline_profile_load_%1").arg(profile_id));
    if (db.isValid() && db.isOpen()) {
        QSqlQuery query(db);
        query.prepare(QStringLiteral(
            "SELECT caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state "
            "FROM security_baseline WHERE profile_id = ?"));
        query.addBindValue(profile_id);
        if (query.exec()) {
            loaded_ok = true;
            while (query.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("sha256"), query.value(1));
                row.insert(QStringLiteral("size_b"), query.value(2));
                row.insert(QStringLiteral("mtime_utc"), query.value(3));
                row.insert(QStringLiteral("snapshot_path"), query.value(4));
                row.insert(QStringLiteral("publisher"), query.value(5));
                row.insert(QStringLiteral("signature_state"), query.value(6));
                loaded_rows.insert(QDir::cleanPath(query.value(0).toString()), row);
            }
        }
    }

    QMutexLocker locker(&baseline_cache_lock());
    BaselineProfileIndex& profile_index = profile_baseline_cache()[profile_id];
    if (loaded_ok) {
        profile_index.rows = loaded_rows;
        profile_index.loaded = true;
    }
    profile_index.loading = false;
    baseline_cache_ready().wakeAll();
    return loaded_ok;
}

bool lookup_cached_baseline_row(const QString& profile_id, const QString& path, QVariantMap* row) {
    QMutexLocker locker(&baseline_cache_lock());
    const auto profile_it = profile_baseline_cache().constFind(profile_id);
    if (profile_it == profile_baseline_cache().constEnd()) {
        return false;
    }
    const QString key = QDir::cleanPath(path);
    if (!profile_it->rows.contains(key)) {
        return false;
    }

    if (row != nullptr) {
        *row = profile_it->rows.value(key);
    }
    return true;
}

void cache_baseline_row(const QString& profile_id, const QString& path, const QVariantMap& row) {
    QMutexLocker locker(&baseline_cache_lock());
    BaselineProfileIndex& profile_index = profile_baseline_cache()[profile_id];
    profile_index.loaded = true;
    profile_index.loading = false;
    if (profile_index.rows.size() >= kMaxBaselineCacheEntries) {
        profile_index.rows.clear();
    }
    profile_index.rows.insert(QDir::cleanPath(path), row);
}

QString current_file_cache_key(const QFileInfo& info) {
    return QDir::cleanPath(info.absoluteFilePath()) +
           QStringLiteral("::%1::%2")
               .arg(info.size())
               .arg(info.lastModified().toUTC().toMSecsSinceEpoch());
}

bool lookup_cached_file_analysis(const QFileInfo& info, QVariantMap* row) {
    const QString key = current_file_cache_key(info);
    QMutexLocker locker(&baseline_cache_lock());
    if (!file_analysis_cache().contains(key)) {
        return false;
    }

    if (row != nullptr) {
        *row = file_analysis_cache().value(key);
    }
    return true;
}

void cache_file_analysis(const QFileInfo& info, const QVariantMap& row) {
    const QString key = current_file_cache_key(info);
    QMutexLocker locker(&baseline_cache_lock());
    if (file_analysis_cache().size() >= kMaxBaselineCacheEntries) {
        file_analysis_cache().clear();
    }
    file_analysis_cache().insert(key, row);
}

QString capture_text_snapshot(const QString& path) {
    const QFileInfo info(path);
    if (!info.exists() || !info.isFile() || info.size() > 256 * 1024 || !is_textual_extension(path)) {
        return QString();
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    const QByteArray content = file.readAll();
    file.close();
    if (content.contains('\0')) {
        return QString();
    }

    return QString::fromUtf8(content);
}

QString load_snapshot_text(const QString& db_path, const QString& snapshot_path) {
    Q_UNUSED(db_path)

    if (snapshot_path.isEmpty()) {
        return QString();
    }

    {
        QMutexLocker locker(&baseline_cache_lock());
        if (snapshot_text_cache().contains(snapshot_path)) {
            return snapshot_text_cache().value(snapshot_path);
        }
    }

    QString snapshot_text;
    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("baseline_snapshot_read"));
    if (!db.isValid() || !db.isOpen()) {
        return snapshot_text;
    }

    {
        QSqlQuery query(db);
        query.prepare(QStringLiteral(
            "SELECT snapshot_text FROM security_baseline_snapshots WHERE snapshot_path = ? LIMIT 1"));
        query.addBindValue(snapshot_path);
        if (query.exec() && query.next()) {
            snapshot_text = query.value(0).toString();
        }
    }

    if (snapshot_text.isEmpty()) {
        QSqlQuery query(db);
        query.prepare(QStringLiteral(
            "SELECT snapshot_text FROM security_baseline WHERE snapshot_path = ? LIMIT 1"));
        query.addBindValue(snapshot_path);
        if (query.exec() && query.next()) {
            snapshot_text = query.value(0).toString();
        }
    }

    QMutexLocker locker(&baseline_cache_lock());
    if (snapshot_text_cache().size() >= kMaxBaselineCacheEntries) {
        snapshot_text_cache().clear();
    }
    snapshot_text_cache().insert(snapshot_path, snapshot_text);
    return snapshot_text;
}

QVariantMap load_baseline_row(const QString& db_path, const QString& profile_id, const QString& path) {
    if (!ensure_profile_baseline_loaded(db_path, profile_id)) {
        return {};
    }

    QVariantMap cached_row;
    if (lookup_cached_baseline_row(profile_id, path, &cached_row)) {
        return cached_row;
    }
    return {};
}

}  // namespace

BaselineCriptografico::BaselineCriptografico(const QString& storage_root) : storage_root_(storage_root) {}

void BaselineCriptografico::set_storage_root(const QString& storage_root) { storage_root_ = storage_root; }

void BaselineCriptografico::ensure_schema(const QString& db_path) const {
    Q_UNUSED(db_path)

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("baseline_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_baseline ("
        "profile_id TEXT NOT NULL, "
        "caminho TEXT NOT NULL, "
        "sha256 TEXT, "
        "size_b BIGINT, "
        "mtime_utc TEXT, "
        "snapshot_path TEXT, "
        "snapshot_text TEXT, "
        "publisher TEXT, "
        "signature_state TEXT, "
        "updated_at TEXT, "
        "PRIMARY KEY(profile_id, caminho))"));
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_baseline_snapshots ("
        "snapshot_path TEXT PRIMARY KEY, "
        "snapshot_text TEXT, "
        "updated_at TEXT)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_baseline_profile_updated "
        "ON security_baseline (profile_id, updated_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_baseline_updated "
        "ON security_baseline (updated_at DESC)"));
}

QVariantMap BaselineCriptografico::enrich(const QVariantMap& evento, const QVariantMap& contexto) const {
    QVariantMap out;
    const QString db_path = contexto.value(QStringLiteral("db_path")).toString();
    const QString profile_id = contexto.value(QStringLiteral("monitor_profile_id")).toString();
    const QString path = event_path(evento);
    const QFileInfo info(path);

    if (db_path.isEmpty() || profile_id.isEmpty() || path.isEmpty()) {
        return out;
    }

    const QVariantMap previous = load_baseline_row(db_path, profile_id, path);
    const QString previous_sha = previous.value(QStringLiteral("sha256")).toString();
    const QString previous_mtime = previous.value(QStringLiteral("mtime_utc")).toString();
    const QString previous_snapshot = previous.value(QStringLiteral("snapshot_path")).toString();
    const QString previous_snapshot_text = previous.value(QStringLiteral("snapshot_text")).toString();
    const qint64 previous_size = previous.value(QStringLiteral("size_b")).toLongLong();
    const QString previous_publisher = previous.value(QStringLiteral("publisher")).toString();
    const QString previous_signature_state = previous.value(QStringLiteral("signature_state")).toString();

    QString current_sha;
    QString current_snapshot;
    QString current_snapshot_text;
    QString status = QStringLiteral("unavailable");
    QStringList integrity_flags;
    const QString current_publisher = evento.value(QStringLiteral("publisher")).toString();
    const QString current_signature_state = evento.value(QStringLiteral("signature_state")).toString();

    if (info.exists() && info.isDir()) {
        status = QStringLiteral("directory");
    } else if (info.exists() && info.isFile()) {
        const QString current_mtime = info.lastModified().toUTC().toString(Qt::ISODateWithMs);
        const bool can_reuse_previous =
            !previous_sha.isEmpty() &&
            previous_mtime == current_mtime &&
            previous_size == info.size() &&
            previous_publisher == current_publisher &&
            previous_signature_state == current_signature_state;

        if (can_reuse_previous) {
            current_sha = previous_sha;
            current_snapshot = previous_snapshot;
            status = QStringLiteral("unchanged");
        } else {
            QVariantMap current_analysis;
            if (!lookup_cached_file_analysis(info, &current_analysis)) {
                current_analysis.insert(QStringLiteral("sha256"), compute_sha256(path));
                current_analysis.insert(QStringLiteral("snapshot_text"), capture_text_snapshot(path));
                cache_file_analysis(info, current_analysis);
            }

            current_sha = current_analysis.value(QStringLiteral("sha256")).toString();
            current_snapshot_text = current_analysis.value(QStringLiteral("snapshot_text")).toString();
            current_snapshot = current_snapshot_text.isEmpty()
                                   ? QString()
                                   : snapshot_reference(profile_id, path);
            status = previous_sha.isEmpty() ? QStringLiteral("new")
                                            : (previous_sha == current_sha ? QStringLiteral("unchanged")
                                                                           : QStringLiteral("changed"));
        }

        if (!previous_sha.isEmpty() && previous_sha != current_sha && previous_mtime == current_mtime) {
            integrity_flags.append(QStringLiteral("hash_changed_without_mtime"));
        }

        const bool baseline_changed =
            previous_sha != current_sha ||
            previous_size != info.size() ||
            previous_mtime != current_mtime ||
            previous_snapshot != current_snapshot ||
            previous_snapshot_text != current_snapshot_text ||
            previous_publisher != current_publisher ||
            previous_signature_state != current_signature_state;

        if (previous_sha.isEmpty() || baseline_changed) {
            const QString updated_at = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

            QVariantMap persisted_row;
            persisted_row.insert(QStringLiteral("sha256"), current_sha);
            persisted_row.insert(QStringLiteral("size_b"), info.size());
            persisted_row.insert(QStringLiteral("mtime_utc"), current_mtime);
            persisted_row.insert(QStringLiteral("snapshot_path"), current_snapshot);
            persisted_row.insert(QStringLiteral("snapshot_text"), current_snapshot_text);
            persisted_row.insert(QStringLiteral("publisher"), current_publisher);
            persisted_row.insert(QStringLiteral("signature_state"), current_signature_state);
            cache_baseline_row(profile_id, path, persisted_row);

            QVariantMap baseline_row;
            baseline_row.insert(QStringLiteral("profile_id"), profile_id);
            baseline_row.insert(QStringLiteral("caminho"), path);
            baseline_row.insert(QStringLiteral("sha256"), current_sha);
            baseline_row.insert(QStringLiteral("size_b"), info.size());
            baseline_row.insert(QStringLiteral("mtime_utc"), current_mtime);
            baseline_row.insert(QStringLiteral("snapshot_path"), current_snapshot);
            baseline_row.insert(QStringLiteral("publisher"), current_publisher);
            baseline_row.insert(QStringLiteral("signature_state"), current_signature_state);
            baseline_row.insert(QStringLiteral("updated_at"), updated_at);
            SecurityDbWriter::instance().enqueue_baseline_upsert(baseline_row);

            if (!current_snapshot.isEmpty() && !current_snapshot_text.isEmpty()) {
                QVariantMap snapshot_row;
                snapshot_row.insert(QStringLiteral("snapshot_path"), current_snapshot);
                snapshot_row.insert(QStringLiteral("snapshot_text"), current_snapshot_text);
                snapshot_row.insert(QStringLiteral("updated_at"), updated_at);
                SecurityDbWriter::instance().enqueue_baseline_snapshot_upsert(snapshot_row);
            }
        }
    } else if (!previous_sha.isEmpty()) {
        status = QStringLiteral("missing");
        integrity_flags.append(QStringLiteral("missing_from_disk"));
    }

    QString previous_snapshot_text_loaded = previous_snapshot_text;
    if (previous_snapshot_text_loaded.isEmpty() && !previous_snapshot.isEmpty() &&
        (status == QStringLiteral("changed") || status == QStringLiteral("missing"))) {
        previous_snapshot_text_loaded = load_snapshot_text(db_path, previous_snapshot);
    }

    out.insert(QStringLiteral("sha256_atual"), current_sha);
    out.insert(QStringLiteral("sha256_baseline"), previous_sha);
    out.insert(QStringLiteral("baseline_status"), status);
    out.insert(QStringLiteral("integrity_flags"), integrity_flags.join(QStringLiteral("; ")));
    if (!previous_snapshot.isEmpty()) {
        out.insert(QStringLiteral("_baseline_snapshot_prev"), previous_snapshot);
    }
    if (!previous_snapshot_text_loaded.isEmpty()) {
        out.insert(QStringLiteral("_baseline_snapshot_prev_text"), previous_snapshot_text_loaded);
    }
    if (!current_snapshot.isEmpty()) {
        out.insert(QStringLiteral("_baseline_snapshot_current"), current_snapshot);
    }
    if (!current_snapshot_text.isEmpty()) {
        out.insert(QStringLiteral("_baseline_snapshot_current_text"), current_snapshot_text);
    }
    return out;
}

void BaselineCriptografico::clear_runtime_cache() const {
    QMutexLocker locker(&baseline_cache_lock());
    profile_baseline_cache().clear();
    file_analysis_cache().clear();
    snapshot_text_cache().clear();
}

}  // namespace source::security
