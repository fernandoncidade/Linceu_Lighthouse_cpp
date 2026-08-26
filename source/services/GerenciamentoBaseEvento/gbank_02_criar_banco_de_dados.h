#pragma once

#include <QString>

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void criar_banco_de_dados(source::services::BaseEvento* self);
void criar_banco_de_dados_path(const QString& db_path);
}
