#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void ajustar_larguras_colunas(source::services::GerenciadorTabela* self, QObject* tabela_dados, const QVariantList& colunas_visiveis);

}  // namespace source::services::GerenciamentoTabela
