#include "gdc_05_accept.h"

#include <QDialog>

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

void aceitar(source::ui::DialogoPaletaCores* self) {
    try {
        if (self->cor_selecionada_.isValid()) {
            emit self->corSelecionada(self->cor_selecionada_);
        }
        self->QDialog::accept();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao confirmar seleção de cor", true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
