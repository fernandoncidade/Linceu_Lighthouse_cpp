#include "gfil_06_sincronizar_com_menu_principal.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QVariantMap>

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void sincronizar_com_menu_principal(source::tools::Filtros* self) {
    try {
        if (!self) return;

        QMainWindow* main_window = nullptr;

        QWidget* parent_widget = self->parentWidget();
        while (parent_widget != nullptr) {
            if (auto* mw = qobject_cast<QMainWindow*>(parent_widget);
                mw != nullptr && mw->property("gerenciador_menus_ui").isValid()) {
                main_window = mw;
                break;
            }
            parent_widget = parent_widget->parentWidget();
        }

        if (main_window == nullptr) {
            for (QWidget* widget : QApplication::topLevelWidgets()) {
                if (auto* mw = qobject_cast<QMainWindow*>(widget);
                    mw != nullptr && mw->property("gerenciador_menus_ui").isValid()) {
                    main_window = mw;
                    break;
                }
            }
        }

        if (main_window == nullptr) {
            for (QWidget* widget : QApplication::topLevelWidgets()) {
                if (auto* mw = qobject_cast<QMainWindow*>(widget); mw != nullptr && mw->menuBar() != nullptr) {
                    main_window = mw;
                    break;
                }
            }
        }

        const QVariantMap checkboxes = self->property("checkboxes_operacao").toMap();
        if (checkboxes.isEmpty()) return;

        if (main_window != nullptr) {
            QMenuBar* menu_bar = main_window->menuBar();
            QMenu* menu_configuracoes = nullptr;
            QMenu* submenu_filtros = nullptr;

            const QString settings_text = self->localizador() ? self->localizador()->get_text("settings")
                                                              : QStringLiteral("Configurações");
            const QString filters_text = self->localizador() ? self->localizador()->get_text("filters")
                                                             : QStringLiteral("Filtros");

            if (menu_bar != nullptr) {
                for (QAction* action : menu_bar->actions()) {
                    if (action != nullptr && action->text() == settings_text) {
                        menu_configuracoes = action->menu();
                        break;
                    }
                }
            }

            if (menu_configuracoes != nullptr) {
                for (QAction* action : menu_configuracoes->actions()) {
                    if (action != nullptr && action->text() == filters_text) {
                        submenu_filtros = action->menu();
                        break;
                    }
                }
            }

            const auto sync_checkbox = [&](QAction* action) {
                if (action == nullptr) return;
                const QString filtro = action->data().toString();
                auto* cb = qobject_cast<QCheckBox*>(checkboxes.value(filtro).value<QObject*>());
                if (!filtro.isEmpty() && cb != nullptr) {
                    cb->blockSignals(true);
                    cb->setChecked(action->isChecked());
                    cb->blockSignals(false);
                }
            };

            if (submenu_filtros != nullptr) {
                for (QAction* action : submenu_filtros->actions()) sync_checkbox(action);
            } else {
                for (QAction* action : main_window->findChildren<QAction*>()) sync_checkbox(action);
            }
        } else {
            source::utils::LogManager::get_logger().warning(
                QStringLiteral("Janela principal não encontrada para sincronização de filtros"));

            auto set_checked = [&](const QString& op, const QString& estado_key) {
                auto* cb = qobject_cast<QCheckBox*>(checkboxes.value(op).value<QObject*>());
                if (cb != nullptr) {
                    cb->setChecked(!source::tools::AdministradorFiltros::filtros_estado.value(estado_key, true));
                }
            };

            set_checked("op_moved", "ignorar_mover");
            set_checked("op_renamed", "ignorar_renomeados");
            set_checked("op_added", "ignorar_adicionados");
            set_checked("op_deleted", "ignorar_excluidos");
            set_checked("op_modified", "ignorar_data_modificados");
            set_checked("op_scanned", "ignorar_escaneados");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao sincronizar filtros com menu principal", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
