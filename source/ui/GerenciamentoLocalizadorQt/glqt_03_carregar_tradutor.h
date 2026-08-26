#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
bool carregar_tradutor(source::ui::LocalizadorQt* self, const QString& idioma);
}
