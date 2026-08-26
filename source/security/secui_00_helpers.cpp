#include "secui_00_helpers.h"

#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QMetaObject>
#include <QPointer>
#include <QSettings>
#include <QStatusBar>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QWidget>

#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"

namespace source::security::ui {
namespace {

QString settings_file_path() {
    return QDir(source::utils::obter_caminho_persistente())
        .filePath(QStringLiteral("security/security_ui.ini"));
}

constexpr char kSecurityRuntimeWorkerProperty[] = "_security_runtime_worker";
constexpr char kSecurityRuntimeThreadProperty[] = "_security_runtime_thread";
constexpr char kSecurityFeedbackKeyProperty[] = "_security_feedback_key";
constexpr char kSecurityFeedbackFallbackProperty[] = "_security_feedback_fallback";
constexpr char kSecurityFeedbackExpiresAtProperty[] = "_security_feedback_expires_at_ms";
constexpr char kSecurityFeedbackGenerationProperty[] = "_security_feedback_generation";

QString property_name_for_key(const QString& key) {
    QString sanitized = key;
    sanitized.replace(QStringLiteral("/"), QStringLiteral("_"));
    sanitized.replace(QStringLiteral("."), QStringLiteral("_"));
    return QStringLiteral("_security_ui_flag_%1").arg(sanitized);
}

QString popup_property_name_for_key(const QString& key) {
    QString sanitized = key;
    sanitized.replace(QStringLiteral("/"), QStringLiteral("_"));
    sanitized.replace(QStringLiteral("."), QStringLiteral("_"));
    return QStringLiteral("_security_popup_cfg_%1").arg(sanitized);
}

int clamp_int(const QVariant& value, int default_value, int minimum, int maximum) {
    bool ok = false;
    int parsed = value.toInt(&ok);
    if (!ok) {
        parsed = default_value;
    }
    return qBound(minimum, parsed, maximum);
}

QVariantMap popup_config_to_map(const SecurityPopupConfig& config) {
    QVariantMap map;
    map.insert(QStringLiteral("enabled"), config.enabled);
    map.insert(QStringLiteral("threshold_count"), config.threshold_count);
    map.insert(QStringLiteral("max_lines"), config.max_lines);
    map.insert(QStringLiteral("cooldown_seconds"), config.cooldown_seconds);
    return map;
}

SecurityPopupConfig popup_config_from_map(const QVariantMap& map,
                                          bool default_enabled,
                                          int default_threshold_count,
                                          int default_max_lines,
                                          int default_cooldown_seconds) {
    SecurityPopupConfig config;
    config.enabled = map.value(QStringLiteral("enabled"), default_enabled).toBool();
    config.threshold_count = clamp_int(map.value(QStringLiteral("threshold_count")),
                                       default_threshold_count,
                                       1,
                                       1000000);
    config.max_lines =
        clamp_int(map.value(QStringLiteral("max_lines")),
                  default_max_lines,
                  1,
                  kSecurityPopupMaxLinesLimit);
    config.cooldown_seconds = clamp_int(map.value(QStringLiteral("cooldown_seconds")),
                                        default_cooldown_seconds,
                                        0,
                                        86400);
    return config;
}

QSettings security_settings() {
    QDir().mkpath(QFileInfo(settings_file_path()).absolutePath());
    return QSettings(settings_file_path(), QSettings::IniFormat);
}

void set_optional_label(QObject* label_obj, const QString& text) {
    if (label_obj == nullptr) {
        return;
    }
    if (auto* label = qobject_cast<QLabel*>(label_obj)) {
        label->setText(text);
        label->setVisible(!text.trimmed().isEmpty());
        label->adjustSize();
        label->updateGeometry();
    } else {
        QMetaObject::invokeMethod(label_obj, "setText", Q_ARG(QString, text));
        label_obj->setProperty("visible", !text.trimmed().isEmpty());
    }
}

bool is_qobject_pointer_variant(const QVariant& value) {
    const QMetaType meta_type = value.metaType();
    return meta_type.isValid() &&
           (meta_type.id() == QMetaType::QObjectStar ||
            meta_type.flags().testFlag(QMetaType::PointerToQObject));
}

QObject* qobject_from_variant(const QVariant& value) {
    if (!value.isValid() || value.isNull() || !is_qobject_pointer_variant(value)) {
        return nullptr;
    }
    return value.value<QObject*>();
}

void apply_security_result_label(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    if (QObject* rotulo_obj = interface_obj->property("rotulo_seguranca").value<QObject*>()) {
        set_optional_label(rotulo_obj, text);
    }
}

int next_security_feedback_generation(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return 0;
    }

