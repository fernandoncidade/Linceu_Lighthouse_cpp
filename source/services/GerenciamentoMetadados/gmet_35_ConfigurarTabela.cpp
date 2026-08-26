#include "gmet_35_ConfigurarTabela.h"

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void configurar_tabela(source::services::GerenciadorColunas* gc, QObject* tabela) {
    try {
        if (gc && gc->gerenciador_tabela) gc->gerenciador_tabela->configurar_tabela(tabela);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar tabela", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
