#include "gadmfil_06_atualizar_contagem_eventos_monitorados.h"

#include <QApplication>
#include <QTableView>
#include <QTableWidget>

#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

namespace {
int total_eventos_widget(QWidget* widget) {
    if (auto* tabela = qobject_cast<QTableWidget*>(widget)) {
        return tabela->rowCount();
    }
    if (auto* view = qobject_cast<QTableView*>(widget)) {
        return (view->model() != nullptr) ? view->model()->rowCount() : 0;
    }
    return 0;
}
}  // namespace

QString atualizar_contagem_eventos_monitorados(source::tools::AdministradorFiltros* self) {
    try {
        if (!self) return QStringLiteral("Eventos monitorados: 0");

        source::ui::LocalizadorQt fallback_loc;
        source::ui::LocalizadorQt* loc = (self->parent_obj() && self->parent_obj()->localizador()) ? self->parent_obj()->localizador() : &fallback_loc;

        if (self->parent_obj()) {
            const int total = total_eventos_widget(self->parent_obj()->tabela_dados());
            return QStringLiteral("%1: %2").arg(loc->get_text("events_monitored")).arg(total);
        }

        for (QWidget* widget : QApplication::topLevelWidgets()) {
            if (auto* monitor = qobject_cast<source::gui::InterfaceMonitor*>(widget); monitor != nullptr) {
                const int total = total_eventos_widget(qobject_cast<QWidget*>(monitor->tabela_dados));
                return QStringLiteral("%1: %2").arg(loc->get_text("events_monitored")).arg(total);
            }

            QObject* tabela_obj = widget->property("tabela_dados").value<QObject*>();
            const int total = total_eventos_widget(qobject_cast<QWidget*>(tabela_obj));
            if (total > 0) return QStringLiteral("%1: %2").arg(loc->get_text("events_monitored")).arg(total);
        }

        return QStringLiteral("%1: 0").arg(loc->get_text("events_monitored"));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar contagem de eventos monitorados", true);
        return QStringLiteral("Eventos monitorados: 0");
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
