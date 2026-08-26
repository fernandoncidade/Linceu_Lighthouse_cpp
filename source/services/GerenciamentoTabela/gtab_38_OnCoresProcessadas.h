#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void on_cores_processadas(source::services::GerenciadorTabela* self, const QVariantMap& resultado);

}  // namespace source::services::GerenciamentoTabela
