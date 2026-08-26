#include "evid_01_exportacao_evidencia.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlQuery>
#include <QTextStream>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"

namespace source::security {
namespace {

QString write_json_file(const QString& path, const QJsonDocument& doc) {
    QDir().mkpath(QFileInfo(path).absolutePath());
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return QString();
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return path;
}

QString sha256_of_file(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 64));
    }
    file.close();
    return QString::fromLatin1(hash.result().toHex());
}

bool is_relevant_security_event(const QVariantMap& evento) {
    const QString alert_level = evento.value(QStringLiteral("alerta_nivel")).toString().toLower();
    const QString risk_level = evento.value(QStringLiteral("risk_level")).toString().toLower();
    const QString baseline_status = evento.value(QStringLiteral("baseline_status")).toString().toLower();
    const QString integrity_flags = evento.value(QStringLiteral("integrity_flags")).toString().trimmed();

    if (!evento.value(QStringLiteral("alerta_resumo")).toString().trimmed().isEmpty() &&
        alert_level != QStringLiteral("none")) {
        return true;
    }
    if (risk_level == QStringLiteral("high") || risk_level == QStringLiteral("critical")) {
        return true;
    }
    if (baseline_status == QStringLiteral("changed") || baseline_status == QStringLiteral("missing")) {
        return true;
    }
    return !integrity_flags.isEmpty();
}

QString serialize_json(const QVariantMap& data) {
    return QString::fromUtf8(QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact));
}

QString evidence_reference(const QString& evidence_id) {
    return QStringLiteral("sqlite://security_evidence/%1").arg(evidence_id);
}

QString sha256_of_payload(const QString& evidence_id, const QString& payload_json,
                          const QString& context_json) {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(evidence_id.toUtf8());
    hash.addData(payload_json.toUtf8());
    hash.addData(context_json.toUtf8());
    return QString::fromLatin1(hash.result().toHex());
}

}  // namespace

ExportacaoEvidencia::ExportacaoEvidencia(const QString& storage_root) : storage_root_(storage_root) {}

void ExportacaoEvidencia::set_storage_root(const QString& storage_root) { storage_root_ = storage_root; }

void ExportacaoEvidencia::ensure_schema(const QString& db_path) const {
    Q_UNUSED(db_path)

    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("evidence_schema"));
    if (!db.isValid() || !db.isOpen()) {
        return;
    }

    QSqlQuery query(db);
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_evidence ("
        "evidence_id TEXT PRIMARY KEY, "
        "created_at TEXT, "
        "profile_id TEXT, "
        "profile_name TEXT, "
        "session_id TEXT, "
        "nome TEXT, "
        "caminho TEXT, "
        "tipo_operacao TEXT, "
        "risk_level TEXT, "
        "evidence_ref TEXT, "
        "evidence_hash TEXT, "
        "event_payload TEXT, "
        "context_payload TEXT)"));
    query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS security_evidence_payloads ("
        "evidence_id TEXT PRIMARY KEY, "
        "event_payload TEXT, "
        "context_payload TEXT, "
        "updated_at TEXT)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_evidence_profile_created "
        "ON security_evidence (profile_id, created_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_evidence_created "
        "ON security_evidence (created_at DESC)"));
    query.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_security_evidence_session "
        "ON security_evidence (session_id)"));
}

