#pragma once

#include <QString>

namespace source::tools {
class AdministradorFiltros;
}

namespace source::tools::GerenciamentoAdministradorFiltros {

bool verificar_filtro_operacao(source::tools::AdministradorFiltros* self, const QString& tipo_operacao_traduzido);

}
