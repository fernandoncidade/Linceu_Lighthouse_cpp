#include "gmet_24_GetDimensoesArquivo.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QMutexLocker>

#include "gmet_19_GetTipoArquivo.h"
#include "gmet_38_GetMetadados.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_dimensoes_arquivo(source::services::GerenciadorColunas* self, const QVariantMap& item, source::ui::LocalizadorQt* loc) {
    try {
        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
        QFileInfo fi(caminho);
        if (!fi.exists()) return item.value("dimensoes").toString();

        if (fi.isDir()) {
            qint64 total = 0;
            QDirIterator it(caminho, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                ++total;
            }
            QMutexLocker locker(&self->lock_cache);
            QVariantMap m = self->cache_metadados.value(caminho).toMap();
            m.insert("arquivos", QString::number(total));
            self->cache_metadados.insert(caminho, m);
            return QString::number(total);
        }

        const QString tipo = identificar_tipo_arquivo(caminho, loc);
        if (tipo == (loc ? loc->get_text("file_image") : QStringLiteral("file_image"))) {
            QImageReader r(caminho);
            const QSize s = r.size();
            if (s.isValid()) return QStringLiteral("%1x%2").arg(s.width()).arg(s.height());
        }

        const QVariantMap metadados = get_metadados_gc(self, item);
        return metadados.value("dimensoes").toString();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter dimensões do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
