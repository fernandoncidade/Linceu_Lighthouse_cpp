#pragma once

#include <QPair>
#include <QString>
#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

QPair<QString, QVariantMap> extrair_metadados(const QString& caminho, source::ui::LocalizadorQt* loc);

}  // namespace source::services::GerenciamentoMetadados
