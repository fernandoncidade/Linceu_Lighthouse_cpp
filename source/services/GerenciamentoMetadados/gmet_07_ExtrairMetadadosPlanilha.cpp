#include "gmet_07_ExtrairMetadadosPlanilha.h"

#include <algorithm>
#include <cmath>
#include <QtEndian>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QTextStream>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

struct ZipStats {
    int total_entries = 0;
    int worksheet_entries = 0;
    int xml_entries = 0;
    qint64 uncompressed_bytes = 0;
    QStringList worksheet_names;
    bool valid = false;
};

inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }

const QSet<QString> kSpreadsheetExts = {
    ".xls", ".xlsx", ".xlsm", ".ods", ".csv", ".tsv", ".numbers", ".xltx", ".xltm", ".xlsb", ".xlr",
    ".xlt", ".xml", ".sxc", ".slk", ".dif", ".prn", ".gsheet", ".123", ".wk1", ".wk3", ".wk4", ".wks",
    ".wb1", ".wb2", ".wb3", ".ab2", ".ab3", ".aws", ".bcsv", ".clf", ".et", ".fm", ".gnumeric", ".mar",
    ".nmbtemplate", ".qpw", ".sdc", ".stc", ".vc", ".xlw"
};

const QSet<QString> kDelimitedExts = {
    ".csv", ".tsv", ".bcsv", ".prn", ".slk", ".dif", ".sdc", ".stc", ".vc", ".wk1", ".wk3", ".wk4",
    ".wks", ".wb1", ".wb2", ".wb3", ".ab2", ".ab3", ".aws", ".clf", ".et", ".fm", ".mar", ".qpw", ".123"
};

const QSet<QString> kZipBasedExts = {
    ".xlsx", ".xlsm", ".xltx", ".xltm", ".xlsb", ".ods", ".numbers", ".nmbtemplate", ".gnumeric"
};

const QSet<QString> kLegacyBinaryExts = {
    ".xls", ".xlt", ".xlr", ".xlw"
};

QChar detectar_delimitador(const QString& linha, const QString& ext) {
    if (ext == QStringLiteral(".tsv")) return QLatin1Char('\t');
    if (ext == QStringLiteral(".bcsv")) return QLatin1Char(';');

    const QList<QChar> candidatos = {
        QLatin1Char(','),
        QLatin1Char(';'),
        QLatin1Char('\t'),
        QLatin1Char('|')
    };

    int melhor_contagem = 0;
    QChar melhor;
    for (const QChar& c : candidatos) {
        const int qtd = linha.count(c);
        if (qtd > melhor_contagem) {
            melhor_contagem = qtd;
            melhor = c;
        }
    }
    return melhor;
}

QStringList dividir_linha(const QString& linha, QChar delimitador) {
    if (!delimitador.isNull()) {
        return linha.split(delimitador);
    }
    return linha.trimmed().split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
}

bool extrair_planilha_texto(const QString& caminho, const QString& ext, QVariantMap& m) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&f);
    in.setAutoDetectUnicode(true);

    int linhas = 0;
    int colunas = 0;
    QChar delimitador;
    const int max_linhas = 200000;

    while (!in.atEnd() && linhas < max_linhas) {
        const QString linha = in.readLine();
        if (linha.trimmed().isEmpty()) continue;

        if (delimitador.isNull()) {
            delimitador = detectar_delimitador(linha, ext);
        }

        const QStringList partes = dividir_linha(linha, delimitador);
        colunas = std::max(colunas, static_cast<int>(partes.size()));
        ++linhas;
    }

    if (linhas <= 0) return false;

    m.insert("planilhas", 1);
    m.insert("total_linhas", linhas);
    m.insert("linhas", linhas);
    m.insert("colunas", std::max(1, colunas));
    m.insert("registros", std::max(0, linhas - 1));
    return true;
}

ZipStats extrair_zip_stats(const QString& caminho) {
    ZipStats stats;
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly)) return stats;

    const qint64 tamanho = f.size();
    if (tamanho < 46) return stats;

    const qint64 limite_leitura = 64ll * 1024ll * 1024ll;
    const qint64 inicio = tamanho > limite_leitura ? (tamanho - limite_leitura) : 0;
    f.seek(inicio);
    const QByteArray bytes = f.read(limite_leitura);
    if (bytes.size() < 46) return stats;

    const uchar* base = reinterpret_cast<const uchar*>(bytes.constData());
    int i = 0;
    while (i + 46 <= bytes.size()) {
        if (base[i + 0] == 0x50 && base[i + 1] == 0x4B &&
            base[i + 2] == 0x01 && base[i + 3] == 0x02) {
            const quint32 uncomp = rd32le(base + i + 24);
            const quint16 nome_len = rd16le(base + i + 28);
            const quint16 extra_len = rd16le(base + i + 30);
            const quint16 coment_len = rd16le(base + i + 32);

            const int total_entry = 46 + nome_len + extra_len + coment_len;
            if (i + total_entry > bytes.size()) break;

            const QByteArray nome_raw = bytes.mid(i + 46, nome_len);
            const QString nome = QString::fromUtf8(nome_raw).trimmed().toLower();

            stats.total_entries += 1;
            stats.uncompressed_bytes += static_cast<qint64>(uncomp);

            if (nome.endsWith(QStringLiteral(".xml"))) stats.xml_entries += 1;
            if (nome.startsWith(QStringLiteral("xl/worksheets/")) && nome.endsWith(QStringLiteral(".xml"))) {
                stats.worksheet_entries += 1;
                stats.worksheet_names << QFileInfo(nome).baseName();
            } else if (nome.startsWith(QStringLiteral("tables/")) && nome.endsWith(QStringLiteral(".xml"))) {
                stats.worksheet_entries += 1;
                stats.worksheet_names << QFileInfo(nome).baseName();
            }

            i += total_entry;
            continue;
        }
        ++i;
    }

    stats.valid = stats.total_entries > 0;
    stats.worksheet_names.removeDuplicates();
    return stats;
}

