#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_cores_colunas(source::services::GerenciadorTabela* self,
                             bool aplicar_em_massa,
                             const QSet<QString>& colunas_afetadas = {});

}  // namespace source::services::GerenciamentoTabela
