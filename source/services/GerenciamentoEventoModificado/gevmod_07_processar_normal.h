#pragma once

#include <QString>

namespace source::services {
class EventoModificado;
}

namespace source::services::GerenciamentoEventoModificado {

bool processar_normal(source::services::EventoModificado* self, const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);

}  // namespace source::services::GerenciamentoEventoModificado
