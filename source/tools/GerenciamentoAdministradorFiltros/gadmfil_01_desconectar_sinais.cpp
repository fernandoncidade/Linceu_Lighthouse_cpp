#include "gadmfil_01_desconectar_sinais.h"

#include <QObject>

#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

void desconectar_sinais(source::tools::AdministradorFiltros* self) {
    try {
        if (!self) return;

        auto& conexoes = self->conexoes_sinais_ref();
        for (const auto& conexao : conexoes) {
            QObject::disconnect(conexao);
        }
        conexoes.clear();
        self->set_parent_obj(nullptr);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao desconectar sinais", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
