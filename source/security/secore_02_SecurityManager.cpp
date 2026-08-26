#include "secore_02_SecurityManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QPointer>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_05_SecurityDbWriter.h"
#include "source/security/attr_01_atribuicao_forense.h"
#include "source/security/base_01_baseline_criptografico.h"
#include "source/security/sdiff_01_diff_conteudo.h"
#include "source/security/evid_01_exportacao_evidencia.h"
#include "source/security/exec_01_inteligencia_executaveis.h"
#include "source/security/mdir_01_multi_diretorio.h"
#include "source/security/quar_01_quarentena.h"
#include "source/security/agre_01_motor_regras.h"
#include "source/security/risk_01_score_risco.h"
#include "source/security/sess_01_correlacao_sessoes.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/utils/CaminhoPersistenteUtils.h"

namespace source::security {
namespace {

QVariantList query_rows(const QString& db_path,
                        const QString& connection_suffix,
                        const QString& sql,
                        const QVariantList& binds = {}) {
    Q_UNUSED(db_path)

    QVariantList rows;
    QSqlDatabase db = source::security::pg::open_database(connection_suffix);
    if (!db.isValid() || !db.isOpen()) {
        return rows;
    }

    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& bind : binds) {
        query.addBindValue(bind);
    }
    if (query.exec()) {
        const QSqlRecord record = query.record();
        while (query.next()) {
            QVariantMap row;
            for (int i = 0; i < record.count(); ++i) {
                row.insert(record.fieldName(i), query.value(i));
            }
            rows.append(row);
        }
    }
    return rows;
}

QVariantList query_monitoramento_rows(const QString& db_path,
                                      const QString& connection_suffix,
                                      const QString& sql,
                                      const QVariantList& binds = {}) {
    QVariantList rows;
    const QString connection_name = QStringLiteral("security_monitoramento_%1").arg(connection_suffix);
    QSqlDatabase db = QSqlDatabase::contains(connection_name)
                          ? QSqlDatabase::database(connection_name, false)
                          : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connection_name);
    if (!db.isValid()) {
        return rows;
    }

    db.setDatabaseName(db_path);
    if (!db.isOpen() && !db.open()) {
        return rows;
    }

    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& bind : binds) {
        query.addBindValue(bind);
    }
    if (query.exec()) {
        const QSqlRecord record = query.record();
        while (query.next()) {
            QVariantMap row;
            for (int i = 0; i < record.count(); ++i) {
                row.insert(record.fieldName(i), query.value(i));
            }
            rows.append(row);
        }
    }
    return rows;
}

bool ensure_parent_dir_for(const QString& destination_path, QString* error_message) {
    const QString parent_dir = QFileInfo(destination_path).absolutePath();
    if (parent_dir.isEmpty()) {
        return true;
    }
    if (QDir().mkpath(parent_dir)) {
        return true;
    }
    if (error_message != nullptr) {
        *error_message = QStringLiteral("Não foi possível criar o diretório de destino.");
    }
    return false;
}

bool overwrite_copy(const QString& source_path, const QString& destination_path, QString* error_message) {
    if (!ensure_parent_dir_for(destination_path, error_message)) {
        return false;
    }
    if (QFileInfo::exists(destination_path) && !QFile::remove(destination_path)) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Não foi possível sobrescrever o arquivo de destino.");
        }
        return false;
    }
    if (!QFile::copy(source_path, destination_path)) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Falha ao copiar o arquivo para o destino.");
        }
        return false;
    }
    return true;
}

bool write_text_file(const QString& text, const QString& destination_path, QString* error_message) {
    if (!ensure_parent_dir_for(destination_path, error_message)) {
        return false;
    }

    QFile file(destination_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Falha ao criar o arquivo de destino.");
        }
        return false;
    }

    const qint64 written = file.write(text.toUtf8());
    file.close();
    if (written < 0) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Falha ao gravar o snapshot restaurado.");
        }
        return false;
    }
    return true;
}

bool write_binary_file(const QByteArray& content, const QString& destination_path, QString* error_message) {
    if (!ensure_parent_dir_for(destination_path, error_message)) {
        return false;
    }

    QFile file(destination_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Falha ao criar o arquivo de destino.");
        }
        return false;
    }

    const qint64 written = file.write(content);
    file.close();
    if (written < 0 || written != content.size()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Falha ao gravar o arquivo restaurado.");
        }
        return false;
    }
    return true;
}

