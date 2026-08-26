#include "geve_06_alterar_idioma.h"

#include <QAction>
#include <QMessageBox>
#include <QMetaObject>
#include <QStringList>
#include <exception>

#include "geve_07_confirmar_mudanca_idioma.h"
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

void alterar_idioma(source::ui::GerenciadorEventosUI* self, const QString& idioma) {
    try {
        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();
        QAction* acao = interface_obj->property("sender_action").value<QAction*>();

        QString novo_idioma = idioma;
        if (acao != nullptr) {
            novo_idioma = acao->data().toString();
        }

        const QString idioma_atual = (loc != nullptr) ? loc->property("idioma_atual").toString() : QString();
        if (novo_idioma.isEmpty() || novo_idioma == idioma_atual) {
            return;
        }

        const bool confirmado = confirmar_mudanca_idioma(self, novo_idioma);
        if (!confirmado) {
            const QList<QAction*> acoes = interface_obj->findChildren<QAction*>();
            for (QAction* item : acoes) {
                if (item != nullptr && item->data().toString() == idioma_atual) {
                    item->setChecked(true);
                }
            }
            return;
        }

        interface_obj->setProperty("_trocando_idioma", true);
        if (loc != nullptr) {
            QMetaObject::invokeMethod(loc, "set_idioma", Q_ARG(QString, novo_idioma));
        }
        interface_obj->setProperty("_trocando_idioma", false);

        QMessageBox::information(qobject_cast<QWidget*>(interface_obj), get_text(loc, "success"), get_text(loc, "language_changed_success"));
    } catch (const std::exception& e) {
        if (self && self->interface_obj()) {
            self->interface_obj()->setProperty("_trocando_idioma", false);
        }
        source::utils::LogManager::get_logger().error(
            QString("Erro ao alterar idioma: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::warning(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            format_text(get_text(self->loc_obj(), "language_change_error"), {QString::fromUtf8(e.what())}));
    } catch (...) {
        if (self && self->interface_obj()) {
            self->interface_obj()->setProperty("_trocando_idioma", false);
        }
        source::utils::LogManager::get_logger().error("Erro ao alterar idioma", true);
        QMessageBox::warning(qobject_cast<QWidget*>(self->interface_obj()), get_text(self->loc_obj(), "error"), get_text(self->loc_obj(), "language_change_error"));
    }
}

}