QVariantMap ExportacaoEvidencia::register_event(const QVariantMap& evento, const QVariantMap& contexto) const {
    QVariantMap out;
    if (!is_relevant_security_event(evento)) {
        return out;
    }

    const QString timestamp = QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd_HHmmss_zzz"));
    const QString evidence_id = QStringLiteral("evidence_%1").arg(timestamp);
    const QString payload_json = serialize_json(evento);
    const QString context_json = serialize_json(contexto);
    const QString manifest_path = evidence_reference(evidence_id);
    const QString evidence_hash = sha256_of_payload(evidence_id, payload_json, context_json);
    const QString created_at = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

    out.insert(QStringLiteral("evidence_id"), evidence_id);
    out.insert(QStringLiteral("evidence_manifest"), manifest_path);
    out.insert(QStringLiteral("evidence_hash"), evidence_hash);

    QVariantMap metadata_row;
    metadata_row.insert(QStringLiteral("evidence_id"), evidence_id);
    metadata_row.insert(QStringLiteral("created_at"), created_at);
    metadata_row.insert(QStringLiteral("profile_id"),
                        contexto.value(QStringLiteral("monitor_profile_id")).toString());
    metadata_row.insert(QStringLiteral("profile_name"),
                        contexto.value(QStringLiteral("monitor_profile_name")).toString());
    metadata_row.insert(QStringLiteral("session_id"), evento.value(QStringLiteral("session_id")).toString());
    metadata_row.insert(QStringLiteral("nome"), evento.value(QStringLiteral("nome")).toString());
    metadata_row.insert(QStringLiteral("caminho"),
                        evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                            ? evento.value(QStringLiteral("dir_anterior")).toString()
                            : evento.value(QStringLiteral("dir_atual")).toString());
    metadata_row.insert(QStringLiteral("tipo_operacao"),
                        evento.value(QStringLiteral("tipo_operacao")).toString());
    metadata_row.insert(QStringLiteral("risk_level"),
                        evento.value(QStringLiteral("risk_level")).toString());
    metadata_row.insert(QStringLiteral("evidence_ref"), manifest_path);
    metadata_row.insert(QStringLiteral("evidence_hash"), evidence_hash);
    SecurityDbWriter::instance().enqueue_evidence_upsert(metadata_row);

    QVariantMap payload_row;
    payload_row.insert(QStringLiteral("evidence_id"), evidence_id);
    payload_row.insert(QStringLiteral("event_payload"), payload_json);
    payload_row.insert(QStringLiteral("context_payload"), context_json);
    payload_row.insert(QStringLiteral("updated_at"), created_at);
    SecurityDbWriter::instance().enqueue_evidence_payload_upsert(payload_row);
    return out;
}

QString ExportacaoEvidencia::export_package(const QVariantList& eventos, const QVariantMap& contexto,
                                            const QString& destination_dir) const {
    const QString base_dir = destination_dir.isEmpty()
                                 ? QDir(storage_root_).filePath(QStringLiteral("evidence/packages"))
                                 : destination_dir;
    const QString package_dir = QDir(base_dir).filePath(
        QStringLiteral("evidence_package_%1")
            .arg(QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd_HHmmss_zzz"))));
    QDir().mkpath(package_dir);

    QJsonArray events_array;
    for (const QVariant& evento : eventos) {
        events_array.append(QJsonObject::fromVariantMap(evento.toMap()));
    }

    const QString events_path = QDir(package_dir).filePath(QStringLiteral("events.json"));
    const QString manifest_path = QDir(package_dir).filePath(QStringLiteral("manifest.json"));
    const QString custody_path = QDir(package_dir).filePath(QStringLiteral("chain_of_custody.txt"));
    const QString checksums_path = QDir(package_dir).filePath(QStringLiteral("checksums.txt"));

    write_json_file(events_path, QJsonDocument(events_array));

    QJsonObject manifest;
    manifest.insert(QStringLiteral("created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    manifest.insert(QStringLiteral("total_events"), eventos.size());
    manifest.insert(QStringLiteral("profile"), QJsonObject::fromVariantMap(contexto));
    manifest.insert(QStringLiteral("events_sha256"), sha256_of_file(events_path));
    write_json_file(manifest_path, QJsonDocument(manifest));

    QFile custody(custody_path);
    if (custody.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&custody);
        out << "Evidence Package\n";
        out << "Created at: " << QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs) << "\n";
        out << "Total events: " << eventos.size() << "\n";
        out << "Profile: " << contexto.value(QStringLiteral("monitor_profile_name")).toString() << "\n";
        custody.close();
    }

    QFile checksums(checksums_path);
    if (checksums.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&checksums);
        out << "events.json " << sha256_of_file(events_path) << "\n";
        out << "manifest.json " << sha256_of_file(manifest_path) << "\n";
        out << "chain_of_custody.txt " << sha256_of_file(custody_path) << "\n";
        checksums.close();
    }

    return package_dir;
}

}  // namespace source::security
