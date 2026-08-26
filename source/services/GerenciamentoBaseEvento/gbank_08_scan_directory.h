#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void scan_directory(source::services::BaseEvento* self, const QString& directory);
}
