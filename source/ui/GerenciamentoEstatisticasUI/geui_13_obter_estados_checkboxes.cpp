#include "geui_13_obter_estados_checkboxes.h"

#include <QCheckBox>
#include <QVariantHash>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

QHash<QString, bool> obter_estados_checkboxes(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        QHash<QString, bool> estados;
        if (!self || !self->interface_obj()) return estados;

        const QVariantHash mapa = self->interface_obj()->property("checkboxes_graficos").toHash();
        for (auto it = mapa.cbegin(); it != mapa.cend(); ++it) {
            const QVariantMap data = it.value().toMap();
            const QVariantMap grafico = data.value("grafico_data").toMap();
            auto* checkbox = data.value("checkbox").value<QCheckBox*>();
            estados.insert(grafico.value("func").toString(), checkbox ? checkbox->isChecked() : true);
        }
        return estados;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter estados dos checkboxes", true);
        return {};
    }
}

}
