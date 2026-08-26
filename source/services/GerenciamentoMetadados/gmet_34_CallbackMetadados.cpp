#include "gmet_34_CallbackMetadados.h"

#include <QMutexLocker>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void callback_metadados(source::services::GerenciadorColunas* gc, const QVariant& futuro) {
    try {
        if (!gc) return;
        const QVariantMap r = futuro.toMap();
        const QString caminho = r.value("caminho").toString();
        const QVariantMap metadados = r.value("metadados").toMap();
        if (!caminho.isEmpty()) {
            QMutexLocker locker(&gc->lock_cache);
            gc->cache_metadados[caminho] = metadados;
        }
        if (gc->interface) QMetaObject::invokeMethod(gc->interface, "atualizar_status", Qt::QueuedConnection);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no callback", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
