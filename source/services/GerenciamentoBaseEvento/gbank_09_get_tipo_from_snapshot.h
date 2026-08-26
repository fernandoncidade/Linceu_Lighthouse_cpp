#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QString get_tipo_from_snapshot(source::services::BaseEvento* self, const QString& nome);
}
