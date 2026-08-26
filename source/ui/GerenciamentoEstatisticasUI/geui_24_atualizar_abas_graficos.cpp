#include "geui_24_atualizar_abas_graficos.h"

#include <QTabWidget>
#include <QVariantHash>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_abas_graficos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos_atualizados, const QVariantHash&) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* tab = iface->property("tab_widget").value<QTabWidget*>();
        if (!tab || tab->count() == 0) return;

        const QVariantHash checkboxes = iface->property("checkboxes_graficos").toHash();
        const QVariantHash graficos_dados = iface->property("graficos_dados").toHash();

        for (int i = 0; i < tab->count(); ++i) {
            const QString titulo_atual = tab->tabText(i);
            QString func_correspondente;

            for (auto it = checkboxes.cbegin(); it != checkboxes.cend(); ++it) {
                const QVariantMap data = it.value().toMap();
                const QVariantMap grafico_data = data.value("grafico_data").toMap();

                if (it.key() == titulo_atual) {
                    func_correspondente = grafico_data.value("func").toString();
                    break;
                }
            }

            if (func_correspondente.isEmpty() && graficos_dados.contains(titulo_atual)) {
                const QVariantMap grafico_data = graficos_dados.value(titulo_atual).toMap();
                func_correspondente = grafico_data.value("func").toString();
            }

            if (func_correspondente.isEmpty()) {
                continue;
            }

            QString novo_titulo;
            for (const QVariant& v : graficos_atualizados) {
                const QVariantMap grafico = v.toMap();
                if (grafico.value("func").toString() == func_correspondente) {
                    novo_titulo = grafico.value("titulo").toString();
                    break;
                }
            }

            if (!novo_titulo.isEmpty() && novo_titulo != titulo_atual) {
                tab->setTabText(i, novo_titulo);
                source::utils::LogManager::get_logger().debug(
                    QString("Aba atualizada de '%1' para '%2'").arg(titulo_atual, novo_titulo));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar abas dos gráficos", true);
    }
}

}
