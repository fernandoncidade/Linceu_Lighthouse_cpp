#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void ajustar_altura_cabecalho(source::services::GerenciadorTabela* self, QObject* tabela);

}  // namespace source::services::GerenciamentoTabela
