#include "geui_12_alternar_todos_checkboxes.h"

#include <QCheckBox>
#include <QVariantHash>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void alternar_todos_checkboxes(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* todos = iface->property("checkbox_todos").value<QCheckBox*>();
        if (!todos) return;

        if (todos->checkState() == Qt::PartiallyChecked) {
            todos->setCheckState(Qt::Checked);
        }

        const bool checked = todos->checkState() == Qt::Checked;
        QVariantHash mapa = iface->property("checkboxes_graficos").toHash();

        for (auto it = mapa.begin(); it != mapa.end(); ++it) {
            QVariantMap data = it.value().toMap();
            auto* checkbox = data.value("checkbox").value<QCheckBox*>();
            if (checkbox) {
                checkbox->blockSignals(true);
                checkbox->setChecked(checked);
                checkbox->blockSignals(false);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar todos os checkboxes", true);
    }
}

}
