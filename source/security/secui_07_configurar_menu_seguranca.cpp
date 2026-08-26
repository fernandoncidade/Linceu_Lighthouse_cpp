#include "secui_07_configurar_menu_seguranca.h"

#include <QAction>
#include <QAbstractButton>
#include <QDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMetaObject>
#include <QPointer>
#include <QVariant>

#include "source/security/secui_00_helpers.h"
#include "source/security/secui_01_SecurityUiController.h"
#include "source/security/secui_08_NotificationPopupConfigDialog.h"
#include "source/ui/GerenciamentoMenusUI/gmui_01_MenuPersistente.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::security::ui {
namespace {

QString gt(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString translated = fallback;
    if (loc != nullptr) {
        QString value;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, value), Q_ARG(QString, key)) &&
            !value.isEmpty()) {
            translated = value;
        }
    }
    return translated;
}

SecurityUiController* controller_for(QObject* interface_obj) {
    return interface_obj != nullptr
               ? dynamic_cast<SecurityUiController*>(
                     interface_obj->property("security_ui_controller").value<QObject*>())
               : nullptr;
}

QMenu* ensure_submenu(QObject* interface_obj,
                      QMenu* menu,
                      const char* property_name,
                      const QString& title) {
    if (interface_obj == nullptr || menu == nullptr || property_name == nullptr) {
        return nullptr;
    }

    auto* submenu = dynamic_cast<source::ui::GerenciamentoMenusUI::MenuPersistente*>(
        interface_obj->property(property_name).value<QObject*>());
    if (submenu == nullptr) {
        submenu =
            new source::ui::GerenciamentoMenusUI::MenuPersistente(title, qobject_cast<QWidget*>(interface_obj));
        interface_obj->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(submenu)));
    }
    submenu->setTitle(title);
    if (!menu->actions().contains(submenu->menuAction())) {
        menu->addMenu(submenu);
    }
    return submenu;
}

QAction* ensure_action(QObject* interface_obj,
                       QMenu* menu,
                       const char* property_name,
                       const QString& text) {
    if (interface_obj == nullptr || menu == nullptr || property_name == nullptr) {
        return nullptr;
    }

    auto* action = qobject_cast<QAction*>(interface_obj->property(property_name).value<QObject*>());
    if (action == nullptr) {
        action = new QAction(text, interface_obj);
        interface_obj->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(action)));
    } else {
        action->setText(text);
    }
    if (!menu->actions().contains(action)) {
        menu->addAction(action);
    }
    return action;
}

void show_footer_notifications_notice(QObject* interface_obj, QObject* loc) {
    if (interface_obj == nullptr) {
        return;
    }

    QMessageBox message_box(qobject_cast<QWidget*>(interface_obj));
    message_box.setIcon(QMessageBox::Information);
    message_box.setWindowTitle(
        gt(loc,
           QStringLiteral("security_footer_notifications_notice_title"),
           QStringLiteral("Notificações de segurança")));
    message_box.setText(
        gt(loc,
           QStringLiteral("security_footer_notifications_notice_text"),
           QStringLiteral("As notificações de segurança serão reportadas no rodapé da GUI.")));
    message_box.setStandardButtons(QMessageBox::Ok);
    if (QAbstractButton* ok_button = message_box.button(QMessageBox::Ok)) {
        ok_button->setText(gt(loc, QStringLiteral("ok"), QStringLiteral("OK")));
    }
    message_box.exec();
}

}  // namespace

