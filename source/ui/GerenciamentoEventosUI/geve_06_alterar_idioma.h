#pragma once

#include <QString>

namespace source::ui {
class GerenciadorEventosUI;
}

namespace source::ui::GerenciamentoEventosUI {

void alterar_idioma(source::ui::GerenciadorEventosUI* self, const QString& idioma = QString());

}