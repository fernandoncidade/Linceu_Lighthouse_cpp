#include "gadmfil_08_atualizar_contagem_apos_idioma.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

void atualizar_contagem_apos_idioma(source::tools::AdministradorFiltros* self, const QString&) {
    try {
        if (!self || !self->parent_obj()) return;

        emit self->parent_obj()->filtroAplicado();
        self->notificar_alteracao_idioma();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar após mudança de idioma", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
