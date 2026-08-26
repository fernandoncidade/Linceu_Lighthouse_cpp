#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void on_idioma_alterado(source::services::GerenciadorTabela* self, const QString& idioma);

}  // namespace source::services::GerenciamentoTabela
