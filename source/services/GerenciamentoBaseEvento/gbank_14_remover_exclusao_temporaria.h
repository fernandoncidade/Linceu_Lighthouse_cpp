#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void _remover_exclusao_temporaria(source::services::BaseEvento* self, const QString& nome, const QString& dir_anterior, const QString& exclusao_id = QString());
}
