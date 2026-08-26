#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services {
class GerenciadorColunas;
}

namespace source::services::GerenciamentoMetadados {

QString get_size_unit(source::services::GerenciadorColunas* gc, const QVariantMap& item, const QString& unidade);

}  // namespace source::services::GerenciamentoMetadados
