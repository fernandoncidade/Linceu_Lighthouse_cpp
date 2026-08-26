#include "geui_29_abrir_diretorio.h"

#include <QDesktopServices>
#include <QUrl>

#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void abrir_diretorio(source::ui::GerenciadorEstatisticasUI*, const QString& caminho) {
    try {
        QDesktopServices::openUrl(QUrl::fromLocalFile(caminho));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao abrir diretório", true);
    }
}

}
