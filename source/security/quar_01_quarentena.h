#pragma once

#include <QString>
#include <QVariantMap>

namespace source::security {

class QuarentenaManager {
public:
    explicit QuarentenaManager(const QString& storage_root = QString());

    void set_storage_root(const QString& storage_root);
    void ensure_schema(const QString& db_path) const;
    QVariantMap process(const QVariantMap& evento, const QVariantMap& contexto) const;

private:
    QString storage_root_;
};

}  // namespace source::security
