#pragma once

#include <QVariantMap>

class QObject;

namespace source::services::GerenciamentoEventoMovido {

QString normalizar_caminho_tabela(const QString& caminho);
void adicionar_item_tabela(QObject* interfaceMonitor, const QVariantMap& evento, bool atualizar_interface = true);

}  // namespace source::services::GerenciamentoEventoMovido
