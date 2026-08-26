#pragma once

namespace source::services {
class BaseEvento;
}

namespace source::services::GerenciamentoBaseEvento {
void criar_banco_de_dados(source::services::BaseEvento* self);
}
