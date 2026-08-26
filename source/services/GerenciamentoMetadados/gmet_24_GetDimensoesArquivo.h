#pragma once

#include <QVariantMap>

namespace source::services {
class GerenciadorColunas;
}
namespace source::ui {
class LocalizadorQt;
}

namespace source::services::GerenciamentoMetadados {

QString get_dimensoes_arquivo(source::services::GerenciadorColunas* self, const QVariantMap& item, source::ui::LocalizadorQt* loc);

}  // namespace source::services::GerenciamentoMetadados
