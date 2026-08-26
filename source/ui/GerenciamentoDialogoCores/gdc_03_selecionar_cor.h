#pragma once

#include <QString>

namespace source::ui {
class DialogoPaletaCores;
}

namespace source::ui::GerenciamentoDialogoCores {

void selecionar_cor(source::ui::DialogoPaletaCores* self, const QString& cor_hex);

}  // namespace source::ui::GerenciamentoDialogoCores
