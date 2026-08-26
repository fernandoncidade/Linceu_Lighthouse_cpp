#include "geve_07_confirmar_mudanca_idioma.h"

#include <QAbstractButton>
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

bool confirmar_mudanca_idioma(source::ui::GerenciadorEventosUI* self, const QString&) {
    try {
        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();

        QMessageBox message_box(qobject_cast<QWidget*>(interface_obj));
        message_box.setIcon(QMessageBox::Warning);
        message_box.setWindowTitle(get_text(loc, "warning"));
        message_box.setText(get_text(loc, "language_change_confirm"));
        message_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        message_box.setDefaultButton(QMessageBox::No);
        message_box.button(QMessageBox::Yes)->setText(get_text(loc, "yes"));
        message_box.button(QMessageBox::No)->setText(get_text(loc, "no"));
        return message_box.exec() == QMessageBox::Yes;
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao confirmar mudança de idioma: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            QString("%1: %2").arg(get_text(self->loc_obj(), "error"), QString::fromUtf8(e.what())));
        return false;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao confirmar mudança de idioma", true);
        QMessageBox::critical(qobject_cast<QWidget*>(self->interface_obj()), get_text(self->loc_obj(), "error"), get_text(self->loc_obj(), "error"));
        return false;
    }
}

}
