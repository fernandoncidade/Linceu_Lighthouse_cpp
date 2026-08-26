#include "gmet_43_AtualizarColunaInterface.h"

#include <QMutexLocker>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void atualizar_coluna_interface(source::services::GerenciadorColunas* gc, const QString& coluna_key) {
    try {
        if (!gc) return;
        QMutexLocker locker(&gc->lock_resultados);
        if (!coluna_key.isEmpty() && gc->resultados_colunas.contains(coluna_key)) {
            gc->resultados_colunas[coluna_key] = QVariantList();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar coluna da interface", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
