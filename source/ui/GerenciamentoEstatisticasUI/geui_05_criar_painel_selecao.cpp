#include "geui_05_criar_painel_selecao.h"

#include <QCheckBox>
#include <QDialog>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QSizePolicy>
#include <QTimer>
#include <QVariantList>
#include <QVBoxLayout>

#include "geui_02_salvar_todos_graficos.h"
#include "geui_07_criar_botao_toggle_painel.h"
#include "geui_12_alternar_todos_checkboxes.h"
#include "geui_15_atualizar_graficos.h"
#include "geui_18_salvar_graficos_selecionados.h"
#include "geui_26_calcular_largura_ideal.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}

void ajustar_altura_painel_selecao(QObject* iface) {
    if (iface == nullptr || iface->property("painel_recolhido").toBool()) {
        return;
    }

    auto* panel = iface->property("painel_selecao_interno").value<QWidget*>();
    auto* container = iface->property("painel_selecao").value<QWidget*>();
    auto* scroll = iface->property("scroll_selecionar_graficos").value<QScrollArea*>();
    auto* titulo = iface->property("titulo_selecionar_graficos").value<QLabel*>();
    auto* btn_toggle = iface->property("btn_toggle_painel").value<QWidget*>();
    auto* btn_salvar_selecionados = iface->property("btn_salvar_selecionados").value<QPushButton*>();
    auto* btn_salvar_todos = iface->property("btn_salvar_todos").value<QPushButton*>();
    auto* btn_atualizar = iface->property("btn_atualizar").value<QPushButton*>();
    auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>();
    auto* splitter = iface->property("splitter").value<QSplitter*>();

    if (panel == nullptr || scroll == nullptr) {
        return;
    }

    auto* scroll_widget = scroll->widget();
    if (panel->layout() != nullptr) {
        panel->layout()->activate();
    }
    if (scroll_widget != nullptr && scroll_widget->layout() != nullptr) {
        scroll_widget->layout()->activate();
        scroll_widget->adjustSize();
    }

    int altura_disponivel = 0;
    if (splitter != nullptr) {
        altura_disponivel = splitter->contentsRect().height();
    }
    if (altura_disponivel <= 0 && dialog != nullptr) {
        altura_disponivel = dialog->contentsRect().height() - 24;
    }
    if (altura_disponivel <= 0 && panel->parentWidget() != nullptr) {
        altura_disponivel = panel->parentWidget()->contentsRect().height();
    }
    if (altura_disponivel <= 0) {
        return;
    }

    const QMargins margins = panel->layout() != nullptr ? panel->layout()->contentsMargins() : QMargins();
    const int spacing = panel->layout() != nullptr ? qMax(0, panel->layout()->spacing()) : 5;
    const int titulo_h = titulo != nullptr ? titulo->sizeHint().height() : 24;
    const int botoes_spacing = 5;
    const int botoes_h =
        (btn_salvar_selecionados != nullptr ? btn_salvar_selecionados->sizeHint().height() : 28) +
        (btn_salvar_todos != nullptr ? btn_salvar_todos->sizeHint().height() : 28) +
        (btn_atualizar != nullptr ? btn_atualizar->sizeHint().height() : 28) +
        (2 * botoes_spacing);

    const int altura_toggle = btn_toggle != nullptr ? btn_toggle->sizeHint().height() : 0;
    const int altura_scroll_conteudo = scroll_widget != nullptr ? scroll_widget->sizeHint().height() : scroll->sizeHint().height();
    const int borda_scroll = (2 * scroll->frameWidth()) + 15;
    const int altura_fixa = margins.top() + margins.bottom() + titulo_h + botoes_h + (2 * spacing);
    const int altura_scroll_desejada = qMax(40, altura_scroll_conteudo + borda_scroll);
    const int altura_scroll_maxima = qMax(40, altura_disponivel - altura_fixa);
    const int altura_scroll_final = qMin(altura_scroll_desejada, altura_scroll_maxima);
    const int altura_painel = altura_fixa + altura_scroll_final;

    scroll->setMinimumHeight(altura_scroll_final);
    scroll->setMaximumHeight(altura_scroll_final);
    panel->setMinimumHeight(altura_painel);
    panel->setMaximumHeight(altura_painel);
    panel->setFixedHeight(altura_painel);
    if (container != nullptr) {
        container->setMinimumHeight(qMax(altura_toggle, altura_painel));
    }
    scroll->updateGeometry();
    panel->updateGeometry();
    if (container != nullptr) {
        container->updateGeometry();
    }
}

class AjustadorAlturaPainelSelecao final : public QObject {
public:
    explicit AjustadorAlturaPainelSelecao(QObject* iface, QObject* parent = nullptr)
        : QObject(parent),
          iface_(iface) {}

    void agendar() {
        if (pendente_) {
            return;
        }
        pendente_ = true;
        QTimer::singleShot(0, this, [this]() {
            pendente_ = false;
            ajustar_altura_painel_selecao(iface_.data());
        });
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        Q_UNUSED(watched)
        if (event != nullptr) {
            switch (event->type()) {
                case QEvent::Resize:
                case QEvent::Show:
                case QEvent::LayoutRequest:
                case QEvent::Polish:
                    agendar();
                    break;
                default:
                    break;
            }
        }
        return false;
    }

private:
    QPointer<QObject> iface_;
    bool pendente_ = false;
};
}

