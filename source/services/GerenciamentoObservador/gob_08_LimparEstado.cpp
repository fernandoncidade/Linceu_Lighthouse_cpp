#include "gob_08_LimparEstado.h"

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void limpar_estado(source::services::Observador* self) {
    try {
        if (!self) return;
        self->eventos_pendentes.clear();
        self->registros_anteriores.clear();
        self->renomeacoes_pendentes.clear();
        self->eventos_ignorados.clear();
        self->arquivos_recem_adicionados.clear();
        self->arquivos_recem_excluidos.clear();
        self->arquivos_recem_renomeados.clear();
        self->arquivos_recem_modificados.clear();
        self->ultima_modificacao.clear();
        self->buffer_eventos.clear();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar estado", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
