#include "ic_03_Configurador.h"

#include <QAction>
#include <QEvent>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include "ic_01_InterfaceMonitor.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_30_botao_rotacionado.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

constexpr auto kControlButtonsContainerProperty = "container_botoes_controle";
constexpr auto kControlButtonsToggleProperty = "botao_toggle_botoes_controle";
constexpr auto kControlButtonsCollapsedProperty = "botoes_controle_recolhidos";
constexpr auto kMainSplitterProperty = "splitter_principal";

QString get_text(source::ui::LocalizadorQt* loc, const QString& key, const QString& fallback = QString()) {
    if (loc == nullptr) {
        return fallback;
    }
    const QString value = loc->get_text(key);
    return value.isEmpty() ? fallback : value;
}

void set_toggle_button_text(QWidget* button, const QString& text) {
    if (button == nullptr) {
        return;
    }

    if (auto* rotated =
            dynamic_cast<source::ui::GerenciamentoEstatisticasUI::BotaoRotacionado*>(button)) {
        rotated->setText(text);
        rotated->updateGeometry();
        return;
    }

    if (auto* push_button = qobject_cast<QPushButton*>(button)) {
        push_button->setText(text);
        push_button->adjustSize();
        push_button->updateGeometry();
    }
}

void reajustar_layout_tabela(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return;
    }

    auto* splitter =
        qobject_cast<QSplitter*>(interface_monitor->property(kMainSplitterProperty).value<QObject*>());
    if (splitter == nullptr || splitter->count() < 2) {
        return;
    }

    QWidget* painel_lateral = splitter->widget(0);
    QWidget* painel_conteudo = splitter->widget(1);
    if (painel_lateral == nullptr || painel_conteudo == nullptr) {
        return;
    }

    painel_lateral->updateGeometry();
    painel_conteudo->updateGeometry();
    splitter->refresh();

    int total = 0;
    const QList<int> atuais = splitter->sizes();
    for (const int size : atuais) {
        total += size;
    }
    if (total <= 0) {
        total = splitter->width();
    }

    int largura_lateral = painel_lateral->sizeHint().width();
    if (largura_lateral <= 0) {
        largura_lateral = painel_lateral->minimumSizeHint().width();
    }
    if (largura_lateral <= 0) {
        largura_lateral = 60;
    }

    const int largura_conteudo_minima = 160;
    const int largura_lateral_maxima = qMax(0, total - largura_conteudo_minima);
    largura_lateral = qMin(largura_lateral, largura_lateral_maxima);
    const int largura_conteudo = qMax(largura_conteudo_minima, total - largura_lateral);

    splitter->setSizes(QList<int>{largura_lateral, largura_conteudo});

    if (auto* tabela_widget = qobject_cast<QWidget*>(interface_monitor->tabela_dados)) {
        tabela_widget->updateGeometry();
        tabela_widget->update();
    }
}

void atualizar_texto_botao_controles(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return;
    }

    QWidget* toggle_button =
        qobject_cast<QWidget*>(interface_monitor->property(kControlButtonsToggleProperty).value<QObject*>());
    if (toggle_button == nullptr) {
        return;
    }

    const bool recolhidos = interface_monitor->property(kControlButtonsCollapsedProperty).toBool();
    const QString key =
        recolhidos ? QStringLiteral("expand_control_buttons") : QStringLiteral("collapse_control_buttons");
    const QString fallback =
        recolhidos ? QStringLiteral("Expandir Botões de Controle")
                   : QStringLiteral("Recolher Botões de Controle");
    set_toggle_button_text(toggle_button, get_text(interface_monitor->loc, key, fallback));
}

void alternar_botoes_controle(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return;
    }

    QWidget* container =
        qobject_cast<QWidget*>(interface_monitor->property(kControlButtonsContainerProperty).value<QObject*>());
    if (container == nullptr) {
        return;
    }

    const bool recolhidos = interface_monitor->property(kControlButtonsCollapsedProperty).toBool();
    container->setVisible(recolhidos);
    interface_monitor->setProperty(kControlButtonsCollapsedProperty, !recolhidos);
    atualizar_texto_botao_controles(interface_monitor);

    container->updateGeometry();
    if (QWidget* parent = container->parentWidget(); parent != nullptr) {
        parent->updateGeometry();
    }
    reajustar_layout_tabela(interface_monitor);
}

