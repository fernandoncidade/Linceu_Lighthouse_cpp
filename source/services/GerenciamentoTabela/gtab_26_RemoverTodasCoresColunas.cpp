#include "gtab_26_RemoverTodasCoresColunas.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void remover_todas_cores_colunas(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        const QSet<QString> colunas_afetadas = self->colunas_para_colorir;
        if (colunas_afetadas.isEmpty()) {
            return;
        }

        self->colunas_para_colorir.clear();
        self->salvar_configuracoes_cores();
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true, colunas_afetadas);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em remover_todas_cores_colunas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
