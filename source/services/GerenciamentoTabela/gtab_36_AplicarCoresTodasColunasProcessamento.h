#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void aplicar_cores_todas_colunas_processamento(source::services::GerenciadorTabela* self, const QVariant& dados);

}  // namespace source::services::GerenciamentoTabela
