#include "ui_09_GerenciadorMonitoramento.h"

#include <QApplication>
#include <QMetaObject>
#include <QMutexLocker>
#include <QTableView>
#include <QVariantMap>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/services/GerenciamentoEventoMovido/gevmov_10_PendenciasMovimentacao.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::ui {
namespace {

void log_error(const char* message) {
    source::utils::LogManager::get_logger().error(message, true);
}

void safe_invoke_noargs(QObject* obj, const char* method, const char* error_message) {
    if (obj == nullptr) {
        return;
    }
    const bool ok = QMetaObject::invokeMethod(obj, method);
    if (!ok) {
        log_error(error_message);
    }
}

void safe_disconnect_all(QObject* obj, const char* error_message) {
    if (obj == nullptr) {
        return;
    }
    try {
        QObject::disconnect(obj, nullptr, nullptr, nullptr);
    } catch (...) {
        log_error(error_message);
    }
}

void reiniciar_runtime_multidiretorio_se_necessario(source::security::SecurityManager* security_manager,
                                                    bool estava_ativo,
                                                    bool scan_estava_rodando) {
    if (security_manager == nullptr || (!estava_ativo && !scan_estava_rodando)) {
        return;
    }

    security_manager->stop_runtime_observers();
    security_manager->configure_runtime_observers();
    security_manager->start_runtime_observers();
}

}  // namespace

GerenciadorMonitoramento::GerenciadorMonitoramento(QObject* interface_principal) : interface_(interface_principal) {
    try {
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorMonitoramento", true);
    }
}

void GerenciadorMonitoramento::reiniciar_sistema_monitoramento() {
    try {
        auto* security_manager = source::security::ui::get_security_manager(interface_);
        if (security_manager != nullptr) {
            security_manager->stop_runtime_observers();
        }

        int rows_antes = 0;
        int rows_depois = 0;

        QObject* tabela_dados = interface_->property("tabela_dados").value<QObject*>();
        if (tabela_dados != nullptr) {
            QMetaObject::invokeMethod(tabela_dados, "rowCount", Q_RETURN_ARG(int, rows_antes));
        }

        auto* observador = interface_->property("observador").value<QObject*>();
        if (observador != nullptr) {
            safe_invoke_noargs(observador, "parar", "Erro ao parar observador");
            interface_->setProperty("observador", QVariant());
        }

        auto* refresh_timer = interface_->property("refresh_timer").value<QObject*>();
        if (refresh_timer != nullptr) {
            safe_invoke_noargs(refresh_timer, "stop", "Erro ao parar refresh_timer");
            safe_disconnect_all(refresh_timer, "Erro ao desconectar refresh_timer");
        }

        auto* exclusao_timer = interface_->property("exclusao_timer").value<QObject*>();
        if (exclusao_timer != nullptr) {
            safe_invoke_noargs(exclusao_timer, "stop", "Erro ao parar exclusao_timer");
            safe_disconnect_all(exclusao_timer, "Erro ao desconectar exclusao_timer");
        }

        source::services::GerenciamentoEventoMovido::limpar_pendencias_movimentacao(interface_);

        auto* processador_evento = interface_->property("processador_evento").value<QObject*>();
        if (processador_evento != nullptr) {
            safe_disconnect_all(processador_evento, "Erro ao desconectar evento_processado");
            interface_->setProperty("processador_evento", QVariant());
        }

        auto* evento_base = interface_->property("evento_base").value<QObject*>();
        if (evento_base != nullptr) {
            safe_invoke_noargs(evento_base, "otimizar_banco", "Erro ao otimizar banco de dados");
        }

        auto* gerenciador_colunas = interface_->property("gerenciador_colunas").value<QObject*>();
        if (auto* gc = qobject_cast<source::services::GerenciadorColunas*>(gerenciador_colunas)) {
            QMutexLocker locker(&gc->lock_cache);
            gc->cache_metadados.clear();
        }

        auto* evento_buffer = interface_->property("evento_buffer").value<QObject*>();
        if (evento_buffer != nullptr) {
            safe_disconnect_all(evento_buffer, "Erro ao limpar evento_buffer");
            evento_buffer->deleteLater();
            interface_->setProperty("evento_buffer", QVariant());
        }

        if (tabela_dados != nullptr) {
            if (qobject_cast<QTableView*>(tabela_dados) != nullptr) {
                if (!QMetaObject::invokeMethod(interface_, "limpar_tabelas_idioma")) {
                    auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(
                        qobject_cast<QTableView*>(tabela_dados)->model());
                    if (model != nullptr) {
                        model->replace_events(QVariantList());
                    }
                }
            } else {
                safe_invoke_noargs(tabela_dados, "clearContents", "Erro ao limpar conteúdos da tabela");
                QMetaObject::invokeMethod(tabela_dados, "setRowCount", Q_ARG(int, 0));
            }
        }

        if (evento_base != nullptr) {
            safe_invoke_noargs(evento_base, "recarregar_registros_interface", "Erro ao recarregar dados da tabela");
        }

        QObject* painel_filtros = interface_->property("painel_filtros").value<QObject*>();
        if (painel_filtros != nullptr) {
            if (auto* filtros = qobject_cast<source::tools::Filtros*>(painel_filtros)) {
                if (auto* administrador = filtros->administrador_filtros_obj()) {
                    administrador->aplicar_filtros();
                }
            } else {
                QObject* administrador_filtros = painel_filtros->property("administrador_filtros").value<QObject*>();
                safe_invoke_noargs(administrador_filtros, "aplicar_filtros", "Erro ao reaplicar filtros");
            }
        }

        if (tabela_dados != nullptr) {
            QMetaObject::invokeMethod(tabela_dados, "rowCount", Q_RETURN_ARG(int, rows_depois));
        }
        interface_->setProperty("monitoramento_rows_antes", rows_antes);
        interface_->setProperty("monitoramento_rows_depois", rows_depois);

        if (security_manager != nullptr) {
            security_manager->configure_runtime_observers();
        }

        QMetaObject::invokeMethod(interface_, "atualizar_status");
        QApplication::processEvents();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao reiniciar sistema de monitoramento", true);
    }
}

