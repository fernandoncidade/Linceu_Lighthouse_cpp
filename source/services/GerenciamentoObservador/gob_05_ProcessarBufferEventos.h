#pragma once

namespace source::services { class Observador; }

namespace source::services::GerenciamentoObservador {

void processar_buffer_eventos(source::services::Observador* self);

}  // namespace source::services::GerenciamentoObservador
