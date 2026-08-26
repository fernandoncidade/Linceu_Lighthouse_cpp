#include "gmui_04_criar_menu_principal.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QMetaObject>

#include "gmui_01_MenuPersistente.h"
#include "gmui_05_configurar_menu_arquivo.h"
#include "gmui_06_configurar_menu_configuracoes.h"
#include "gmui_07_configurar_menu_opcoes.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
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

void criar_menu_principal(source::ui::GerenciadorMenusUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (iface->property("_criando_menu_principal").toBool()) {
            return;
        }
        iface->setProperty("_criando_menu_principal", true);

        auto* mw = qobject_cast<QMainWindow*>(self->interface_obj());
        if (!mw) {
            iface->setProperty("_criando_menu_principal", false);
            return;
        }

        QMenuBar* menu_bar = mw->menuBar();
        menu_bar->clear();

        auto* menu_arquivo = new MenuPersistente(get_text(self->loc_obj(), "file_menu", "Arquivo"), mw);
        auto* menu_configuracoes = new MenuPersistente(get_text(self->loc_obj(), "settings", "Configurações"), mw);
        auto* menu_opcoes = new MenuPersistente(get_text(self->loc_obj(), "options_menu", "Opções"), mw);

        menu_bar->addMenu(menu_arquivo);
        menu_bar->addMenu(menu_configuracoes);
        menu_bar->addMenu(menu_opcoes);

        configurar_menu_arquivo(self, menu_arquivo);
        configurar_menu_configuracoes(self, menu_configuracoes);
        configurar_menu_opcoes(self, menu_opcoes);
        iface->setProperty("_criando_menu_principal", false);
    } catch (...) {
        if (self && self->interface_obj()) {
            self->interface_obj()->setProperty("_criando_menu_principal", false);
        }
        source::utils::LogManager::get_logger().error("Erro ao criar menu principal", true);
    }
}

}
