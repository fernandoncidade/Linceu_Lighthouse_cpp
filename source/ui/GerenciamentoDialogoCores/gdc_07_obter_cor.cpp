#include "gdc_07_obter_cor.h"

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

QColor obter_cor(source::ui::DialogoPaletaCores* self) {
    try {
        return self->cor_selecionada_.isValid() ? self->cor_selecionada_ : self->cor_atual_;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter cor", true);
        return self->cor_atual_;
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
