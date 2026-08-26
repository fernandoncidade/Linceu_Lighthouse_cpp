#pragma once

#include <QVariantMap>

class QObject;
namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

QVariantMap salvar_estado_tabela(source::services::GerenciadorColunas* gc, QObject* tabela);

}  // namespace source::services::GerenciamentoMetadados
