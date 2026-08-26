#pragma once

#include <QString>

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {
void on_traducoes_carregadas(source::ui::GerenciadorMenusUI* self, const QString& idioma);
}
