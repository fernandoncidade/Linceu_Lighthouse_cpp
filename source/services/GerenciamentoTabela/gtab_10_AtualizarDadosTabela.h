#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_dados_tabela(source::services::GerenciadorTabela* self, QObject* tabela_dados, int row_especifico);

}  // namespace source::services::GerenciamentoTabela
