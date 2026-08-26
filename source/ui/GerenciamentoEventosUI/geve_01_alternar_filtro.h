#pragma once

#include <QString>

namespace source::ui {
class GerenciadorEventosUI;
}

namespace source::ui::GerenciamentoEventosUI {

void alternar_filtro(source::ui::GerenciadorEventosUI* self, const QString& filtro = QString());

}