    const int generation = interface_obj->property(kSecurityFeedbackGenerationProperty).toInt() + 1;
    interface_obj->setProperty(kSecurityFeedbackGenerationProperty, generation);
    return generation;
}

void reset_security_feedback_state(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    interface_obj->setProperty(kSecurityFeedbackKeyProperty, QString());
    interface_obj->setProperty(kSecurityFeedbackFallbackProperty, QString());
    interface_obj->setProperty(kSecurityFeedbackExpiresAtProperty, QVariant::fromValue<qint64>(0));
}

QObject* ensure_security_runtime_worker(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    auto* existing_worker =
        interface_obj->property(kSecurityRuntimeWorkerProperty).value<QObject*>();
    auto* existing_thread =
        qobject_cast<QThread*>(interface_obj->property(kSecurityRuntimeThreadProperty).value<QObject*>());
    if (existing_worker != nullptr && existing_thread != nullptr && existing_thread->isRunning()) {
        return existing_worker;
    }

    if (existing_thread != nullptr) {
        existing_thread->quit();
        existing_thread->wait();
        existing_thread->deleteLater();
    }

    auto* runtime_thread = new QThread();
    runtime_thread->setObjectName(QStringLiteral("SecurityRuntimeThread"));

    auto* runtime_worker = new QObject();
    runtime_worker->moveToThread(runtime_thread);

    QObject::connect(runtime_thread, &QThread::finished, runtime_worker, &QObject::deleteLater);
    QObject::connect(runtime_thread, &QObject::destroyed, interface_obj, [interface_guard = QPointer<QObject>(interface_obj)]() {
        if (interface_guard == nullptr) {
            return;
        }

        interface_guard->setProperty(kSecurityRuntimeThreadProperty, QVariant::fromValue(static_cast<QObject*>(nullptr)));
    });
    QObject::connect(runtime_worker, &QObject::destroyed, interface_obj, [interface_guard = QPointer<QObject>(interface_obj)]() {
        if (interface_guard == nullptr) {
            return;
        }

        interface_guard->setProperty(kSecurityRuntimeWorkerProperty, QVariant::fromValue(static_cast<QObject*>(nullptr)));
    });
    QObject::connect(interface_obj,
                     &QObject::destroyed,
                     runtime_thread,
                     [runtime_thread](QObject*) {
                         if (runtime_thread->isRunning()) {
                             runtime_thread->quit();
                             runtime_thread->wait();
                         }
                         runtime_thread->deleteLater();
                     },
                     Qt::DirectConnection);

    runtime_thread->start();
    interface_obj->setProperty(kSecurityRuntimeThreadProperty,
                               QVariant::fromValue(static_cast<QObject*>(runtime_thread)));
    interface_obj->setProperty(kSecurityRuntimeWorkerProperty,
                               QVariant::fromValue(static_cast<QObject*>(runtime_worker)));
    return runtime_worker;
}

}  // namespace

QString get_text(QObject* loc, const QString& key, const QString& fallback) {
    QString text = fallback;
    if (loc != nullptr) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty()) {
            text = translated;
        }
    }
    return text;
}

source::ui::LocalizadorQt* get_localizador(QObject* interface_obj) {
    return interface_obj != nullptr
               ? qobject_cast<source::ui::LocalizadorQt*>(interface_obj->property("loc").value<QObject*>())
               : nullptr;
}

source::security::SecurityManager* get_security_manager(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    if (auto* interface_monitor = qobject_cast<source::gui::InterfaceMonitor*>(interface_obj);
        interface_monitor != nullptr && interface_monitor->security_manager != nullptr) {
        return interface_monitor->security_manager;
    }

    return qobject_cast<source::security::SecurityManager*>(
        qobject_from_variant(interface_obj->property("security_manager")));
}

QObject* get_security_runtime_worker(QObject* interface_obj) {
    return ensure_security_runtime_worker(interface_obj);
}

void run_on_security_runtime_thread(QObject* interface_obj, std::function<void()> task) {
    if (!task) {
        return;
    }

    QObject* worker = ensure_security_runtime_worker(interface_obj);
    if (worker == nullptr) {
        task();
        return;
    }

    QMetaObject::invokeMethod(
        worker,
        [task = std::move(task)]() mutable {
            if (task) {
                task();
            }
        },
        Qt::QueuedConnection);
}

