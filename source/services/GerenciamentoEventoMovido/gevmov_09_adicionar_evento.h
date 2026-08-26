#pragma once

#include <QVariant>

class QObject;

namespace source::services::GerenciamentoEventoMovido {

void adicionar_evento(QObject* interfaceMonitor, const QVariant& evento);

}  // namespace source::services::GerenciamentoEventoMovido
