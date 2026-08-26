#pragma once

#include <QString>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void atualizar_coluna_interface(source::services::GerenciadorColunas* gc, const QString& coluna_key = QString());

}  // namespace source::services::GerenciamentoMetadados
