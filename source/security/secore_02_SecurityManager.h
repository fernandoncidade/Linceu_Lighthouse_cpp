#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

namespace source::services {
class Observador;
}

namespace source::security {

class AtribuicaoForense;
class BaselineCriptografico;
class CorrelacaoSessoes;
class DiffConteudo;
class ExportacaoEvidencia;
class InteligenciaExecutaveis;
class MotorRegras;
class QuarentenaManager;
class ScoreRisco;

class SecurityManager : public QObject {
    Q_OBJECT
public:
    explicit SecurityManager(QObject* interface_obj = nullptr);
    ~SecurityManager() override;

    void configure_primary_directory(const QString& directory);
    bool set_profile_name(const QString& profile_name);
    bool add_monitored_directory(const QString& directory);
    bool remove_monitored_directory(const QString& directory);
    QStringList monitored_directories() const;
    QVariantMap profile_summary() const;
    QVariantMap runtime_scan_summary() const;
    QString storage_root() const;
    QString db_path() const;

    void ensure_schema(const QString& db_path = QString()) const;
    QVariantMap enrich_event(const QVariantMap& evento) const;
    QString export_evidence_package(const QVariantList& eventos,
                                    const QString& destination_dir = QString()) const;
    QVariantList list_alerts(int limit = 200) const;
    QVariantList list_alerts_since(qint64 after_id, int limit = 200) const;
    QVariantList list_sessions(int limit = 200) const;
    QVariantList list_sessions_updated_after(const QString& after_updated_at,
                                             int limit = 200) const;
    QVariantList list_quarantine_items(int limit = 200) const;
    QVariantList list_quarantine_items_since(qint64 after_id, int limit = 200) const;
    QVariantList list_baseline_entries(int limit = 200) const;
    QVariantList list_baseline_entries_updated_after(const QString& after_updated_at,
                                                     int limit = 200) const;
    QVariantList list_evidence_entries(int limit = 200) const;
    bool clear_security_database(QString* error_message = nullptr);
    bool restore_quarantine_item(qint64 quarantine_id,
                                 const QString& destination_path,
                                 QString* error_message = nullptr) const;
    bool restore_monitored_version(const QString& original_path,
                                   const QString& destination_path,
                                   QString* error_message = nullptr) const;

    source::services::Observador* configure_runtime_observers() const;
    void stop_runtime_observers() const;
    void start_runtime_observers() const;
    void stop_runtime_scans() const;
    void sync_runtime_language(const QString& language) const;
    bool any_runtime_active() const;
    bool any_runtime_scan_running() const;
    source::services::Observador* primary_observer() const;

signals:
    void runtime_alert_detected(const QVariantMap& row);
    void runtime_session_detected(const QVariantMap& row);

private:
    QVariantMap current_context() const;
    void update_interface_properties() const;

private:
    QObject* interface_;
    QString db_path_;
    QString storage_root_;
    std::unique_ptr<BaselineCriptografico> baseline_;
    std::unique_ptr<InteligenciaExecutaveis> executables_;
    std::unique_ptr<CorrelacaoSessoes> sessions_;
    std::unique_ptr<DiffConteudo> diff_;
    std::unique_ptr<AtribuicaoForense> attribution_;
    std::unique_ptr<MotorRegras> rules_;
    std::unique_ptr<ScoreRisco> risk_;
    std::unique_ptr<QuarentenaManager> quarantine_;
    std::unique_ptr<ExportacaoEvidencia> evidence_;
    bool skip_monitoramento_fallback_;
};

}  // namespace source::security
