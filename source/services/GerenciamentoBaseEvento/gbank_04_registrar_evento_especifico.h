#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void registrar_evento_especifico(source::services::BaseEvento* self, const QString& tabela, const QVariantMap& evento);
}
