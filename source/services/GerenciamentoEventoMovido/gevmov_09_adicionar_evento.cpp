#include "gevmov_09_adicionar_evento.h"

#include "gevmov_02_verificar_movimentacao.h"
#include "gevmov_06_inicializar_sistema_evento.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

void adicionar_evento(QObject* interfaceMonitor, const QVariant& eventoVar) {
    try {
        if (interfaceMonitor == nullptr || !interfaceMonitor->property("_aceitar_eventos_monitoramento").toBool() ||
            interfaceMonitor->property("_encerrando_interface").toBool()) {
            return;
        }

        const QVariantMap evento = eventoVar.toMap();
        if (evento.isEmpty() || !evento.contains("tipo_operacao") || !evento.contains("nome")) {
            source::utils::LogManager::get_logger().warning(QStringLiteral("Evento inválido recebido"));
            return;
        }

        inicializar_sistema_evento(interfaceMonitor);

        const QVariant evento_processado = verificar_movimentacao(interfaceMonitor, evento);
        if (!evento_processado.isNull()) {
            auto* processador = interfaceMonitor ? qobject_cast<source::services::EventoMovido*>(interfaceMonitor->property("processador_evento").value<QObject*>()) : nullptr;
            if (processador) {
                processador->processar_evento(evento_processado.toMap());
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em adicionar_evento", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
