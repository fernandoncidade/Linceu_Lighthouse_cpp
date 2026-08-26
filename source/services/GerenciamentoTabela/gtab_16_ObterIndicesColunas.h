#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

QVariantMap obter_indices_colunas(source::services::GerenciadorTabela* self, QObject* tabela);

}  // namespace source::services::GerenciamentoTabela
