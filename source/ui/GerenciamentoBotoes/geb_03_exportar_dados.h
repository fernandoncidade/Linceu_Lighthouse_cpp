#pragma once

#include <QString>

namespace source::ui {
class GerenciadorBotoes;
}

namespace source::ui::GerenciamentoBotoes {

bool exportar_dados(source::ui::GerenciadorBotoes* self, void* tabela_dados, const QString& formato,
                    const QString& nome_arquivo = QString());

}  // namespace source::ui::GerenciamentoBotoes