void criar_submenu_seguranca(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (self == nullptr || menu_configuracoes == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        QObject* interface_obj = self->interface_obj();
        QObject* loc = self->loc_obj();

        auto* submenu =
            dynamic_cast<source::ui::GerenciamentoMenusUI::MenuPersistente*>(
                interface_obj->property("security_menu_configuracoes").value<QObject*>());
        if (submenu == nullptr) {
            submenu = new source::ui::GerenciamentoMenusUI::MenuPersistente(
                gt(loc, QStringLiteral("security_menu"), QStringLiteral("Segurança")),
                qobject_cast<QWidget*>(interface_obj));
            interface_obj->setProperty("security_menu_configuracoes",
                                       QVariant::fromValue(static_cast<QObject*>(submenu)));
        }
        if (!menu_configuracoes->actions().contains(submenu->menuAction())) {
            menu_configuracoes->addMenu(submenu);
        }
        submenu->setTitle(gt(loc, QStringLiteral("security_menu"), QStringLiteral("Segurança")));
        submenu->clear();

        QAction* action_multi =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_multi_directory",
                          gt(loc, QStringLiteral("security_manage_directories"),
                             QStringLiteral("Perfis e multi-diretório")));
        QAction* action_alerts =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_alert_center",
                          gt(loc, QStringLiteral("security_alert_center"),
                             QStringLiteral("Central de alertas")));
        QAction* action_sessions =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_session_center",
                          gt(loc, QStringLiteral("security_session_center"),
                             QStringLiteral("Sessões em tempo real")));
        QAction* action_recovery =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_recovery_center",
                          gt(loc, QStringLiteral("security_recovery_center"),
                             QStringLiteral("Quarentena e restauração")));
        QAction* action_evidence =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_evidence_center",
                          gt(loc, QStringLiteral("security_evidence_center"),
                             QStringLiteral("Pacote de evidência")));
        submenu->addSeparator();
        QMenu* notifications_menu = ensure_submenu(
            interface_obj,
            submenu,
            "security_notifications_menu",
            gt(loc, QStringLiteral("security_notifications_menu"), QStringLiteral("Notificações")));
        if (notifications_menu != nullptr) {
            notifications_menu->clear();
        }

        QAction* action_alerts_popup =
            ensure_action(interface_obj,
                          notifications_menu,
                          "security_action_alert_popup_notifications",
                          gt(loc, QStringLiteral("security_alert_popup_notifications"),
                             QStringLiteral("Pop-ups de alertas")));
        QAction* action_sessions_popup =
            ensure_action(interface_obj,
                          notifications_menu,
                          "security_action_session_popup_notifications",
                          gt(loc, QStringLiteral("security_session_popup_notifications"),
                             QStringLiteral("Pop-ups de sessões")));
        QAction* action_quarantine_popup =
            ensure_action(interface_obj,
                          notifications_menu,
                          "security_action_quarantine_popup_notifications",
                          gt(loc, QStringLiteral("security_quarantine_popup_notifications"),
                             QStringLiteral("Pop-ups de quarentena")));
        QAction* action_summary_notifications =
            ensure_action(interface_obj,
                          notifications_menu,
                          "security_action_summary_notifications",
                          gt(loc, QStringLiteral("security_summary_notifications"),
                             QStringLiteral("Alertas de segurança")));

        const auto configure_popup_action = [interface_obj](QAction* action,
                                                            const QString& key,
                                                            bool default_enabled,
                                                            const QString& popup_label_key,
                                                            const QString& popup_label_fallback,
                                                            const QString& unit_key,
                                                            const QString& unit_fallback) {
            if (action == nullptr) {
                return;
            }

            const SecurityPopupConfig config =
                get_security_popup_config(interface_obj, key, default_enabled);
            action->setCheckable(true);
            action->setChecked(config.enabled);
            if (!action->property("_security_connected_popup_config").toBool()) {
                QObject::connect(
                    action, &QAction::triggered, interface_obj,
                    [interface_guard = QPointer<QObject>(interface_obj),
                     action_guard = QPointer<QAction>(action),
                     key,
                     default_enabled,
                     popup_label_key,
                     popup_label_fallback,
                     unit_key,
                     unit_fallback](bool) {
                        QObject* interface_viva = interface_guard.data();
                        QAction* action_viva = action_guard.data();
                        if (interface_viva == nullptr || action_viva == nullptr) {
                            return;
                        }

                        const SecurityPopupConfig current =
                            get_security_popup_config(interface_viva, key, default_enabled);
                        action_viva->blockSignals(true);
                        action_viva->setChecked(current.enabled);
                        action_viva->blockSignals(false);

                        NotificationPopupConfigDialog dialog(
                            qobject_cast<QWidget*>(interface_viva),
                            interface_viva,
                            key,
                            popup_label_key,
                            popup_label_fallback,
                            unit_key,
                            unit_fallback,
                            default_enabled);
                        if (dialog.exec() != QDialog::Accepted) {
                            return;
                        }

                        const SecurityPopupConfig updated = dialog.config();
                        set_security_popup_config(interface_viva, key, updated);

                        action_viva->blockSignals(true);
                        action_viva->setChecked(updated.enabled);
                        action_viva->blockSignals(false);

                        if (auto* controller = controller_for(interface_viva)) {
                            controller->notification_preferences_changed();
                        }
                    });
                action->setProperty("_security_connected_popup_config", true);
            }
        };

        configure_popup_action(action_alerts_popup,
                               QStringLiteral("alerts_popup"),
                               false,
                               QStringLiteral("security_alert_popup_notifications"),
                               QStringLiteral("Pop-ups de alertas"),
                               QStringLiteral("security_popup_unit_alerts"),
                               QStringLiteral("alertas"));
        configure_popup_action(action_sessions_popup,
                               QStringLiteral("sessions_popup"),
                               false,
                               QStringLiteral("security_session_popup_notifications"),
                               QStringLiteral("Pop-ups de sessões"),
                               QStringLiteral("security_popup_unit_sessions"),
                               QStringLiteral("sessões"));
        configure_popup_action(action_quarantine_popup,
                               QStringLiteral("quarantine_popup"),
                               false,
                               QStringLiteral("security_quarantine_popup_notifications"),
                               QStringLiteral("Pop-ups de quarentena"),
                               QStringLiteral("security_popup_unit_quarantine"),
                               QStringLiteral("itens"));
        if (action_summary_notifications != nullptr) {
            const bool summary_enabled =
                get_security_ui_flag(interface_obj, QStringLiteral("notifications_line"), false) ||
                get_security_ui_flag(interface_obj, QStringLiteral("notifications_status_bar"), false);
            action_summary_notifications->setCheckable(true);
            action_summary_notifications->setChecked(summary_enabled);
            if (!action_summary_notifications->property("_security_connected_summary_toggle").toBool()) {
                QObject::connect(
                    action_summary_notifications,
                    &QAction::toggled,
                    interface_obj,
                    [interface_guard = QPointer<QObject>(interface_obj),
                     loc_guard = QPointer<QObject>(loc)](bool checked) {
                        QObject* interface_viva = interface_guard.data();
                        if (interface_viva == nullptr) {
                            return;
                        }

                        set_security_ui_flag(interface_viva, QStringLiteral("notifications_line"), checked);
                        set_security_ui_flag(interface_viva,
                                             QStringLiteral("notifications_status_bar"),
                                             checked);

                        if (!checked) {
                            clear_security_footer_summary_notification(interface_viva);
                        } else {
                            show_footer_notifications_notice(interface_viva, loc_guard.data());
                        }

                        if (auto* controller = controller_for(interface_viva)) {
                            controller->notification_preferences_changed();
                        }
                    });
                action_summary_notifications->setProperty("_security_connected_summary_toggle", true);
            }
        }

        submenu->addSeparator();
        QAction* action_clear_db =
            ensure_action(interface_obj,
                          submenu,
                          "security_action_clear_database",
                          gt(loc, QStringLiteral("security_clear_database"),
                             QStringLiteral("Limpar banco de segurança")));

        if (action_multi != nullptr && !action_multi->property("_security_connected").toBool()) {
            QObject::connect(action_multi, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->open_multi_directory_dialog();
                }
            });
            action_multi->setProperty("_security_connected", true);
        }
        if (action_alerts != nullptr && !action_alerts->property("_security_connected").toBool()) {
            QObject::connect(action_alerts, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->open_alert_center_dialog();
                }
            });
            action_alerts->setProperty("_security_connected", true);
        }
        if (action_sessions != nullptr && !action_sessions->property("_security_connected").toBool()) {
            QObject::connect(action_sessions, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->open_session_center_dialog();
                }
            });
            action_sessions->setProperty("_security_connected", true);
        }
        if (action_recovery != nullptr && !action_recovery->property("_security_connected").toBool()) {
            QObject::connect(action_recovery, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->open_recovery_dialog();
                }
            });
            action_recovery->setProperty("_security_connected", true);
        }
        if (action_evidence != nullptr && !action_evidence->property("_security_connected").toBool()) {
            QObject::connect(action_evidence, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->open_evidence_dialog();
                }
            });
            action_evidence->setProperty("_security_connected", true);
        }
        if (action_clear_db != nullptr && !action_clear_db->property("_security_connected").toBool()) {
            QObject::connect(action_clear_db, &QAction::triggered, interface_obj, [interface_obj]() {
                if (auto* controller = controller_for(interface_obj)) {
                    controller->clear_security_database();
                }
            });
            action_clear_db->setProperty("_security_connected", true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu de segurança", true);
    }
}

}  // namespace source::security::ui
