#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void remover_cor_coluna(source::services::GerenciadorTabela* self, const QString& coluna_key);

}  // namespace source::services::GerenciamentoTabela
