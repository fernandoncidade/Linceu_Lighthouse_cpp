#include "secui_01_SecurityUiController.h"

#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QPointer>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

#include "secui_00_helpers.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/security/secui_02_MultiDirectoryDialog.h"
#include "source/security/secui_03_AlertCenterDialog.h"
#include "source/security/secui_04_SessionCenterDialog.h"
#include "source/security/secui_05_RecoveryDialog.h"
#include "source/security/secui_06_EvidenceDialog.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {
namespace {

QString summarize_lines(const QStringList& lines, int max_lines = 4) {
    const int bounded_max = qBound(1, max_lines, kSecurityPopupMaxLinesLimit);
    QStringList shown = lines.mid(0, bounded_max);
    if (lines.size() > bounded_max) {
        shown.append(QStringLiteral("..."));
    }
    return shown.join(QStringLiteral("\n"));
}

QString max_iso_timestamp(const QString& current, const QString& candidate) {
    return current < candidate ? candidate : current;
}

bool is_high_priority_alert_row(const QVariantMap& row) {
    const QString raw_level = row.value(QStringLiteral("risk_level")).toString().toLower();
    return raw_level == QStringLiteral("high") || raw_level == QStringLiteral("critical");
}

void append_sample_rows(QVariantList& sample_rows, const QVariantList& new_rows, int max_rows) {
    const int bounded_max = qBound(1, max_rows, 1000);
    for (const QVariant& row : new_rows) {
        sample_rows.append(row);
        while (sample_rows.size() > bounded_max) {
            sample_rows.removeFirst();
        }
    }
}

QMessageBox::StandardButton exec_translated_message_box(QWidget* parent,
                                                        QObject* loc,
                                                        QMessageBox::Icon icon,
                                                        const QString& title,
                                                        const QString& text,
                                                        QMessageBox::StandardButtons buttons,
                                                        QMessageBox::StandardButton default_button =
                                                            QMessageBox::NoButton) {
    QMessageBox message_box(parent);
    message_box.setIcon(icon);
    message_box.setWindowTitle(title);
    message_box.setText(text);
    message_box.setStandardButtons(buttons);
    if (default_button != QMessageBox::NoButton) {
        message_box.setDefaultButton(default_button);
    }

    if (buttons.testFlag(QMessageBox::Yes)) {
        if (QAbstractButton* button = message_box.button(QMessageBox::Yes)) {
            button->setText(get_text(loc, QStringLiteral("yes"), QStringLiteral("Yes")));
        }
    }
    if (buttons.testFlag(QMessageBox::No)) {
        if (QAbstractButton* button = message_box.button(QMessageBox::No)) {
            button->setText(get_text(loc, QStringLiteral("no"), QStringLiteral("No")));
        }
    }
    if (buttons.testFlag(QMessageBox::Ok)) {
        if (QAbstractButton* button = message_box.button(QMessageBox::Ok)) {
            button->setText(get_text(loc, QStringLiteral("ok"), QStringLiteral("OK")));
        }
    }

    return static_cast<QMessageBox::StandardButton>(message_box.exec());
}

void clear_temporary_security_feedback(QObject* interface_obj, const QString& expected_text, int delay_ms) {
    if (interface_obj == nullptr || expected_text.trimmed().isEmpty()) {
        return;
    }

    QPointer<QObject> interface_guard(interface_obj);
    QTimer::singleShot(delay_ms, interface_obj, [interface_guard, expected_text]() {
        QObject* interface_viva = interface_guard.data();
        if (interface_viva == nullptr) {
            return;
        }

        bool clear_label = false;
        if (QObject* label_obj = interface_viva->property("rotulo_seguranca").value<QObject*>()) {
            if (auto* label = qobject_cast<QLabel*>(label_obj)) {
                clear_label = (label->text() == expected_text);
            } else {
                clear_label = (label_obj->property("text").toString() == expected_text);
            }
        }
        if (clear_label) {
            clear_security_result_label(interface_viva);
        }

        if (QObject* status_obj = interface_viva->property("status_bar").value<QObject*>()) {
            if (auto* status_bar = qobject_cast<QStatusBar*>(status_obj)) {
                if (status_bar->currentMessage() == expected_text) {
                    clear_status_message(interface_viva);
                }
            }
        }
    });
}

class NotificationPopupDialog : public QDialog {
public:
    NotificationPopupDialog(QWidget* parent,
                            const QString& title,
                            const QString& content,
                            bool high_priority,
                            QObject* loc)
        : QDialog(parent) {
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(false);
        setWindowTitle(title);
        resize(700, 420);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(12, 12, 12, 12);
        layout->setSpacing(10);

        auto* details_edit = new QPlainTextEdit(this);
        details_edit->setReadOnly(true);
        details_edit->setPlainText(content);
        details_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
        if (high_priority) {
            details_edit->setStyleSheet(QStringLiteral("color: #8b1e1e; font-weight: 600;"));
        }
        layout->addWidget(details_edit, 1);

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
        if (auto* close_button =
                qobject_cast<QPushButton*>(buttons->button(QDialogButtonBox::Close))) {
            close_button->setText(
                get_text(loc, QStringLiteral("close"), QStringLiteral("Fechar")));
        }
        QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::close);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::close);
        layout->addWidget(buttons);
    }
};

}

