#pragma once

#include <QString>

namespace source::tools {
class AdministradorFiltros;
}

namespace source::tools::GerenciamentoAdministradorFiltros {

void atualizar_contagem_apos_idioma(source::tools::AdministradorFiltros* self, const QString& idioma = QString());

}
