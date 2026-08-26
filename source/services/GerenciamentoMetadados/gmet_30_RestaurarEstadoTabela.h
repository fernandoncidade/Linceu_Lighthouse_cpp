#pragma once

#include <QList>
#include <QVariantMap>

class QObject;
namespace source::services { class GerenciadorColunas; }

namespace source::services::GerenciamentoMetadados {

void restaurar_estado_tabela(source::services::GerenciadorColunas* gc, QObject* tabela, const QList<QVariantMap>& eventos);

}  // namespace source::services::GerenciamentoMetadados
