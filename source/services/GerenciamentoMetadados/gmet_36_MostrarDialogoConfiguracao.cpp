#include "gmet_36_MostrarDialogoConfiguracao.h"

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void mostrar_dialogo_configuracao(source::services::GerenciadorColunas* gc, const QPoint& pos) {
    try {
        Q_UNUSED(pos)
        if (gc && gc->gerenciador_tabela) gc->gerenciador_tabela->mostrar_dialogo_configuracao();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao mostrar diálogo de configuração", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
