#include "gtab_35_IniciarProcessamentoPesado.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void iniciar_processamento_pesado(source::services::GerenciadorTabela* self, const QVariant& dados) {
    try {
        Q_UNUSED(dados)
        if (!self || !self->interface) return;
        QObject* tabela = self->interface->property("tabela_dados").value<QObject*>();
        if (!tabela) return;

        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(true);
        self->atualizar_dados_tabela(tabela);
        self->atualizar_linha_mais_recente(tabela, QVariantMap{});
        self->atualizar_visualizacao_tabela();
        self->atualizar_visibilidade_colunas(true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em iniciar_processamento_pesado", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
