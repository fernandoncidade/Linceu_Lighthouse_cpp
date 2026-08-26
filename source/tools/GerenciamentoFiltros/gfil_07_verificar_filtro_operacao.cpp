#include "gfil_07_verificar_filtro_operacao.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

bool verificar_filtro_operacao(source::tools::Filtros* self, const QString& operation) {
    try {
        if (!self || !self->administrador_filtros_obj()) return true;
        return self->administrador_filtros_obj()->verificar_filtro_operacao(operation);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar filtro de operação", true);
        return true;
    }
}

} // namespace source::tools::GerenciamentoFiltros
