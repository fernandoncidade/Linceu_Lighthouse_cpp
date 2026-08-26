#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void invalidar_cache_cores(source::services::GerenciadorTabela* self);

}  // namespace source::services::GerenciamentoTabela
