#include "gtab_08_RedimensionarCabecalho.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void redimensionar_cabecalho(source::services::GerenciadorTabela* self, int logicalIndex, int oldSize, int newSize) {
    try {
        Q_UNUSED(oldSize)
        Q_UNUSED(newSize)
        if (!self) return;
        if (self->property("_suspender_quebra_cabecalho").toBool()) return;

        if (self->interface) {
            QObject* tabela = self->interface->property("tabela_dados").value<QObject*>();
            if (tabela) {
                self->aplicar_quebra_linha_cabecalho(tabela, logicalIndex);
                self->ajustar_altura_cabecalho(tabela);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em redimensionar_cabecalho", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

