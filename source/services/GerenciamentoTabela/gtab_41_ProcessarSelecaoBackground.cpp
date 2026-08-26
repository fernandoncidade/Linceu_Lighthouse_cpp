#include "gtab_41_ProcessarSelecaoBackground.h"

#include <QTimer>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

bool processar_selecao_background(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return false;
        QTimer::singleShot(0, self, [self]() { self->ajustar_cor_selecao(); });
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em processar_selecao_background", true);
    }
    return false;
}
}  // namespace source::services::GerenciamentoTabela

