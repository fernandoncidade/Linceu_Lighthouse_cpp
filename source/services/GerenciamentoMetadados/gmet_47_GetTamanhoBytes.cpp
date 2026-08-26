#include "gmet_47_GetTamanhoBytes.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMutexLocker>
#include <QSet>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {
namespace {

struct EstatisticasDiretorio {
    qint64 tamanho_bytes = 0;
    qint64 arquivos = 0;
};

QString normalizar_caminho_cache(const QString& caminho) {
    QFileInfo info(caminho);
    QString normalizado = info.canonicalFilePath();
    if (normalizado.isEmpty()) {
        normalizado = info.absoluteFilePath();
    }
#ifdef Q_OS_WIN
    normalizado = normalizado.toLower();
#endif
    return QDir::cleanPath(normalizado);
}

bool obter_estatisticas_cache(source::services::GerenciadorColunas* gc,
                              const QString& caminho,
                              EstatisticasDiretorio* out) {
    if (gc == nullptr || out == nullptr || caminho.isEmpty()) {
        return false;
    }

    QMutexLocker locker(&gc->lock_cache);
    const QVariantMap cache = gc->cache_metadados.value(caminho).toMap();
    if (!cache.value(QStringLiteral("_dir_stats_ready")).toBool()) {
        return false;
    }

    out->tamanho_bytes = cache.value(QStringLiteral("size_b")).toLongLong();
    out->arquivos = cache.value(QStringLiteral("arquivos")).toLongLong();
    return true;
}

void salvar_estatisticas_cache(source::services::GerenciadorColunas* gc,
                               const QString& caminho,
                               const EstatisticasDiretorio& stats) {
    if (gc == nullptr || caminho.isEmpty()) {
        return;
    }

    QMutexLocker locker(&gc->lock_cache);
    QVariantMap cache = gc->cache_metadados.value(caminho).toMap();
    cache.insert(QStringLiteral("size_b"), stats.tamanho_bytes);
    cache.insert(QStringLiteral("size_kb"), QString::number(stats.tamanho_bytes / 1024.0, 'f', 2));
    cache.insert(QStringLiteral("size_mb"), QString::number(stats.tamanho_bytes / (1024.0 * 1024.0), 'f', 2));
    cache.insert(QStringLiteral("size_gb"),
                 QString::number(stats.tamanho_bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2));
    cache.insert(QStringLiteral("size_tb"),
                 QString::number(stats.tamanho_bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2));
    cache.insert(QStringLiteral("arquivos"), stats.arquivos);
    cache.insert(QStringLiteral("_dir_stats_ready"), true);
    cache.insert(QStringLiteral("_dir_stats_cached_at"), QDateTime::currentSecsSinceEpoch());
    gc->cache_metadados.insert(caminho, cache);
}

EstatisticasDiretorio calcular_estatisticas_diretorio(source::services::GerenciadorColunas* gc,
                                                       const QString& caminho,
                                                       QSet<QString>& em_andamento) {
    EstatisticasDiretorio cached_stats;
    if (obter_estatisticas_cache(gc, caminho, &cached_stats)) {
        return cached_stats;
    }

    const QString chave_cache = normalizar_caminho_cache(caminho);
    if (chave_cache.isEmpty() || em_andamento.contains(chave_cache)) {
        return {};
    }

    em_andamento.insert(chave_cache);

    EstatisticasDiretorio stats;
    QDir dir(caminho);
    const QFileInfoList entradas =
        dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDir::Name | QDir::DirsFirst);

    for (const QFileInfo& entrada : entradas) {
        if (!entrada.exists() || entrada.isSymLink()) {
            continue;
        }
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        if (entrada.isJunction()) {
            continue;
        }
#endif

        if (entrada.isDir()) {
            const EstatisticasDiretorio child_stats =
                calcular_estatisticas_diretorio(gc, entrada.absoluteFilePath(), em_andamento);
            stats.tamanho_bytes += child_stats.tamanho_bytes;
            stats.arquivos += child_stats.arquivos;
        } else {
            stats.tamanho_bytes += entrada.size();
            stats.arquivos += 1;
        }
    }

    em_andamento.remove(chave_cache);
    salvar_estatisticas_cache(gc, caminho, stats);
    return stats;
}

}  // namespace

qint64 get_tamanho_bytes(source::services::GerenciadorColunas* gc, const QVariantMap& item) {
    try {
        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString()
                                                                              : item.value("dir_atual").toString();
        if (caminho.isEmpty() || !QFileInfo::exists(caminho)) return 0;

        QFileInfo fi(caminho);
        if (fi.isFile()) return fi.size();

        EstatisticasDiretorio stats;
        if (obter_estatisticas_cache(gc, caminho, &stats)) {
            return stats.tamanho_bytes;
        }

        QSet<QString> em_andamento;
        stats = calcular_estatisticas_diretorio(gc, caminho, em_andamento);
        return stats.tamanho_bytes;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter tamanho", true);
        return 0;
    }
}

}  // namespace source::services::GerenciamentoMetadados
