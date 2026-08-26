#include "gmet_25_GetDuracaoArquivo.h"

#include "gmet_38_GetMetadados.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_duracao_arquivo(source::services::GerenciadorColunas* gerenciador, const QVariantMap& item) {
    try {
        const QVariantMap metadados = get_metadados_gc(gerenciador, item);
        return metadados.value("duracao").toString();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter duração do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
