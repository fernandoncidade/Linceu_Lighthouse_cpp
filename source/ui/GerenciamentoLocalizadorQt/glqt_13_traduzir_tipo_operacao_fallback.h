#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString traduzir_tipo_operacao_fallback(source::ui::LocalizadorQt* self, const QString& valor);
}
