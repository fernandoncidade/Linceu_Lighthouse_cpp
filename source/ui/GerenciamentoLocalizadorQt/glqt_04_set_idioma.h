#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
void set_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
}
