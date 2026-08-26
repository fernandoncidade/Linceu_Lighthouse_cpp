#pragma once

class QMenu;

namespace source::ui {
class GerenciadorMenusUI;
}

namespace source::security::ui {

void criar_submenu_seguranca(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes);

}  // namespace source::security::ui
