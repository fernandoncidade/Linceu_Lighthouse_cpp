#include "geui_11_verificar_estado_checkbox_todos.h"

#include <QCheckBox>
#include <QVariantHash>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void verificar_estado_checkbox_todos(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* todos = iface->property("checkbox_todos").value<QCheckBox*>();
        if (!todos) return;

        const QVariantHash mapa = iface->property("checkboxes_graficos").toHash();
        if (mapa.isEmpty()) return;

        int marcados = 0;
        for (auto it = mapa.cbegin(); it != mapa.cend(); ++it) {
            auto* checkbox = it.value().toMap().value("checkbox").value<QCheckBox*>();
            if (checkbox && checkbox->isChecked()) ++marcados;
        }

        todos->blockSignals(true);
        if (marcados == 0) todos->setCheckState(Qt::Unchecked);
        else if (marcados == mapa.size()) todos->setCheckState(Qt::Checked);
        else todos->setCheckState(Qt::PartiallyChecked);
        todos->blockSignals(false);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar estado dos checkboxes", true);
    }
}

}
