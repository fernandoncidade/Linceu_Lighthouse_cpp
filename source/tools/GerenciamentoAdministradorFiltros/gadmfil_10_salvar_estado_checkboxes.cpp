#include "gadmfil_10_salvar_estado_checkboxes.h"

#include <QCheckBox>

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

void salvar_estado_checkboxes(source::tools::AdministradorFiltros* self) {
    try {
        if (!self || !self->parent_obj()) return;
        auto* parent = self->parent_obj();

        auto save = [&](const QString& objectName, const QString& key) {
            if (auto* cb = parent->findChild<QCheckBox*>(objectName)) {
                source::tools::AdministradorFiltros::filtros_estado[key] = cb->isChecked();
            }
        };

        save("ignorar_mover", "ignorar_mover");
        save("ignorar_renomeados", "ignorar_renomeados");
        save("ignorar_adicionados", "ignorar_adicionados");
        save("ignorar_excluidos", "ignorar_excluidos");
        save("ignorar_data_modificados", "ignorar_data_modificados");
        save("ignorar_escaneados", "ignorar_escaneados");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao salvar estado dos filtros", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
