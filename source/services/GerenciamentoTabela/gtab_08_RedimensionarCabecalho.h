#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void redimensionar_cabecalho(source::services::GerenciadorTabela* self, int logicalIndex, int oldSize, int newSize);

}  // namespace source::services::GerenciamentoTabela
