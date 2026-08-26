#include "gob_10_ProcessarEventoInterno.h"

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void _processar_evento_interno(source::services::Observador* self, const QVariantMap& evento) {
    try {
        if (!self) return;
        if (self->pausado || !self->pause_event_set) {
            std::lock_guard<std::mutex> lock(self->evento_lock);
            self->buffer_eventos.push_front(evento);
            return;
        }

        self->total_eventos_processados += 1;
        self->processar_evento(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar evento interno", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
