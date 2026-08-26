#pragma once

#include <QStringList>

class QGridLayout;

namespace source::ui {
class DialogoPaletaCores;
}

namespace source::ui::GerenciamentoDialogoCores {

void adicionar_grid_cores(source::ui::DialogoPaletaCores* self, QGridLayout* grid_layout,
                          const QStringList& lista_cores, int colunas);

}  // namespace source::ui::GerenciamentoDialogoCores
