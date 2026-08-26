#pragma once

#include <QVariantMap>

namespace source::security {

class AtribuicaoForense {
public:
    QVariantMap attribute(const QVariantMap& evento, const QVariantMap& contexto) const;
};

}  // namespace source::security
