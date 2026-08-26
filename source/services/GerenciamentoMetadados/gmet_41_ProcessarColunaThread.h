#pragma once

#include <QString>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void processar_coluna_thread(source::services::GerenciadorColunas* gc, const QString& coluna_key);

}  // namespace source::services::GerenciamentoMetadados
