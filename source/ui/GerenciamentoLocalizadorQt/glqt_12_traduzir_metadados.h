#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::ui::GerenciamentoLocalizadorQt {
QString traduzir_metadados(source::ui::LocalizadorQt* self, const QString& valor, const QString& campo);
}
