#include "exec_01_inteligencia_executaveis.h"

#include <QFileInfo>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>

#include "source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.h"

namespace source::security {
namespace {

constexpr int kMaxExecutableCacheEntries = 16384;

QString event_path(const QVariantMap& evento) {
    const QString atual = evento.value(QStringLiteral("dir_atual")).toString();
    if (!atual.isEmpty() && QFileInfo::exists(atual)) {
        return atual;
    }
    return evento.value(QStringLiteral("dir_anterior")).toString();
}

bool is_script_or_executable(const QString& path) {
    static const QSet<QString> kRelevantExts = {
        QStringLiteral("exe"), QStringLiteral("dll"), QStringLiteral("bat"), QStringLiteral("cmd"),
        QStringLiteral("ps1"), QStringLiteral("vbs"), QStringLiteral("js"),  QStringLiteral("jse"),
        QStringLiteral("scr"), QStringLiteral("msi"), QStringLiteral("com"),
    };
    return kRelevantExts.contains(QFileInfo(path).suffix().toLower());
}

bool is_script_extension(const QString& ext) {
    return ext == QStringLiteral("ps1") || ext == QStringLiteral("bat") || ext == QStringLiteral("cmd") ||
           ext == QStringLiteral("vbs") || ext == QStringLiteral("js") || ext == QStringLiteral("jse");
}

bool is_binary_executable_extension(const QString& ext) {
    return ext == QStringLiteral("exe") || ext == QStringLiteral("dll") || ext == QStringLiteral("msi") ||
           ext == QStringLiteral("scr") || ext == QStringLiteral("com");
}

QString first_non_empty_value(const QVariantMap& values, const QStringList& keys) {
    for (const QString& key : keys) {
        const QString value = values.value(key).toString().trimmed();
        if (!value.isEmpty()) {
            return value;
        }
    }
    return QString();
}

QMutex& executable_cache_lock() {
    static QMutex mutex;
    return mutex;
}

QHash<QString, QVariantMap>& executable_cache() {
    static QHash<QString, QVariantMap> cache;
    return cache;
}

QString executable_cache_key(const QFileInfo& info) {
    return info.absoluteFilePath() +
           QStringLiteral("::%1::%2")
               .arg(info.size())
               .arg(info.lastModified().toUTC().toMSecsSinceEpoch());
}

}  // namespace

QVariantMap InteligenciaExecutaveis::analyze(const QVariantMap& evento, const QVariantMap&) const {
    QVariantMap out;
    const QString path = event_path(evento);
    const QFileInfo info(path);
    if (path.isEmpty() || !info.exists() || !info.isFile() || !is_script_or_executable(path)) {
        return out;
    }

    const QString cache_key = executable_cache_key(info);
    {
        QMutexLocker locker(&executable_cache_lock());
        const auto it = executable_cache().constFind(cache_key);
        if (it != executable_cache().constEnd()) {
            return it.value();
        }
    }

    const QString ext = QFileInfo(path).suffix().toLower();
    QString publisher = first_non_empty_value(
        evento,
        {QStringLiteral("publisher"), QStringLiteral("empresa"), QStringLiteral("produto"),
         QStringLiteral("autor")});
    QString signature_state = first_non_empty_value(
        evento,
        {QStringLiteral("signature_state"), QStringLiteral("assinado")});

    if (is_binary_executable_extension(ext) &&
        (publisher.isEmpty() || signature_state.isEmpty())) {
        const QVariantMap metadata =
            source::services::GerenciamentoMetadados::extrair_metadados_executavel(path, nullptr);
        if (publisher.isEmpty()) {
            publisher = first_non_empty_value(
                metadata,
                {QStringLiteral("empresa"), QStringLiteral("produto")});
            if (publisher.isEmpty()) {
                publisher = evento.value(QStringLiteral("autor")).toString();
            }
        }
        if (signature_state.isEmpty()) {
            signature_state = metadata.value(QStringLiteral("assinado")).toString();
        }
    }

    QString script_risk = QStringLiteral("low");
    if (is_script_extension(ext)) {
        script_risk = QStringLiteral("high");
        if (signature_state.isEmpty()) {
            signature_state = QStringLiteral("Desconhecido");
        }
    } else if (is_binary_executable_extension(ext)) {
        script_risk = signature_state.compare(QStringLiteral("Não"), Qt::CaseInsensitive) == 0 ||
                              signature_state.compare(QStringLiteral("No"), Qt::CaseInsensitive) == 0
                          ? QStringLiteral("high")
                          : QStringLiteral("medium");
    }

    out.insert(QStringLiteral("publisher"), publisher);
    out.insert(QStringLiteral("signature_state"), signature_state);
    out.insert(QStringLiteral("script_risk"), script_risk);

    {
        QMutexLocker locker(&executable_cache_lock());
        if (executable_cache().size() >= kMaxExecutableCacheEntries) {
            executable_cache().clear();
        }
        executable_cache().insert(cache_key, out);
    }
    return out;
}

}  // namespace source::security
