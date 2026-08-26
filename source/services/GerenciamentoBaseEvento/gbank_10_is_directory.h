#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
bool is_directory(source::services::BaseEvento* self, const QString& path);
}
