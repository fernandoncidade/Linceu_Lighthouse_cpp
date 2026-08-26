#include "geve_10_reiniciar_aplicativo.h"

#include <QCoreApplication>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QPointer>
#include <QProcess>
#include <QStringList>
#include <QTimer>

#include <exception>

#include "geve_08_limpar_dados_monitorados.h"
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

QString format_text(QString pattern, const QStringList& args) {
    for (const QString& value : args) {
        const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
        if (brace_pos >= 0) {
            pattern.replace(brace_pos, 2, value);
        } else if (pattern.contains(QLatin1Char('%'))) {
            pattern = pattern.arg(value);
        }
    }
    return pattern;
}
}

namespace source::ui::GerenciamentoEventosUI {

void reiniciar_aplicativo(source::ui::GerenciadorEventosUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();
        auto* rotulo = interface_obj->property("rotulo_resultado").value<QLabel*>();
        if (rotulo != nullptr) {
            rotulo->setText(get_text(loc, "language_change_confirm").split('?').value(0) + "...");
        }

        QCoreApplication::processEvents();
        limpar_dados_monitorados(self);

        QPointer<QObject> interface_guard(interface_obj);
        QTimer::singleShot(500, interface_obj, [interface_guard]() {
            try {
                const QString program = QCoreApplication::applicationFilePath();
                const QStringList args = QCoreApplication::arguments().mid(1);
                QProcess::startDetached(program, args);
                if (interface_guard != nullptr) {
                    interface_guard->setProperty("_permitir_fechamento_programatico", true);
                }
                QCoreApplication::exit(0);
            } catch (const std::exception& e) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro no timer de reinício: %1").arg(QString::fromUtf8(e.what())), true);
            } catch (...) {
                source::utils::LogManager::get_logger().error("Erro no timer de reinício", true);
            }
        });
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao reiniciar aplicativo: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            format_text(get_text(self->loc_obj(), "restart_error"), {QString::fromUtf8(e.what())}));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao reiniciar aplicativo", true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            get_text(self->loc_obj(), "restart_error"));
    }
}

}
