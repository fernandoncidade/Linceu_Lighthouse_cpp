#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QVariantMap _criar_evento_padrao(source::services::BaseEvento* self, const QString& tipo_operacao, const QString& nome_base, const QString& dir_anterior, const QString& dir_atual);
}
