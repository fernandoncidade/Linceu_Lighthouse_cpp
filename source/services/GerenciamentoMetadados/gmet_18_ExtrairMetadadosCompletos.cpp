#include "gmet_18_ExtrairMetadadosCompletos.h"

#include "gmet_38_GetMetadados.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_completos(const QVariantMap& item, source::ui::LocalizadorQt* loc, source::services::GerenciadorColunas* contexto) {
    try {
        Q_UNUSED(loc)
        if (!contexto) return {};
        return get_metadados_gc(contexto, item);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados completos", true);
        return {};
    }
}

QVariantMap get_metadados(source::services::GerenciadorColunas* self, const QVariantMap& item) {
    return get_metadados_gc(self, item);
}

}  // namespace source::services::GerenciamentoMetadados
