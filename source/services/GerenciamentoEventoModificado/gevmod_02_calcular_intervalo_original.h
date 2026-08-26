#pragma once

#include <QString>

namespace source::services {
class EventoModificado;
}

namespace source::services::GerenciamentoEventoModificado {

double calcular_intervalo_original(source::services::EventoModificado* self, const QString& caminho_completo);

}  // namespace source::services::GerenciamentoEventoModificado
