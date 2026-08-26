#pragma once

#include "ob_02_BaseEvento.h"

namespace source::services {

class EventoExcluido : public BaseEvento {
public:
    explicit EventoExcluido(source::services::Observador* observador);
    bool processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);
};

}  // namespace source::services
