#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
void salvar_preferencia_idioma(source::ui::LocalizadorQt* self, const QString& idioma);
}
