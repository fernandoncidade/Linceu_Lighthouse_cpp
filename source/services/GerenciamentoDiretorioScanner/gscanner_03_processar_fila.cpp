#include "gscanner_03_processar_fila.h"

#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

void _processar_fila(source::services::DiretorioScanner* self) {
    while (self) {
        try {
            if (self->fila_processamento.isEmpty()) break;
            const QVariantList item = self->fila_processamento.dequeue();
            const QString nome = item.value(0).toString();
            const QString caminho = item.value(1).toString();
            const QString tipo = item.value(2).toString();
            self->_processar_item(nome, caminho, tipo);
            self->contador_processados += 1;
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro no processamento da fila", true);
        }
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