SecurityUiController::SecurityUiController(QObject* interface_obj)
    : QObject(interface_obj),
      interface_(interface_obj),
      poll_timer_(new QTimer(this)),
      feedback_render_timer_(new QTimer(this)),
      alerts_popup_timer_(new QTimer(this)),
      sessions_popup_timer_(new QTimer(this)),
      quarantine_popup_timer_(new QTimer(this)),
      initialized_state_(false),
      last_alert_id_(0),
      last_quarantine_id_(0),
      last_notification_kind_(RuntimeNotificationKind::None) {
    poll_timer_->setInterval(3000);
    feedback_render_timer_->setSingleShot(true);
    feedback_render_timer_->setInterval(120);
    alerts_popup_timer_->setSingleShot(true);
    sessions_popup_timer_->setSingleShot(true);
    quarantine_popup_timer_->setSingleShot(true);
    QObject::connect(poll_timer_, &QTimer::timeout, this, &SecurityUiController::poll_runtime_events);
    QObject::connect(feedback_render_timer_, &QTimer::timeout, this,
                     [this]() { render_last_runtime_notification(); });
    QObject::connect(alerts_popup_timer_, &QTimer::timeout, this,
                     [this]() { try_emit_popup(RuntimeNotificationKind::Alerts); });
    QObject::connect(sessions_popup_timer_, &QTimer::timeout, this,
                     [this]() { try_emit_popup(RuntimeNotificationKind::Sessions); });
    QObject::connect(quarantine_popup_timer_, &QTimer::timeout, this,
                     [this]() { try_emit_popup(RuntimeNotificationKind::Quarantine); });
    if (auto* loc = get_localizador(interface_)) {
        QObject::connect(loc, &source::ui::LocalizadorQt::idioma_alterado, this,
                         [this](const QString&) { retranslate_runtime_feedback(); });
        QObject::connect(loc, &source::ui::LocalizadorQt::traducoes_carregadas, this,
                         [this](const QString&) { retranslate_runtime_feedback(); });
    }
    if (auto* security_manager = get_security_manager(interface_)) {
        QObject::connect(security_manager, &source::security::SecurityManager::runtime_alert_detected, this,
                         [this](const QVariantMap& row) { on_runtime_alert_row(row); });
        QObject::connect(security_manager, &source::security::SecurityManager::runtime_session_detected, this,
                         [this](const QVariantMap& row) { on_runtime_session_row(row); });
    }
    refresh_runtime_watch_state();
}

void SecurityUiController::open_multi_directory_dialog() {
    if (interface_ == nullptr) {
        return;
    }
    if (QObject* existing = interface_->property("_security_multidirectory_dialog").value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(existing)) {
            widget->show();
            widget->raise();
            widget->activateWindow();
            return;
        }
    }

    auto* dialog = new MultiDirectoryDialog(nullptr, interface_);
    register_dialog(interface_, "_security_multidirectory_dialog", dialog);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    refresh_runtime_watch_state();
}

void SecurityUiController::open_alert_center_dialog() {
    if (interface_ == nullptr) {
        return;
    }
    if (QObject* existing = interface_->property("_security_alert_center_dialog").value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(existing)) {
            widget->show();
            widget->raise();
            widget->activateWindow();
            return;
        }
    }

    auto* dialog = new AlertCenterDialog(nullptr, interface_);
    register_dialog(interface_, "_security_alert_center_dialog", dialog);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    refresh_runtime_watch_state();
}

void SecurityUiController::open_session_center_dialog() {
    if (interface_ == nullptr) {
        return;
    }
    if (QObject* existing = interface_->property("_security_session_center_dialog").value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(existing)) {
            widget->show();
            widget->raise();
            widget->activateWindow();
            return;
        }
    }

    auto* dialog = new SessionCenterDialog(nullptr, interface_);
    register_dialog(interface_, "_security_session_center_dialog", dialog);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    refresh_runtime_watch_state();
}

