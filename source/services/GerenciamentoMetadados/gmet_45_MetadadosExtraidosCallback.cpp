#include "gmet_45_MetadadosExtraidosCallback.h"

#include <QMetaObject>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void metadados_extraidos_callback(source::services::GerenciadorColunas* gc, const QVariant& futuro) {
    Q_UNUSED(futuro)
    try {
        if (gc && gc->interface) {
            QMetaObject::invokeMethod(gc->interface, "atualizar_colunas_tabela", Qt::QueuedConnection);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar metadados extraídos", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
