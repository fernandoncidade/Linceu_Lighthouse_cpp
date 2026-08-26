#include "gob_04_IniciarMonitoramento.h"

#include <QMetaObject>

#include "source/services/ob_01_Observador.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void iniciar_monitoramento(source::services::Observador* self) {
    try {
        if (!self) return;

        self->thread = std::make_unique<std::thread>([self]() { self->monitorar(); });
        self->thread_buffer = std::make_unique<std::thread>([self]() { self->processar_buffer_eventos(); });

        if (self->interface) {
            // A tabela ja recebe os eventos do scan em streaming.
            // Evite reler tudo do banco aqui para nao travar a GUI na transicao 99%->100%.
            QMetaObject::invokeMethod(self->interface, "atualizar_status", Qt::QueuedConnection);

            QObject* rotulo = self->interface->property("rotulo_resultado").value<QObject*>();
            if (rotulo && self->loc) {
                QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, self->loc->get_text("monitoring_started")));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao iniciar monitoramento", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
