#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

namespace source::ui {
class GerenciadorBotoes;
}

namespace source::ui::GerenciamentoBotoes {

bool exportar_para_sqlite(source::ui::GerenciadorBotoes* self,
                          void* tabela_dados,
                          const QString& nome_arquivo);

void exportar_para_sqlite(source::ui::GerenciadorBotoes* self,
                          const QList<QMap<QString, QString>>& dados,
                          const QString& nome_arquivo,
                          const QStringList& colunas_primeira_linha = QStringList());

}  // namespace source::ui::GerenciamentoBotoes
