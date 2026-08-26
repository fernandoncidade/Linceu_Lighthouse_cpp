#pragma once

#include <QString>

class QObject;

namespace source::services::GerenciamentoEventoMovido {

bool remover_exclusao(QObject* interfaceMonitor, const QString& nome, const QString& dir_anterior, const QString& exclusao_id = QString());

}  // namespace source::services::GerenciamentoEventoMovido
