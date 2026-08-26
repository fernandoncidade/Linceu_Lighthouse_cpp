#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void notificar_evento(source::services::BaseEvento* self, const QString& tipo_operacao, const QString& nome, const QString& dir_anterior, const QString& dir_atual);
}
