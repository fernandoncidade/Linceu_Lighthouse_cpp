#include "ob_08_EventoMovido.h"

#include <QApplication>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTableView>

#include "GerenciamentoEventoMovido/gevmov_01_MovimentacaoWorker.h"
#include "GerenciamentoEventoMovido/gevmov_02_verificar_movimentacao.h"
#include "GerenciamentoEventoMovido/gevmov_03_remover_exclusao.h"
#include "GerenciamentoEventoMovido/gevmov_04_atualizar_linha_recente.h"
#include "GerenciamentoEventoMovido/gevmov_05_atualizar_tabela_completa.h"
#include "GerenciamentoEventoMovido/gevmov_06_inicializar_sistema_evento.h"
#include "GerenciamentoEventoMovido/gevmov_07_processar_exclusoes_pendentes.h"
#include "GerenciamentoEventoMovido/gevmov_08_adicionar_item_tabela.h"
#include "GerenciamentoEventoMovido/gevmov_09_adicionar_evento.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services {

namespace {

QTableView* obter_tabela(QObject* interface_monitor) {
    if (!interface_monitor) {
        return nullptr;
    }
    QObject* tabela_obj = interface_monitor->property("tabela_dados").value<QObject*>();
    return qobject_cast<QTableView*>(tabela_obj);
}

source::ui::LocalizadorQt* obter_loc(QObject* interface_monitor) {
    if (!interface_monitor) {
        return nullptr;
    }
    auto* loc = interface_monitor->property("loc").value<source::ui::LocalizadorQt*>();
    if (loc) {
        return loc;
    }
    QObject* loc_obj = interface_monitor->property("loc").value<QObject*>();
    return qobject_cast<source::ui::LocalizadorQt*>(loc_obj);
}

source::services::GerenciamentoEventoMovido::MovimentacaoWorker* obter_worker_movimentacao(QObject* interface_monitor) {
    if (!interface_monitor) {
        return nullptr;
    }
    QObject* worker_obj = interface_monitor->property("movimentacao_worker").value<QObject*>();
    return qobject_cast<source::services::GerenciamentoEventoMovido::MovimentacaoWorker*>(worker_obj);
}

void encaminhar_evento_movido(QObject* interface_monitor, const QVariantMap& evento) {
    auto* worker = obter_worker_movimentacao(interface_monitor);
    if (worker) {
        worker->adicionar_evento(evento);
    }
}

void atualizar_viewport_tabela(QTableView* tabela) {
    if (tabela && tabela->viewport()) {
        tabela->viewport()->update();
    }
}

bool filtro_permite_operacao(QObject* painel_filtros, const QString& tipo_operacao_traduzido) {
    if (!painel_filtros) return true;

    if (auto* filtros = qobject_cast<source::tools::Filtros*>(painel_filtros)) {
        return filtros->verificar_filtro_operacao(tipo_operacao_traduzido);
    }

    const int method_index = painel_filtros->metaObject()->indexOfMethod("verificar_filtro_operacao(QString)");
    if (method_index >= 0) {
        bool permitido = true;
        QMetaObject::invokeMethod(painel_filtros, "verificar_filtro_operacao", Qt::DirectConnection,
                                  Q_RETURN_ARG(bool, permitido), Q_ARG(QString, tipo_operacao_traduzido));
        return permitido;
    }

    return true;
}

}  // namespace

EventoRunnable::EventoRunnable(const QVariantMap& evento, ProcessFunc func_processar)
    : QObject(nullptr), evento_(evento), func_processar_(std::move(func_processar)) {}

void EventoRunnable::run() {
    try {
        if (!func_processar_) return;
        QVariantMap resultado = func_processar_(evento_);
        if (!resultado.isEmpty()) emit concluido(resultado);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoRunnable.run", true);
    }
}

EventoMovido::EventoMovido(QObject* parent)
    : QObject(parent) {
    processamento_timer_.setInterval(100);
    QObject::connect(&processamento_timer_, &QTimer::timeout, this, &EventoMovido::processar_lote);
    processamento_timer_.start();
    thread_pool_.setMaxThreadCount(10);
}