class WindowStateFilter final : public QObject {
public:
    explicit WindowStateFilter(source::gui::InterfaceMonitor* iface) : QObject(iface), iface_(iface) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (obj == iface_ && event != nullptr && event->type() == QEvent::WindowStateChange) {
            try {
                if (ajustando_) {
                    return QObject::eventFilter(obj, event);
                }
                if (!iface_->isMaximized() && !iface_->isFullScreen()) {
                    ajustando_ = true;
                    iface_->resize(900, 500);
                    ajustando_ = false;
                }
            } catch (...) {
                ajustando_ = false;
                source::utils::LogManager::get_logger().error("Erro no filtro de estado da janela", true);
            }
        }
        return QObject::eventFilter(obj, event);
    }

private:
    source::gui::InterfaceMonitor* iface_;
    bool ajustando_ = false;
};

}  // namespace

void Configurador::setup_ui(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        const QString icon_file = source::utils::get_icon_path("linceu_sem_fundo.ico");
        if (!QFileInfo::exists(icon_file)) {
            source::utils::LogManager::get_logger().error(
                QString("Ícone principal não encontrado: %1").arg(icon_file));
        }

        interface_monitor->setWindowTitle(get_text(interface_monitor->loc, "window_title", "Linceu Lighthouse"));
        interface_monitor->setWindowIcon(QIcon(icon_file));
        interface_monitor->setGeometry(100, 100, 900, 500);

        auto* widget_central = new QWidget(interface_monitor);
        widget_central->setContentsMargins(0, 0, 0, 0);
        interface_monitor->setCentralWidget(widget_central);

        auto* splitter_principal = new QSplitter(Qt::Horizontal, widget_central);
        splitter_principal->setContentsMargins(0, 0, 0, 0);
        auto* painel_lateral = new QWidget(splitter_principal);
        auto* layout_painel_lateral = new QVBoxLayout(painel_lateral);
        layout_painel_lateral->setContentsMargins(0, 0, 0, 0);
        layout_painel_lateral->setSpacing(0);

        auto* layout_lateral_inferior = new QVBoxLayout();
        layout_lateral_inferior->setContentsMargins(10, 10, 10, 10);
        layout_lateral_inferior->setSpacing(8);

        auto* container_botoes = new QWidget(painel_lateral);
        container_botoes->setLayout(layout_lateral_inferior);
        container_botoes->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        container_botoes->setMaximumHeight(200);

        if (interface_monitor->gerenciador_botoes_ui != nullptr) {
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "select_dir"), "selecione.ico",
                [interface_monitor]() { interface_monitor->selecionar_diretorio(); }, icon_file,
                "select_dir");
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "start_stop"), "analyze.ico",
                [interface_monitor]() { interface_monitor->alternar_analise_diretorio(); }, icon_file,
                "start_stop");
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "pause_analysis"), "pause2.ico",
                [interface_monitor]() { interface_monitor->pausar_monitoramento_ou_escaneamento(); }, icon_file,
                "pause_analysis");
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "save_as"), "save_as.ico",
                [interface_monitor]() { interface_monitor->abrir_salvar_como(); }, icon_file, "save_as");
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "statistics"), "statistics.ico",
                [interface_monitor]() { interface_monitor->mostrar_estatisticas(); }, icon_file, "statistics");
            interface_monitor->gerenciador_botoes_ui->add_button_with_label(
                layout_lateral_inferior, get_text(interface_monitor->loc, "clear_data"), "clear.ico",
                [interface_monitor]() { interface_monitor->limpar_dados(); }, icon_file, "clear_data");
        } else {
            auto criar_botao = [interface_monitor](const QString& texto, const auto& callback) {
                auto* button = new QPushButton(texto, interface_monitor);
                button->setMinimumWidth(3 * button->fontMetrics().horizontalAdvance('m'));
                button->setMaximumWidth(3 * button->fontMetrics().horizontalAdvance('m'));
                QObject::connect(button, &QPushButton::clicked, interface_monitor, callback);
                return button;
            };

            layout_lateral_inferior->addWidget(
                criar_botao(get_text(interface_monitor->loc, "select_dir"), &InterfaceMonitor::selecionar_diretorio));
            layout_lateral_inferior->addWidget(criar_botao(get_text(interface_monitor->loc, "start_stop"),
                                                            &InterfaceMonitor::alternar_analise_diretorio));
            layout_lateral_inferior->addWidget(
                criar_botao(get_text(interface_monitor->loc, "pause_analysis"),
                            &InterfaceMonitor::pausar_monitoramento_ou_escaneamento));
            layout_lateral_inferior->addWidget(
                criar_botao(get_text(interface_monitor->loc, "save_as"), &InterfaceMonitor::abrir_salvar_como));
            layout_lateral_inferior->addWidget(
                criar_botao(get_text(interface_monitor->loc, "statistics"), &InterfaceMonitor::mostrar_estatisticas));
            layout_lateral_inferior->addWidget(
                criar_botao(get_text(interface_monitor->loc, "clear_data"), &InterfaceMonitor::limpar_dados));
        }

        auto* botao_toggle_controles =
            new source::ui::GerenciamentoEstatisticasUI::BotaoRotacionado(
                get_text(interface_monitor->loc, "collapse_control_buttons",
                         "Recolher Botões de Controle"),
                painel_lateral);
        botao_toggle_controles->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        interface_monitor->setProperty(
            kControlButtonsContainerProperty, QVariant::fromValue(static_cast<QObject*>(container_botoes)));
        interface_monitor->setProperty(
            kControlButtonsToggleProperty, QVariant::fromValue(static_cast<QObject*>(botao_toggle_controles)));
        interface_monitor->setProperty(kControlButtonsCollapsedProperty, false);

        auto* container_controles = new QWidget(painel_lateral);
        auto* layout_controles = new QHBoxLayout(container_controles);
        layout_controles->setContentsMargins(10, 10, 10, 10);
        layout_controles->setSpacing(6);
        layout_controles->addWidget(botao_toggle_controles, 0, Qt::AlignLeft | Qt::AlignBottom);
        layout_controles->addWidget(container_botoes, 0, Qt::AlignLeft | Qt::AlignBottom);
        container_controles->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

        QObject::connect(botao_toggle_controles, &QPushButton::clicked, interface_monitor,
                         [interface_monitor]() { alternar_botoes_controle(interface_monitor); });
        atualizar_texto_botao_controles(interface_monitor);

        layout_painel_lateral->addStretch(1);
        layout_painel_lateral->addWidget(container_controles, 0, Qt::AlignLeft | Qt::AlignBottom);

        auto* layout_conteudo = new QVBoxLayout();
        layout_conteudo->setContentsMargins(0, 0, 0, 0);
        layout_conteudo->setSpacing(0);
        auto* container_conteudo = new QWidget(splitter_principal);
        container_conteudo->setLayout(layout_conteudo);

        auto* rotulo_diretorio =
            new QLabel(get_text(interface_monitor->loc, "no_dir"), container_conteudo);
        auto* rotulo_resultado =
            new QLabel(get_text(interface_monitor->loc, "select_to_start"), container_conteudo);
        auto* rotulo_seguranca = new QLabel(QString(), container_conteudo);
        rotulo_seguranca->setWordWrap(true);
        rotulo_seguranca->hide();

        interface_monitor->setProperty(
            "rotulo_diretorio", QVariant::fromValue(static_cast<QObject*>(rotulo_diretorio)));
        interface_monitor->setProperty(
            "rotulo_resultado", QVariant::fromValue(static_cast<QObject*>(rotulo_resultado)));
        interface_monitor->setProperty(
            "rotulo_seguranca", QVariant::fromValue(static_cast<QObject*>(rotulo_seguranca)));

        auto* layout_info_h = new QHBoxLayout();
        layout_info_h->setContentsMargins(0, 0, 0, 0);
        layout_info_h->setSpacing(0);
        layout_info_h->addWidget(rotulo_resultado, 1);

        if (interface_monitor->loc != nullptr) {
            QObject::connect(interface_monitor->loc, &source::ui::LocalizadorQt::idioma_alterado,
                             interface_monitor, [interface_monitor](const QString&) {
                                 interface_monitor->atualizar_status();
                                 atualizar_texto_botao_controles(interface_monitor);
                             });
        }

        if (interface_monitor->tabela_dados == nullptr) {
            interface_monitor->tabela_dados = new QTableWidget(container_conteudo);
            interface_monitor->setProperty(
                "tabela_dados", QVariant::fromValue(static_cast<QObject*>(interface_monitor->tabela_dados)));
        }

        auto* barra_progresso = new QProgressBar(interface_monitor);
        barra_progresso->setMaximum(100);
        barra_progresso->setMinimum(0);
        barra_progresso->setValue(0);
        barra_progresso->setTextVisible(true);
        barra_progresso->setFormat("%p%");
        barra_progresso->setFixedHeight(20);
        barra_progresso->hide();
        interface_monitor->setProperty(
            "barra_progresso", QVariant::fromValue(static_cast<QObject*>(barra_progresso)));

        auto* info_widget = new QWidget(container_conteudo);
        auto* info_layout = new QVBoxLayout(info_widget);
        info_layout->setContentsMargins(0, 0, 0, 0);
        info_layout->setSpacing(0);
        info_layout->addWidget(rotulo_diretorio);
        info_layout->addLayout(layout_info_h);
        info_layout->addWidget(rotulo_seguranca);
        info_layout->addWidget(barra_progresso);
        info_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        layout_conteudo->addWidget(info_widget, 0, Qt::AlignTop);
        layout_conteudo->addWidget(qobject_cast<QWidget*>(interface_monitor->tabela_dados), 1);

        splitter_principal->addWidget(painel_lateral);
        splitter_principal->addWidget(container_conteudo);
        splitter_principal->setStretchFactor(0, 0);
        splitter_principal->setStretchFactor(1, 1);
        interface_monitor->setProperty(
            kMainSplitterProperty, QVariant::fromValue(static_cast<QObject*>(splitter_principal)));

        auto* layout_wrapper = new QHBoxLayout(widget_central);
        layout_wrapper->setContentsMargins(0, 0, 0, 0);
        layout_wrapper->setSpacing(0);
        layout_wrapper->addWidget(splitter_principal);

        QTimer::singleShot(0, interface_monitor, [interface_monitor]() {
            if (interface_monitor == nullptr) {
                return;
            }
            interface_monitor->setProperty(kControlButtonsCollapsedProperty, false);

            QWidget* container =
                qobject_cast<QWidget*>(interface_monitor->property(kControlButtonsContainerProperty).value<QObject*>());
            if (container != nullptr) {
                container->setVisible(true);
                container->updateGeometry();
            }

            atualizar_texto_botao_controles(interface_monitor);
            reajustar_layout_tabela(interface_monitor);
        });

        interface_monitor->atualizar_status();

        auto* state_filter = new WindowStateFilter(interface_monitor);
        interface_monitor->setProperty(
            "_window_state_filter", QVariant::fromValue(static_cast<QObject*>(state_filter)));
        interface_monitor->installEventFilter(state_filter);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar interface principal", true);
    }
}

