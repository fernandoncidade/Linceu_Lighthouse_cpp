#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_visualizacao_tabela(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
