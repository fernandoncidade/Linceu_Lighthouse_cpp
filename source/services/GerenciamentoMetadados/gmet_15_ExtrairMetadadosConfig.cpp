#include "gmet_15_ExtrairMetadadosConfig.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

const QSet<QString> kConfigExts = {
    ".ini", ".cfg", ".config", ".conf", ".properties", ".plist", ".toml", ".settings", ".rc", ".json", ".yaml",
    ".yml", ".env", ".desktop", ".profile", ".bashrc", ".zshrc", ".cshrc", ".login", ".logout", ".npmrc", ".editorconfig",
    ".cf", ".cnf", ".inf", ".reg", ".scr", ".sys", ".xml", ".prefs", ".opt", ".options", ".ds_store", ".gitconfig",
    ".htaccess", ".htpasswd", ".manifest", ".pro", ".props", ".sln", ".user", ".vcxproj", ".xcu", ".xmc"
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

QVariantMap extrair_metadados_config(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString nome_lower = fi.fileName().toLower();
        if (!has_ext(nome_lower, kConfigExts)) return m;

        const QString formato = detectar_ext(nome_lower, kConfigExts);
        if (!formato.isEmpty()) m.insert("formato", formato);
        m.insert("tipo_config", QStringLiteral("Configuração genérica"));
        m.insert("tamanho_config_bytes", fi.size());

        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) {
            m.insert("linhas", QStringLiteral("0"));
            m.insert("config_binario", true);
            return m;
        }

        const QByteArray amostra = f.read(4096);
        const bool texto = parece_texto(amostra);
        f.seek(0);

        if (texto) {
            const qint64 linhas = contar_linhas_rapido(f);
            m.insert("linhas", QString::number(linhas));
            m.insert("config_binario", false);
        } else {
            m.insert("linhas", QStringLiteral("0"));
            m.insert("config_binario", true);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados da configuração", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
