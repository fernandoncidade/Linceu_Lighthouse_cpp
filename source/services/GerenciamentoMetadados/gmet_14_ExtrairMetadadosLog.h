#pragma once

#include <QString>
#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_log(const QString& caminho, source::ui::LocalizadorQt* loc);

}  // namespace source::services::GerenciamentoMetadados
