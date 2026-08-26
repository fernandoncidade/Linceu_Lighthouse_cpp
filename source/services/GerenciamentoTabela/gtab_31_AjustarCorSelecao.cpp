#include "gtab_31_AjustarCorSelecao.h"

#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void ajustar_cor_selecao(source::services::GerenciadorTabela* self) {
    try {
        if (!self || !self->interface) return;
        auto* tabela = qobject_cast<QTableWidget*>(self->interface->property("tabela_dados").value<QObject*>());
        if (!tabela) return;
        tabela->viewport()->update();
        self->atualizacao_pendente = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em ajustar_cor_selecao", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
