#pragma once

#include <QString>

namespace source::tools { class Filtros; }

namespace source::tools::GerenciamentoFiltros {
bool verificar_filtro_operacao(source::tools::Filtros* self, const QString& tipo_operacao_traduzido);
}
