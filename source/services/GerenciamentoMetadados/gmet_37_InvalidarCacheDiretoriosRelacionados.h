#pragma once

#include <QString>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void invalidar_cache_diretorios_relacionados(source::services::GerenciadorColunas* gc, const QString& caminho);

}  // namespace source::services::GerenciamentoMetadados
