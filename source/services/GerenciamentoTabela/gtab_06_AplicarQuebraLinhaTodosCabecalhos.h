#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void aplicar_quebra_linha_todos_cabecalhos(source::services::GerenciadorTabela* self, QObject* tabela);

}  // namespace source::services::GerenciamentoTabela
