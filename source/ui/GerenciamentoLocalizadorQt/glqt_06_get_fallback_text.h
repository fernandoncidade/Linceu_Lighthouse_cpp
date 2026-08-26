#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString get_fallback_text(source::ui::LocalizadorQt* self, const QString& key);
}
