#include "gmet_26_GetTaxaBitsArquivo.h"

#include "gmet_38_GetMetadados.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_taxa_bits_arquivo(source::services::GerenciadorColunas* gerenciador, const QVariantMap& item) {
    try {
        const QVariantMap metadados = get_metadados_gc(gerenciador, item);
        return metadados.value("taxa_bits").toString();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter taxa de bits do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
