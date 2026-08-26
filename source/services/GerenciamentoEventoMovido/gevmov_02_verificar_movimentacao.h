#pragma once

#include <QVariant>
#include <QVariantMap>

class QObject;

namespace source::services::GerenciamentoEventoMovido {

QString normalizar_caminho(const QString& caminho);
QVariant verificar_movimentacao(QObject* interfaceMonitor, const QVariant& evento);

}  // namespace source::services::GerenciamentoEventoMovido
