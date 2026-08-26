#pragma once

#include <QPoint>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void mostrar_dialogo_configuracao(source::services::GerenciadorColunas* gc, const QPoint& pos = QPoint());

}  // namespace source::services::GerenciamentoMetadados
