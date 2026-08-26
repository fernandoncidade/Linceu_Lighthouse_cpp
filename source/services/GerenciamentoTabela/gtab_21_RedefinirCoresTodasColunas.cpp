#include "gtab_21_RedefinirCoresTodasColunas.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void redefinir_cores_todas_colunas(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em redefinir_cores_todas_colunas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
