#pragma once

#include <QString>

namespace source::services { class Observador; }

namespace source::services::GerenciamentoObservador {

void handle_scan_error(source::services::Observador* self, const QString& error_msg);

}  // namespace source::services::GerenciamentoObservador
