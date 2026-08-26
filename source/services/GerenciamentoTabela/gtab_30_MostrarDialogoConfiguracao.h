#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void mostrar_dialogo_configuracao(source::services::GerenciadorTabela* self, const QPoint& pos);

}  // namespace source::services::GerenciamentoTabela
