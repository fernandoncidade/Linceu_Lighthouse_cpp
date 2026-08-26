#include "gmet_41_ProcessarColunaThread.h"

#include <QMetaObject>
#include <QMutexLocker>
#include <QThread>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void processar_coluna_thread(source::services::GerenciadorColunas* gc, const QString& coluna_key) {
    try {
        if (!gc || !gc->filas_colunas.contains(coluna_key)) return;
        while (true) {
            if (gc->filas_colunas[coluna_key].isEmpty()) {
                QThread::msleep(50);
                continue;
            }
            const QVariantMap item = gc->filas_colunas[coluna_key].dequeue();
            QVariant valor = item.value(coluna_key);
            {
                QMutexLocker locker(&gc->lock_resultados);
                QVariantList list = gc->resultados_colunas.value(coluna_key).toList();
                list.append(QVariantMap{{"item", item}, {"valor", valor}});
                gc->resultados_colunas[coluna_key] = list;
            }
            if (gc->interface) QMetaObject::invokeMethod(gc->interface, "atualizar_coluna_interface", Qt::QueuedConnection);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro na thread da coluna", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
