#include "gfil_10_limpar_filtros.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void limpar_filtros(source::tools::Filtros* self) {
    try {
        if (!self || !self->administrador_filtros_obj()) return;
        self->administrador_filtros_obj()->limpar_filtros();
        emit self->filtroAplicado();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar filtros", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
