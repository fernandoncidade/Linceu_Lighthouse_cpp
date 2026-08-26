#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void executar_retraducao_agendada(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
