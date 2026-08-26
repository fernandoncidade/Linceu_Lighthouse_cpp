#pragma once

#include <QString>

namespace source::ui {
class LocalizadorQt;
}

namespace source::services::GerenciamentoMetadados {

QString extensao_tipo_preferencial(const QString& nome_arquivo);
QString identificar_tipo_arquivo(const QString& caminho, source::ui::LocalizadorQt* loc, const QString& nome_arquivo = QString());

}  // namespace source::services::GerenciamentoMetadados
