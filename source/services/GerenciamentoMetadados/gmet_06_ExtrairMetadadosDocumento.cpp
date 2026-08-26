#include "gmet_06_ExtrairMetadadosDocumento.h"

#include <algorithm>
#include <QtEndian>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpressionMatch>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>

#include "gmet_16_ExtrairMetadadosOlefile.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }

struct ZipStats {
    int total_entries = 0;
    int xml_entries = 0;
    int page_like_entries = 0;
    qint64 uncompressed_bytes = 0;
    bool valid = false;
};

const QSet<QString> kDocumentExts = {
    ".doc", ".docx", ".pdf", ".rtf", ".txt", ".odt", ".wpd", ".pages", ".map", ".md5", ".dotx", ".docm", ".dotm",
    ".msg", ".pst", ".ost", ".pub", ".tex", ".latex", ".djvu", ".epub", ".fb2", ".mobi", ".azw", ".tsv",
    ".rtfd", ".abw", ".sxw", ".sdw", ".gdoc", ".gslides", ".gdraw", ".gtable", ".gform", ".gmap", ".xps", ".oxps",
    ".lit", ".prc", ".pdb", ".ps", ".psw", ".log", ".nfo", ".me", ".1st", ".srt", ".sub", ".lrc", ".dvi", ".ans",
    ".azw3", ".ceb", ".chm", ".cwk", ".dox", ".eml", ".etf", ".ibooks", ".kwd", ".lwp", ".mbp", ".mcw", ".nb",
    ".odm", ".pml", ".stw", ".tcr", ".wn", ".wps", ".zrtf", ".asc", ".ascii", ".bib", ".diz", ".err",
    ".exc", ".faq", ".idx", ".info", ".man", ".readme", ".rel", ".sig", ".uue", ".vcf", ".vcard", ".wp", ".wp5", ".wp6", ".wri"
};

const QSet<QString> kTextDocExts = {
    ".txt", ".rtf", ".map", ".md5", ".tex", ".latex", ".tsv", ".ps", ".psw", ".log", ".nfo", ".me", ".1st",
    ".srt", ".sub", ".lrc", ".dvi", ".ans", ".gdoc", ".gslides", ".gdraw", ".gtable", ".gform", ".gmap",
    ".nb", ".pml", ".wn", ".asc", ".ascii", ".bib", ".diz", ".err", ".exc", ".faq", ".idx", ".info",
    ".man", ".readme", ".rel", ".sig", ".uue", ".vcf", ".vcard", ".fb2", ".eml"
};

const QSet<QString> kZipBasedDocExts = {
    ".docx", ".docm", ".dotx", ".dotm", ".odt", ".odm", ".epub", ".xps", ".oxps", ".pages",
    ".rtfd", ".sxw", ".abw", ".ibooks", ".azw3"
};

void inserir_estimativa(QVariantMap& m, qint64 bytes, double bytes_por_pagina = 25000.0) {
    const int paginas = std::max(1, static_cast<int>(bytes / bytes_por_pagina));
    const int linhas = std::max(1, paginas * 40);
    const int palavras = std::max(1, linhas * 8);
    m.insert("paginas", paginas);
    m.insert("paginas_estimadas", paginas);
    m.insert("linhas", linhas);
    m.insert("palavras", palavras);
}

bool extrair_texto_linhas(const QString& caminho, QVariantMap& m) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&f);
    in.setAutoDetectUnicode(true);

    int linhas = 0;
    int palavras = 0;
    int caracteres = 0;
    const int max_linhas = 1000000;
    while (!in.atEnd() && linhas < max_linhas) {
        const QString line = in.readLine();
        ++linhas;
        palavras += line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
        caracteres += line.size();
    }

    if (linhas <= 0) return false;
    m.insert("linhas", linhas);
    m.insert("palavras", std::max(1, palavras));
    m.insert("caracteres", caracteres);

    const int paginas = std::max(1, static_cast<int>(linhas / 40.0));
    m.insert("paginas", paginas);
    m.insert("paginas_estimadas", paginas);
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
            if (nome.endsWith(QStringLiteral(".xml")) || nome.endsWith(QStringLiteral(".xhtml")) || nome.endsWith(QStringLiteral(".html"))) {
                stats.xml_entries += 1;
            }
            if ((nome.contains(QStringLiteral("page")) || nome.contains(QStringLiteral("chapter")) || nome.contains(QStringLiteral("content"))) &&
                (nome.endsWith(QStringLiteral(".xml")) || nome.endsWith(QStringLiteral(".xhtml")) || nome.endsWith(QStringLiteral(".html")))) {
                stats.page_like_entries += 1;
            }

            i += total_entry;
            continue;
        }
        ++i;
    }

    stats.valid = stats.total_entries > 0;
    return stats;
}

