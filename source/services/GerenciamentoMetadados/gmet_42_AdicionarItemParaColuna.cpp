#include "gmet_42_AdicionarItemParaColuna.h"

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void adicionar_item_para_coluna(source::services::GerenciadorColunas* gc, const QString& coluna_key, const QVariantMap& item) {
    try {
        if (gc && gc->filas_colunas.contains(coluna_key)) {
            gc->filas_colunas[coluna_key].enqueue(item);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar item para a coluna", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
