#pragma once

#include <QVariantMap>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

QVariantMap get_metadados_gc(source::services::GerenciadorColunas* gc, const QVariantMap& item);

}  // namespace source::services::GerenciamentoMetadados
