#include "gbank_13_notificar_evento.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QThread>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

namespace {
bool interface_aceita_eventos(QObject* interface_obj) {
    if (!interface_obj) {
        return false;
    }

    if (interface_obj->property("_encerrando_interface").toBool()) {
        return false;
    }

    if (interface_obj->property("_aceitar_eventos_monitoramento").isValid()) {
        return interface_obj->property("_aceitar_eventos_monitoramento").toBool();
    }

    return true;
}

void garantir_sistema_evento_inicializado(QObject* interface_obj) {
    if (!interface_obj) return;

    const bool possui_processador = interface_obj->property("processador_evento").value<QObject*>() != nullptr;
    const bool possui_buffer = interface_obj->property("evento_buffer").value<QObject*>() != nullptr;
    const bool possui_timer_exclusao = interface_obj->property("exclusao_timer").value<QObject*>() != nullptr;
    if (possui_processador && possui_buffer && possui_timer_exclusao) {
        return;
    }

    if (QThread::currentThread() == interface_obj->thread()) {
        source::services::_inicializar_sistema_evento(interface_obj);
        return;
    }

    QMetaObject::invokeMethod(interface_obj,
                              [interface_obj]() { source::services::_inicializar_sistema_evento(interface_obj); },
                              Qt::BlockingQueuedConnection);
}
}  // namespace

void notificar_evento(source::services::BaseEvento* self, const QString& tipo_operacao, const QString& nome, const QString& dir_anterior, const QString& dir_atual) {
    if (!self || !self->observador) return;
    if (!self->observador->ativo || self->observador->desligando) return;

    try {
        const QString nome_base = QFileInfo(nome).fileName();
        auto* loc = self->observador->loc;
        const QString operacao = chave_canonica_operacao(tipo_operacao, loc);
        if (operacao.isEmpty()) {
            return;
        }
        QVariantMap evento;

        if (operacao == QStringLiteral("op_deleted")) {
            evento = self->_criar_evento_exclusao(nome_base, dir_anterior);
            if (evento.isEmpty()) return;
            if (!evento.contains("timestamp") || evento.value("timestamp").toString().isEmpty()) {
                evento["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            }

            QObject* interface_obj = self->observador->interface;
            if (interface_obj && interface_aceita_eventos(interface_obj)) {
                garantir_sistema_evento_inicializado(interface_obj);
                const QVariant evento_processado = source::services::verificar_movimentacao(interface_obj, evento);
                if (!evento_processado.isValid() || evento_processado.isNull()) {
                    // A ausência de match imediato pode ser um "move" em andamento.
                    // O evento fica apenas em memória e será confirmado depois por timeout
                    // (ou descartado quando reconciliado como "movido").
                    evento["_temporario"] = true;
                    return;
                }

                evento = evento_processado.toMap();
            }
        } else {
            evento = self->_criar_evento_padrao(operacao, nome_base, dir_anterior, dir_atual);
            if (evento.isEmpty()) return;

            QObject* interface_obj = self->observador->interface;
            if (interface_obj && interface_aceita_eventos(interface_obj)) {
                garantir_sistema_evento_inicializado(interface_obj);
                const QVariant evento_processado = source::services::verificar_movimentacao(interface_obj, evento);
                if (evento_processado.isValid() && !evento_processado.isNull()) {
                    evento = evento_processado.toMap();
                } else {
                    return;
                }
            }
        }
        if (!evento.isEmpty()) self->registrar_evento_no_banco(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao notificar evento", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
