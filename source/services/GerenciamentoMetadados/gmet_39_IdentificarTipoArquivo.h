#pragma once

#include <QString>

namespace source::services {
class GerenciadorColunas;
}

namespace source::services::GerenciamentoMetadados {

QString identificar_tipo_arquivo_gc(source::services::GerenciadorColunas* gc, const QString& caminho);

}  // namespace source::services::GerenciamentoMetadados
