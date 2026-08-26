#pragma once

#include <QString>
#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_documento(const QString& caminho, source::ui::LocalizadorQt* loc);
QString parse_pdf_date(const QString& date_str);

}  // namespace source::services::GerenciamentoMetadados
