#pragma once

#include <QString>

namespace source::services {
class EventoModificado;
}

namespace source::services::GerenciamentoEventoModificado {

bool is_arquivo_codigo_grande(source::services::EventoModificado* self, const QString& caminho);

}  // namespace source::services::GerenciamentoEventoModificado