void ensure_alert_reporting(QVariantMap& evento) {
    const QString current_summary = evento.value(QStringLiteral("alerta_resumo")).toString().trimmed();
    if (!current_summary.isEmpty()) {
        return;
    }

    QString summary;
    QString level = evento.value(QStringLiteral("alerta_nivel")).toString().trimmed();

    const QString quarantine_status = evento.value(QStringLiteral("quarantine_status")).toString().trimmed();
    if (summary.isEmpty() && quarantine_status == QStringLiteral("captured")) {
        summary = QStringLiteral("security_quarantine_captured");
        if (level.isEmpty() || level == QStringLiteral("none")) {
            level = QStringLiteral("high");
        }
    }

    const QString integrity_flags = evento.value(QStringLiteral("integrity_flags")).toString().trimmed();
    if (summary.isEmpty() && !integrity_flags.isEmpty()) {
        summary = integrity_flags;
        if (level.isEmpty() || level == QStringLiteral("none")) {
            level = QStringLiteral("high");
        }
    }

    const QString baseline_status = evento.value(QStringLiteral("baseline_status")).toString().trimmed();
    if (summary.isEmpty() &&
        (baseline_status == QStringLiteral("changed") || baseline_status == QStringLiteral("missing"))) {
        summary = QStringLiteral("security_baseline_%1").arg(baseline_status);
        if (level.isEmpty() || level == QStringLiteral("none")) {
            level = baseline_status == QStringLiteral("missing") ? QStringLiteral("high")
                                                                 : QStringLiteral("medium");
        }
    }

    const QString risk_level = evento.value(QStringLiteral("risk_level")).toString().trimmed();
    const QString risk_reasons = evento.value(QStringLiteral("risk_reasons")).toString().trimmed();
    if (summary.isEmpty() &&
        (risk_level == QStringLiteral("high") || risk_level == QStringLiteral("critical")) &&
        !risk_reasons.isEmpty()) {
        summary = risk_reasons;
        if (level.isEmpty() || level == QStringLiteral("none")) {
            level = risk_level;
        }
    }

    if (summary.isEmpty()) {
        return;
    }

    evento.insert(QStringLiteral("alerta_resumo"), summary);
    if (!level.isEmpty()) {
        evento.insert(QStringLiteral("alerta_nivel"), level);
    }
}

