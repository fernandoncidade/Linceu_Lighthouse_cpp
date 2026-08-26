#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void configurar_tabela(source::services::GerenciadorTabela* self, QObject* tabela_dados);

}  // namespace source::services::GerenciamentoTabela
