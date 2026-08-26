#include "gscanner_07_atualizar_interface.h"

#include <QMetaObject>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

void _atualizar_interface(source::services::DiretorioScanner* self, int progresso, int contador, int total) {
    try {
        if (!self || !self->observador || !self->observador->interface) return;

        QMetaObject::invokeMethod(self->observador->interface,
                                  "atualizar_progresso_scan",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, progresso),
                                  Q_ARG(int, contador),
                                  Q_ARG(int, total));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface", true);
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
