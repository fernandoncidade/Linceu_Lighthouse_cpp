#include "gadmfil_07_verificar_filtro_operacao.h"

#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_00_operacao_utils.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

bool verificar_filtro_operacao(source::tools::AdministradorFiltros* self, const QString& tipo_operacao_traduzido) {
    try {
        if (!self || !self->parent_obj()) return true;
        return detail::operacao_permitida(self->parent_obj(), tipo_operacao_traduzido);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar filtro de operação", true);
        return true;
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
