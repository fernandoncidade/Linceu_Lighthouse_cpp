#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_interface_pos_processamento(source::services::GerenciadorTabela* self, const QVariantMap& resultado);

}  // namespace source::services::GerenciamentoTabela
