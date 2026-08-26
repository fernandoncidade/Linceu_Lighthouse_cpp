#include "geui_20_atualizar_textos_painel_selecao.h"

#include <QCheckBox>
#include <QLabel>
#include <QMetaObject>
#include <QPushButton>
#include <QWidget>

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

void atualizar_textos_painel_selecao(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        if (auto* c = iface->property("checkbox_todos").value<QCheckBox*>()) c->setText(get_text(loc, "select_all", "Selecionar todos"));
        if (auto* b = iface->property("btn_salvar_selecionados").value<QPushButton*>()) b->setText(get_text(loc, "save_selected", "Salvar selecionados"));
        if (auto* b = iface->property("btn_salvar_todos").value<QPushButton*>()) b->setText(get_text(loc, "save_all", "Salvar todos"));
        if (auto* b = iface->property("btn_atualizar").value<QPushButton*>()) b->setText(get_text(loc, "refresh", "Atualizar"));

        if (auto* titulo = iface->property("titulo_selecionar_graficos").value<QLabel*>()) {
            titulo->setText(get_text(loc, "select_graphs", "Selecionar gráficos"));
        } else if (auto* panel = iface->property("painel_selecao_interno").value<QWidget*>()) {
            if (auto* titulo_fallback = panel->findChild<QLabel*>()) {
                titulo_fallback->setText(get_text(loc, "select_graphs", "Selecionar gráficos"));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar textos do painel de seleção", true);
    }
}

}