QVariantMap build_runtime_alert_row(const QVariantMap& evento, const QVariantMap& contexto) {
    QVariantMap row;
    const QString summary = evento.value(QStringLiteral("alerta_resumo")).toString().trimmed();
    const QString level = evento.value(QStringLiteral("alerta_nivel")).toString().trimmed();
    if (summary.isEmpty() || level.isEmpty() || level == QStringLiteral("none")) {
        return row;
    }

    row.insert(QStringLiteral("created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    row.insert(QStringLiteral("profile_id"), contexto.value(QStringLiteral("monitor_profile_id")).toString());
    row.insert(QStringLiteral("nome"), evento.value(QStringLiteral("nome")).toString());
    row.insert(QStringLiteral("caminho"),
               evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                   ? evento.value(QStringLiteral("dir_anterior")).toString()
                   : evento.value(QStringLiteral("dir_atual")).toString());
    row.insert(QStringLiteral("risk_level"), level);
    row.insert(QStringLiteral("rules"), evento.value(QStringLiteral("alerta_regras")).toString());
    row.insert(QStringLiteral("summary"), summary);
    row.insert(QStringLiteral("session_id"), evento.value(QStringLiteral("session_id")).toString());
    row.insert(QStringLiteral("evidence_id"), evento.value(QStringLiteral("evidence_id")).toString());
    return row;
}

QVariantMap build_runtime_session_row(const QVariantMap& evento, const QVariantMap& contexto) {
    QVariantMap row;
    const QString session_id = evento.value(QStringLiteral("session_id")).toString().trimmed();
    if (session_id.isEmpty()) {
        return row;
    }

    row.insert(QStringLiteral("session_id"), session_id);
    row.insert(QStringLiteral("profile_id"), contexto.value(QStringLiteral("monitor_profile_id")).toString());
    row.insert(QStringLiteral("label"), evento.value(QStringLiteral("session_label")).toString());
    row.insert(QStringLiteral("updated_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));
    row.insert(QStringLiteral("nome"), evento.value(QStringLiteral("nome")).toString());
    row.insert(QStringLiteral("caminho"),
               evento.value(QStringLiteral("dir_atual")).toString().isEmpty()
                   ? evento.value(QStringLiteral("dir_anterior")).toString()
                   : evento.value(QStringLiteral("dir_atual")).toString());
    return row;
}

}  // namespace

SecurityManager::SecurityManager(QObject* interface_obj)
    : QObject(interface_obj),
      interface_(interface_obj),
      db_path_(QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("monitoramento.db"))),
      storage_root_(QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("security"))),
      baseline_(new BaselineCriptografico(storage_root_)),
      executables_(new InteligenciaExecutaveis()),
      sessions_(new CorrelacaoSessoes()),
      diff_(new DiffConteudo()),
      attribution_(new AtribuicaoForense()),
      rules_(new MotorRegras()),
      risk_(new ScoreRisco()),
      quarantine_(new QuarentenaManager(storage_root_)),
      evidence_(new ExportacaoEvidencia(storage_root_)),
      skip_monitoramento_fallback_(false) {
    QDir().mkpath(storage_root_);
    source::security::pg::prepare_runtime();
    source::security::SecurityDbWriter::instance().start();
    ensure_schema(db_path_);
    update_interface_properties();
}

SecurityManager::~SecurityManager() {
    source::security::SecurityDbWriter::instance().wait_until_idle(1500);
    source::security::SecurityDbWriter::instance().stop();
}

void SecurityManager::configure_primary_directory(const QString& directory) {
    MultiDiretorioCoordinator::instance().ensure_profile_for_interface(interface_, directory);
    update_interface_properties();
}

bool SecurityManager::set_profile_name(const QString& profile_name) {
    const bool updated = MultiDiretorioCoordinator::instance().set_profile_name(interface_, profile_name);
    update_interface_properties();
    return updated;
}

bool SecurityManager::add_monitored_directory(const QString& directory) {
    const bool added = MultiDiretorioCoordinator::instance().add_directory(interface_, directory);
    update_interface_properties();
    return added;
}

bool SecurityManager::remove_monitored_directory(const QString& directory) {
    const bool removed = MultiDiretorioCoordinator::instance().remove_directory(interface_, directory);
    update_interface_properties();
    return removed;
}

QStringList SecurityManager::monitored_directories() const {
    return MultiDiretorioCoordinator::instance().directories(interface_);
}

QVariantMap SecurityManager::profile_summary() const {
    return MultiDiretorioCoordinator::instance().profile_summary(interface_);
}

QVariantMap SecurityManager::runtime_scan_summary() const {
    return MultiDiretorioCoordinator::instance().runtime_scan_summary(interface_);
}

QString SecurityManager::storage_root() const { return storage_root_; }

QString SecurityManager::db_path() const { return db_path_; }

void SecurityManager::ensure_schema(const QString& db_path) const {
    const QString effective_db = db_path.isEmpty() ? db_path_ : db_path;
    source::security::pg::prepare_runtime();
    MultiDiretorioCoordinator::instance().ensure_storage_schema();
    baseline_->ensure_schema(effective_db);
    sessions_->ensure_schema(effective_db);
    rules_->ensure_schema(effective_db);
    quarantine_->ensure_schema(effective_db);
    evidence_->ensure_schema(effective_db);
}

QVariantMap SecurityManager::current_context() const {
    QVariantMap context = profile_summary();
    context.insert(QStringLiteral("db_path"), db_path_);
    context.insert(QStringLiteral("storage_root"), storage_root_);
    context.insert(QStringLiteral("monitor_diretorios"),
                   QString::fromUtf8(
                       QJsonDocument::fromVariant(context.value(QStringLiteral("monitor_diretorios_list")))
                           .toJson(QJsonDocument::Compact)));
    return context;
}

void SecurityManager::update_interface_properties() const {
    if (interface_ == nullptr) {
        return;
    }
    const QVariantMap summary = profile_summary();
    interface_->setProperty("security_profile_summary", summary);
    interface_->setProperty("security_multidiretorios",
                            summary.value(QStringLiteral("monitor_diretorios_list")));
    interface_->setProperty("security_storage_backend", source::security::pg::effective_backend_name());
    interface_->setProperty("security_storage_target", source::security::pg::effective_target());
    interface_->setProperty("security_storage_last_error", source::security::pg::last_error());
}

QVariantMap SecurityManager::enrich_event(const QVariantMap& evento) const {
    if (evento.isEmpty()) {
        return evento;
    }

    QVariantMap enriched = evento;
    const QVariantMap context = current_context();
    enriched.insert(QStringLiteral("monitor_profile_id"),
                    context.value(QStringLiteral("monitor_profile_id")).toString());
    enriched.insert(QStringLiteral("monitor_profile_name"),
                    context.value(QStringLiteral("monitor_profile_name")).toString());
    enriched.insert(QStringLiteral("monitor_diretorios"),
                    context.value(QStringLiteral("monitor_diretorios")).toString());

    const auto merge = [&enriched](const QVariantMap& data) {
        for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
            enriched.insert(it.key(), it.value());
        }
    };

    merge(executables_->analyze(enriched, context));
    merge(baseline_->enrich(enriched, context));
    merge(sessions_->correlate(enriched, context));
    merge(diff_->summarize(enriched, context));
    merge(attribution_->attribute(enriched, context));
    merge(rules_->evaluate(enriched, context));
    merge(risk_->score(enriched, context));
    merge(quarantine_->process(enriched, context));
    ensure_alert_reporting(enriched);
    merge(evidence_->register_event(enriched, context));
    rules_->persist_if_needed(enriched, context);

    if (auto* self = const_cast<SecurityManager*>(this); self != nullptr) {
        const QVariantMap alert_row = build_runtime_alert_row(enriched, context);
        if (!alert_row.isEmpty()) {
            emit self->runtime_alert_detected(alert_row);
        }

        const QVariantMap session_row = build_runtime_session_row(enriched, context);
        if (!session_row.isEmpty()) {
            emit self->runtime_session_detected(session_row);
        }
    }
    return enriched;
}