void extrair_pdf_basico(const QString& caminho, const QFileInfo& fi, QVariantMap& m) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly)) {
        inserir_estimativa(m, fi.size(), 150000.0);
        return;
    }

    const QByteArray bytes = f.read(qMin(f.size(), static_cast<qint64>(16 * 1024 * 1024)));
    const QString texto = QString::fromLatin1(bytes);

    int paginas = texto.count(QStringLiteral("/Type /Page"));
    if (paginas <= 0) paginas = std::max(1, static_cast<int>(fi.size() / 150000));
    const int linhas_estimadas = std::max(1, paginas * 40);
    m.insert("paginas", paginas);
    m.insert("paginas_estimadas", paginas);
    m.insert("linhas", linhas_estimadas);
    m.insert("palavras", std::max(1, linhas_estimadas * 8));

    const QRegularExpression re_titulo(QStringLiteral("/Title\\s*\\(([^\\)]{1,200})\\)"));
    const QRegularExpression re_autor(QStringLiteral("/Author\\s*\\(([^\\)]{1,200})\\)"));
    const QRegularExpression re_criador(QStringLiteral("/Creator\\s*\\(([^\\)]{1,200})\\)"));
    const QRegularExpression re_produtor(QStringLiteral("/Producer\\s*\\(([^\\)]{1,200})\\)"));
    const QRegularExpression re_criado(QStringLiteral("/CreationDate\\s*\\(([^\\)]{1,80})\\)"));
    const QRegularExpression re_mod(QStringLiteral("/ModDate\\s*\\(([^\\)]{1,80})\\)"));

    const auto extrair = [&](const QRegularExpression& re) -> QString {
        const QRegularExpressionMatch match = re.match(texto);
        return match.hasMatch() ? match.captured(1).trimmed() : QString();
    };

    const QString titulo = extrair(re_titulo);
    const QString autor = extrair(re_autor);
    const QString criador = extrair(re_criador);
    const QString produtor = extrair(re_produtor);
    const QString data_criacao = extrair(re_criado);
    const QString data_mod = extrair(re_mod);
    if (!titulo.isEmpty()) m.insert("titulo", titulo);
    if (!autor.isEmpty()) m.insert("autor", autor);
    if (!criador.isEmpty()) m.insert("criador", criador);
    if (!produtor.isEmpty()) m.insert("produtor", produtor);
    if (!data_criacao.isEmpty()) m.insert("data_criacao_doc", parse_pdf_date(data_criacao));
    if (!data_mod.isEmpty()) m.insert("data_mod_doc", parse_pdf_date(data_mod));
}

}  // namespace

QVariantMap extrair_metadados_documento(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        if (!kDocumentExts.contains(ext)) return m;

        m.insert("formato_documento", ext);
        bool extraido = false;

        if (ext == QStringLiteral(".pdf")) {
            extrair_pdf_basico(caminho, fi, m);
            extraido = true;
        }

        if (!extraido && kTextDocExts.contains(ext)) {
            extraido = extrair_texto_linhas(caminho, m);
        }

        if (!extraido && kZipBasedDocExts.contains(ext)) {
            const ZipStats stats = extrair_zip_stats(caminho);
            if (stats.valid) {
                const int paginas = stats.page_like_entries > 0
                                        ? stats.page_like_entries
                                        : std::max(1, static_cast<int>(stats.uncompressed_bytes / 90000.0));
                const int linhas = std::max(1, paginas * 40);
                const int palavras = std::max(1, linhas * 8);
                m.insert("paginas", paginas);
                m.insert("paginas_estimadas", paginas);
                m.insert("linhas", linhas);
                m.insert("palavras", palavras);
                extraido = true;
            }
        }

        if (!extraido) {
            inserir_estimativa(m, fi.size());
        }

        if (ext == QStringLiteral(".doc")) {
            const QVariantMap ole = extrair_metadados_olefile(caminho, loc);
            for (auto it = ole.constBegin(); it != ole.constEnd(); ++it) {
                if (!m.contains(it.key()) || m.value(it.key()).toString().isEmpty()) {
                    m.insert(it.key(), it.value());
                }
            }
        }

        if (!m.contains("paginas")) m.insert("paginas", 1);
        if (!m.contains("paginas_estimadas")) m.insert("paginas_estimadas", m.value("paginas", 1));
        if (!m.contains("linhas")) m.insert("linhas", std::max(1, m.value("paginas", 1).toInt() * 40));
        if (!m.contains("palavras")) m.insert("palavras", std::max(1, m.value("linhas", 1).toInt() * 8));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral ao extrair metadados do documento", true);
    }
    return m;
}

QString parse_pdf_date(const QString& date_str) {
    QString d = date_str.trimmed();
    if (d.startsWith(QStringLiteral("D:"))) d = d.mid(2);
    d.remove('\'');
    if (d.size() >= 14) {
        const QString ano = d.mid(0, 4);
        const QString mes = d.mid(4, 2);
        const QString dia = d.mid(6, 2);
        const QString hora = d.mid(8, 2);
        const QString min = d.mid(10, 2);
        const QString seg = d.mid(12, 2);
        return QStringLiteral("%1-%2-%3 %4:%5:%6").arg(ano, mes, dia, hora, min, seg);
    }
    return d;
}

}  // namespace source::services::GerenciamentoMetadados
