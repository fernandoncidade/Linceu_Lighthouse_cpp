#pragma once

#include <QVariantMap>

namespace source::services {
class GerenciadorColunas;
}

namespace source::services::GerenciamentoMetadados {

qint64 get_tamanho_bytes(source::services::GerenciadorColunas* gc, const QVariantMap& item);

}  // namespace source::services::GerenciamentoMetadados