void SecurityUiController::open_recovery_dialog() {
    if (interface_ == nullptr) {
        return;
    }
    if (QObject* existing = interface_->property("_security_recovery_dialog").value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(existing)) {
            widget->show();
            widget->raise();
            widget->activateWindow();
            return;
        }
    }

    auto* dialog = new RecoveryDialog(nullptr, interface_);
    register_dialog(interface_, "_security_recovery_dialog", dialog);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    refresh_runtime_watch_state();
}

void SecurityUiController::open_evidence_dialog() {
    if (interface_ == nullptr) {
        return;
    }
    if (QObject* existing = interface_->property("_security_evidence_dialog").value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(existing)) {
            widget->show();
            widget->raise();
            widget->activateWindow();
            return;
        }
    }

    auto* dialog = new EvidenceDialog(nullptr, interface_);
    register_dialog(interface_, "_security_evidence_dialog", dialog);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    refresh_runtime_watch_state();
}

bool SecurityUiController::clear_security_database(bool ask_confirmation,
                                                   bool show_feedback,
                                                   QString* error_message) {
    auto* security_manager = get_security_manager(interface_);
    auto* loc = get_localizador(interface_);
    auto* parent = qobject_cast<QWidget*>(interface_);
    if (error_message != nullptr) {
        error_message->clear();
    }

    if (security_manager == nullptr) {
        if (error_message != nullptr) {
            *error_message = get_text(loc,
                                      QStringLiteral("security_no_security_manager"),
                                      QStringLiteral("Gerenciador de segurança indisponível."));
        }
        return false;
    }

    if (ask_confirmation) {
        const QString question = get_text(
            loc,
            QStringLiteral("security_clear_database_confirm"),
            QStringLiteral("Deseja limpar todos os dados persistidos em security.db?\n\n"
                           "Novos eventos poderão voltar a ser gravados imediatamente se o monitoramento continuar ativo."));
        const auto answer = exec_translated_message_box(
            parent,
            loc,
            QMessageBox::Warning,
            get_text(loc, QStringLiteral("security_clear_database"),
                     QStringLiteral("Limpar banco de segurança")),
            question,
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);
        if (answer != QMessageBox::Yes) {
            return false;
        }
    }

    QString local_error_message;
    if (!security_manager->clear_security_database(&local_error_message)) {
        if (error_message != nullptr) {
            *error_message = local_error_message;
        }

        if (!show_feedback) {
            return false;
        }

        clear_runtime_notification();
        const QString text = local_error_message.isEmpty()
                                 ? get_text(loc, QStringLiteral("security_operation_failed"),
                                            QStringLiteral("Operação não concluída."))
                                 : local_error_message;
        set_security_result_label(interface_, text);
        show_status_message(interface_, text);
        exec_translated_message_box(
            parent,
            loc,
            QMessageBox::Warning,
            get_text(loc, QStringLiteral("security_operation_failed"),
                     QStringLiteral("Operação não concluída")),
            text,
            QMessageBox::Ok,
            QMessageBox::Ok);
        return false;
    }

    reset_notification_state();
    refresh_open_dialog("_security_alert_center_dialog", "refresh_data");
    refresh_open_dialog("_security_session_center_dialog", "refresh_data");
    refresh_open_dialog("_security_recovery_dialog", "refresh_data");
    refresh_open_dialog("_security_evidence_dialog", "refresh_data");

    clear_runtime_notification();
    const QString success = get_text(
        loc,
        QStringLiteral("security_clear_database_success"),
        QStringLiteral("Todos os dados persistidos em security.db foram removidos."));
    if (show_feedback) {
        clear_security_result_label(interface_);
        set_result_label(interface_, success);
        show_status_message(interface_, success, 3000);
        exec_translated_message_box(
            parent,
            loc,
            QMessageBox::Information,
            get_text(loc, QStringLiteral("security_operation_completed"),
                     QStringLiteral("Operação concluída")),
            success,
            QMessageBox::Ok,
            QMessageBox::Ok);
    } else {
        clear_security_result_label(interface_);
        clear_status_message(interface_);
    }
    refresh_runtime_watch_state();
    return true;
}

void SecurityUiController::reset_notification_state() {
    initialized_state_ = false;
    last_alert_id_ = 0;
    last_quarantine_id_ = 0;
    last_session_updated_at_.clear();
    last_baseline_updated_at_.clear();
    seen_session_labels_.clear();
    clear_runtime_notification();
    clear_all_popup_aggregate_states();
}

