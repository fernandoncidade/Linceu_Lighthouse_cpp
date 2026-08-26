#include "gmet_14_ExtrairMetadadosLog.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

const QSet<QString> kLogExts = {
    ".log", ".trace", ".dmp", ".dump", ".hprof", ".core", ".err", ".out", ".blg", ".etl", ".evtx", ".glog",
    ".lst", ".prn", ".trc", ".wrn", ".clg", ".audit", ".journal", ".lg", ".txt", ".dat", ".svclog", ".history", ".evt"
};

bool has_ext(const QString& nome_lower, const QSet<QString>& exts) {
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext)) return true;
    }
    return false;
}

QString detectar_ext(const QString& nome_lower, const QSet<QString>& exts) {
    QString melhor;
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext) && ext.size() > melhor.size()) {
            melhor = ext;
        }
    }
    return melhor;
}

bool parece_texto(const QByteArray& amostra) {
    if (amostra.isEmpty()) return true;
    int invalidos = 0;
    for (const char c : amostra) {
        const unsigned char b = static_cast<unsigned char>(c);
        if (b == 0) return false;
        if (b < 9 || (b > 13 && b < 32)) ++invalidos;
    }
    return (static_cast<double>(invalidos) / static_cast<double>(amostra.size())) < 0.30;
}

qint64 contar_linhas_rapido(QFile& f) {
    qint64 linhas = 0;
    while (!f.atEnd()) {
        linhas += f.read(1024 * 1024).count('\n');
    }
    return linhas;
}

}  // namespace

QVariantMap extrair_metadados_log(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString nome_lower = fi.fileName().toLower();
        if (!has_ext(nome_lower, kLogExts) && !nome_lower.contains(QStringLiteral("log"))) return m;

        m.insert("tipo_log", QStringLiteral("Log genérico"));
        m.insert("tamanho_log_bytes", fi.size());

        const QString formato = detectar_ext(nome_lower, kLogExts);
        if (!formato.isEmpty()) m.insert("formato_log", formato);

        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) {
            m.insert("linhas", QStringLiteral("0"));
            m.insert("log_binario", true);
            return m;
        }

        const QByteArray amostra = f.read(4096);
        const bool texto = parece_texto(amostra);
        f.seek(0);

        if (texto) {
            const qint64 linhas = contar_linhas_rapido(f);
            m.insert("linhas", QString::number(linhas));
            m.insert("log_binario", false);
        } else {
            m.insert("linhas", QStringLiteral("0"));
            m.insert("log_binario", true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados do log", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
