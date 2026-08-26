#pragma once

#include <QString>

namespace source::ui {
class GerenciadorEventosUI;
}

namespace source::ui::GerenciamentoEventosUI {

void alternar_visibilidade_coluna(source::ui::GerenciadorEventosUI* self, const QString& coluna = QString(), bool visivel = false);

}