bool extrair_planilha_xml(const QString& caminho, QVariantMap& m) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&f);
    in.setAutoDetectUnicode(true);
    const QString amostra = in.read(1024 * 1024);
    if (amostra.trimmed().isEmpty()) return false;

    const QString lower = amostra.toLower();
    const bool parece_planilha =
        lower.contains(QStringLiteral("<worksheet")) ||
        lower.contains(QStringLiteral("<workbook")) ||
        lower.contains(QStringLiteral("<table")) ||
        lower.contains(QStringLiteral("urn:schemas-microsoft-com:office:spreadsheet")) ||
        lower.contains(QStringLiteral("spreadsheetml"));

    if (!parece_planilha) return false;

    const int linhas_a = amostra.count(QRegularExpression(QStringLiteral("<row\\b"), QRegularExpression::CaseInsensitiveOption));
    const int linhas_b = amostra.count(QRegularExpression(QStringLiteral("<ss:row\\b"), QRegularExpression::CaseInsensitiveOption));
    const int linhas = std::max(linhas_a + linhas_b, 1);

    const int celulas_a = amostra.count(QRegularExpression(QStringLiteral("<c\\b"), QRegularExpression::CaseInsensitiveOption));
    const int celulas_b = amostra.count(QRegularExpression(QStringLiteral("<cell\\b"), QRegularExpression::CaseInsensitiveOption));
    const int celulas = celulas_a + celulas_b;

    const int colunas = std::max(1, (linhas > 0 ? (celulas / linhas) : celulas));

    m.insert("planilhas", 1);
    m.insert("total_linhas", linhas);
    m.insert("linhas", linhas);
    m.insert("colunas", colunas);
    m.insert("registros", std::max(0, linhas - 1));
    return true;
}

void preencher_estimativa_generica(const QFileInfo& fi, QVariantMap& m) {
    const qint64 bytes = std::max<qint64>(fi.size(), 1);
    const int colunas_estimadas = std::max(1, std::min(256, static_cast<int>(std::sqrt(static_cast<double>(bytes) / 1024.0))));
    const int linhas_estimadas = std::max(1, static_cast<int>(bytes / (colunas_estimadas * 24.0)));

    m.insert("planilhas", 1);
    m.insert("total_linhas", linhas_estimadas);
    m.insert("linhas", linhas_estimadas);
    m.insert("colunas", colunas_estimadas);
    m.insert("registros", std::max(0, linhas_estimadas - 1));
}

}  // namespace

QVariantMap extrair_metadados_planilha(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();

        if (!kSpreadsheetExts.contains(ext)) return m;

        m.insert("formato_planilha", ext);
        m.insert("nomes_planilhas", fi.baseName());

        bool extraido = false;

        if (ext == QStringLiteral(".xml")) {
            extraido = extrair_planilha_xml(caminho, m);
        } else if (kDelimitedExts.contains(ext)) {
            extraido = extrair_planilha_texto(caminho, ext, m);
        } else if (kZipBasedExts.contains(ext)) {
            const ZipStats stats = extrair_zip_stats(caminho);
            if (stats.valid) {
                const int planilhas = std::max(1, stats.worksheet_entries);
                const int linhas_estimadas = std::max(1, static_cast<int>(stats.uncompressed_bytes / 96.0));
                const int colunas_estimadas = std::max(1, std::min(256, static_cast<int>(std::sqrt(static_cast<double>(stats.uncompressed_bytes) / 2048.0))));

                m.insert("planilhas", planilhas);
                m.insert("total_linhas", linhas_estimadas);
                m.insert("linhas", linhas_estimadas);
                m.insert("colunas", colunas_estimadas);
                m.insert("registros", std::max(0, linhas_estimadas - planilhas));

                if (!stats.worksheet_names.isEmpty()) {
                    m.insert("nomes_planilhas", stats.worksheet_names.join(QStringLiteral(", ")));
                }
                extraido = true;
            }
        } else if (kLegacyBinaryExts.contains(ext)) {
            preencher_estimativa_generica(fi, m);
            extraido = true;
        } else {
            // Outros formatos proprietários da lista: fallback com estimativa.
            preencher_estimativa_generica(fi, m);
            extraido = true;
        }

        if (!extraido) {
            preencher_estimativa_generica(fi, m);
        }

        if (!m.contains("planilhas")) m.insert("planilhas", 1);
        if (!m.contains("colunas")) m.insert("colunas", 1);
        if (!m.contains("total_linhas")) m.insert("total_linhas", 1);
        if (!m.contains("linhas")) m.insert("linhas", m.value("total_linhas", 1));
        if (!m.contains("registros")) {
            const int linhas = m.value("total_linhas", 1).toInt();
            m.insert("registros", std::max(0, linhas - 1));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral ao extrair metadados da planilha", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
