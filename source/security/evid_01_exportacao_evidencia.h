#pragma once

#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace source::security {

class ExportacaoEvidencia {
public:
    explicit ExportacaoEvidencia(const QString& storage_root = QString());

    void set_storage_root(const QString& storage_root);
    void ensure_schema(const QString& db_path) const;
    QVariantMap register_event(const QVariantMap& evento, const QVariantMap& contexto) const;
    QString export_package(const QVariantList& eventos, const QVariantMap& contexto,
                           const QString& destination_dir) const;

private:
    QString storage_root_;
};

}  // namespace source::security
