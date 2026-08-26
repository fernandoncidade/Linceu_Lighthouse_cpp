#pragma once

#include <QVariantMap>

namespace source::security {

class ScoreRisco {
public:
    QVariantMap score(const QVariantMap& evento, const QVariantMap& contexto) const;
};

}  // namespace source::security