void SecurityUiController::initialize_notification_state(bool needs_alerts,
                                                         bool needs_sessions,
                                                         bool needs_quarantine,
                                                         bool needs_baseline) {
    if (initialized_state_) {
        return;
    }

    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        return;
    }

    if (needs_alerts) {
        const QVariantList alerts = security_manager->list_alerts(16);
        for (const QVariant& item : alerts) {
            last_alert_id_ = qMax(last_alert_id_, item.toMap().value(QStringLiteral("id")).toLongLong());
        }
    }

    if (needs_quarantine) {
        const QVariantList quarantine_items = security_manager->list_quarantine_items(16);
        for (const QVariant& item : quarantine_items) {
            last_quarantine_id_ = qMax(last_quarantine_id_, item.toMap().value(QStringLiteral("id")).toLongLong());
        }
    }

    if (needs_sessions) {
        const QVariantList sessions = security_manager->list_sessions(32);
        for (const QVariant& item : sessions) {
            const QVariantMap row = item.toMap();
            last_session_updated_at_ =
                max_iso_timestamp(last_session_updated_at_, row.value(QStringLiteral("updated_at")).toString());
            seen_session_labels_.insert(row.value(QStringLiteral("session_id")).toString(),
                                        row.value(QStringLiteral("label")).toString());
        }
    }

    if (needs_baseline) {
        const QVariantList baseline_entries = security_manager->list_baseline_entries(16);
        for (const QVariant& item : baseline_entries) {
            last_baseline_updated_at_ =
                max_iso_timestamp(last_baseline_updated_at_,
                                  item.toMap().value(QStringLiteral("updated_at")).toString());
        }
    }

    initialized_state_ = true;
}

bool SecurityUiController::is_dialog_visible(const char* property_name) const {
    if (interface_ == nullptr || property_name == nullptr) {
        return false;
    }
    if (QObject* dialog = interface_->property(property_name).value<QObject*>()) {
        if (auto* widget = qobject_cast<QWidget*>(dialog)) {
            return widget->isVisible();
        }
    }
    return false;
}

bool SecurityUiController::should_poll_alerts() const {
    return is_dialog_visible("_security_alert_center_dialog") ||
           is_dialog_visible("_security_evidence_dialog");
}

bool SecurityUiController::should_poll_sessions() const {
    return false;
}

bool SecurityUiController::should_poll_quarantine() const {
    const SecurityPopupConfig popup_config =
        get_security_popup_config(interface_, QStringLiteral("quarantine_popup"), false);
    return is_dialog_visible("_security_recovery_dialog") ||
           popup_config.enabled ||
           get_security_ui_flag(interface_, QStringLiteral("notifications_line"), false) ||
           get_security_ui_flag(interface_, QStringLiteral("notifications_status_bar"), false);
}

bool SecurityUiController::should_poll_baseline() const {
    return is_dialog_visible("_security_recovery_dialog");
}

void SecurityUiController::refresh_runtime_watch_state() {
    auto* security_manager = get_security_manager(interface_);
    const bool any_dialog_visible =
        is_dialog_visible("_security_alert_center_dialog") ||
        is_dialog_visible("_security_recovery_dialog") ||
        is_dialog_visible("_security_evidence_dialog") ||
        is_dialog_visible("_security_multidirectory_dialog");
    const bool runtime_active = security_manager != nullptr &&
                                (security_manager->any_runtime_active() ||
                                 security_manager->any_runtime_scan_running());

    const bool needs_runtime_watch =
        any_dialog_visible ||
        (runtime_active &&
         (should_poll_alerts() || should_poll_sessions() || should_poll_quarantine() || should_poll_baseline()));
    if (!needs_runtime_watch) {
        poll_timer_->stop();
        return;
    }

    const int target_interval_ms = any_dialog_visible ? 900 : 5000;
    if (poll_timer_->interval() != target_interval_ms) {
        poll_timer_->setInterval(target_interval_ms);
    }
    if (!poll_timer_->isActive()) {
        poll_timer_->start();
    }
}

QString SecurityUiController::notification_title(RuntimeNotificationKind kind) const {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr) {
        return QString();
    }

    switch (kind) {
        case RuntimeNotificationKind::Alerts:
            return get_text(loc, "security_alert_popup_title", "Alertas de segurança");
        case RuntimeNotificationKind::Sessions:
            return get_text(loc, "security_session_popup_title", "Sessões detectadas");
        case RuntimeNotificationKind::Quarantine:
            return get_text(loc, "security_quarantine_popup_title", "Quarentena atualizada");
        case RuntimeNotificationKind::None:
            return QString();
    }

    return QString();
}

