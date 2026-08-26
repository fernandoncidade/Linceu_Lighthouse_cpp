#pragma once

#include <QString>
#include <QVariantMap>

namespace source::ui { class LocalizadorQt; }

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_backup(const QString& caminho, source::ui::LocalizadorQt* loc);
bool eh_arquivo_texto(const QString& caminho);
int contar_linhas(const QString& caminho);

}  // namespace source::services::GerenciamentoMetadados