QString SecurityManager::export_evidence_package(const QVariantList& eventos,
                                                 const QString& destination_dir) const {
    return evidence_->export_package(eventos, current_context(), destination_dir);
}

QVariantList SecurityManager::list_alerts(int limit) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path_,
            QStringLiteral("alerts"),
            QStringLiteral("SELECT id, created_at, profile_id, nome, caminho, risk_level, rules, summary, session_id, evidence_id "
                           "FROM security_alerts WHERE profile_id = ? ORDER BY id DESC LIMIT ?"),
            {profile_id, safe_limit});
        if (!rows.isEmpty() || skip_monitoramento_fallback_) {
            return rows;
        }

        return query_monitoramento_rows(
            db_path_,
            QStringLiteral("alerts_monitoramento"),
            QStringLiteral("SELECT rowid AS id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                           "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, "
                           "COALESCE(NULLIF(alerta_nivel, ''), risk_level) AS risk_level, "
                           "alerta_regras AS rules, alerta_resumo AS summary, session_id, evidence_id "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(alerta_resumo, '') <> '' "
                           "ORDER BY rowid DESC LIMIT ?"),
            {profile_id, safe_limit});
    }
    QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("alerts_all"),
        QStringLiteral("SELECT id, created_at, profile_id, nome, caminho, risk_level, rules, summary, session_id, evidence_id "
                       "FROM security_alerts ORDER BY id DESC LIMIT ?"),
        {safe_limit});
    if (!rows.isEmpty() || skip_monitoramento_fallback_) {
        return rows;
    }

    return query_monitoramento_rows(
        db_path_,
        QStringLiteral("alerts_monitoramento_all"),
        QStringLiteral("SELECT rowid AS id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                       "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, "
                       "COALESCE(NULLIF(alerta_nivel, ''), risk_level) AS risk_level, "
                       "alerta_regras AS rules, alerta_resumo AS summary, session_id, evidence_id "
                       "FROM monitoramento WHERE COALESCE(alerta_resumo, '') <> '' "
                       "ORDER BY rowid DESC LIMIT ?"),
        {safe_limit});
}