QStringList SecurityUiController::notification_lines(RuntimeNotificationKind kind,
                                                     const QVariantList& rows) const {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr || rows.isEmpty()) {
        return {};
    }

    QStringList lines;
    for (const QVariant& item : rows) {
        const QVariantMap row = item.toMap();
        switch (kind) {
            case RuntimeNotificationKind::Alerts: {
                const QString summary = source::security::traduzir_valor_ui(
                    loc, QStringLiteral("alerta_resumo"), row.value(QStringLiteral("summary")).toString());
                const QString level = source::security::traduzir_valor_ui(
                    loc, QStringLiteral("risk_level"), row.value(QStringLiteral("risk_level")).toString());
                lines.append(QStringLiteral("[%1] %2").arg(level, summary));
                break;
            }
            case RuntimeNotificationKind::Sessions: {
                const QString label = source::security::traduzir_valor_ui(
                    loc, QStringLiteral("session_label"), row.value(QStringLiteral("label")).toString());
                lines.append(QStringLiteral("%1 (%2)")
                                 .arg(label, row.value(QStringLiteral("session_id")).toString()));
                break;
            }
            case RuntimeNotificationKind::Quarantine: {
                const QString status = source::security::traduzir_valor_ui(
                    loc, QStringLiteral("quarantine_status"), row.value(QStringLiteral("status")).toString());
                lines.append(QStringLiteral("%1: %2")
                                 .arg(status, row.value(QStringLiteral("original_path")).toString()));
                break;
            }
            case RuntimeNotificationKind::None:
                break;
        }
    }
    return lines;
}

void SecurityUiController::remember_runtime_notification(RuntimeNotificationKind kind,
                                                         const QVariantList& rows) {
    last_notification_kind_ = kind;
    last_notification_rows_ = rows;
}

void SecurityUiController::clear_runtime_notification() {
    last_notification_kind_ = RuntimeNotificationKind::None;
    last_notification_rows_.clear();
}

void SecurityUiController::schedule_runtime_feedback_render() {
    if (feedback_render_timer_ == nullptr) {
        render_last_runtime_notification();
        return;
    }
    feedback_render_timer_->start();
}

void SecurityUiController::render_last_runtime_notification() {
    if (last_notification_kind_ == RuntimeNotificationKind::None || last_notification_rows_.isEmpty()) {
        clear_security_footer_summary_notification(interface_);
        return;
    }

    auto* loc = get_localizador(interface_);
    if (loc == nullptr) {
        return;
    }

    const QString title = notification_title(last_notification_kind_);
    const QStringList lines = notification_lines(last_notification_kind_, last_notification_rows_);
    if (title.isEmpty() || lines.isEmpty()) {
        return;
    }

    const bool show_summary =
        get_security_ui_flag(interface_, QStringLiteral("notifications_line"), false) ||
        get_security_ui_flag(interface_, QStringLiteral("notifications_status_bar"), false);
    if (!show_summary) {
        clear_security_footer_summary_notification(interface_);
        return;
    }

    QString detail = lines.first();
    if (last_notification_kind_ != RuntimeNotificationKind::Alerts) {
        detail = title + QStringLiteral(": ") + detail;
    }

    const QString summary_label =
        get_text(loc, QStringLiteral("security_runtime_summary_label"),
                 QStringLiteral("Alertas de segurança"));
    set_security_footer_summary_notification(
        interface_, QStringLiteral("%1: %2").arg(summary_label, detail));
}

void SecurityUiController::notification_preferences_changed() {
    clear_all_popup_aggregate_states();
    refresh_runtime_watch_state();
    retranslate_runtime_feedback();
}

void SecurityUiController::on_runtime_alert_row(const QVariantMap& row) {
    if (row.isEmpty()) {
        return;
    }

    const SecurityPopupConfig popup_cfg =
        get_security_popup_config(interface_, QStringLiteral("alerts_popup"), false);
    const bool wants_feedback =
        popup_cfg.enabled ||
        get_security_ui_flag(interface_, QStringLiteral("notifications_line"), false) ||
        get_security_ui_flag(interface_, QStringLiteral("notifications_status_bar"), false);

    if (!wants_feedback) {
        return;
    }

    notify_new_alerts(QVariantList{row});
}

