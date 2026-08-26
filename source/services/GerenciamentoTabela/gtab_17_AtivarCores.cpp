#include "gtab_17_AtivarCores.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void ativar_cores(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        self->cores_visiveis = true;
        self->atualizar_cores_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em ativar_cores", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

