#pragma once

#include <QString>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void atualizar_interface(source::services::GerenciadorColunas* gc, const QString& idioma);

}  // namespace source::services::GerenciamentoMetadados
