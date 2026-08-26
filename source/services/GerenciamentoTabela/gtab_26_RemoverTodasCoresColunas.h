#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void remover_todas_cores_colunas(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
