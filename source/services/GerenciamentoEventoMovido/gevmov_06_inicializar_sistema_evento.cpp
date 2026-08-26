#include "gevmov_06_inicializar_sistema_evento.h"

#include <QMetaObject>
#include <QTimer>

#include "gevmov_01_MovimentacaoWorker.h"
#include "gevmov_07_processar_exclusoes_pendentes.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

void inicializar_sistema_evento(QObject* interfaceMonitor) {
    try {
        if (!interfaceMonitor) return;
        interfaceMonitor->setProperty("_aceitar_eventos_monitoramento", true);

        auto* processador = qobject_cast<source::services::EventoMovido*>(interfaceMonitor->property("processador_evento").value<QObject*>());
        if (!processador) {
            processador = new source::services::EventoMovido(interfaceMonitor);
            interfaceMonitor->setProperty("processador_evento", QVariant::fromValue(static_cast<QObject*>(processador)));

            auto* buffer = qobject_cast<source::services::EventoBuffer*>(interfaceMonitor->property("evento_buffer").value<QObject*>());
            if (!buffer) {
                buffer = new source::services::EventoBuffer(interfaceMonitor, interfaceMonitor);
                interfaceMonitor->setProperty("evento_buffer", QVariant::fromValue(static_cast<QObject*>(buffer)));
            }

            auto* workerObj = interfaceMonitor->property("movimentacao_worker").value<QObject*>();
            if (!workerObj) {
                auto* worker = new source::services::GerenciamentoEventoMovido::MovimentacaoWorker(interfaceMonitor, interfaceMonitor);
                workerObj = worker;
                interfaceMonitor->setProperty("movimentacao_worker", QVariant::fromValue(workerObj));
                QObject::connect(worker, &source::services::GerenciamentoEventoMovido::MovimentacaoWorker::processamento_concluido, interfaceMonitor, [interfaceMonitor]() {
                    QObject* gerenciador_tabela = interfaceMonitor->property("gerenciador_tabela").value<QObject*>();
                    QObject* tabela_dados = interfaceMonitor->property("tabela_dados").value<QObject*>();
                    if (gerenciador_tabela && tabela_dados) {
                        QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_linha_mais_recente", Qt::QueuedConnection,
                                                  Q_ARG(QObject*, tabela_dados));
                    }
                });
            } else if (auto* worker = qobject_cast<source::services::GerenciamentoEventoMovido::MovimentacaoWorker*>(workerObj)) {
                worker->reactivate();
            }

            QObject::connect(processador, &source::services::EventoMovido::evento_processado, buffer, &source::services::EventoBuffer::adicionar_evento);
            QObject::connect(processador, &source::services::EventoMovido::eventos_em_lote_processados, buffer, &source::services::EventoBuffer::adicionar_eventos_lote);
        }

        if (!interfaceMonitor->property("contador_eventos").isValid()) {
            interfaceMonitor->setProperty("contador_eventos", 0);
        }

        auto* refreshTimer = qobject_cast<QTimer*>(interfaceMonitor->property("refresh_timer").value<QObject*>());
        if (!refreshTimer) {
            refreshTimer = new QTimer(interfaceMonitor);
            QObject::connect(refreshTimer, &QTimer::timeout, interfaceMonitor, [interfaceMonitor]() {
                QMetaObject::invokeMethod(interfaceMonitor, "atualizar_status", Qt::QueuedConnection);
            });
            refreshTimer->start(1000);
            interfaceMonitor->setProperty("refresh_timer", QVariant::fromValue(static_cast<QObject*>(refreshTimer)));
        }

        auto* exclusaoTimer = qobject_cast<QTimer*>(interfaceMonitor->property("exclusao_timer").value<QObject*>());
        if (!exclusaoTimer) {
            exclusaoTimer = new QTimer(interfaceMonitor);
            exclusaoTimer->setTimerType(Qt::PreciseTimer);
            QObject::connect(exclusaoTimer, &QTimer::timeout, interfaceMonitor, [interfaceMonitor]() {
                processar_exclusoes_pendentes(interfaceMonitor);
            });
            exclusaoTimer->start(100);
            interfaceMonitor->setProperty("exclusao_timer", QVariant::fromValue(static_cast<QObject*>(exclusaoTimer)));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar sistema de eventos", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
