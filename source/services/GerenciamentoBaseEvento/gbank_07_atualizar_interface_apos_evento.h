#pragma once

#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void _atualizar_interface_apos_evento(source::services::BaseEvento* self, const QVariantMap& evento);
}
