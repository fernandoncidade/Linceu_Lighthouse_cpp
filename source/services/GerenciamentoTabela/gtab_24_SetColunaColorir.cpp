#include "gtab_24_SetColunaColorir.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void set_coluna_colorir(source::services::GerenciadorTabela* self, const QString& coluna_key, bool checked) {
    try {
        if (!self || coluna_key.isEmpty()) return;
        const bool ja_ativo = self->colunas_para_colorir.contains(coluna_key);
        const bool precisa_religar_visual = checked && !self->cores_visiveis;
        if (checked == ja_ativo && !precisa_religar_visual) {
            return;
        }

        if (checked) {
            self->colunas_para_colorir.insert(coluna_key);
            self->cores_visiveis = true;
        } else {
            self->colunas_para_colorir.remove(coluna_key);
        }
        self->salvar_configuracoes_cores();
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(false, QSet<QString>{coluna_key});
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em set_coluna_colorir", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
