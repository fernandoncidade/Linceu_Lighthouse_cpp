#include "ic_07_ManipuladorTabela.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>

#include "ic_01_InterfaceMonitor.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::gui {

void ManipuladorTabela::configurar_tabela(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }
        if (interface_monitor->gerenciador_tabela == nullptr) {
            source::utils::LogManager::get_logger().error("GerenciadorTabela não foi inicializado");
            return;
        }
        if (interface_monitor->tabela_dados == nullptr) {
            source::utils::LogManager::get_logger().error("Tabela de dados não foi inicializada");
            return;
        }

        interface_monitor->gerenciador_tabela->configurar_tabela(interface_monitor->tabela_dados);

        if (!interface_monitor->gerenciador_tabela->property("_conexoes_configuradas").toBool()) {
            QObject::connect(interface_monitor->gerenciador_tabela,
                             &source::services::GerenciadorTabela::cores_processadas, interface_monitor,
                             &source::gui::InterfaceMonitor::atualizar_cores_tabela);
            interface_monitor->gerenciador_tabela->setProperty("_conexoes_configuradas", true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar tabela", true);
    }
}

void ManipuladorTabela::atualizar_visibilidade_colunas(InterfaceMonitor* interface_monitor,
                                                        bool atualizar_em_massa) {
    try {
        if (interface_monitor == nullptr || interface_monitor->gerenciador_tabela == nullptr) {
            source::utils::LogManager::get_logger().warning(
                "gerenciador_tabela não está disponível para atualizar visibilidade das colunas");
            return;
        }

        interface_monitor->gerenciador_tabela->atualizar_visibilidade_colunas(atualizar_em_massa);
        if (interface_monitor->tabela_dados != nullptr) {
            interface_monitor->gerenciador_tabela->ajustar_altura_cabecalho(interface_monitor->tabela_dados);
        }

        if (atualizar_em_massa) {
            interface_monitor->gerenciador_tabela->aplicar_cores_todas_colunas_processamento(
                QVariantMap{{"atualizar_visibilidade", true}});
        }

        interface_monitor->atualizar_status();
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao atualizar visibilidade das colunas", true);
    }
}

void ManipuladorTabela::processar_cores_assincronamente(InterfaceMonitor* interface_monitor,
                                                         const QVariantMap& dados) {
    try {
        if (interface_monitor == nullptr || interface_monitor->gerenciador_tabela == nullptr) {
            source::utils::LogManager::get_logger().warning(
                "GerenciadorTabela não disponível para processamento assíncrono");
            return;
        }

        const QVariantMap dados_processamento =
            dados.isEmpty() ? QVariantMap{{"processamento_manual", true}} : dados;
        interface_monitor->gerenciador_tabela->aplicar_cores_todas_colunas_processamento(
            dados_processamento);
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao iniciar processamento assíncrono de cores", true);
    }
}

void ManipuladorTabela::aguardar_processamentos_pendentes(InterfaceMonitor* interface_monitor,
                                                           int timeout) {
    try {
        if (interface_monitor == nullptr || interface_monitor->gerenciador_tabela == nullptr ||
            timeout <= 0) {
            return;
        }

        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed() < (timeout * 1000)) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (!interface_monitor->gerenciador_tabela->atualizacao_pendente) {
                break;
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao aguardar processamentos", true);
    }
}

}  // namespace source::gui
