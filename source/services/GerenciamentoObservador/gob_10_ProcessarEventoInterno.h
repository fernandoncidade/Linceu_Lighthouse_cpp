#pragma once

#include <QVariantMap>

namespace source::services { class Observador; }

namespace source::services::GerenciamentoObservador {

void _processar_evento_interno(source::services::Observador* self, const QVariantMap& evento);

}  // namespace source::services::GerenciamentoObservador
