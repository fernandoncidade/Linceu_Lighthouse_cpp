#pragma once

#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void processar_exclusao(source::services::BaseEvento* self, const QVariantMap& evento);
}
