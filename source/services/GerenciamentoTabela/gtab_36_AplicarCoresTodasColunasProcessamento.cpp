#include "gtab_36_AplicarCoresTodasColunasProcessamento.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void aplicar_cores_todas_colunas_processamento(source::services::GerenciadorTabela* self, const QVariant& dados) {
    try {
        Q_UNUSED(dados)
        if (!self) return;

        // Evita pipeline síncrono pesado (resultado por célula) que bloqueia a GUI
        // em tabelas grandes. Usa aplicação incremental em lotes via QTimer.
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em aplicar_cores_todas_colunas_processamento", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
