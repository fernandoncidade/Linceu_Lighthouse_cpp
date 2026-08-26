#include "gmet_44_ExtrairMetadadosEmLote.h"

#include <QThreadPool>

#include "gmet_38_GetMetadados.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void extrair_metadados_em_lote(source::services::GerenciadorColunas* gc, const QList<QVariantMap>& lista_itens) {
    try {
        if (!gc) return;
        for (const QVariantMap& item : lista_itens) {
            QThreadPool::globalInstance()->start([gc, item]() {
                Q_UNUSED(get_metadados_gc(gc, item));
                if (gc->interface) QMetaObject::invokeMethod(gc->interface, "atualizar_colunas_tabela", Qt::QueuedConnection);
            });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados em lote", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