void SecurityUiController::on_runtime_session_row(const QVariantMap& row) {
    if (row.isEmpty()) {
        return;
    }

    const QString session_id = row.value(QStringLiteral("session_id")).toString();
    const QString label = row.value(QStringLiteral("label")).toString();
    if (session_id.isEmpty()) {
        return;
    }

    const QString previous_label = seen_session_labels_.value(session_id);
    const bool already_seen = seen_session_labels_.contains(session_id);
    seen_session_labels_.insert(session_id, label);

    const SecurityPopupConfig popup_cfg =
        get_security_popup_config(interface_, QStringLiteral("sessions_popup"), false);
    const bool wants_feedback =
        popup_cfg.enabled ||
        get_security_ui_flag(interface_, QStringLiteral("notifications_line"), false) ||
        get_security_ui_flag(interface_, QStringLiteral("notifications_status_bar"), false);
    if (!wants_feedback) {
        return;
    }

    if (!already_seen || (previous_label != label && label != QStringLiteral("sessao_monitoramento"))) {
        notify_new_sessions(QVariantList{row});
    }
}

void SecurityUiController::retranslate_runtime_feedback() {
    render_last_runtime_notification();
    refresh_runtime_watch_state();
}

void SecurityUiController::poll_runtime_events() {
    auto* security_manager = get_security_manager(interface_);
    auto* loc = get_localizador(interface_);
    if (security_manager == nullptr || loc == nullptr) {
        return;
    }

    refresh_runtime_watch_state();
    const bool poll_alerts = should_poll_alerts();
    const bool poll_sessions = should_poll_sessions();
    const bool poll_quarantine = should_poll_quarantine();
    const bool poll_baseline = should_poll_baseline();
    if (!poll_alerts && !poll_sessions && !poll_quarantine && !poll_baseline) {
        return;
    }

    initialize_notification_state(poll_alerts, poll_sessions, poll_quarantine, poll_baseline);

    if (poll_alerts) {
        const QVariantList alerts = security_manager->list_alerts_since(last_alert_id_, 50);
        QVariantList new_alerts;
        for (const QVariant& item : alerts) {
            const QVariantMap row = item.toMap();
            const qint64 id = row.value(QStringLiteral("id")).toLongLong();
            if (id > last_alert_id_) {
                new_alerts.append(row);
                last_alert_id_ = qMax(last_alert_id_, id);
            }
        }
        if (!new_alerts.isEmpty()) {
            refresh_open_dialog("_security_alert_center_dialog", "refresh_data");
            refresh_open_dialog("_security_evidence_dialog", "refresh_data");
        }
    }

    if (poll_quarantine) {
        const QVariantList quarantine_items =
            security_manager->list_quarantine_items_since(last_quarantine_id_, 50);
        QVariantList new_quarantine;
        for (const QVariant& item : quarantine_items) {
            const QVariantMap row = item.toMap();
            const qint64 id = row.value(QStringLiteral("id")).toLongLong();
            if (id > last_quarantine_id_) {
                new_quarantine.append(row);
                last_quarantine_id_ = qMax(last_quarantine_id_, id);
            }
        }
        if (!new_quarantine.isEmpty()) {
            notify_new_quarantine_records(new_quarantine);
            refresh_open_dialog("_security_recovery_dialog", "refresh_data");
        }
    }

    if (poll_sessions) {
        const QVariantList sessions =
            security_manager->list_sessions_updated_after(last_session_updated_at_, 50);
        QVariantList new_sessions;
        for (const QVariant& item : sessions) {
            const QVariantMap row = item.toMap();
            const QString session_id = row.value(QStringLiteral("session_id")).toString();
            const QString label = row.value(QStringLiteral("label")).toString();
            last_session_updated_at_ =
                max_iso_timestamp(last_session_updated_at_, row.value(QStringLiteral("updated_at")).toString());
            const QString previous_label = seen_session_labels_.value(session_id);
            if (!seen_session_labels_.contains(session_id)) {
                new_sessions.append(row);
                seen_session_labels_.insert(session_id, label);
            } else if (previous_label != label && label != QStringLiteral("sessao_monitoramento")) {
                new_sessions.append(row);
                seen_session_labels_.insert(session_id, label);
            }
        }
        if (!sessions.isEmpty()) {
            refresh_open_dialog("_security_session_center_dialog", "refresh_data");
        }
    }

    if (poll_baseline) {
        const QVariantList baseline_updates =
            security_manager->list_baseline_entries_updated_after(last_baseline_updated_at_, 50);
        for (const QVariant& item : baseline_updates) {
            last_baseline_updated_at_ =
                max_iso_timestamp(last_baseline_updated_at_,
                                  item.toMap().value(QStringLiteral("updated_at")).toString());
        }
        if (!baseline_updates.isEmpty()) {
            refresh_open_dialog("_security_recovery_dialog", "refresh_data");
        }
    }

    refresh_runtime_watch_state();
}

