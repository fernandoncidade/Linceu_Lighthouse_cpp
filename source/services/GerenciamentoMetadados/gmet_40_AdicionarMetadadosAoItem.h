#pragma once

#include <QVariantMap>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

QVariantMap adicionar_metadados_ao_item(source::services::GerenciadorColunas* gc, QVariantMap item);

}  // namespace source::services::GerenciamentoMetadados
