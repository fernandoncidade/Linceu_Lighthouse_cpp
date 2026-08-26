#include "gtab_38_OnCoresProcessadas.h"

#include <QMetaObject>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void on_cores_processadas(source::services::GerenciadorTabela* self, const QVariantMap& resultado) {
    try {
        if (!self) return;
        emit self->cores_processadas(resultado);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em on_cores_processadas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

