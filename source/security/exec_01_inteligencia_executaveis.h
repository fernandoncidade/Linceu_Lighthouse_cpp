#pragma once

#include <QVariantMap>

namespace source::security {

class InteligenciaExecutaveis {
public:
    QVariantMap analyze(const QVariantMap& evento, const QVariantMap& contexto) const;
};

}  // namespace source::security