void SecurityUiController::notify_new_alerts(const QVariantList& alerts) {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr || alerts.isEmpty()) {
        return;
    }

    remember_runtime_notification(RuntimeNotificationKind::Alerts, alerts);
    schedule_runtime_feedback_render();
    accumulate_popup_rows(RuntimeNotificationKind::Alerts, alerts);
}

void SecurityUiController::notify_new_sessions(const QVariantList& sessions) {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr || sessions.isEmpty()) {
        return;
    }

    remember_runtime_notification(RuntimeNotificationKind::Sessions, sessions);
    schedule_runtime_feedback_render();
    accumulate_popup_rows(RuntimeNotificationKind::Sessions, sessions);
}

void SecurityUiController::notify_new_quarantine_records(const QVariantList& quarantine_records) {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr || quarantine_records.isEmpty()) {
        return;
    }

    remember_runtime_notification(RuntimeNotificationKind::Quarantine, quarantine_records);
    schedule_runtime_feedback_render();
    accumulate_popup_rows(RuntimeNotificationKind::Quarantine, quarantine_records);
}

QString SecurityUiController::popup_config_key(RuntimeNotificationKind kind) const {
    switch (kind) {
        case RuntimeNotificationKind::Alerts:
            return QStringLiteral("alerts_popup");
        case RuntimeNotificationKind::Sessions:
            return QStringLiteral("sessions_popup");
        case RuntimeNotificationKind::Quarantine:
            return QStringLiteral("quarantine_popup");
        case RuntimeNotificationKind::None:
            break;
    }
    return QString();
}

SecurityPopupConfig SecurityUiController::popup_config(RuntimeNotificationKind kind) const {
    const QString key = popup_config_key(kind);
    if (key.isEmpty()) {
        return {};
    }

    return get_security_popup_config(interface_, key, false);
}

SecurityUiController::PopupAggregateState& SecurityUiController::popup_state(
    RuntimeNotificationKind kind) {
    switch (kind) {
        case RuntimeNotificationKind::Alerts:
            return alerts_popup_state_;
        case RuntimeNotificationKind::Sessions:
            return sessions_popup_state_;
        case RuntimeNotificationKind::Quarantine:
            return quarantine_popup_state_;
        case RuntimeNotificationKind::None:
            return alerts_popup_state_;
    }
    return alerts_popup_state_;
}

QTimer* SecurityUiController::popup_timer(RuntimeNotificationKind kind) const {
    switch (kind) {
        case RuntimeNotificationKind::Alerts:
            return alerts_popup_timer_;
        case RuntimeNotificationKind::Sessions:
            return sessions_popup_timer_;
        case RuntimeNotificationKind::Quarantine:
            return quarantine_popup_timer_;
        case RuntimeNotificationKind::None:
            break;
    }
    return nullptr;
}

void SecurityUiController::clear_popup_aggregate_state(RuntimeNotificationKind kind) {
    PopupAggregateState& state = popup_state(kind);
    state.pending_count = 0;
    state.sample_rows.clear();
    state.cooldown_until_ms = 0;
    state.high_priority = false;
    if (QTimer* timer = popup_timer(kind)) {
        timer->stop();
    }
}

void SecurityUiController::clear_all_popup_aggregate_states() {
    clear_popup_aggregate_state(RuntimeNotificationKind::Alerts);
    clear_popup_aggregate_state(RuntimeNotificationKind::Sessions);
    clear_popup_aggregate_state(RuntimeNotificationKind::Quarantine);
}

void SecurityUiController::accumulate_popup_rows(RuntimeNotificationKind kind,
                                                 const QVariantList& rows) {
    if (rows.isEmpty() || kind == RuntimeNotificationKind::None) {
        return;
    }

    const SecurityPopupConfig config = popup_config(kind);
    if (!config.enabled) {
        clear_popup_aggregate_state(kind);
        return;
    }

    PopupAggregateState& state = popup_state(kind);
    state.pending_count += rows.size();
    const int row_buffer_limit =
        qBound(50, qMax(config.threshold_count, config.max_lines * 8), 1000);
    append_sample_rows(state.sample_rows, rows, row_buffer_limit);
    if (kind == RuntimeNotificationKind::Alerts) {
        for (const QVariant& item : rows) {
            if (is_high_priority_alert_row(item.toMap())) {
                state.high_priority = true;
                break;
            }
        }
    }

    try_emit_popup(kind);
}

