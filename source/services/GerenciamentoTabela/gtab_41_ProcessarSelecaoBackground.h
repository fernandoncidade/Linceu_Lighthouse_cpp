#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

bool processar_selecao_background(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
