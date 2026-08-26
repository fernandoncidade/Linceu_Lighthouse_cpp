#pragma once

#include <QString>

class QIcon;

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {
QIcon criar_icone_cor(source::ui::GerenciadorMenusUI* self, const QString& cor_hex);
}