void SecurityUiController::try_emit_popup(RuntimeNotificationKind kind) {
    if (kind == RuntimeNotificationKind::None) {
        return;
    }

    PopupAggregateState& state = popup_state(kind);
    const SecurityPopupConfig config = popup_config(kind);
    if (!config.enabled) {
        clear_popup_aggregate_state(kind);
        return;
    }

    const int threshold = qMax(1, config.threshold_count);
    if (state.pending_count < threshold || state.sample_rows.isEmpty()) {
        return;
    }

    const qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
    if (state.cooldown_until_ms > now_ms) {
        if (QTimer* timer = popup_timer(kind)) {
            if (!timer->isActive()) {
                timer->start(static_cast<int>(state.cooldown_until_ms - now_ms));
            }
        }
        return;
    }

    const QVariantList popup_rows = state.sample_rows;
    const qint64 total_count = state.pending_count;
    const bool high_priority = state.high_priority;
    state.pending_count = 0;
    state.sample_rows.clear();
    state.high_priority = false;
    state.cooldown_until_ms =
        now_ms + static_cast<qint64>(qMax(0, config.cooldown_seconds) * 1000);
    if (QTimer* timer = popup_timer(kind)) {
        timer->stop();
    }

    show_popup_message(kind, popup_rows, total_count, high_priority);
}

void SecurityUiController::show_popup_message(RuntimeNotificationKind kind,
                                              const QVariantList& rows,
                                              qint64 total_count,
                                              bool high_priority) {
    auto* loc = get_localizador(interface_);
    if (loc == nullptr || rows.isEmpty()) {
        return;
    }

    const SecurityPopupConfig config = popup_config(kind);
    if (!config.enabled) {
        return;
    }

    const QString title = notification_title(kind);
    const QStringList lines = notification_lines(kind, rows);
    if (title.isEmpty() || lines.isEmpty()) {
        return;
    }

    const int max_summary_lines = qBound(1, config.max_lines, kSecurityPopupMaxLinesLimit);
    QString popup_content =
        get_text(loc, QStringLiteral("security_popup_batch_count"), QStringLiteral("Total acumulado: %1"))
            .arg(total_count);
    const QString summary_preview = summarize_lines(lines, max_summary_lines);
    if (!summary_preview.isEmpty()) {
        popup_content += QStringLiteral("\n\n") + summary_preview;
    }

    auto* dialog = new NotificationPopupDialog(qobject_cast<QWidget*>(interface_),
                                               title,
                                               popup_content,
                                               kind == RuntimeNotificationKind::Alerts && high_priority,
                                               loc);
    dialog->show();
}

void SecurityUiController::refresh_open_dialog(const char* property_name, const char* method_name) {
    Q_UNUSED(method_name)
    if (interface_ == nullptr || property_name == nullptr) {
        return;
    }
    if (QObject* dialog = interface_->property(property_name).value<QObject*>()) {
        auto* widget = qobject_cast<QWidget*>(dialog);
        if (widget == nullptr || !widget->isVisible()) {
            return;
        }
        if (auto* multi_dialog = dynamic_cast<MultiDirectoryDialog*>(dialog)) {
            QTimer::singleShot(0, multi_dialog, [multi_dialog]() { multi_dialog->refresh_data(); });
        } else if (auto* alert_dialog = dynamic_cast<AlertCenterDialog*>(dialog)) {
            QTimer::singleShot(0, alert_dialog, [alert_dialog]() { alert_dialog->refresh_data(); });
        } else if (auto* session_dialog = dynamic_cast<SessionCenterDialog*>(dialog)) {
            QTimer::singleShot(0, session_dialog, [session_dialog]() { session_dialog->refresh_data(); });
        } else if (auto* recovery_dialog = dynamic_cast<RecoveryDialog*>(dialog)) {
            QTimer::singleShot(0, recovery_dialog, [recovery_dialog]() { recovery_dialog->refresh_data(); });
        } else if (auto* evidence_dialog = dynamic_cast<EvidenceDialog*>(dialog)) {
            QTimer::singleShot(0, evidence_dialog, [evidence_dialog]() { evidence_dialog->refresh_data(); });
        }
    }
}

}  // namespace source::security::ui
