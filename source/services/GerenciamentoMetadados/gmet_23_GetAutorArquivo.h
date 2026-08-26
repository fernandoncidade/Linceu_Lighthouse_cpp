#pragma once

#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

QString get_autor_arquivo(const QVariantMap& item, source::ui::LocalizadorQt* loc);

}  // namespace source::services::GerenciamentoMetadados
