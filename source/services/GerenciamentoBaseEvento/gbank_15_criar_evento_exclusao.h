#pragma once

#include <QString>
#include <QVariantMap>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QVariantMap _criar_evento_exclusao(source::services::BaseEvento* self, const QString& nome_base, const QString& dir_anterior);
}
