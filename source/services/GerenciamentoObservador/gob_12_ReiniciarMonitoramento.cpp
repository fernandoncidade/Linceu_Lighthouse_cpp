#include "gob_12_ReiniciarMonitoramento.h"

#include <thread>

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void reiniciar_monitoramento(source::services::Observador* self) {
    try {
        if (!self) return;
        self->parar();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        self->falhas_consecutivas = 0;
        self->iniciar();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao reiniciar monitoramento", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
