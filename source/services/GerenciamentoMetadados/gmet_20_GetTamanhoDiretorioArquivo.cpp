#include "gmet_20_GetTamanhoDiretorioArquivo.h"

#include <QDateTime>
#include <QDirIterator>
#include <QFileInfo>
#include <QMutexLocker>

#include "gmet_21_GetFormataTamanho.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_tamanho_diretorio_arquivo(source::services::GerenciadorColunas* self, const QVariantMap& item_data, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    try {
        const QString caminho = item_data.value("dir_atual").toString().isEmpty() ? item_data.value("dir_anterior").toString() : item_data.value("dir_atual").toString();
        if (caminho.isEmpty() || !QFileInfo::exists(caminho)) return QString();

        QFileInfo fi(caminho);
        if (fi.isFile()) {
            return get_formata_tamanho(fi.size());
        }

        qint64 total = 0;
        QDirIterator it(caminho, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            total += it.fileInfo().size();
        }

        {
            QMutexLocker locker(&self->lock_cache);
            QVariantMap m = self->cache_metadados.value(caminho).toMap();
            m.insert("tamanho_dir", get_formata_tamanho(total));
            m.insert("tamanho_dir_bytes", total);
            m.insert("tamanho_dir_cached_at", QDateTime::currentSecsSinceEpoch());
            self->cache_metadados.insert(caminho, m);
        }

        return get_formata_tamanho(total);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral no cálculo de tamanho", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
