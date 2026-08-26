#include "gob_01_DetectarOperacaoMassiva.h"

#include <chrono>

#include "source/services/ob_01_Observador.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

bool detectar_operacao_massiva(source::services::Observador* self) {
    try {
        if (!self) return false;
        const double tempo_atual = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();

        if ((tempo_atual - self->ultimo_reset_contador) >= 1.0) {
            if (self->contador_eventos_segundo > 100) {
                self->operacao_massiva_detectada = true;
            } else if (self->contador_eventos_segundo < 10) {
                self->operacao_massiva_detectada = false;
            }
            self->contador_eventos_segundo = 0;
            self->ultimo_reset_contador = tempo_atual;
        }

        self->contador_eventos_segundo += 1;
        return self->operacao_massiva_detectada;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao detectar operação massiva", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoObservador
