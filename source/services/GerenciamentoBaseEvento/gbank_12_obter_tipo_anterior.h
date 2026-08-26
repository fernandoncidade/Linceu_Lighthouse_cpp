#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
QString obter_tipo_anterior(source::services::BaseEvento* self, const QString& nome_base);
}
