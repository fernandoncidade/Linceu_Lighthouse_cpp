#include "geui_27_ajustar_largura_painel_selecao.h"

#include <QSplitter>
#include <QWidget>

#include "geui_08_criar_lista_graficos.h"
#include "geui_21_atualizar_textos_checkboxes.h"
#include "geui_26_calcular_largura_ideal.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void ajustar_largura_painel_selecao(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        const qulonglong gerador_ptr = iface->property("_gerador_cpp_ptr").toULongLong();
        if (gerador_ptr == 0) return;

        auto* painel = iface->property("painel_selecao").value<QWidget*>();
        auto* interno = iface->property("painel_selecao_interno").value<QWidget*>();
        auto* splitter = iface->property("splitter").value<QSplitter*>();
        if (!painel || !interno) return;

        QVariantList graficos = criar_lista_graficos(self);
        const int nova = calcular_largura_ideal(self, graficos);
        const bool recolhido = iface->property("painel_recolhido").toBool();

        iface->setProperty("tamanho_painel_original", nova);
        if (!recolhido) {
            interno->setFixedWidth(nova);
            painel->setFixedWidth(25 + nova);
        }

        atualizar_textos_checkboxes(self, graficos);

        if (splitter && !recolhido) {
            int total = 0;
            for (int s : splitter->sizes()) total += s;
            splitter->setSizes({painel->width(), qMax(100, total - painel->width())});
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao ajustar largura do painel", true);
    }
}

}
