#pragma once

namespace source::ui { class GerenciadorMenusUI; }
class QMenu;

namespace source::ui::GerenciamentoMenusUI {
void configurar_menu_opcoes(source::ui::GerenciadorMenusUI* self, QMenu* menu_opcoes = nullptr);
}
