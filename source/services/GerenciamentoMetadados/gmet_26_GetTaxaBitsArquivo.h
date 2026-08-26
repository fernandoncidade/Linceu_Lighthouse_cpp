#pragma once

#include <QVariantMap>

namespace source::services {
class GerenciadorColunas;
}

namespace source::services::GerenciamentoMetadados {

QString get_taxa_bits_arquivo(source::services::GerenciadorColunas* gerenciador, const QVariantMap& item);

}  // namespace source::services::GerenciamentoMetadados
