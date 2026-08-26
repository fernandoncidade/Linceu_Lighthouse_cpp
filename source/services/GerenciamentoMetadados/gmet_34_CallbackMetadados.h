#pragma once

#include <QVariant>

namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void callback_metadados(source::services::GerenciadorColunas* gc, const QVariant& futuro);

}  // namespace source::services::GerenciamentoMetadados
