#include "gmet_40_AdicionarMetadadosAoItem.h"

#include "gmet_38_GetMetadados.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap adicionar_metadados_ao_item(source::services::GerenciadorColunas* gc, QVariantMap item) {
    try {
        const QVariantMap metadados = get_metadados_gc(gc, item);
        for (auto it = metadados.constBegin(); it != metadados.constEnd(); ++it) {
            if (!item.contains(it.key()) || item.value(it.key()).toString().isEmpty()) {
                item[it.key()] = it.value();
            }
        }
        return item;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar metadados ao item", true);
        return item;
    }
}

}  // namespace source::services::GerenciamentoMetadados
