#pragma once

#include <QString>

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {
void confirmar_alteracao_idioma(source::ui::GerenciadorMenusUI* self, const QString& codigo_idioma);
}
