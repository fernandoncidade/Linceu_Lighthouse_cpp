#pragma once

#include <QHash>
#include <QObject>
#include <QVariantList>

#include "source/security/secui_00_helpers.h"

class QTimer;

namespace source::security::ui {

class SecurityUiController : public QObject {
public:
    explicit SecurityUiController(QObject* interface_obj);

    void open_multi_directory_dialog();
    void open_alert_center_dialog();
    void open_session_center_dialog();
    void open_recovery_dialog();
    void open_evidence_dialog();
    bool clear_security_database(bool ask_confirmation = true,
                                 bool show_feedback = true,
                                 QString* error_message = nullptr);
    void reset_notification_state();
    void refresh_runtime_watch_state();
    void retranslate_runtime_feedback();
    void notification_preferences_changed();

public slots:
    void poll_runtime_events();

private:
    enum class RuntimeNotificationKind {
        None,
        Alerts,
        Sessions,
        Quarantine,
    };

    struct PopupAggregateState {
        qint64 pending_count = 0;
        QVariantList sample_rows;
        qint64 cooldown_until_ms = 0;
        bool high_priority = false;
    };

    void initialize_notification_state(bool needs_alerts,
                                       bool needs_sessions,
                                       bool needs_quarantine,
                                       bool needs_baseline);
    bool is_dialog_visible(const char* property_name) const;
    bool should_poll_alerts() const;
    bool should_poll_sessions() const;
    bool should_poll_quarantine() const;
    bool should_poll_baseline() const;
    QString notification_title(RuntimeNotificationKind kind) const;
    QStringList notification_lines(RuntimeNotificationKind kind,
                                   const QVariantList& rows) const;
    void remember_runtime_notification(RuntimeNotificationKind kind,
                                       const QVariantList& rows);
    void clear_runtime_notification();
    void render_last_runtime_notification();
    void schedule_runtime_feedback_render();
    void on_runtime_alert_row(const QVariantMap& row);
    void on_runtime_session_row(const QVariantMap& row);
    void notify_new_alerts(const QVariantList& alerts);
    void notify_new_sessions(const QVariantList& sessions);
    void notify_new_quarantine_records(const QVariantList& quarantine_records);
    QString popup_config_key(RuntimeNotificationKind kind) const;
    SecurityPopupConfig popup_config(RuntimeNotificationKind kind) const;
    PopupAggregateState& popup_state(RuntimeNotificationKind kind);
    QTimer* popup_timer(RuntimeNotificationKind kind) const;
    void clear_popup_aggregate_state(RuntimeNotificationKind kind);
    void clear_all_popup_aggregate_states();
    void accumulate_popup_rows(RuntimeNotificationKind kind, const QVariantList& rows);
    void try_emit_popup(RuntimeNotificationKind kind);
    void show_popup_message(RuntimeNotificationKind kind,
                            const QVariantList& rows,
                            qint64 total_count,
                            bool high_priority);
    void refresh_open_dialog(const char* property_name, const char* method_name);

    QObject* interface_;
    QTimer* poll_timer_;
    QTimer* feedback_render_timer_;
    QTimer* alerts_popup_timer_;
    QTimer* sessions_popup_timer_;
    QTimer* quarantine_popup_timer_;
    bool initialized_state_;
    qint64 last_alert_id_;
    qint64 last_quarantine_id_;
    QString last_session_updated_at_;
    QString last_baseline_updated_at_;
    QHash<QString, QString> seen_session_labels_;
    RuntimeNotificationKind last_notification_kind_;
    QVariantList last_notification_rows_;
    PopupAggregateState alerts_popup_state_;
    PopupAggregateState sessions_popup_state_;
    PopupAggregateState quarantine_popup_state_;
};

}  // namespace source::security::ui