bool GerenciadorMonitoramento::adicionar_diretorio_monitorado(const QString& diretorio) {
    auto* security_manager = source::security::ui::get_security_manager(interface_);
    if (security_manager == nullptr) {
        return false;
    }

    const bool estava_ativo = security_manager->any_runtime_active();
    const bool scan_estava_rodando = security_manager->any_runtime_scan_running();
    const bool added = security_manager->add_monitored_directory(diretorio);
    if (added) {
        reiniciar_runtime_multidiretorio_se_necessario(security_manager, estava_ativo, scan_estava_rodando);
    }
    return added;
}

bool GerenciadorMonitoramento::remover_diretorio_monitorado(const QString& diretorio) {
    auto* security_manager = source::security::ui::get_security_manager(interface_);
    if (security_manager == nullptr) {
        return false;
    }

    const bool estava_ativo = security_manager->any_runtime_active();
    const bool scan_estava_rodando = security_manager->any_runtime_scan_running();
    const bool removed = security_manager->remove_monitored_directory(diretorio);
    if (removed) {
        reiniciar_runtime_multidiretorio_se_necessario(security_manager, estava_ativo, scan_estava_rodando);
    }
    return removed;
}

QStringList GerenciadorMonitoramento::obter_diretorios_monitorados() const {
    auto* security_manager = source::security::ui::get_security_manager(interface_);
    return security_manager != nullptr ? security_manager->monitored_directories() : QStringList();
}

QVariantMap GerenciadorMonitoramento::obter_resumo_seguranca() const {
    auto* security_manager = source::security::ui::get_security_manager(interface_);
    return security_manager != nullptr ? security_manager->profile_summary() : QVariantMap();
}

QString GerenciadorMonitoramento::exportar_pacote_evidencia(const QString& destino) const {
    auto* security_manager = source::security::ui::get_security_manager(interface_);
    if (security_manager == nullptr) {
        return QString();
    }

    auto* table_view = qobject_cast<QTableView*>(interface_->property("tabela_dados").value<QObject*>());
    auto* model = table_view != nullptr
                      ? qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(table_view->model())
                      : nullptr;
    if (model == nullptr) {
        return QString();
    }

    return security_manager->export_evidence_package(model->snapshot_events(), destino);
}

}  // namespace source::ui
