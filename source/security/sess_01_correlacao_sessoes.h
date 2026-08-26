#pragma once

#include <QVariantMap>

namespace source::security {

class CorrelacaoSessoes {
public:
    void ensure_schema(const QString& db_path) const;
    QVariantMap correlate(const QVariantMap& evento, const QVariantMap& contexto);
    void clear_runtime_cache() const;
};

}  // namespace source::security
