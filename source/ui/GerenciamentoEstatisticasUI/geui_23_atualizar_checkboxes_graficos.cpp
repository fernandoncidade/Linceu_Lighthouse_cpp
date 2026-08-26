#include "geui_23_atualizar_checkboxes_graficos.h"

#include <QCheckBox>
#include <QVariantMap>
#include <QVBoxLayout>

#include "geui_11_verificar_estado_checkbox_todos.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_checkboxes_graficos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos_atualizados, const QHash<QString, bool>& estados_checkboxes, const QVariantHash&) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* checksLayout = iface->property("checkboxes_layout").value<QVBoxLayout*>();
        if (!checksLayout) return;

        while (checksLayout->count() > 2) {
            auto* child = checksLayout->takeAt(2);
            if (child && child->widget()) child->widget()->deleteLater();
            delete child;
        }

        QVariantHash mapa;
        for (const QVariant& v : graficos_atualizados) {
            const QVariantMap g = v.toMap();
            auto* c = new QCheckBox(g.value("titulo").toString());
            c->setChecked(estados_checkboxes.value(g.value("func").toString(), true));
            QObject::connect(c, &QCheckBox::clicked, [self]() { verificar_estado_checkbox_todos(self); });
            checksLayout->addWidget(c);

            QVariantMap data;
            data.insert("checkbox", QVariant::fromValue(c));
            data.insert("grafico_data", g);
            mapa.insert(g.value("titulo").toString(), data);
        }

        iface->setProperty("checkboxes_graficos", mapa);
        verificar_estado_checkbox_todos(self);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar checkboxes", true);
    }
}

}
