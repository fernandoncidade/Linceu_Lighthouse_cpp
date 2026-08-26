#include "gmui_07_configurar_menu_opcoes.h"

#include <QAction>
#include <QMenu>
#include <QMetaObject>

#include "gmui_18_criar_submenu_idiomas.h"
#include "gmui_22_exibir_sobre.h"
#include "gmui_24_exibir_manual.h"
#include "gmui_25_exibir_ajuda.h"
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

void configurar_menu_opcoes(source::ui::GerenciadorMenusUI* self, QMenu* menu_opcoes) {
    try {
        if (!self || !menu_opcoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();

        criar_submenu_idiomas(self, menu_opcoes);
        menu_opcoes->addSeparator();

        auto* manual = new QAction(gt(self->loc_obj(), "manual", "Manual"), iface);
        manual->setShortcut(QStringLiteral("Ctrl+M"));
        QObject::connect(manual, &QAction::triggered, iface, [self]() { exibir_manual(self); });
        menu_opcoes->addAction(manual);

        menu_opcoes->addSeparator();

        auto* sobre = new QAction(gt(self->loc_obj(), "about", "Sobre"), iface);
        sobre->setShortcut(QStringLiteral("Ctrl+I"));
        QObject::connect(sobre, &QAction::triggered, iface, [self]() { exibir_sobre(self); });
        menu_opcoes->addAction(sobre);

        auto* ajuda = new QAction(gt(self->loc_obj(), "help", "Ajuda"), iface);
        ajuda->setShortcut(QStringLiteral("F1"));
        QObject::connect(ajuda, &QAction::triggered, iface, [self]() { exibir_ajuda(self); });
        menu_opcoes->addAction(ajuda);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar menu opções", true);
    }
}

}
