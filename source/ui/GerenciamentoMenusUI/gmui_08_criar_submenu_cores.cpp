#include "gmui_08_criar_submenu_cores.h"

#include <QAction>
#include <QMenu>
#include <QMetaObject>

#include "gmui_01_MenuPersistente.h"
#include "gmui_02_GerenciadorCores.h"
#include "gmui_09_criar_icone_cor.h"
#include "gmui_10_abrir_dialogo_cor.h"
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

void criar_submenu_cores(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (!self || !menu_configuracoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* gerenciador_cores = qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(
            iface->property("gerenciador_cores").value<QObject*>());

        auto* submenu_cores = new MenuPersistente(gt(self->loc_obj(), "configure_colors", "Configurar cores"), qobject_cast<QWidget*>(iface));
        menu_configuracoes->addMenu(submenu_cores);
        auto* submenu_ops = new MenuPersistente(gt(self->loc_obj(), "operation_colors", "Cores por operação"), qobject_cast<QWidget*>(iface));
        submenu_cores->addMenu(submenu_ops);

        const QStringList tipos = {"op_renamed", "op_added", "op_deleted", "op_modified", "op_moved", "op_scanned"};
        for (const QString& op : tipos) {
            auto* acao = new QAction(gt(self->loc_obj(), op, op), iface);
            acao->setData(op);
            const QString cor_atual = (gerenciador_cores != nullptr)
                                          ? gerenciador_cores->obter_cor_hex(op)
                                          : QStringLiteral("#333333");
            acao->setIcon(criar_icone_cor(self, cor_atual));
            QObject::connect(acao, &QAction::triggered, iface, [self, op]() { abrir_dialogo_cor(self, op); });
            submenu_ops->addAction(acao);
        }

        submenu_cores->addSeparator();
        auto* resetar = new QAction(gt(self->loc_obj(), "reset_colors", "Resetar cores"), iface);
        QObject::connect(resetar, &QAction::triggered, iface, [iface]() {
            QMetaObject::invokeMethod(iface, "resetar_cores");
        });
        submenu_cores->addAction(resetar);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu cores", true);
    }
}

}