QVariantList SecurityManager::list_alerts_since(qint64 after_id, int limit) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path_,
            QStringLiteral("alerts_since"),
            QStringLiteral("SELECT id, created_at, profile_id, nome, caminho, risk_level, rules, summary, session_id, evidence_id "
                           "FROM security_alerts WHERE profile_id = ? AND id > ? ORDER BY id ASC LIMIT ?"),
            {profile_id, after_id, safe_limit});
        if (!rows.isEmpty() || skip_monitoramento_fallback_) {
            return rows;
        }

        return query_monitoramento_rows(
            db_path_,
            QStringLiteral("alerts_monitoramento_since"),
            QStringLiteral("SELECT rowid AS id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                           "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, "
                           "COALESCE(NULLIF(alerta_nivel, ''), risk_level) AS risk_level, "
                           "alerta_regras AS rules, alerta_resumo AS summary, session_id, evidence_id "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND rowid > ? AND COALESCE(alerta_resumo, '') <> '' "
                           "ORDER BY rowid ASC LIMIT ?"),
            {profile_id, after_id, safe_limit});
    }

    QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("alerts_since_all"),
        QStringLiteral("SELECT id, created_at, profile_id, nome, caminho, risk_level, rules, summary, session_id, evidence_id "
                       "FROM security_alerts WHERE id > ? ORDER BY id ASC LIMIT ?"),
        {after_id, safe_limit});
    if (!rows.isEmpty() || skip_monitoramento_fallback_) {
        return rows;
    }

    return query_monitoramento_rows(
        db_path_,
        QStringLiteral("alerts_monitoramento_since_all"),
        QStringLiteral("SELECT rowid AS id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                       "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, "
                       "COALESCE(NULLIF(alerta_nivel, ''), risk_level) AS risk_level, "
                       "alerta_regras AS rules, alerta_resumo AS summary, session_id, evidence_id "
                       "FROM monitoramento WHERE rowid > ? AND COALESCE(alerta_resumo, '') <> '' "
                       "ORDER BY rowid ASC LIMIT ?"),
        {after_id, safe_limit});
}

QVariantList SecurityManager::list_sessions(int limit) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path_,
            QStringLiteral("sessions"),
            QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                           "FROM security_sessions WHERE profile_id = ? ORDER BY updated_at DESC LIMIT ?"),
            {profile_id, safe_limit});
        if (!rows.isEmpty() || skip_monitoramento_fallback_) {
            return rows;
        }

        return query_monitoramento_rows(
            db_path_,
            QStringLiteral("sessions_monitoramento"),
            QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                           "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                           "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(session_id, '') <> '' "
                           "GROUP BY session_id ORDER BY MAX(rowid) DESC LIMIT ?"),
            {profile_id, safe_limit});
    }
    QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("sessions_all"),
        QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                       "FROM security_sessions ORDER BY updated_at DESC LIMIT ?"),
        {safe_limit});
    if (!rows.isEmpty() || skip_monitoramento_fallback_) {
        return rows;
    }

    return query_monitoramento_rows(
        db_path_,
        QStringLiteral("sessions_monitoramento_all"),
        QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                       "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                       "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                       "FROM monitoramento WHERE COALESCE(session_id, '') <> '' "
                       "GROUP BY session_id ORDER BY MAX(rowid) DESC LIMIT ?"),
        {safe_limit});
}

