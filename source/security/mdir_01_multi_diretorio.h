#pragma once

#include <functional>

#include <QVariantMap>
#include <QStringList>

class QObject;

namespace source::services {
class Observador;
}

namespace source::security {

class MultiDiretorioCoordinator {
public:
    static MultiDiretorioCoordinator& instance();

    void ensure_storage_schema() const;
    void ensure_profile_for_interface(QObject* interface_obj, const QString& primary_directory);
    bool set_profile_name(QObject* interface_obj, const QString& profile_name);
    bool add_directory(QObject* interface_obj, const QString& directory);
    bool remove_directory(QObject* interface_obj, const QString& directory);
    QStringList directories(QObject* interface_obj) const;
    QVariantMap profile_summary(QObject* interface_obj) const;
    QVariantMap runtime_scan_summary(QObject* interface_obj) const;

    source::services::Observador* configure_observers(
        QObject* interface_obj,
        const std::function<void(const QVariantMap&)>& callback);
    void stop_interface(QObject* interface_obj);
    void start_interface(QObject* interface_obj);
    void stop_scan_interface(QObject* interface_obj);
    void sync_interface_language(QObject* interface_obj, const QString& language) const;
    bool any_scan_running(QObject* interface_obj) const;
    bool any_active(QObject* interface_obj) const;
    source::services::Observador* primary_observer(QObject* interface_obj) const;

private:
    MultiDiretorioCoordinator() = default;
};

}  // namespace source::security
