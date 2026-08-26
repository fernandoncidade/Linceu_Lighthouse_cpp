#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_linha_mais_recente(source::services::GerenciadorTabela* self, QObject* tabela_dados, const QVariantMap& evento);

}  // namespace source::services::GerenciamentoTabela
