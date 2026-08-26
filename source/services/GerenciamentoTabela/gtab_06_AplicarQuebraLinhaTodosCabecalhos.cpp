#include "gtab_06_AplicarQuebraLinhaTodosCabecalhos.h"

#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void aplicar_quebra_linha_todos_cabecalhos(source::services::GerenciadorTabela* self, QObject* tabela) {
    try {
        if (!self) return;
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        if (!tw) return;

        for (int i = 0; i < tw->columnCount(); ++i) {
            self->aplicar_quebra_linha_cabecalho(tw, i);
        }
        self->ajustar_altura_cabecalho(tw);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em aplicar_quebra_linha_todos_cabecalhos", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

