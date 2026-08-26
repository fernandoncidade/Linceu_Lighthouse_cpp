#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString carregar_preferencia_idioma(source::ui::LocalizadorQt* self);
}
