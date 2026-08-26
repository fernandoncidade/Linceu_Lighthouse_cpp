#include "gscanner_08_finalizar_scan.h"

#include <QMetaObject>
#include <QTimer>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

void _finalizar_scan(source::services::DiretorioScanner* self) {
    try {
        if (!self || !self->observador || !self->observador->interface) return;

        QObject* interface_obj = self->observador->interface;

        // Nao recarregar a tabela inteira ao final do scan: com dezenas de milhares
        // de linhas isso bloqueia a thread da GUI justamente na reta final.
        QMetaObject::invokeMethod(interface_obj, "atualizar_status", Qt::QueuedConnection);

        // Processa eventos pendentes da GUI apos liberar o worker.
        QTimer::singleShot(0, interface_obj, [interface_obj]() {
            if (interface_obj != nullptr) {
                QMetaObject::invokeMethod(interface_obj, "atualizar_status", Qt::QueuedConnection);
            }
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao finalizar scan", true);
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
