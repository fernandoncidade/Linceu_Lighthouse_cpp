#include "geui_15_atualizar_graficos.h"

#include <exception>

#include <QMessageBox>
#include <QMetaObject>
#include <QWidget>

#include "geui_16_atualizar_graficos_sem_fechar.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_graficos(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (self != nullptr && self->interface_obj() != nullptr) {
            QObject* iface = self->interface_obj();
            if (iface->property("_trocando_idioma").toBool() ||
                iface->property("_atualizando_idioma").toBool()) {
                return;
            }
        }
        atualizar_graficos_sem_fechar(self);
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao atualizar gráficos: %1").arg(QString::fromUtf8(e.what())), true);
        if (self && self->interface_obj()) {
            QWidget* parent = qobject_cast<QWidget*>(self->interface_obj());
            QMessageBox::critical(
                parent,
                get_text(self->loc_obj(), "error", "Error"),
                get_text(self->loc_obj(), "stats_error", "Erro ao atualizar gráficos").arg(QString::fromUtf8(e.what())));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar gráficos", true);
        if (self && self->interface_obj()) {
            QWidget* parent = qobject_cast<QWidget*>(self->interface_obj());
            QMessageBox::critical(
                parent,
                get_text(self->loc_obj(), "error", "Error"),
                get_text(self->loc_obj(), "stats_error", "Erro ao atualizar gráficos"));
        }
    }
}

}
