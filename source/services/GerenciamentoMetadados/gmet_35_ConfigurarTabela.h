#pragma once

class QObject;
namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void configurar_tabela(source::services::GerenciadorColunas* gc, QObject* tabela);

}  // namespace source::services::GerenciamentoMetadados