QVariantList SecurityManager::list_sessions_updated_after(const QString& after_updated_at, int limit) const {
    if (after_updated_at.isEmpty()) {
        return list_sessions(limit);
    }

    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path_,
            QStringLiteral("sessions_since"),
            QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                           "FROM security_sessions WHERE profile_id = ? AND updated_at > ? "
                           "ORDER BY updated_at ASC LIMIT ?"),
            {profile_id, after_updated_at, safe_limit});
        if (!rows.isEmpty()) {
            return rows;
        }

        return query_monitoramento_rows(
            db_path_,
            QStringLiteral("sessions_monitoramento_since"),
            QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                           "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                           "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(session_id, '') <> '' "
                           "GROUP BY session_id HAVING MAX(timestamp) > ? "
                           "ORDER BY MAX(timestamp) ASC LIMIT ?"),
            {profile_id, after_updated_at, safe_limit});
    }

    QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("sessions_since_all"),
        QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                       "FROM security_sessions WHERE updated_at > ? ORDER BY updated_at ASC LIMIT ?"),
        {after_updated_at, safe_limit});
    if (!rows.isEmpty()) {
        return rows;
    }

    return query_monitoramento_rows(
        db_path_,
        QStringLiteral("sessions_monitoramento_since_all"),
        QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                       "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                       "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                       "FROM monitoramento WHERE COALESCE(session_id, '') <> '' "
                       "GROUP BY session_id HAVING MAX(timestamp) > ? ORDER BY MAX(timestamp) ASC LIMIT ?"),
        {after_updated_at, safe_limit});
}

QVariantList SecurityManager::list_quarantine_items(int limit) const {
    const int safe_limit = qMax(1, limit);
    return query_rows(
        db_path_,
        QStringLiteral("quarantine"),
        QStringLiteral("SELECT id, created_at, evidence_id, original_path, quarantine_path, sha256, status "
                       "FROM security_quarantine ORDER BY id DESC LIMIT ?"),
        {safe_limit});
}

QVariantList SecurityManager::list_quarantine_items_since(qint64 after_id, int limit) const {
    const int safe_limit = qMax(1, limit);
    return query_rows(
        db_path_,
        QStringLiteral("quarantine_since"),
        QStringLiteral("SELECT id, created_at, evidence_id, original_path, quarantine_path, sha256, status "
                       "FROM security_quarantine WHERE id > ? ORDER BY id ASC LIMIT ?"),
        {after_id, safe_limit});
}

QVariantList SecurityManager::list_baseline_entries(int limit) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        return query_rows(
            db_path_,
            QStringLiteral("baseline"),
            QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state, updated_at "
                           "FROM security_baseline WHERE profile_id = ? ORDER BY updated_at DESC LIMIT ?"),
            {profile_id, safe_limit});
    }
    return query_rows(
        db_path_,
        QStringLiteral("baseline_all"),
        QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state, updated_at "
                       "FROM security_baseline ORDER BY updated_at DESC LIMIT ?"),
        {safe_limit});
}

QVariantList SecurityManager::list_baseline_entries_updated_after(const QString& after_updated_at,
                                                                 int limit) const {
    if (after_updated_at.isEmpty()) {
        return list_baseline_entries(limit);
    }

    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        return query_rows(
            db_path_,
            QStringLiteral("baseline_since"),
            QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state, updated_at "
                           "FROM security_baseline WHERE profile_id = ? AND updated_at > ? "
                           "ORDER BY updated_at ASC LIMIT ?"),
            {profile_id, after_updated_at, safe_limit});
    }

    return query_rows(
        db_path_,
        QStringLiteral("baseline_since_all"),
        QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state, updated_at "
                       "FROM security_baseline WHERE updated_at > ? ORDER BY updated_at ASC LIMIT ?"),
        {after_updated_at, safe_limit});
}

QVariantList SecurityManager::list_evidence_entries(int limit) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path_,
            QStringLiteral("evidence"),
            QStringLiteral("SELECT evidence_id, created_at, profile_id, nome, caminho, tipo_operacao, risk_level "
                           "FROM security_evidence WHERE profile_id = ? ORDER BY created_at DESC LIMIT ?"),
            {profile_id, safe_limit});
        if (!rows.isEmpty()) {
            return rows;
        }

        return query_monitoramento_rows(
            db_path_,
            QStringLiteral("evidence_monitoramento"),
            QStringLiteral("SELECT evidence_id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                           "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, tipo_operacao, risk_level "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(evidence_id, '') <> '' "
                           "ORDER BY rowid DESC LIMIT ?"),
            {profile_id, safe_limit});
    }

    QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("evidence_all"),
        QStringLiteral("SELECT evidence_id, created_at, profile_id, nome, caminho, tipo_operacao, risk_level "
                       "FROM security_evidence ORDER BY created_at DESC LIMIT ?"),
        {safe_limit});
    if (!rows.isEmpty()) {
        return rows;
    }

    return query_monitoramento_rows(
        db_path_,
        QStringLiteral("evidence_monitoramento_all"),
        QStringLiteral("SELECT evidence_id, timestamp AS created_at, monitor_profile_id AS profile_id, nome, "
                       "COALESCE(NULLIF(dir_atual, ''), dir_anterior) AS caminho, tipo_operacao, risk_level "
                       "FROM monitoramento WHERE COALESCE(evidence_id, '') <> '' ORDER BY rowid DESC LIMIT ?"),
        {safe_limit});
}

