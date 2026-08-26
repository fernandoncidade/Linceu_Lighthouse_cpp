#include "geui_07_criar_botao_toggle_painel.h"

#include <QPushButton>

#include "geui_19_toggle_painel_selecao.h"
#include "geui_30_botao_rotacionado.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

QWidget* criar_botao_toggle_painel(source::ui::GerenciadorEstatisticasUI* self, const QString& texto) {
    try {
        auto* botao = new BotaoRotacionado(texto);
        QObject::connect(botao, &QPushButton::clicked, [self]() { toggle_painel_selecao(self); });
        return botao;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar botão de toggle do painel", true);
        return nullptr;
    }
}

}
