#pragma once

namespace source::ui { class GerenciadorMenusUI; }
class QMenu;

namespace source::ui::GerenciamentoMenusUI {
void configurar_menu_arquivo(source::ui::GerenciadorMenusUI* self, QMenu* menu_arquivo = nullptr);
}
