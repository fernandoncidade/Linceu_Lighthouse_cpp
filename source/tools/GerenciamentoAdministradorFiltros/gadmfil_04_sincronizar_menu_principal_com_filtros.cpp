#include "gadmfil_04_sincronizar_menu_principal_com_filtros.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QVariantMap>

#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

void sincronizar_menu_principal_com_filtros(source::tools::AdministradorFiltros* self) {
    try {
        if (!self || !self->parent_obj()) return;

        source::tools::Filtros* parent = self->parent_obj();
        source::gui::InterfaceMonitor* main_window = nullptr;
        for (QWidget* widget : QApplication::topLevelWidgets()) {
            auto* mw = qobject_cast<source::gui::InterfaceMonitor*>(widget);
            if (mw != nullptr) {
                main_window = mw;
                break;
            }
        }
        if (!main_window || !main_window->menuBar()) return;

        QMenu* menu_config = nullptr;
        QMenu* submenu_filtros = nullptr;
        const QString settingsText = parent->localizador() ? parent->localizador()->get_text("settings") : QStringLiteral("Configurações");
        const QString filtersText = parent->localizador() ? parent->localizador()->get_text("filters") : QStringLiteral("Filtros");

        for (QAction* action : main_window->menuBar()->actions()) {
            if (action && action->text() == settingsText) {
                menu_config = action->menu();
                break;
            }
        }

        if (menu_config) {
            for (QAction* action : menu_config->actions()) {
                if (action && action->text() == filtersText) {
                    submenu_filtros = action->menu();
                    break;
                }
            }
        }

        const QVariantMap checkboxes = parent->property("checkboxes_operacao").toMap();
        auto syncAction = [&](QAction* acao) {
            if (!acao) return;
            const QString filtro = acao->data().toString();
            auto* cb = qobject_cast<QCheckBox*>(checkboxes.value(filtro).value<QObject*>());
            if (!filtro.isEmpty() && cb) {
                acao->blockSignals(true);
                acao->setChecked(cb->isChecked());
                acao->blockSignals(false);
            }
        };

        if (submenu_filtros) {
            for (QAction* acao : submenu_filtros->actions()) syncAction(acao);
        } else {
            for (QAction* acao : main_window->findChildren<QAction*>()) syncAction(acao);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao sincronizar menu principal com filtros", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