void EventoMovido::processar_evento(const QVariantMap& evento) {
    try {
        emit evento_processado(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoMovido.processar_evento", true);
    }
}

void EventoMovido::adicionar_evento_para_lote(const QVariantMap& evento) {
    try {
        QMutexLocker locker(&eventos_lock_);
        eventos_pendentes_.append(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoMovido.adicionar_evento_para_lote", true);
    }
}

void EventoMovido::processar_lote() {
    try {
        QList<QVariantMap> eventos_para_processar;
        {
            QMutexLocker locker(&eventos_lock_);
            if (!eventos_pendentes_.isEmpty()) {
                eventos_para_processar = eventos_pendentes_;
                eventos_pendentes_.clear();
            }
        }

        if (!eventos_para_processar.isEmpty()) {
            emit eventos_em_lote_processados(eventos_para_processar);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoMovido.processar_lote", true);
    }
}

void EventoMovido::shutdown() {
    processamento_timer_.stop();
    QMutexLocker locker(&eventos_lock_);
    eventos_pendentes_.clear();
    thread_pool_.waitForDone();
}

EventoBuffer::EventoBuffer(QObject* interface_monitor, QObject* parent)
    : QObject(parent), interface_(interface_monitor), ultimo_update_(0) {
    timer_.setInterval(250);
    QObject::connect(&timer_, &QTimer::timeout, this, &EventoBuffer::atualizar_interface);
    timer_.start();
}

void EventoBuffer::adicionar_evento(const QVariantMap& evento) {
    try {
        QMutexLocker locker(&lock_);
        _processar_evento_imediato(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoBuffer.adicionar_evento", true);
    }
}

void EventoBuffer::adicionar_eventos_lote(const QList<QVariantMap>& eventos) {
    if (eventos.isEmpty()) return;

    QTableView* tabela = obter_tabela(interface_);
    bool ordenacao_habilitada = false;

    try {
        QMutexLocker locker(&lock_);
        if (tabela) {
            tabela->blockSignals(true);
            ordenacao_habilitada = tabela->isSortingEnabled();
            tabela->setSortingEnabled(false);
        }

        for (const auto& evento : eventos) {
            _processar_evento_imediato(evento);
        }

        for (const auto& evento : eventos) {
            if (source::services::GerenciamentoBaseEvento::operacao_corresponde(
                    evento.value("tipo_operacao").toString(),
                    QStringLiteral("op_moved"),
                    obter_loc(interface_))) {
                encaminhar_evento_movido(interface_, evento);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoBuffer.adicionar_eventos_lote", true);
    }

    if (tabela) {
        tabela->setSortingEnabled(ordenacao_habilitada);
        tabela->blockSignals(false);
        atualizar_viewport_tabela(tabela);
    }
    QApplication::processEvents();
}

void EventoBuffer::_adicionar_evento_lote(const QVariantMap& evento) {
    try {
        if (!interface_) {
            return;
        }

        const QString tipo_operacao = evento.value("tipo_operacao").toString();
        const QString tipo_operacao_traduzido = obter_loc(interface_)
                                                    ? obter_loc(interface_)->traduzir_tipo_operacao(tipo_operacao)
                                                    : tipo_operacao;

        QObject* painel_filtros = interface_->property("painel_filtros").value<QObject*>();
        if (!filtro_permite_operacao(painel_filtros, tipo_operacao_traduzido)) {
            return;
        }

        QTableView* tabela = obter_tabela(interface_);
        if (tabela) {
            tabela->setUpdatesEnabled(false);
        }
        _adicionar_item_tabela(interface_, evento, false);
        if (tabela) {
            tabela->setUpdatesEnabled(true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar evento em lote", true);
    }
}

void EventoBuffer::_processar_evento_imediato(const QVariantMap& evento) {
    QTableView* tabela = obter_tabela(interface_);
    bool ordenacao_habilitada = false;
    if (tabela) {
        ordenacao_habilitada = tabela->isSortingEnabled();
        tabela->setSortingEnabled(false);
    }

    try {
        _adicionar_item_tabela(interface_, evento, false);
        if (source::services::GerenciamentoBaseEvento::operacao_corresponde(
                evento.value("tipo_operacao").toString(),
                QStringLiteral("op_moved"),
                obter_loc(interface_))) {
            encaminhar_evento_movido(interface_, evento);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoBuffer._processar_evento_imediato", true);
    }

    if (tabela) {
        tabela->setSortingEnabled(ordenacao_habilitada);
        atualizar_viewport_tabela(tabela);
    }
    QApplication::processEvents();
}

void EventoBuffer::atualizar_interface() {
    try {
        if (!interface_) return;
        QMetaObject::invokeMethod(interface_, "atualizar_status", Qt::QueuedConnection);
        QObject* gerenciador_tabela = interface_->property("gerenciador_tabela").value<QObject*>();
        if (gerenciador_tabela) {
            gerenciador_tabela->setProperty("atualizacao_pendente", true);
        }
        atualizar_viewport_tabela(obter_tabela(interface_));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em EventoBuffer.atualizar_interface", true);
    }
}

void EventoBuffer::shutdown() {
    timer_.stop();
    QMutexLocker locker(&lock_);
    eventos_pendentes_.clear();
}

QVariant verificar_movimentacao(QObject* self, const QVariant& evento) {
    return GerenciamentoEventoMovido::verificar_movimentacao(self, evento);
}

void _remover_exclusao(QObject* self, const QString& nome, const QString& dir_anterior, const QString& exclusao_id) {
    GerenciamentoEventoMovido::remover_exclusao(self, nome, dir_anterior, exclusao_id);
}

void _atualizar_linha_recente(QObject* self) {
    GerenciamentoEventoMovido::atualizar_linha_recente(self);
}

void _atualizar_tabela_completa(QObject* self) {
    GerenciamentoEventoMovido::atualizar_tabela_completa(self);
}

void _inicializar_sistema_evento(QObject* self) {
    GerenciamentoEventoMovido::inicializar_sistema_evento(self);
}

void _processar_exclusoes_pendentes(QObject* self) {
    GerenciamentoEventoMovido::processar_exclusoes_pendentes(self);
}

void _adicionar_item_tabela(QObject* self, const QVariantMap& evento, bool atualizar_interface) {
    GerenciamentoEventoMovido::adicionar_item_tabela(self, evento, atualizar_interface);
}

void adicionar_evento(QObject* self, const QVariant& evento) {
    GerenciamentoEventoMovido::adicionar_evento(self, evento);
}

void parar_sistema_evento(QObject* self, int timeout_ms) {
    try {
        if (self == nullptr) {
            return;
        }

        self->setProperty("_aceitar_eventos_monitoramento", false);
        self->setProperty("excluidos_recentemente", QVariantMap());

        if (auto* refresh_timer = qobject_cast<QTimer*>(self->property("refresh_timer").value<QObject*>());
            refresh_timer != nullptr) {
            refresh_timer->stop();
            QObject::disconnect(refresh_timer, nullptr, nullptr, nullptr);
            refresh_timer->deleteLater();
            self->setProperty("refresh_timer", QVariant());
        }

        if (auto* exclusao_timer = qobject_cast<QTimer*>(self->property("exclusao_timer").value<QObject*>());
            exclusao_timer != nullptr) {
            exclusao_timer->stop();
            QObject::disconnect(exclusao_timer, nullptr, nullptr, nullptr);
            exclusao_timer->deleteLater();
            self->setProperty("exclusao_timer", QVariant());
        }

        if (auto* worker = qobject_cast<source::services::GerenciamentoEventoMovido::MovimentacaoWorker*>(
                self->property("movimentacao_worker").value<QObject*>());
            worker != nullptr) {
            worker->shutdown(timeout_ms);
            QObject::disconnect(worker, nullptr, nullptr, nullptr);
            worker->deleteLater();
            self->setProperty("movimentacao_worker", QVariant());
        }

        if (auto* processador =
                qobject_cast<source::services::EventoMovido*>(self->property("processador_evento").value<QObject*>());
            processador != nullptr) {
            processador->shutdown();
            QObject::disconnect(processador, nullptr, nullptr, nullptr);
            processador->deleteLater();
            self->setProperty("processador_evento", QVariant());
        }

        if (auto* buffer = qobject_cast<source::services::EventoBuffer*>(self->property("evento_buffer").value<QObject*>());
            buffer != nullptr) {
            buffer->shutdown();
            QObject::disconnect(buffer, nullptr, nullptr, nullptr);
            buffer->deleteLater();
            self->setProperty("evento_buffer", QVariant());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao parar sistema de eventos", true);
    }
}

}  // namespace source::services
