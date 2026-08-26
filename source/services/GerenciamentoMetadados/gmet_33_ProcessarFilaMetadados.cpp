#include "gmet_33_ProcessarFilaMetadados.h"

#include <QFileInfo>
#include <QMetaObject>
#include <QMimeDatabase>
#include <QMimeType>
#include <QThread>

#include "gmet_38_GetMetadados.h"
#include "gmet_19_GetTipoArquivo.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {
namespace {
QString detectar_mime_por_extensao(const QString& caminho, const QString& nome) {
    QString candidato = caminho.trimmed();
    if (candidato.isEmpty()) {
        candidato = nome.trimmed();
    }
    if (candidato.isEmpty()) {
        return {};
    }

    QMimeDatabase mime_db;
    const QMimeType mime = mime_db.mimeTypeForFile(candidato, QMimeDatabase::MatchExtension);
    return mime.isValid() ? mime.name() : QString();
}
}

void processar_fila_metadados(source::services::GerenciadorColunas* gc) {
    try {
        if (!gc) return;
        while (true) {
            if (gc->fila_metadados.isEmpty()) {
                QThread::msleep(50);
                continue;
            }
            const QVariantMap item = gc->fila_metadados.dequeue();
            const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
            QVariantMap metadados;
            if (!QFileInfo::exists(caminho)) {
                auto* loc = qobject_cast<source::ui::LocalizadorQt*>(gc->loc);
                metadados.insert("tipo", identificar_tipo_arquivo(caminho, loc, item.value("nome").toString()));
                metadados.insert("mime", detectar_mime_por_extensao(caminho, item.value("nome").toString()));
            } else {
                metadados = get_metadados_gc(gc, item);
            }
            if (!caminho.isEmpty()) {
                QMutexLocker locker(&gc->lock_cache);
                gc->cache_metadados[caminho] = metadados;
            }
            if (gc->interface) QMetaObject::invokeMethod(gc->interface, "atualizar_colunas_tabela", Qt::QueuedConnection);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no processamento de metadados", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
