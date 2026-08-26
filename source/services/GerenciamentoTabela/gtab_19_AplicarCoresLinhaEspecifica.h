#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void aplicar_cores_linha_especifica(source::services::GerenciadorTabela* self, QObject* tabela, int row, const QString& tipo_operacao_valor);

}  // namespace source::services::GerenciamentoTabela
