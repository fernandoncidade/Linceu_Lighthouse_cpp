#pragma once

#include <QHash>

#include "ob_02_BaseEvento.h"

namespace source::services {

class EventoRenomeado : public BaseEvento {
public:
    explicit EventoRenomeado(source::services::Observador* observador);

    bool processar(const QString& nome_arquivo, const QString& caminho_completo, int acao);

private:
    QHash<QString, double> renomeacao_em_andamento;
};

}  // namespace source::services
