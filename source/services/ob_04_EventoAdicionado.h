#pragma once

#include "ob_02_BaseEvento.h"

namespace source::services {

class EventoAdicionado : public BaseEvento {
public:
    explicit EventoAdicionado(source::services::Observador* observador);
    bool processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);
};

}  // namespace source::services
