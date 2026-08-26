#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void set_colunas_colorir_em_massa(source::services::GerenciadorTabela* self, const QSet<QString>& novas_colunas);

}  // namespace source::services::GerenciamentoTabela
