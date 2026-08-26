#include "gob_05_ProcessarBufferEventos.h"

#include <thread>

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {
namespace {
constexpr int kMaxEventosPorCiclo = 512;
}

void processar_buffer_eventos(source::services::Observador* self) {
    while (self && self->ativo) {
        try {
            if (!self->pause_event_set) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            std::deque<QVariantMap> eventos_para_processar;
            {
                std::lock_guard<std::mutex> lock(self->evento_lock);
                while (!self->buffer_eventos.empty() &&
                       static_cast<int>(eventos_para_processar.size()) < kMaxEventosPorCiclo) {
                    eventos_para_processar.push_back(self->buffer_eventos.front());
                    self->buffer_eventos.pop_front();
                }
            }

            if (eventos_para_processar.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            for (auto it = eventos_para_processar.cbegin(); it != eventos_para_processar.cend(); ++it) {
                if (!self->pause_event_set || self->pausado || !self->ativo) {
                    std::lock_guard<std::mutex> lock(self->evento_lock);
                    for (auto rit = eventos_para_processar.crbegin();
                         rit != std::deque<QVariantMap>::const_reverse_iterator(it);
                         ++rit) {
                        self->buffer_eventos.push_front(*rit);
                    }
                    break;
                }
                try {
                    self->_processar_evento_interno(*it);
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao processar evento do buffer", true);
                }
            }
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro crítico ao processar buffer de eventos", true);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

}  // namespace source::services::GerenciamentoObservador
