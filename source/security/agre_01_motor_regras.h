#pragma once

#include <QVariantMap>

namespace source::security {

class MotorRegras {
public:
    void ensure_schema(const QString& db_path) const;
    QVariantMap evaluate(const QVariantMap& evento, const QVariantMap& contexto);
    void persist_if_needed(const QVariantMap& evento, const QVariantMap& contexto) const;
    void clear_runtime_cache() const;
};

}  // namespace source::security
