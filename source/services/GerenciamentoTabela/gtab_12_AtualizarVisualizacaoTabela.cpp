#include "gtab_12_AtualizarVisualizacaoTabela.h"

#include <QTableView>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_visualizacao_tabela(source::services::GerenciadorTabela* self) {
    try {
        if (!self || !self->interface || !self->atualizacao_pendente) return;
        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_obj)) {
            tabela->viewport()->update();
            self->atualizacao_pendente = false;
            return;
        }
        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
            tabela_view->viewport()->update();
            self->atualizacao_pendente = false;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_visualizacao_tabela", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
