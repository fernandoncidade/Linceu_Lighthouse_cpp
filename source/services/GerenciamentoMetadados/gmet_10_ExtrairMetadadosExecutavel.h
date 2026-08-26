#pragma once

#include <QString>
#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

bool verificar_assinatura_executavel(const QString& caminho);
QVariantMap extrair_metadados_executavel(const QString& caminho, source::ui::LocalizadorQt* loc = nullptr);

}  // namespace source::services::GerenciamentoMetadados