void register_dialog(QObject* interface_obj, const char* property_name, QWidget* dialog) {
    if (interface_obj == nullptr || property_name == nullptr || dialog == nullptr) {
        return;
    }

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setProperty("interface_principal", QVariant::fromValue(interface_obj));
    QObject::connect(dialog, &QObject::destroyed, interface_obj, [interface_obj, property_name](QObject* destroyed) {
        QObject* current = interface_obj->property(property_name).value<QObject*>();
        if (current == destroyed) {
            interface_obj->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(nullptr)));
        }
    });
    interface_obj->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(dialog)));
}

void set_result_label(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    if (QObject* rotulo_obj = interface_obj->property("rotulo_resultado").value<QObject*>()) {
        if (auto* label = qobject_cast<QLabel*>(rotulo_obj)) {
            label->setText(text);
        } else {
            QMetaObject::invokeMethod(rotulo_obj, "setText", Q_ARG(QString, text));
        }
    }
}

void set_security_result_label(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    next_security_feedback_generation(interface_obj);
    reset_security_feedback_state(interface_obj);
    apply_security_result_label(interface_obj, text);
}

void show_translated_security_feedback(QObject* interface_obj,
                                       const QString& key,
                                       const QString& fallback,
                                       int timeout_ms) {
    if (interface_obj == nullptr) {
        return;
    }

    const QString effective_key = key.trimmed();
    if (effective_key.isEmpty()) {
        set_security_result_label(interface_obj, fallback);
        return;
    }

    const int safe_timeout_ms = qMax(0, timeout_ms);
    const qint64 expires_at_ms = QDateTime::currentMSecsSinceEpoch() + safe_timeout_ms;
    const int generation = next_security_feedback_generation(interface_obj);
    interface_obj->setProperty(kSecurityFeedbackKeyProperty, effective_key);
    interface_obj->setProperty(kSecurityFeedbackFallbackProperty, fallback);
    interface_obj->setProperty(kSecurityFeedbackExpiresAtProperty, QVariant::fromValue(expires_at_ms));

    const QString translated = get_text(get_localizador(interface_obj), effective_key, fallback);
    apply_security_result_label(interface_obj, translated);

    if (safe_timeout_ms <= 0) {
        return;
    }

    QPointer<QObject> interface_guard(interface_obj);
    QTimer::singleShot(safe_timeout_ms, interface_obj, [interface_guard, generation]() {
        QObject* interface_viva = interface_guard.data();
        if (interface_viva == nullptr) {
            return;
        }

        if (interface_viva->property(kSecurityFeedbackGenerationProperty).toInt() != generation) {
            return;
        }

        const qint64 expires_at = interface_viva->property(kSecurityFeedbackExpiresAtProperty).toLongLong();
        if (expires_at > QDateTime::currentMSecsSinceEpoch()) {
            return;
        }

        clear_security_result_label(interface_viva);
    });
}

void clear_security_result_label(QObject* interface_obj) {
    set_security_result_label(interface_obj, QString());
}

void retranslate_active_security_feedback(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    const QString key = interface_obj->property(kSecurityFeedbackKeyProperty).toString().trimmed();
    if (key.isEmpty()) {
        return;
    }

    const qint64 expires_at_ms = interface_obj->property(kSecurityFeedbackExpiresAtProperty).toLongLong();
    const qint64 now_ms = QDateTime::currentMSecsSinceEpoch();
    if (expires_at_ms > 0 && expires_at_ms <= now_ms) {
        clear_security_result_label(interface_obj);
        return;
    }

    const QString fallback = interface_obj->property(kSecurityFeedbackFallbackProperty).toString();
    apply_security_result_label(interface_obj, get_text(get_localizador(interface_obj), key, fallback));
}

void set_security_footer_summary_notification(QObject* interface_obj, const QString& text) {
    if (interface_obj == nullptr) {
        return;
    }
    if (QObject* rotulo_obj = interface_obj->property("rotulo_rodape_resumo_seguranca").value<QObject*>()) {
        set_optional_label(rotulo_obj, text);
    }
}

void clear_security_footer_summary_notification(QObject* interface_obj) {
    set_security_footer_summary_notification(interface_obj, QString());
}

bool get_security_ui_flag(QObject* interface_obj, const QString& key, bool default_value) {
    const QString property_name = property_name_for_key(key);
    if (interface_obj != nullptr && interface_obj->property(property_name.toUtf8().constData()).isValid()) {
        return interface_obj->property(property_name.toUtf8().constData()).toBool();
    }

    QSettings settings = security_settings();
    const bool value = settings.value(QStringLiteral("notifications/%1").arg(key), default_value).toBool();
    if (interface_obj != nullptr) {
        interface_obj->setProperty(property_name.toUtf8().constData(), value);
    }
    return value;
}

