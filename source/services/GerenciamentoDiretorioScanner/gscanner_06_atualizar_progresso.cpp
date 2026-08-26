#include "gscanner_06_atualizar_progresso.h"

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoDiretorioScanner {

void _atualizar_progresso(source::services::DiretorioScanner* self) {
    try {
        if (!self) return;
        if (self->observador && self->observador->interface) {
            const double progresso = self->total_arquivos > 0 ? (static_cast<double>(self->contador_processados) / static_cast<double>(self->total_arquivos)) * 100.0 : 0.0;
            if (qAbs(progresso - self->ultimo_progresso) >= 1.0) {
                self->ultimo_progresso = progresso;
                emit self->progresso_atualizado(static_cast<int>(progresso), self->contador_processados, self->total_arquivos);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar progresso", true);
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
