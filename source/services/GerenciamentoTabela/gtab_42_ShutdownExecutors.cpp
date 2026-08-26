#include "gtab_42_ShutdownExecutors.h"

#include "source/services/GerenciamentoTabela/gtab_34_MonitorTemaWindows.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void shutdown_executors(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;
        if (auto* monitor = qobject_cast<source::services::GerenciamentoTabela::MonitorTemaWindows*>(self->monitor_tema)) {
            monitor->parar_monitoramento();
        }
        self->timer_tema.stop();
        self->timer_retraducao.stop();
        self->timer_atualizacao.stop();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em shutdown_executors", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
