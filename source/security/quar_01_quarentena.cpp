#include "quar_01_quarentena.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QSqlQuery>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"

namespace source::security {
namespace {

bool is_script_or_executable(const QString& name) {
    const QString ext = QFileInfo(name).suffix().toLower();
    return ext == QStringLiteral("exe") || ext == QStringLiteral("dll") || ext == QStringLiteral("msi") ||
           ext == QStringLiteral("ps1") || ext == QStringLiteral("bat") || ext == QStringLiteral("cmd") ||
           ext == QStringLiteral("vbs") || ext == QStringLiteral("js");
}

QString quarantine_reference(const QString& original_path, const QString& sha256,
                             const QString& created_at) {
    const QByteArray digest = QCryptographicHash::hash(
        (original_path + QStringLiteral("::") + sha256 + QStringLiteral("::") + created_at).toUtf8(),
        QCryptographicHash::Sha1).toHex();
    return QStringLiteral("sqlite://security_quarantine/%1").arg(QString::fromLatin1(digest));
}

}  // namespace

QuarentenaManager::QuarentenaManager(const QString& storage_root) : storage_root_(storage_root) {}

void QuarentenaManager::set_storage_root(const QString& storage_root) { storage_root_ = storage_root; }

void QuarentenaManager::ensure_schema(const QString& db_path) const {
    Q_UNUSED(db_path)

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("quarantine_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_quarantine ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created_at TEXT, "
        "evidence_id TEXT, "
        "original_path TEXT, "
        "quarantine_path TEXT, "
        "quarantine_blob BLOB, "
        "original_name TEXT, "
        "sha256 TEXT, "
        "status TEXT, "
        "blob_state TEXT, "
        "blob_size_b BIGINT, "
        "blob_updated_at TEXT)"));
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_quarantine_blobs ("
        "quarantine_path TEXT PRIMARY KEY, "
        "original_name TEXT, "
        "blob_content BLOB, "
        "size_b BIGINT, "
        "created_at TEXT)"));
    query.exec(QStringLiteral("ALTER TABLE security_quarantine ADD COLUMN blob_state TEXT"));
    query.exec(QStringLiteral("ALTER TABLE security_quarantine ADD COLUMN blob_size_b BIGINT"));
    query.exec(QStringLiteral("ALTER TABLE security_quarantine ADD COLUMN blob_updated_at TEXT"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_quarantine_created "
        "ON security_quarantine (created_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_quarantine_evidence "
        "ON security_quarantine (evidence_id)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_quarantine_status "
        "ON security_quarantine (status, created_at DESC)"));
}

QVariantMap QuarentenaManager::process(const QVariantMap& evento, const QVariantMap& contexto) const {
    Q_UNUSED(contexto)

    QVariantMap out;
    const QString risk_level = evento.value(QStringLiteral("risk_level")).toString();
    const QString source_path = evento.value(QStringLiteral("dir_atual")).toString();
    const QFileInfo info(source_path);

    if ((risk_level != QStringLiteral("high") && risk_level != QStringLiteral("critical")) ||
        source_path.isEmpty() || !info.exists() || !info.isFile() || !is_script_or_executable(info.fileName()) ||
        info.size() > (64 * 1024 * 1024)) {
        out.insert(QStringLiteral("quarantine_status"), QStringLiteral("not_applicable"));
        return out;
    }

    const QString created_at = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    const QString reference =
        quarantine_reference(source_path, evento.value(QStringLiteral("sha256_atual")).toString(), created_at);
    const QString status = QStringLiteral("queued");

    out.insert(QStringLiteral("quarantine_status"), status);
    out.insert(QStringLiteral("quarantine_path"), reference);

    QVariantMap metadata_row;
    metadata_row.insert(QStringLiteral("created_at"), created_at);
    metadata_row.insert(QStringLiteral("evidence_id"), evento.value(QStringLiteral("evidence_id")).toString());
    metadata_row.insert(QStringLiteral("original_path"), source_path);
    metadata_row.insert(QStringLiteral("quarantine_path"), reference);
    metadata_row.insert(QStringLiteral("original_name"), evento.value(QStringLiteral("nome")).toString());
    metadata_row.insert(QStringLiteral("sha256"), evento.value(QStringLiteral("sha256_atual")).toString());
    metadata_row.insert(QStringLiteral("status"), status);
    metadata_row.insert(QStringLiteral("blob_state"), QStringLiteral("pending"));
    metadata_row.insert(QStringLiteral("blob_size_b"), 0);
    metadata_row.insert(QStringLiteral("blob_updated_at"), created_at);
    SecurityDbWriter::instance().enqueue_quarantine_metadata(metadata_row);

    QVariantMap blob_job;
    blob_job.insert(QStringLiteral("source_path"), source_path);
    blob_job.insert(QStringLiteral("quarantine_path"), reference);
    blob_job.insert(QStringLiteral("original_name"), evento.value(QStringLiteral("nome")).toString());
    SecurityDbWriter::instance().enqueue_quarantine_blob_capture(blob_job);
    return out;
}

}  // namespace source::security