void Configurador::setup_menu_bar(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        if (interface_monitor->gerenciador_menus_ui != nullptr) {
            interface_monitor->gerenciador_menus_ui->criar_menu_principal();
            return;
        }

        QMenuBar* bar = interface_monitor->menuBar();
        if (bar == nullptr) {
            return;
        }
        bar->setContentsMargins(0, 0, 0, 0);
        bar->clear();

        QMenu* menu_arquivo = bar->addMenu(get_text(interface_monitor->loc, "file", "Arquivo"));
        QAction* acao_sair = menu_arquivo->addAction(get_text(interface_monitor->loc, "exit", "Sair"));
        QObject::connect(acao_sair, &QAction::triggered, interface_monitor, &InterfaceMonitor::sair_aplicacao);

        QMenu* menu_config =
            bar->addMenu(get_text(interface_monitor->loc, "settings", "Configurações"));
        menu_config->addAction(get_text(interface_monitor->loc, "filters", "Filtros"),
                               interface_monitor, &InterfaceMonitor::abrir_janela_filtros);

        QMenu* menu_opcoes = bar->addMenu(get_text(interface_monitor->loc, "options", "Opções"));
        menu_opcoes->addAction(get_text(interface_monitor->loc, "statistics", "Estatísticas"),
                               interface_monitor, &InterfaceMonitor::mostrar_estatisticas);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar menu principal", true);
    }
}

}  // namespace source::gui
