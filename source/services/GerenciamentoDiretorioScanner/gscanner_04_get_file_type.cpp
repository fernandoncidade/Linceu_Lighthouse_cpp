#include "gscanner_04_get_file_type.h"

#include <QCoreApplication>
#include <QFileInfo>

#include "source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

QString get_file_type(source::services::DiretorioScanner* self, const QString& caminho) {
    try {
        if (caminho.isEmpty() || !QFileInfo::exists(caminho)) {
            return {};
        }

        QFileInfo fi(caminho);
        if (fi.isDir()) {
            return self && self->observador && self->observador->loc ? self->observador->loc->get_text("folder") : QStringLiteral("folder");
        }

        const QString ext = fi.suffix().toLower();
        if (!ext.isEmpty()) {
            return ext;
        }

        source::ui::LocalizadorQt* loc =
            self && self->observador ? self->observador->loc : nullptr;
        const QString tipo = source::services::GerenciamentoMetadados::identificar_tipo_arquivo(
            caminho, loc, fi.fileName());
        if (tipo.isEmpty() || tipo == QStringLiteral("file_unknown")) {
            return {};
        }
        return tipo;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em get_file_type", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
