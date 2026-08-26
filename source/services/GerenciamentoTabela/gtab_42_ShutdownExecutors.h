#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void shutdown_executors(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
