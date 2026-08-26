#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::security {

bool is_ui_translatable_field(const QString& field_key);
QString traduzir_valor_ui(source::ui::LocalizadorQt* loc,
                          const QString& field_key,
                          const QString& raw_value);

}  // namespace source::security
