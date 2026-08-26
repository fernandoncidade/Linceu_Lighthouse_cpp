#pragma once

#include <QString>
#include <QVariantMap>

namespace source::security {

class BaselineCriptografico {
public:
    explicit BaselineCriptografico(const QString& storage_root = QString());

    void set_storage_root(const QString& storage_root);
    void ensure_schema(const QString& db_path) const;
    QVariantMap enrich(const QVariantMap& evento, const QVariantMap& contexto) const;
    void clear_runtime_cache() const;

private:
    QString storage_root_;
};

}  // namespace source::security
