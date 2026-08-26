#pragma once

#include <QVariantMap>

namespace source::services {
class GerenciadorColunas;
}
namespace source::ui {
class LocalizadorQt;
}

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_completos(const QVariantMap& item, source::ui::LocalizadorQt* loc = nullptr, source::services::GerenciadorColunas* contexto = nullptr);
QVariantMap get_metadados(source::services::GerenciadorColunas* self, const QVariantMap& item = QVariantMap());

}  // namespace source::services::GerenciamentoMetadados
