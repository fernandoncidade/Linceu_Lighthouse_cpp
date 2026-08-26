#include "geve_11_atualizar_interface.h"

#include <QAction>
#include <QAbstractButton>
#include <QApplication>
#include <QDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMetaObject>

#include <exception>

#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}
}

namespace source::ui::GerenciamentoEventosUI {

void atualizar_interface(source::ui::GerenciadorEventosUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();

        for (auto* widget : QApplication::topLevelWidgets()) {
            auto* dialog = qobject_cast<QDialog*>(widget);
            if (dialog != nullptr) {
                QObject* filtro_panel = dialog->property("filtro_panel").value<QObject*>();
                if (filtro_panel != nullptr) {
                    QMetaObject::invokeMethod(filtro_panel, "atualizar_interface");
                }
            }

            auto* msg = qobject_cast<QMessageBox*>(widget);
            if (msg != nullptr) {
                if (msg->standardButtons() & QMessageBox::Yes) msg->button(QMessageBox::Yes)->setText(get_text(loc, "yes"));
                if (msg->standardButtons() & QMessageBox::No) msg->button(QMessageBox::No)->setText(get_text(loc, "no"));
                if (msg->standardButtons() & QMessageBox::Ok) msg->button(QMessageBox::Ok)->setText(get_text(loc, "ok"));
                if (msg->standardButtons() & QMessageBox::Cancel) msg->button(QMessageBox::Cancel)->setText(get_text(loc, "cancel"));
            }
        }

        QMenu* context_menu = interface_obj->property("context_menu").value<QMenu*>();
        if (context_menu != nullptr) {
            for (auto* action : context_menu->actions()) {
                const QString key = action->data().toString();
                if (key.startsWith("menu_")) {
                    action->setText(get_text(loc, key));
                }
            }
        }
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao atualizar interface do gerenciador de eventos: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface do gerenciador de eventos", true);
    }
}

}
