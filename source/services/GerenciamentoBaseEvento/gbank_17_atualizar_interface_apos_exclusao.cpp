#include "gbank_17_atualizar_interface_apos_exclusao.h"

#include "source/services/GerenciamentoBaseEvento/gbank_07_atualizar_interface_apos_evento.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

void _atualizar_interface_apos_exclusao(source::services::BaseEvento* self, const QVariantMap& evento) {
    try {
        if (!self) return;
        self->eventos_excluidos += 1;
        _atualizar_interface_apos_evento(self, evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface após exclusão", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
