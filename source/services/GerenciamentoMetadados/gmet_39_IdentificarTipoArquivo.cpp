#include "gmet_39_IdentificarTipoArquivo.h"

#include "gmet_19_GetTipoArquivo.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString identificar_tipo_arquivo_gc(source::services::GerenciadorColunas* gc, const QString& caminho) {
    try {
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(gc ? gc->loc : nullptr);
        return identificar_tipo_arquivo(caminho, loc);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao identificar tipo de arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
