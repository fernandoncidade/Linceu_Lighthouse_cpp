#include "gscanner_09_scan_worker_run.h"

#include <exception>

#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

void scan_worker_run(source::services::ScanWorker* self) {
    try {
        if (!self || !self->scanner_obj()) return;
        self->scanner_obj()->scan_directory(self->directory());
        emit self->finished();
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro no scan do diretório %1: %2").arg(self->directory(), QString::fromUtf8(e.what())),
            true);
        emit self->error(QString::fromUtf8(e.what()));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no scan_worker_run", true);
        emit self->error(QStringLiteral("scan error"));
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