bool SecurityManager::clear_security_database(QString* error_message) {
    auto& writer = source::security::SecurityDbWriter::instance();
    if (!writer.suspend_processing(3000)) {
        if (error_message != nullptr) {
            *error_message =
                QStringLiteral("Não foi possível pausar a persistência do banco de segurança.");
        }
        return false;
    }

    writer.discard_pending();
    source::security::pg::close_all_local_cache_connections();
    ensure_schema();
    source::security::pg::close_all_local_cache_connections();
    bool clear_ok = false;
    QString internal_error;
    {
        QSqlDatabase db = source::security::pg::open_database(QStringLiteral("security_clear_all"));
        if (!db.isValid() || !db.isOpen()) {
            const QString storage_error = source::security::pg::last_error();
            internal_error = storage_error.isEmpty()
                                 ? QStringLiteral("Não foi possível abrir o banco de dados de segurança.")
                                 : QStringLiteral("Não foi possível abrir o banco de dados de segurança: %1")
                                       .arg(storage_error);
        } else if (!db.transaction()) {
            const QString sql_error = db.lastError().text();
            internal_error = sql_error.isEmpty()
                                 ? QStringLiteral("Não foi possível iniciar a limpeza do banco de segurança.")
                                 : QStringLiteral("Não foi possível iniciar a limpeza do banco de segurança: %1")
                                       .arg(sql_error);
        } else {
            const QStringList statements = {
                QStringLiteral("DELETE FROM security_quarantine_blobs"),
                QStringLiteral("DELETE FROM security_quarantine"),
                QStringLiteral("DELETE FROM security_baseline_snapshots"),
                QStringLiteral("DELETE FROM security_baseline"),
                QStringLiteral("DELETE FROM security_evidence_payloads"),
                QStringLiteral("DELETE FROM security_evidence"),
                QStringLiteral("DELETE FROM security_alerts"),
                QStringLiteral("DELETE FROM security_sessions"),
                QStringLiteral("DELETE FROM security_profile_directories"),
                QStringLiteral("DELETE FROM security_profiles"),
            };

            bool statements_ok = true;
            QString statement_error;
            {
                QSqlQuery query(db);
                for (const QString& statement : statements) {
                    if (!query.exec(statement)) {
                        statements_ok = false;
                        statement_error = query.lastError().text();
                        break;
                    }
                }

                if (!statements_ok) {
                    db.rollback();
                    internal_error = statement_error.isEmpty()
                                         ? QStringLiteral("Falha ao limpar os dados do banco de segurança.")
                                         : QStringLiteral("Falha ao limpar os dados do banco de segurança: %1")
                                               .arg(statement_error);
                } else if (!db.commit()) {
                    const QString sql_error = db.lastError().text();
                    internal_error = sql_error.isEmpty()
                                         ? QStringLiteral("Falha ao concluir a limpeza do banco de segurança.")
                                         : QStringLiteral("Falha ao concluir a limpeza do banco de segurança: %1")
                                               .arg(sql_error);
                } else {
                    query.exec(QStringLiteral("PRAGMA wal_checkpoint(TRUNCATE)"));
                    query.exec(QStringLiteral("VACUUM"));
                    clear_ok = true;
                }
            }
        }

        if (db.isValid()) {
            db.close();
        }
    }
    source::security::pg::close_all_local_cache_connections();

    if (!clear_ok) {
        writer.resume_processing();
        if (error_message != nullptr) {
            *error_message = internal_error;
        }
        return false;
    }

    const QString db_file = source::security::pg::local_cache_path();
    QFile::remove(db_file + QStringLiteral("-wal"));
    QFile::remove(db_file + QStringLiteral("-shm"));

    skip_monitoramento_fallback_ = true;
    baseline_->clear_runtime_cache();
    sessions_->clear_runtime_cache();
    rules_->clear_runtime_cache();
    update_interface_properties();
    writer.resume_processing();
    return true;
}

