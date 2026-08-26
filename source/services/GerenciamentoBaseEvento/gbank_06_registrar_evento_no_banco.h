#pragma once

#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void registrar_evento_no_banco(source::services::BaseEvento* self, const QVariantMap& evento);
}
