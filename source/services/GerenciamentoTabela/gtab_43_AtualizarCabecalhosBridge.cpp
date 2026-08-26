#include "gtab_43_AtualizarCabecalhosBridge.h"

#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_cabecalhos_bridge(source::services::GerenciadorTabela* self) {
    try {
        if (!self || !self->interface) return;
        auto* tabela = qobject_cast<QTableWidget*>(self->interface->property("tabela_dados").value<QObject*>());
        QObject* gc = self->interface->property("gerenciador_colunas").value<QObject*>();
        if (!tabela || !gc) {
            source::utils::LogManager::get_logger().warning("Interface ou atributos necessários não estão prontos para atualizar cabeçalhos.");
            return;
        }

        self->atualizar_cabecalhos();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_cabecalhos_bridge", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