bool SecurityManager::restore_quarantine_item(qint64 quarantine_id,
                                              const QString& destination_path,
                                              QString* error_message) const {
    const QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("restore_quarantine"),
        QStringLiteral("SELECT COALESCE(b.blob_content, q.quarantine_blob) AS quarantine_blob "
                       "FROM security_quarantine q "
                       "LEFT JOIN security_quarantine_blobs b ON b.quarantine_path = q.quarantine_path "
                       "WHERE q.id = ? LIMIT 1"),
        {quarantine_id});
    if (rows.isEmpty()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Registro de quarentena não encontrado.");
        }
        return false;
    }

    const QByteArray content = rows.first().toMap().value(QStringLiteral("quarantine_blob")).toByteArray();
    if (content.isEmpty()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Arquivo em quarentena não está mais disponível.");
        }
        return false;
    }

    return write_binary_file(content, destination_path, error_message);
}

bool SecurityManager::restore_monitored_version(const QString& original_path,
                                                const QString& destination_path,
                                                QString* error_message) const {
    const QString profile_id = current_context().value(QStringLiteral("monitor_profile_id")).toString();
    if (profile_id.isEmpty()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Perfil de monitoramento não definido.");
        }
        return false;
    }

    const QVariantList rows = query_rows(
        db_path_,
        QStringLiteral("restore_baseline"),
        QStringLiteral("SELECT COALESCE(s.snapshot_text, b.snapshot_text) AS snapshot_text "
                       "FROM security_baseline b "
                       "LEFT JOIN security_baseline_snapshots s ON s.snapshot_path = b.snapshot_path "
                       "WHERE b.profile_id = ? AND b.caminho = ? LIMIT 1"),
        {profile_id, original_path});
    if (rows.isEmpty()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Versão monitorada não encontrada.");
        }
        return false;
    }

    const QString snapshot_text = rows.first().toMap().value(QStringLiteral("snapshot_text")).toString();
    if (snapshot_text.isEmpty()) {
        if (error_message != nullptr) {
            *error_message = QStringLiteral("Não existe snapshot disponível para restauração.");
        }
        return false;
    }

    return write_text_file(snapshot_text, destination_path, error_message);
}

source::services::Observador* SecurityManager::configure_runtime_observers() const {
    if (interface_ == nullptr) {
        return nullptr;
    }

    auto callback = [interface_guard = QPointer<QObject>(interface_)](const QVariantMap& evento) {
        QObject* interface_viva = interface_guard.data();
        if (interface_viva == nullptr) {
            return;
        }
        source::services::adicionar_evento(interface_viva, QVariant(evento));
    };

    return MultiDiretorioCoordinator::instance().configure_observers(interface_, callback);
}

void SecurityManager::stop_runtime_observers() const {
    source::services::parar_sistema_evento(interface_);
    MultiDiretorioCoordinator::instance().stop_interface(interface_);
}

void SecurityManager::start_runtime_observers() const {
    MultiDiretorioCoordinator::instance().start_interface(interface_);
}

void SecurityManager::stop_runtime_scans() const {
    MultiDiretorioCoordinator::instance().stop_scan_interface(interface_);
}

void SecurityManager::sync_runtime_language(const QString& language) const {
    MultiDiretorioCoordinator::instance().sync_interface_language(interface_, language);
}

bool SecurityManager::any_runtime_active() const {
    return MultiDiretorioCoordinator::instance().any_active(interface_);
}

bool SecurityManager::any_runtime_scan_running() const {
    return MultiDiretorioCoordinator::instance().any_scan_running(interface_);
}

source::services::Observador* SecurityManager::primary_observer() const {
    return MultiDiretorioCoordinator::instance().primary_observer(interface_);
}

}  // namespace source::security
