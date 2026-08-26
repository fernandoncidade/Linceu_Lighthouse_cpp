#pragma once

#include <QString>

namespace source::tools { class Filtros; }

namespace source::tools::GerenciamentoFiltros {
void on_idioma_alterado(source::tools::Filtros* self, const QString& idioma);
}
