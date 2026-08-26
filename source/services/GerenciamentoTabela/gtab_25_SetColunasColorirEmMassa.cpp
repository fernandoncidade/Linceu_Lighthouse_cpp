#include "gtab_25_SetColunasColorirEmMassa.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void set_colunas_colorir_em_massa(source::services::GerenciadorTabela* self, const QSet<QString>& novas_colunas) {
    try {
        if (!self) return;
        const bool visual_reativado = !novas_colunas.isEmpty() && !self->cores_visiveis;
        if (visual_reativado) {
            self->cores_visiveis = true;
        }

        const QSet<QString> colunas_anteriores = self->colunas_para_colorir;
        QSet<QString> colunas_afetadas = colunas_anteriores;
        colunas_afetadas.subtract(novas_colunas);
        QSet<QString> colunas_adicionadas = novas_colunas;
        colunas_adicionadas.subtract(colunas_anteriores);
        colunas_afetadas.unite(colunas_adicionadas);

        if (colunas_afetadas.isEmpty() && !visual_reativado) {
            return;
        }

        self->colunas_para_colorir = novas_colunas;
        self->salvar_configuracoes_cores();
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true, colunas_afetadas);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em set_colunas_colorir_em_massa", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
