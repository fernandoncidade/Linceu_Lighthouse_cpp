#pragma once

#include <functional>

#include <QString>

class QObject;
class QWidget;

namespace source::security {
class SecurityManager;
}

namespace source::ui {
class LocalizadorQt;
}

namespace source::security::ui {

constexpr int kSecurityPopupMaxLinesLimit = 1000;

struct SecurityPopupConfig {
    bool enabled = false;
    int threshold_count = 1;
    int max_lines = 4;
    int cooldown_seconds = 0;
};

QString get_text(QObject* loc, const QString& key, const QString& fallback = QString());
source::ui::LocalizadorQt* get_localizador(QObject* interface_obj);
source::security::SecurityManager* get_security_manager(QObject* interface_obj);
QObject* get_security_runtime_worker(QObject* interface_obj);
void run_on_security_runtime_thread(QObject* interface_obj, std::function<void()> task);
void register_dialog(QObject* interface_obj, const char* property_name, QWidget* dialog);
void set_result_label(QObject* interface_obj, const QString& text);
void set_security_result_label(QObject* interface_obj, const QString& text);
void set_security_result_label(QObject* interface_obj, const QString& key, const QString& fallback);
void show_translated_security_feedback(QObject* interface_obj,
                                       const QString& key,
                                       const QString& fallback = QString(),
                                       int timeout_ms = 10000);
void clear_security_result_label(QObject* interface_obj);
void retranslate_active_security_feedback(QObject* interface_obj);
void set_security_footer_summary_notification(QObject* interface_obj, const QString& text);
void clear_security_footer_summary_notification(QObject* interface_obj);
bool get_security_ui_flag(QObject* interface_obj, const QString& key, bool default_value = false);
void set_security_ui_flag(QObject* interface_obj, const QString& key, bool value);
SecurityPopupConfig get_security_popup_config(QObject* interface_obj,
                                              const QString& key,
                                              bool default_enabled = false,
                                              int default_threshold_count = 1,
                                              int default_max_lines = 4,
                                              int default_cooldown_seconds = 0);
void set_security_popup_config(QObject* interface_obj,
                               const QString& key,
                               const SecurityPopupConfig& config);
void clear_status_message(QObject* interface_obj);
void show_status_message(QObject* interface_obj, const QString& text, int timeout_ms = 8000);
void show_translated_status_message(QObject* interface_obj,
                                    const QString& key,
                                    const QString& fallback = QString(),
                                    int timeout_ms = 8000);

}  // namespace source::security::ui