void set_security_ui_flag(QObject* interface_obj, const QString& key, bool value) {
    QSettings settings = security_settings();
    settings.setValue(QStringLiteral("notifications/%1").arg(key), value);
    settings.sync();

    if (interface_obj != nullptr) {
        const QString property_name = property_name_for_key(key);
        interface_obj->setProperty(property_name.toUtf8().constData(), value);
    }
}

SecurityPopupConfig get_security_popup_config(QObject* interface_obj,
                                              const QString& key,
                                              bool default_enabled,
                                              int default_threshold_count,
                                              int default_max_lines,
                                              int default_cooldown_seconds) {
    const QString property_name = popup_property_name_for_key(key);
    if (interface_obj != nullptr && interface_obj->property(property_name.toUtf8().constData()).isValid()) {
        const QVariantMap cached_map =
            interface_obj->property(property_name.toUtf8().constData()).toMap();
        return popup_config_from_map(cached_map,
                                     default_enabled,
                                     default_threshold_count,
                                     default_max_lines,
                                     default_cooldown_seconds);
    }

    QSettings settings = security_settings();
    const QString prefix = QStringLiteral("notifications/%1/").arg(key);

    SecurityPopupConfig config;
    const bool has_enabled = settings.contains(prefix + QStringLiteral("enabled"));
    config.enabled = has_enabled
                         ? settings.value(prefix + QStringLiteral("enabled"), default_enabled).toBool()
                         : settings.value(QStringLiteral("notifications/%1").arg(key), default_enabled).toBool();
    config.threshold_count =
        clamp_int(settings.value(prefix + QStringLiteral("threshold_count")),
                  default_threshold_count,
                  1,
                  1000000);
    config.max_lines = clamp_int(settings.value(prefix + QStringLiteral("max_lines")),
                                 default_max_lines,
                                 1,
                                 kSecurityPopupMaxLinesLimit);
    config.cooldown_seconds =
        clamp_int(settings.value(prefix + QStringLiteral("cooldown_seconds")),
                  default_cooldown_seconds,
                  0,
                  86400);

    if (interface_obj != nullptr) {
        interface_obj->setProperty(property_name.toUtf8().constData(), popup_config_to_map(config));
        const QString flag_property = property_name_for_key(key);
        interface_obj->setProperty(flag_property.toUtf8().constData(), config.enabled);
    }
    return config;
}

void set_security_popup_config(QObject* interface_obj,
                               const QString& key,
                               const SecurityPopupConfig& config) {
    SecurityPopupConfig normalized = config;
    normalized.threshold_count = qBound(1, normalized.threshold_count, 1000000);
    normalized.max_lines = qBound(1, normalized.max_lines, kSecurityPopupMaxLinesLimit);
    normalized.cooldown_seconds = qBound(0, normalized.cooldown_seconds, 86400);

    QSettings settings = security_settings();
    const QString prefix = QStringLiteral("notifications/%1/").arg(key);
    settings.setValue(prefix + QStringLiteral("enabled"), normalized.enabled);
    settings.setValue(prefix + QStringLiteral("threshold_count"), normalized.threshold_count);
    settings.setValue(prefix + QStringLiteral("max_lines"), normalized.max_lines);
    settings.setValue(prefix + QStringLiteral("cooldown_seconds"), normalized.cooldown_seconds);
    settings.setValue(QStringLiteral("notifications/%1").arg(key), normalized.enabled);
    settings.sync();

    if (interface_obj != nullptr) {
        const QString property_name = popup_property_name_for_key(key);
        interface_obj->setProperty(property_name.toUtf8().constData(),
                                   popup_config_to_map(normalized));
        const QString flag_property = property_name_for_key(key);
        interface_obj->setProperty(flag_property.toUtf8().constData(), normalized.enabled);
    }
}

void clear_status_message(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }
    if (!interface_obj->property("_security_status_message_active").toBool()) {
        return;
    }
    if (QObject* status_obj = interface_obj->property("status_bar").value<QObject*>()) {
        if (auto* status = qobject_cast<QStatusBar*>(status_obj)) {
            status->clearMessage();
        }
    }
    interface_obj->setProperty("_security_status_message_active", false);
}

void show_status_message(QObject* interface_obj, const QString& text, int timeout_ms) {
    if (interface_obj == nullptr || text.isEmpty()) {
        return;
    }
    if (QObject* status_obj = interface_obj->property("status_bar").value<QObject*>()) {
        if (auto* status = qobject_cast<QStatusBar*>(status_obj)) {
            status->showMessage(text, timeout_ms);
            interface_obj->setProperty("_security_status_message_active", true);
        }
    }
}

}  // namespace source::security::ui
