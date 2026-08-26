#pragma once

#include <QList>
#include <QVariantMap>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void extrair_metadados_em_lote(source::services::GerenciadorColunas* gc, const QList<QVariantMap>& lista_itens);

}  // namespace source::services::GerenciamentoMetadados
