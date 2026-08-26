#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void iniciar_processamento_pesado(source::services::GerenciadorTabela* self, const QVariant& dados);

}  // namespace source::services::GerenciamentoTabela
