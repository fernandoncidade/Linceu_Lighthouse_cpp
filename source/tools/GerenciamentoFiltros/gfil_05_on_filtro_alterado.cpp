#include "gfil_05_on_filtro_alterado.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void on_filtro_alterado(source::tools::Filtros* self) {
    try {
        if (!self || !self->administrador_filtros_obj()) return;
        self->administrador_filtros_obj()->aplicar_filtros();
        self->administrador_filtros_obj()->sincronizar_menu_principal_com_filtros();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar alteração de filtro", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
