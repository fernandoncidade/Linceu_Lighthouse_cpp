#pragma once

#include <QVariantMap>

namespace source::services { class Observador; }

namespace source::services::GerenciamentoObservador {

void processar_evento(source::services::Observador* self, const QVariantMap& evento);

}  // namespace source::services::GerenciamentoObservador
