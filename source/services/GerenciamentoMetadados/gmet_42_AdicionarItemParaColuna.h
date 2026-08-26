#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void adicionar_item_para_coluna(source::services::GerenciadorColunas* gc, const QString& coluna_key, const QVariantMap& item);

}  // namespace source::services::GerenciamentoMetadados
