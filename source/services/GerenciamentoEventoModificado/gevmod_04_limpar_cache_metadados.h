#pragma once

#include <QString>

namespace source::services {
class EventoModificado;
}

namespace source::services::GerenciamentoEventoModificado {

void limpar_cache_metadados(source::services::EventoModificado* self, const QString& caminho_completo, const QString& ext);

}  // namespace source::services::GerenciamentoEventoModificado
