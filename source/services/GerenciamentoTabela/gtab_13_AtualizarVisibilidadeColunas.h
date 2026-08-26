#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_visibilidade_colunas(source::services::GerenciadorTabela* self, bool atualizar_em_massa);

}  // namespace source::services::GerenciamentoTabela
