#include "gtab_18_OcultarCores.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void ocultar_cores(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        self->cores_visiveis = false;
        self->atualizar_cores_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em ocultar_cores", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

