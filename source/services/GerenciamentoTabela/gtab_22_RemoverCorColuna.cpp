#include "gtab_22_RemoverCorColuna.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void remover_cor_coluna(source::services::GerenciadorTabela* self, const QString& coluna_key) {
    try {
        if (!self || coluna_key.isEmpty()) return;
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(false, QSet<QString>{coluna_key});
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em remover_cor_coluna", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
