#pragma once

#include <QString>

namespace source::ui {
class GerenciadorEventosUI;
}

namespace source::ui::GerenciamentoEventosUI {

bool confirmar_mudanca_idioma(source::ui::GerenciadorEventosUI* self, const QString& idioma = QString());

}