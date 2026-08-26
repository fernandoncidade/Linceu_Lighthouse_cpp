#pragma once

namespace source::ui { class GerenciadorMenusUI; }
class QMenu;

namespace source::ui::GerenciamentoMenusUI {
void criar_submenu_idiomas(source::ui::GerenciadorMenusUI* self, QMenu* menu_opcoes = nullptr);
}
