#include "gbank_01_set_callback.h"

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

void set_callback(source::services::BaseEvento* self, const QVariant& callback) {
    try {
        if (!self) return;
        self->callback = callback;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao definir callback", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
