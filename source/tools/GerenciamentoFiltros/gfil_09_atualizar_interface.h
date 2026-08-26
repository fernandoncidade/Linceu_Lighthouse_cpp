#pragma once

#include <QString>

namespace source::tools { class Filtros; }

namespace source::tools::GerenciamentoFiltros {
void atualizar_interface(source::tools::Filtros* self, const QString& idioma = QString());
}
