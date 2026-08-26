#pragma once

#include <QString>

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {
QString get_texto_traduzido_para_idioma(source::ui::GerenciadorMenusUI* self, const QString& chave, const QString& idioma);
}
