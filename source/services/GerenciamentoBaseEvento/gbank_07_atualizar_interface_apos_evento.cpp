#include "gbank_07_atualizar_interface_apos_evento.h"

#include <QHash>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QVariantList>

#include <utility>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {
namespace {

struct InterfaceBatchState {
    QVariantList eventos;
    bool flush_agendado = false;
    bool cleanup_conectado = false;
};

constexpr int kMaxEventosPorFlushUi = 512;
QMutex g_batch_mutex;
QHash<QObject*, InterfaceBatchState> g_batches_por_interface;

bool eh_evento_exclusao(const QVariantMap& evento) {
    const QString operacao = evento.value(QStringLiteral("tipo_operacao")).toString().toLower();
    return operacao == QStringLiteral("op_deleted") ||
           operacao.contains(QStringLiteral("deleted")) ||
           operacao.contains(QStringLiteral("exclu"));
}

void limpar_batch_interface(QObject* interface) {
    if (interface == nullptr) {
        return;
    }
    QMutexLocker locker(&g_batch_mutex);
    g_batches_por_interface.remove(interface);
}

void processar_batch_interface(QObject* interface);

void agendar_flush_batch(QObject* interface) {
    if (interface == nullptr) {
        return;
    }
    QMetaObject::invokeMethod(interface, [interface]() { processar_batch_interface(interface); }, Qt::QueuedConnection);
}

void processar_batch_interface(QObject* interface) {
    if (interface == nullptr) {
        return;
    }

    QVariantList lote;
    QVariantList restante;
    bool reagendar_restante = false;
    {
        QMutexLocker locker(&g_batch_mutex);
        auto it = g_batches_por_interface.find(interface);
        if (it == g_batches_por_interface.end()) {
            return;
        }
        lote.swap(it->eventos);
        it->flush_agendado = false;
    }

    if (lote.size() > kMaxEventosPorFlushUi) {
        restante = lote.mid(kMaxEventosPorFlushUi);
        lote = lote.mid(0, kMaxEventosPorFlushUi);

        QMutexLocker locker(&g_batch_mutex);
        auto it = g_batches_por_interface.find(interface);
        if (it == g_batches_por_interface.end()) {
            InterfaceBatchState state;
            state.eventos = restante;
            state.flush_agendado = true;
            state.cleanup_conectado = true;
            g_batches_por_interface.insert(interface, state);
        } else {
            QVariantList eventos_reordenados = restante;
            eventos_reordenados.append(it->eventos);
            it->eventos = std::move(eventos_reordenados);
            it->flush_agendado = true;
        }
        reagendar_restante = true;
    }

    if (!lote.isEmpty()) {
        bool ok_batch = QMetaObject::invokeMethod(interface,
                                                  "inserir_eventos_streaming",
                                                  Qt::DirectConnection,
                                                  Q_ARG(QVariantList, lote));
        if (!ok_batch) {
            for (const QVariant& evento_var : lote) {
                QMetaObject::invokeMethod(interface,
                                          "inserir_evento_streaming",
                                          Qt::DirectConnection,
                                          Q_ARG(QVariantMap, evento_var.toMap()));
            }
        }
    }

    bool reagendar = false;
    {
        QMutexLocker locker(&g_batch_mutex);
        auto it = g_batches_por_interface.find(interface);
        if (it != g_batches_por_interface.end() && !it->eventos.isEmpty() && !it->flush_agendado) {
            it->flush_agendado = true;
            reagendar = true;
        }
    }

    if (reagendar) {
        agendar_flush_batch(interface);
    }
    if (reagendar_restante) {
        agendar_flush_batch(interface);
    }
}

void enfileirar_evento_interface(QObject* interface, const QVariantMap& evento) {
    if (interface == nullptr || evento.isEmpty()) {
        return;
    }

    bool agendar = false;
    bool conectar_cleanup = false;
    {
        QMutexLocker locker(&g_batch_mutex);
        auto it = g_batches_por_interface.find(interface);
        if (it == g_batches_por_interface.end()) {
            InterfaceBatchState state;
            state.eventos.append(evento);
            state.flush_agendado = true;
            state.cleanup_conectado = true;
            g_batches_por_interface.insert(interface, state);
            agendar = true;
            conectar_cleanup = true;
        } else {
            it->eventos.append(evento);
            if (!it->flush_agendado) {
                it->flush_agendado = true;
                agendar = true;
            }
            if (!it->cleanup_conectado) {
                it->cleanup_conectado = true;
                conectar_cleanup = true;
            }
        }
    }

    if (conectar_cleanup) {
        QObject::connect(interface,
                         &QObject::destroyed,
                         interface,
                         [](QObject* obj) { limpar_batch_interface(obj); },
                         Qt::DirectConnection);
    }
    if (agendar) {
        agendar_flush_batch(interface);
    }
}

}  // namespace

void _atualizar_interface_apos_evento(source::services::BaseEvento* self, const QVariantMap& evento) {
    try {
        if (!self) return;

        QObject* interface = nullptr;
        if (self->callback.isValid()) {
            interface = self->callback.value<QObject*>();
        }
        if (!interface && self->observador) {
            interface = self->observador->interface;
        }
        if (!interface) return;

        if (eh_evento_exclusao(evento) &&
            interface->metaObject()->indexOfMethod("inserir_evento_streaming(QVariantMap)") >= 0) {
            QMetaObject::invokeMethod(interface,
                                      "inserir_evento_streaming",
                                      Qt::QueuedConnection,
                                      Q_ARG(QVariantMap, evento));
            return;
        }

        enfileirar_evento_interface(interface, evento);

        if (interface->metaObject()->indexOfMethod("inserir_evento_streaming(QVariantMap)") < 0 &&
            interface->metaObject()->indexOfMethod("inserir_eventos_streaming(QVariantList)") < 0) {
            QObject* gerenciador_tabela = interface->property("gerenciador_tabela").value<QObject*>();
            QObject* tabela_dados = interface->property("tabela_dados").value<QObject*>();
            if (gerenciador_tabela && tabela_dados) {
                QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_linha_mais_recente", Qt::QueuedConnection,
                                          Q_ARG(QObject*, tabela_dados), Q_ARG(QVariantMap, evento));
            }
            QMetaObject::invokeMethod(interface, "atualizar_status", Qt::QueuedConnection);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface após evento", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
