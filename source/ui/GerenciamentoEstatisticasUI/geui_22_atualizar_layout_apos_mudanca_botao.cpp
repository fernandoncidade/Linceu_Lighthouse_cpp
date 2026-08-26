#include "geui_22_atualizar_layout_apos_mudanca_botao.h"

#include <QPushButton>
#include <QSplitter>
#include <QWidget>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_layout_apos_mudanca_botao(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();

        auto* panel = iface->property("painel_selecao").value<QWidget*>();
        auto* splitter = iface->property("splitter").value<QSplitter*>();
        auto* btn = iface->property("btn_toggle_painel").value<QPushButton*>();
        if (!panel || !btn) return;

        const int larguraBotao = btn->width();
        const bool recolhido = iface->property("painel_recolhido").toBool();
        const int tamanhoOriginal = iface->property("tamanho_painel_original").toInt();

        panel->setFixedWidth(recolhido ? larguraBotao : larguraBotao + tamanhoOriginal);

        if (splitter) {
            QList<int> sizes = splitter->sizes();
            int total = 0;
            for (int s : sizes) total += s;
            splitter->setSizes({panel->width(), qMax(100, total - panel->width())});
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar layout após mudança do botão", true);
    }
}

}
