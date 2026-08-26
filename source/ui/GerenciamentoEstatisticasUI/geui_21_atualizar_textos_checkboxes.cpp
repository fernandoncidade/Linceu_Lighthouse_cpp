#include "geui_21_atualizar_textos_checkboxes.h"

#include <QCheckBox>
#include <QVariantHash>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_textos_checkboxes(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos) {
    try {
        if (!self || !self->interface_obj()) return;
        QVariantHash mapa = self->interface_obj()->property("checkboxes_graficos").toHash();
        if (mapa.isEmpty()) return;

        for (const QVariant& vg : graficos) {
            const QVariantMap g = vg.toMap();
            const QString func = g.value("func").toString();
            const QString novoTitulo = g.value("titulo").toString();

            for (auto it = mapa.begin(); it != mapa.end(); ++it) {
                QVariantMap data = it.value().toMap();
                QVariantMap gData = data.value("grafico_data").toMap();
                if (gData.value("func").toString() == func) {
                    if (auto* checkbox = data.value("checkbox").value<QCheckBox*>()) {
                        checkbox->setText(novoTitulo);
                    }
                }
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar textos dos checkboxes", true);
    }
}

}
