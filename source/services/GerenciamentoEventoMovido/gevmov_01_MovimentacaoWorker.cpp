#include "gevmov_01_MovimentacaoWorker.h"

#include <QDeadlineTimer>
#include <QMetaObject>
#include <QMutexLocker>
#include <QThreadPool>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

MovimentacaoWorker::MovimentacaoWorker(QObject* interface, QObject* parent)
    : QObject(parent),
      interface_(interface),
      evento_base_(interface ? interface->property("evento_base").value<QObject*>() : nullptr),
      shutting_down_(false),
      active_tasks_(0) {}

namespace {
source::services::BaseEvento* resolver_evento_base(QObject* interface_obj, QObject* evento_base_obj) {
    source::services::BaseEvento* base = nullptr;
    if (interface_obj) {
        if (!interface_obj->property("_aceitar_eventos_monitoramento").toBool() ||
            interface_obj->property("_encerrando_interface").toBool()) {
            return nullptr;
        }

        base = interface_obj->property("evento_base").value<source::services::BaseEvento*>();
        if (base) return base;

        const qulonglong obs_ptr = interface_obj->property("_observador_cpp_ptr").toULongLong();
        if (obs_ptr != 0) {
            auto* obs = reinterpret_cast<source::services::Observador*>(obs_ptr);
            if (obs && obs->evento_base) return obs->evento_base.get();
        }
    }

    Q_UNUSED(evento_base_obj)
    return nullptr;
}
}  // namespace

void MovimentacaoWorker::adicionar_evento(const QVariantMap& evento) {
    try {
        if (shutting_down_.load()) {
            return;
        }

        QMutexLocker locker(&mutex_);
        if (shutting_down_.load()) {
            return;
        }

        active_tasks_.fetch_add(1);
        QThreadPool::globalInstance()->start([this, evento]() { _processar_evento_individual(evento); });
    } catch (...) {
        active_tasks_.fetch_sub(1);
        source::utils::LogManager::get_logger().error("Erro em MovimentacaoWorker.adicionar_evento", true);
    }
}

void MovimentacaoWorker::_processar_evento_individual(const QVariantMap& evento) {
    try {
        const auto finalizar = [this]() {
            active_tasks_.fetch_sub(1);
            QMutexLocker locker(&state_mutex_);
            idle_cv_.wakeAll();
        };

        if (shutting_down_.load()) {
            finalizar();
            return;
        }

        emit atualizacao_progresso(0, 1);
        if (auto* evento_base = resolver_evento_base(interface_, evento_base_)) {
            QList<QVariantMap> eventos;
            eventos.append(evento);
            evento_base->processar_eventos_movimentacao(
                eventos, [this]() { _concluir_processamento(); });
        } else if (evento_base_) {
            QList<QVariantMap> eventos;
            eventos.append(evento);
            QMetaObject::invokeMethod(evento_base_, "processar_eventos_movimentacao", Qt::DirectConnection,
                                      Q_ARG(QList<QVariantMap>, eventos));
            _concluir_processamento();
        } else {
            _concluir_processamento();
        }
        emit atualizacao_progresso(1, 1);
        finalizar();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar evento individual", true);
        _concluir_processamento();
        active_tasks_.fetch_sub(1);
        QMutexLocker locker(&state_mutex_);
        idle_cv_.wakeAll();
    }
}

void MovimentacaoWorker::_concluir_processamento() {
    try {
        emit processamento_concluido();
        if (interface_) {
            QObject* gerenciador_tabela = interface_->property("gerenciador_tabela").value<QObject*>();
            if (gerenciador_tabela) {
                gerenciador_tabela->setProperty("atualizacao_pendente", true);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em MovimentacaoWorker._concluir_processamento", true);
    }
}

void MovimentacaoWorker::shutdown(int timeout_ms) {
    shutting_down_.store(true);

    QMutexLocker locker(&state_mutex_);
    if (timeout_ms < 0) {
        timeout_ms = 0;
    }

    QDeadlineTimer deadline(timeout_ms);
    while (active_tasks_.load() > 0) {
        if (!idle_cv_.wait(&state_mutex_, deadline)) {
            break;
        }
    }
}

void MovimentacaoWorker::reactivate() {
    shutting_down_.store(false);
}

}  // namespace source::services::GerenciamentoEventoMovido
