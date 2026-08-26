#pragma once

namespace source::services { class Observador; }

namespace source::services::GerenciamentoObservador {

void monitorar(source::services::Observador* self);
void close_handle_dir_safely(source::services::Observador* self);

}  // namespace source::services::GerenciamentoObservador
