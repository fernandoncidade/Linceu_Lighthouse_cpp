#include "geve_01_alternar_filtro.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QDialog>
#include <QMetaObject>
#include <QVariantMap>

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/LogManager.h"

namespace {
void sincronizar_checkbox_filtro(QObject* painel, const QString& filtro, bool checked, bool aplicar_filtros) {
    if (painel == nullptr || filtro.isEmpty()) {
        return;
    }

    const QVariantMap checkboxes = painel->property("checkboxes_operacao").toMap();
    if (!checkboxes.contains(filtro)) {
        return;
    }

    QObject* checkbox_obj = checkboxes.value(filtro).value<QObject*>();
    auto* checkbox = qobject_cast<QCheckBox*>(checkbox_obj);
    if (checkbox == nullptr) {
        checkbox = checkboxes.value(filtro).value<QCheckBox*>();
    }
    if (checkbox == nullptr) {
        return;
    }

    checkbox->blockSignals(true);
    checkbox->setChecked(checked);
    checkbox->blockSignals(false);

    if (aplicar_filtros) {
        if (auto* filtros = qobject_cast<source::tools::Filtros*>(painel)) {
            if (filtros->administrador_filtros_obj() != nullptr) {
                filtros->administrador_filtros_obj()->aplicar_filtros();
            }
        }
    }
}
}

namespace source::ui::GerenciamentoEventosUI {

void alternar_filtro(source::ui::GerenciadorEventosUI* self, const QString& filtro) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        QAction* acao = interface_obj->property("sender_action").value<QAction*>();
        QString filtro_key = filtro;
        bool checked = true;
        if (acao != nullptr) {
            filtro_key = acao->data().toString();
            checked = acao->isChecked();
        } else {
            return;
        }

        QObject* painel_filtros = interface_obj->property("painel_filtros").value<QObject*>();
        sincronizar_checkbox_filtro(painel_filtros, filtro_key, checked, true);

        for (QWidget* widget : QApplication::topLevelWidgets()) {
            if (widget == painel_filtros) {
                continue;
            }
            if (auto* filtros = qobject_cast<source::tools::Filtros*>(widget)) {
                sincronizar_checkbox_filtro(filtros, filtro_key, checked, true);
                continue;
            }
            if (qobject_cast<QDialog*>(widget) == nullptr) {
                continue;
            }
            const QList<source::tools::Filtros*> paineis = widget->findChildren<source::tools::Filtros*>();
            for (source::tools::Filtros* painel : paineis) {
                if (painel == painel_filtros) {
                    continue;
                }
                sincronizar_checkbox_filtro(painel, filtro_key, checked, true);
            }
        }

        QMetaObject::invokeMethod(interface_obj, "atualizar_status");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar filtro", true);
    }
}

}
