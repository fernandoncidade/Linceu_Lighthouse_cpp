#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace source::security::core {

const QStringList& colunas_evento_monitoramento();
const QList<QPair<QString, QString>>& definicoes_colunas_evento_monitoramento();
void garantir_campos_evento_monitoramento(QVariantMap& evento);

}  // namespace source::security::core
