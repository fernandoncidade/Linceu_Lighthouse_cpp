#include "gmui_16_criar_submenu_exportacao.h"

#include <QAction>
#include <QMenu>
#include <QMetaObject>

#include "gmui_01_MenuPersistente.h"
#include "gmui_17_resetar_opcoes_exportacao.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace {
QString gt(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString t = fallback;
    if (loc) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty()) {
            t = translated;
        }
    }
    return t;
}
}

namespace source::ui::GerenciamentoMenusUI {

void criar_submenu_exportacao(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (!self || !menu_configuracoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* submenu = new MenuPersistente(gt(self->loc_obj(), "export_options", "Opções de exportação"), qobject_cast<QWidget*>(iface));
        menu_configuracoes->addMenu(submenu);

        auto* a1 = new QAction(gt(self->loc_obj(), "export_active_columns", "Exportar colunas ativas"), iface);
        a1->setCheckable(true);
        submenu->addAction(a1);

        auto* a2 = new QAction(gt(self->loc_obj(), "export_active_filters", "Exportar filtros ativos"), iface);
        a2->setCheckable(true);
        submenu->addAction(a2);

        auto* a3 = new QAction(gt(self->loc_obj(), "export_selected_data", "Exportar seleção"), iface);
        a3->setCheckable(true);
        submenu->addAction(a3);

        iface->setProperty("acao_exportar_colunas_ativas", QVariant::fromValue(a1));
        iface->setProperty("acao_exportar_filtros_ativos", QVariant::fromValue(a2));
        iface->setProperty("acao_exportar_selecao", QVariant::fromValue(a3));

        submenu->addSeparator();
        auto* reset = new QAction(gt(self->loc_obj(), "reset_export_options", "Resetar opções de exportação"), iface);
        QObject::connect(reset, &QAction::triggered, iface, [self]() { resetar_opcoes_exportacao(self); });
        submenu->addAction(reset);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu de exportação", true);
    }
}

}
