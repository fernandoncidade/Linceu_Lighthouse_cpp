#include "gmet_08_ExtrairMetadadosApresentacao.h"

#include <algorithm>
#include <QtEndian>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }

struct ZipStats {
    int total_entries = 0;
    int slide_entries = 0;
    qint64 uncompressed_bytes = 0;
    bool valid = false;
};

const QSet<QString> kPresentationExts = {
    ".ppt", ".pptx", ".odp", ".key", ".potx", ".ppsx", ".pptm", ".potm", ".ppsm", ".pps", ".pot", ".sxi",
    ".gslides", ".sti", ".sdd", ".sdp", ".fods", ".keynote", ".nb", ".odg", ".otp", ".pez", ".shf", ".show", ".sxd", ".watch"
};

const QSet<QString> kZipBasedPresentationExts = {
    ".pptx", ".potx", ".ppsx", ".pptm", ".potm", ".ppsm", ".odp", ".otp", ".key", ".keynote", ".sxi", ".odg", ".watch", ".pez", ".show"
};

const QSet<QString> kTextXmlPresentationExts = {
    ".fods", ".sxd", ".sti", ".sdd", ".sdp", ".gslides", ".nb", ".show"
};

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

            if ((nome.startsWith(QStringLiteral("ppt/slides/slide")) && nome.endsWith(QStringLiteral(".xml"))) ||
                (nome.contains(QStringLiteral("/slides/slide")) && nome.endsWith(QStringLiteral(".xml"))) ||
                (nome.contains(QStringLiteral("slide")) && nome.endsWith(QStringLiteral(".xml")))) {
                stats.slide_entries += 1;
            }

            i += total_entry;
            continue;
        }
        ++i;
    }

    stats.valid = stats.total_entries > 0;
    return stats;
}

bool extrair_slides_texto_xml(const QString& caminho, QVariantMap& m) {
    QFile f(caminho);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&f);
    in.setAutoDetectUnicode(true);
    const QString amostra = in.read(2 * 1024 * 1024).toLower();
    if (amostra.trimmed().isEmpty()) return false;

    const int p_sld = amostra.count(QStringLiteral("<p:sld"));
    const int draw_page = amostra.count(QStringLiteral("<draw:page"));
    const int slide_tag = amostra.count(QRegularExpression(QStringLiteral("<slide\\b"), QRegularExpression::CaseInsensitiveOption));

    const int slides = std::max(1, p_sld + draw_page + slide_tag);
    m.insert("slides", slides);
    m.insert("slides_estimados", slides);
    m.insert("paginas", slides);
    return true;
}

void preencher_estimativa_generica(const QFileInfo& fi, QVariantMap& m) {
    const qint64 bytes = std::max<qint64>(fi.size(), 1);
    const int slides = std::max(1, static_cast<int>(bytes / 140000.0));
    m.insert("slides", slides);
    m.insert("slides_estimados", slides);
    m.insert("paginas", std::max(1, slides));
}

}  // namespace

QVariantMap extrair_metadados_apresentacao(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        if (!kPresentationExts.contains(ext)) return m;

        m.insert("formato_apresentacao", ext);
        bool extraido = false;

        if (kZipBasedPresentationExts.contains(ext)) {
            const ZipStats stats = extrair_zip_stats(caminho);
            if (stats.valid) {
                const int slides = stats.slide_entries > 0
                                       ? stats.slide_entries
                                       : std::max(1, static_cast<int>(stats.uncompressed_bytes / 180000.0));
                m.insert("slides", slides);
                m.insert("slides_estimados", slides);
                m.insert("paginas", std::max(1, slides));
                extraido = true;
            }
        }

        if (!extraido && kTextXmlPresentationExts.contains(ext)) {
            extraido = extrair_slides_texto_xml(caminho, m);
        }

        if (!extraido) {
            preencher_estimativa_generica(fi, m);
        }

        if (!m.contains("slides")) m.insert("slides", 1);
        if (!m.contains("slides_estimados")) m.insert("slides_estimados", m.value("slides", 1));
        if (!m.contains("paginas")) m.insert("paginas", m.value("slides", 1));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral ao extrair metadados da apresentação", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
