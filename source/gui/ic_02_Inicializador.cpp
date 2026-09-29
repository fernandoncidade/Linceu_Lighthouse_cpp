#include "ic_02_Inicializador.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMutex>
#include <QStatusBar>
#include <QTableWidget>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include "ic_01_InterfaceMonitor.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/ui/ui_05_GerenciadorProgressoUI.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {

void Inicializador::inicializar_componentes(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        if (interface_monitor->tabela_dados == nullptr) {
            interface_monitor->tabela_dados = new QTableWidget(interface_monitor);
        }
        interface_monitor->setProperty("tabela_dados",
                                       QVariant::fromValue(static_cast<QObject*>(interface_monitor->tabela_dados)));
        if (!interface_monitor->mutex_interface) {
            interface_monitor->mutex_interface = std::make_unique<QMutex>();
        }
        interface_monitor->setProperty("mutex", QVariant::fromValue(interface_monitor->mutex_interface.get()));

        auto* status = interface_monitor->statusBar();
        if (status != nullptr) {
            auto* container_rodape = new QWidget(interface_monitor);
            auto* layout_rodape = new QVBoxLayout(container_rodape);
            layout_rodape->setContentsMargins(4, 2, 4, 2);
            layout_rodape->setSpacing(2);

            auto* primeira_linha = new QWidget(container_rodape);
            auto* layout_primeira_linha = new QHBoxLayout(primeira_linha);
            layout_primeira_linha->setContentsMargins(0, 0, 0, 0);
            layout_primeira_linha->setSpacing(8);

            auto* label_contagem = new QLabel(container_rodape);
            auto* rotulo_contador_eventos = new QLabel(container_rodape);
            auto* rotulo_rodape_resumo_seguranca = new QLabel(container_rodape);

            rotulo_rodape_resumo_seguranca->setWordWrap(true);
            rotulo_rodape_resumo_seguranca->hide();

            layout_primeira_linha->addWidget(label_contagem, 1);
            layout_primeira_linha->addWidget(rotulo_contador_eventos, 0);
            layout_rodape->addWidget(primeira_linha);
            layout_rodape->addWidget(rotulo_rodape_resumo_seguranca);

            status->addPermanentWidget(container_rodape, 1);
            interface_monitor->setProperty("status_bar", QVariant::fromValue(static_cast<QObject*>(status)));
            interface_monitor->setProperty("label_contagem",
                                           QVariant::fromValue(static_cast<QObject*>(label_contagem)));
            interface_monitor->setProperty(
                "rotulo_contador_eventos",
                QVariant::fromValue(static_cast<QObject*>(rotulo_contador_eventos)));
            interface_monitor->setProperty(
                "rotulo_rodape_resumo_seguranca",
                QVariant::fromValue(static_cast<QObject*>(rotulo_rodape_resumo_seguranca)));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar componentes", true);
    }
}

