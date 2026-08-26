#include "gob_03_HandleScanError.h"

#include <QMetaObject>

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void handle_scan_error(source::services::Observador* self, const QString& error_msg) {
    source::utils::LogManager::get_logger().error(QStringLiteral("Erro durante escaneamento: ") + error_msg, true);
    if (!self) return;
    self->ativo = false;

    if (self->interface) {
        QObject* rotulo = self->interface->property("rotulo_resultado").value<QObject*>();
        if (rotulo) {
            QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, QStringLiteral("Erro: ") + error_msg));
        }
    }
}

}  // namespace source::services::GerenciamentoObservador
