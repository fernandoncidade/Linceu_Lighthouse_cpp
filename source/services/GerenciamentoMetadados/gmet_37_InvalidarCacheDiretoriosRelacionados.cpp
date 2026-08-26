#include "gmet_37_InvalidarCacheDiretoriosRelacionados.h"

#include <QFileInfo>
#include <QMutexLocker>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void invalidar_cache_diretorios_relacionados(source::services::GerenciadorColunas* gc, const QString& caminho) {
    try {
        if (!gc) return;
        QMutexLocker locker(&gc->lock_cache);
        QString dir_atual = QFileInfo(caminho).isFile() ? QFileInfo(caminho).absolutePath() : caminho;
        while (!dir_atual.isEmpty()) {
            QVariantMap m = gc->cache_metadados.value(dir_atual).toMap();
            m.remove("tamanho_dir");
            m.remove("tamanho");
            m.remove("tamanho_dir_bytes");
            m.remove("tamanho_dir_mtime");
            m.remove("tamanho_dir_cached_at");
            if (m.isEmpty()) gc->cache_metadados.remove(dir_atual);
            else gc->cache_metadados[dir_atual] = m;

            const QString pai = QFileInfo(dir_atual).absolutePath();
            if (pai == dir_atual) break;
            dir_atual = pai;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao invalidar cache de diretórios", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
