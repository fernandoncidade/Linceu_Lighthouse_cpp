#include "gtab_40_AtualizarInterfacePosProcessamento.h"

#include <QMetaObject>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_interface_pos_processamento(source::services::GerenciadorTabela* self, const QVariantMap& resultado) {
    try {
        Q_UNUSED(resultado)
        if (!self || !self->interface) return;

        self->atualizar_visualizacao_tabela();

        if (self->interface->metaObject()->indexOfMethod("atualizar_status()") >= 0) {
            QMetaObject::invokeMethod(self->interface, "atualizar_status", Qt::QueuedConnection);
        }

        self->ajustar_cor_selecao();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_interface_pos_processamento", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

