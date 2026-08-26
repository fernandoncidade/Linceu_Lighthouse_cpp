#pragma once

#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void _atualizar_interface_apos_exclusao(source::services::BaseEvento* self, const QVariantMap& evento = QVariantMap());
}
