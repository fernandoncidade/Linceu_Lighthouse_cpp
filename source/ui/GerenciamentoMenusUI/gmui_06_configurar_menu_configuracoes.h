#pragma once

namespace source::ui { class GerenciadorMenusUI; }
class QMenu;

namespace source::ui::GerenciamentoMenusUI {
void configurar_menu_configuracoes(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes = nullptr);
}
