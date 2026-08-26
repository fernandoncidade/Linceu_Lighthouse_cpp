#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

bool eh_coluna_personalizada_colorida(source::services::GerenciadorTabela* self, const QString& key_coluna);

}  // namespace source::services::GerenciamentoTabela