namespace source::ui::GerenciamentoEstatisticasUI {

QWidget* criar_painel_selecao(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos) {
    try {
        if (!self || !self->interface_obj()) {
            return nullptr;
        }
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        auto* container = new QWidget();
        container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        auto* layout = new QHBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto* btn = criar_botao_toggle_painel(self, get_text(loc, "hide_selection_panel", "Ocultar"));
        btn->setFixedWidth(25);
        layout->addWidget(btn, 0, Qt::AlignLeft | Qt::AlignTop);

        auto* panel = new QFrame(container);
        panel->setFrameStyle(QFrame::StyledPanel);
        panel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        const int max_width = calcular_largura_ideal(self, graficos);
        panel->setFixedWidth(max_width);

        auto* panelLayout = new QVBoxLayout(panel);
        panelLayout->setContentsMargins(5, 5, 5, 5);
        auto* titulo_selecao = new QLabel(get_text(loc, "select_graphs", "Selecionar gráficos"), panel);
        panelLayout->addWidget(titulo_selecao);

        auto* scroll = new QScrollArea(panel);
        scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto* scrollWidget = new QWidget(scroll);
        scrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        auto* checksLayout = new QVBoxLayout(scrollWidget);
        checksLayout->setSizeConstraint(QLayout::SetMinimumSize);

        auto* todos = new QCheckBox(get_text(loc, "select_all", "Selecionar todos"), scrollWidget);
        todos->setChecked(true);
        todos->setTristate(true);
        checksLayout->addWidget(todos);
        auto* sep = new QFrame(scrollWidget);
        sep->setFrameStyle(QFrame::HLine);
        checksLayout->addWidget(sep);
        checksLayout->addStretch();

        scrollWidget->setLayout(checksLayout);
        scroll->setWidget(scrollWidget);
        scroll->setWidgetResizable(true);
        scroll->setMinimumHeight(40);
        panelLayout->addWidget(scroll, 1);

        auto* buttonsLayout = new QVBoxLayout();
        buttonsLayout->setSpacing(5);

        auto* btn_salvar_selecionados = new QPushButton(get_text(loc, "save_selected", "Salvar selecionados"), panel);
        btn_salvar_selecionados->setIcon(QIcon(source::utils::get_icon_path("salvar.ico")));
        QObject::connect(btn_salvar_selecionados, &QPushButton::clicked, [self]() { salvar_graficos_selecionados(self); });
        buttonsLayout->addWidget(btn_salvar_selecionados);

        auto* btn_salvar_todos = new QPushButton(get_text(loc, "save_all", "Salvar todos"), panel);
        btn_salvar_todos->setIcon(QIcon(source::utils::get_icon_path("salvar.ico")));
        QObject::connect(btn_salvar_todos, &QPushButton::clicked, [self]() { salvar_todos_graficos(self); });
        buttonsLayout->addWidget(btn_salvar_todos);

        auto* btn_atualizar = new QPushButton(get_text(loc, "refresh", "Atualizar"), panel);
        btn_atualizar->setIcon(QIcon(source::utils::get_icon_path("atualizar.ico")));
        QObject::connect(btn_atualizar, &QPushButton::clicked, [self]() { atualizar_graficos(self); });
        buttonsLayout->addWidget(btn_atualizar);

        panelLayout->addLayout(buttonsLayout, 0);
        layout->addWidget(panel, 0, Qt::AlignTop);
        container->setFixedWidth(25 + max_width);

        iface->setProperty("btn_toggle_painel", QVariant::fromValue(btn));
        iface->setProperty("painel_selecao_interno", QVariant::fromValue(panel));
        iface->setProperty("painel_selecao", QVariant::fromValue(container));
        iface->setProperty("scroll_selecionar_graficos", QVariant::fromValue(scroll));
        iface->setProperty("checkbox_todos", QVariant::fromValue(todos));
        iface->setProperty("checkboxes_layout", QVariant::fromValue(checksLayout));
        iface->setProperty("titulo_selecionar_graficos", QVariant::fromValue(titulo_selecao));
        iface->setProperty("btn_salvar_selecionados", QVariant::fromValue(btn_salvar_selecionados));
        iface->setProperty("btn_salvar_todos", QVariant::fromValue(btn_salvar_todos));
        iface->setProperty("btn_atualizar", QVariant::fromValue(btn_atualizar));
        iface->setProperty("painel_recolhido", false);
        iface->setProperty("tamanho_painel_original", max_width);
        QObject::connect(todos, &QCheckBox::clicked, [self]() { alternar_todos_checkboxes(self); });

        auto* ajustador = new AjustadorAlturaPainelSelecao(iface, container);
        iface->setProperty("ajustador_altura_painel_selecao", QVariant::fromValue(static_cast<QObject*>(ajustador)));
        container->installEventFilter(ajustador);
        panel->installEventFilter(ajustador);
        scroll->installEventFilter(ajustador);
        scrollWidget->installEventFilter(ajustador);
        if (auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>()) {
            dialog->installEventFilter(ajustador);
        }
        ajustador->agendar();

        return container;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar painel de seleção", true);
        return nullptr;
    }
}

}
