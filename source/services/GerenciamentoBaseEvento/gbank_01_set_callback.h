#pragma once

#include <QVariant>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void set_callback(source::services::BaseEvento* self, const QVariant& callback);
}
