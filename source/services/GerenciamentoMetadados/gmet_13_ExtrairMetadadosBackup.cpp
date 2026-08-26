#include "gmet_13_ExtrairMetadadosBackup.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "gmet_21_GetFormataTamanho.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

bool eh_arquivo_texto(const QString& caminho) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray sample = f.read(1024);
    if (sample.isEmpty()) return false;
    int inval = 0;
    for (char c : sample) {
        const unsigned char b = static_cast<unsigned char>(c);
        if (b < 9 || (b > 13 && b < 32 && b != 27)) ++inval;
    }
    return (static_cast<double>(inval) / static_cast<double>(sample.size())) < 0.3;
}

int contar_linhas(const QString& caminho) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QTextStream in(&f);
    int linhas = 0;
    while (!in.atEnd()) { in.readLine(); ++linhas; }
    return linhas;
}

QVariantMap extrair_metadados_backup(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return m;
        m.insert("tipo_backup", QStringLiteral("Backup genérico"));
        m.insert("tamanho", formata_tamanho(fi.size()));
        if (eh_arquivo_texto(caminho)) m.insert("linhas", QString::number(contar_linhas(caminho)));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados do backup", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
