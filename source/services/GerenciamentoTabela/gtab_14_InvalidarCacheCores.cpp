#include "gtab_14_InvalidarCacheCores.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void invalidar_cache_cores(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        self->cache_cores.clear();
        self->cache_indices_colunas.clear();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em invalidar_cache_cores", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