void Inicializador::inicializar_gerenciadores(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        if (interface_monitor->loc == nullptr) {
            interface_monitor->loc = new source::ui::LocalizadorQt(interface_monitor);
        }
        interface_monitor->setProperty("loc",
                                       QVariant::fromValue(static_cast<QObject*>(interface_monitor->loc)));

        if (interface_monitor->gerenciador_tabela == nullptr) {
            interface_monitor->gerenciador_tabela = new source::services::GerenciadorTabela(interface_monitor);
            interface_monitor->gerenciador_tabela->_carregar_configuracoes_cores();
            QObject::connect(interface_monitor->gerenciador_tabela,
                             &source::services::GerenciadorTabela::cores_processadas, interface_monitor,
                             &source::gui::InterfaceMonitor::atualizar_cores_tabela);
        }
        interface_monitor->setProperty(
            "gerenciador_tabela",
            QVariant::fromValue(static_cast<QObject*>(interface_monitor->gerenciador_tabela)));

        if (interface_monitor->gerenciador_colunas == nullptr) {
            interface_monitor->gerenciador_colunas =
                new source::services::GerenciadorColunas(interface_monitor);
        }
        interface_monitor->setProperty(
            "gerenciador_colunas",
            QVariant::fromValue(static_cast<QObject*>(interface_monitor->gerenciador_colunas)));

        if (interface_monitor->gerenciador_botoes == nullptr) {
            interface_monitor->gerenciador_botoes =
                new source::ui::GerenciadorBotoes(interface_monitor, interface_monitor->loc);
        }

        if (interface_monitor->gerenciador_botoes_ui == nullptr) {
            interface_monitor->gerenciador_botoes_ui =
                new source::ui::GerenciadorBotoesUI(interface_monitor);
        }

        if (interface_monitor->painel_filtros == nullptr) {
            auto* tabela_widget = qobject_cast<QWidget*>(interface_monitor->tabela_dados);
            if (tabela_widget != nullptr) {
                interface_monitor->painel_filtros = new source::tools::Filtros(
                    tabela_widget, interface_monitor->loc, interface_monitor);
                QObject::connect(interface_monitor->painel_filtros,
                                 &source::tools::Filtros::filtroAplicado, interface_monitor,
                                 &source::gui::InterfaceMonitor::atualizar_status);
            }
        }
        interface_monitor->setProperty(
            "painel_filtros",
            QVariant::fromValue(static_cast<QObject*>(interface_monitor->painel_filtros)));

        if (interface_monitor->gerenciador_menus_ui == nullptr) {
            interface_monitor->gerenciador_menus_ui =
                new source::ui::GerenciadorMenusUI(interface_monitor);
        }
        if (interface_monitor->gerenciador_eventos_ui == nullptr) {
            interface_monitor->gerenciador_eventos_ui =
                new source::ui::GerenciadorEventosUI(interface_monitor);
        }
        if (interface_monitor->gerenciador_progresso_ui == nullptr) {
            interface_monitor->gerenciador_progresso_ui =
                new source::ui::GerenciadorProgressoUI(interface_monitor);
        }
        if (interface_monitor->gerenciador_estatisticas_ui == nullptr) {
            interface_monitor->gerenciador_estatisticas_ui =
                new source::ui::GerenciadorEstatisticasUI(interface_monitor);
        }

        QVariantMap tipos_operacao;
        tipos_operacao.insert("op_renamed", interface_monitor->loc->get_text("op_renamed"));
        tipos_operacao.insert("op_added", interface_monitor->loc->get_text("op_added"));
        tipos_operacao.insert("op_deleted", interface_monitor->loc->get_text("op_deleted"));
        tipos_operacao.insert("op_modified", interface_monitor->loc->get_text("op_modified"));
        tipos_operacao.insert("op_moved", interface_monitor->loc->get_text("op_moved"));
        tipos_operacao.insert("op_scanned", interface_monitor->loc->get_text("op_scanned"));
        interface_monitor->setProperty("tipos_operacao", tipos_operacao);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar gerenciadores", true);
    }
}

void Inicializador::inicializar_atributos(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        interface_monitor->setProperty("diretorio_atual", QVariant());
        interface_monitor->setProperty("ultimo_salvamento", QVariant());
        interface_monitor->setProperty("excluidos_recentemente", QVariantMap());
        interface_monitor->setProperty("contador_eventos", 0);
        interface_monitor->setProperty("ultimo_update_status", 0);
        interface_monitor->setProperty("_permitir_carga_db", false);
        interface_monitor->setProperty("_scan_em_execucao", false);
        interface_monitor->setProperty("_scan_contador", 0);
        interface_monitor->setProperty("_scan_total", 0);
        interface_monitor->setProperty("_aceitar_eventos_monitoramento", false);
        interface_monitor->setProperty("_traducao_em_execucao", false);
        interface_monitor->setProperty("_traducao_total_unidades", 0);
        interface_monitor->setProperty("_traducao_concluidas_unidades", 0);
        interface_monitor->setProperty("_traducao_unidades_ui_total", 0);
        interface_monitor->setProperty("_traducao_unidades_tabela_total", 0);
        interface_monitor->setProperty("_traducao_ui_contabilizada", false);

        interface_monitor->observador = nullptr;
        interface_monitor->evento_base = nullptr;
        interface_monitor->security_manager = nullptr;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar atributos", true);
    }
}

}  // namespace source::gui
