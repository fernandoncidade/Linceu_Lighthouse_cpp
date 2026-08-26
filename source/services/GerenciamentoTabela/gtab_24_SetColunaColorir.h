#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void set_coluna_colorir(source::services::GerenciadorTabela* self, const QString& coluna_key, bool checked);

}  // namespace source::services::GerenciamentoTabela
