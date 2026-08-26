#include "geui_19_toggle_painel_selecao.h"

#include <QMetaObject>
#include <QSplitter>
#include <QWidget>
#include <QPushButton>

#include "geui_30_botao_rotacionado.h"
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

void set_toggle_button_text(QWidget* widget, const QString& text) {
    if (widget == nullptr) return;

    if (auto* rotated = dynamic_cast<source::ui::GerenciamentoEstatisticasUI::BotaoRotacionado*>(widget)) {
        rotated->setText(text);
        rotated->updateGeometry();
        return;
    }

    if (auto* button = qobject_cast<QPushButton*>(widget)) {
        button->setText(text);
        button->adjustSize();
        button->updateGeometry();
    }
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void toggle_painel_selecao(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();

        auto* panel = iface->property("painel_selecao_interno").value<QWidget*>();
        auto* panelContainer = iface->property("painel_selecao").value<QWidget*>();
        auto* btn_widget = iface->property("btn_toggle_painel").value<QWidget*>();
        auto* btn = qobject_cast<QPushButton*>(btn_widget);
        auto* splitter = iface->property("splitter").value<QSplitter*>();
        if (!panel || !panelContainer || !btn_widget || !btn) return;

        const int larguraBotao = btn->width();
        bool recolhido = iface->property("painel_recolhido").toBool();
        int tamanhoOriginal = iface->property("tamanho_painel_original").toInt();

        if (recolhido) {
            panel->setVisible(true);
            panel->setFixedWidth(tamanhoOriginal);
            panelContainer->setFixedWidth(larguraBotao + tamanhoOriginal);
            set_toggle_button_text(btn_widget, get_text(self->loc_obj(), "hide_selection_panel", "Ocultar painel de seleção"));
        } else {
            panel->setVisible(false);
            panel->setFixedWidth(0);
            panelContainer->setFixedWidth(larguraBotao);
            set_toggle_button_text(btn_widget, get_text(self->loc_obj(), "expand_selection_panel", "Expandir painel de seleção"));
        }

        iface->setProperty("painel_recolhido", !recolhido);

        if (splitter) {
            QList<int> sizes = splitter->sizes();
            int total = 0;
            for (int s : sizes) total += s;
            splitter->setSizes({panelContainer->width(), qMax(100, total - panelContainer->width())});
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar painel de seleção", true);
    }
}

}
