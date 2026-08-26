#include "gtab_39_AtualizarCoresNaInterface.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_cores_na_interface(source::services::GerenciadorTabela* self, const QVariantMap& resultado) {
    try {
        Q_UNUSED(resultado);
        if (!self) return;
        self->atualizar_cores_colunas(true);
        self->atualizacao_pendente = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_cores_na_interface", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
