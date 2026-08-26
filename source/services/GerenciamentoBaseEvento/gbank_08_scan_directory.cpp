#include "gbank_08_scan_directory.h"

#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

void scan_directory(source::services::BaseEvento* self, const QString& directory) {
    try {
        if (!self || !self->observador) return;
        source::services::DiretorioScanner scanner(self->observador);
        scanner.scan_directory(directory);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao escanear diretório", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
