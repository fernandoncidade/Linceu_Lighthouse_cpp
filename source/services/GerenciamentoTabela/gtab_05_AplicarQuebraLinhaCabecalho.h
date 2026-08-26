#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void aplicar_quebra_linha_cabecalho(source::services::GerenciadorTabela* self, QObject* tabela, int coluna_index);

}  // namespace source::services::GerenciamentoTabela
