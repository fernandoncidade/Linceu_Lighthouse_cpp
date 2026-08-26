#pragma once

#include <QVariantMap>

namespace source::security {

class DiffConteudo {
public:
    QVariantMap summarize(const QVariantMap& evento, const QVariantMap& contexto) const;
};

}  // namespace source::security
