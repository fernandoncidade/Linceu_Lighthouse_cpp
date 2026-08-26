#include "gmet_03_ExtrairMetadadosImagem.h"

#include <QtEndian>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QRegularExpression>
#include <QSet>
#include <cstring>
#include <limits>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

inline quint32 rd32be(const uchar* p) { return qFromBigEndian<quint32>(p); }
inline quint16 rd16be(const uchar* p) { return qFromBigEndian<quint16>(p); }
inline quint64 rd64be(const uchar* p) { return qFromBigEndian<quint64>(p); }
inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }

void inserir_dim(QVariantMap& m, int w, int h) {
    if (w > 0 && h > 0) m.insert("dimensoes", QStringLiteral("%1x%2").arg(w).arg(h));
}

void extrair_png(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(24);
    if (d.size() < 24) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    static const uchar sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    if (memcmp(p, sig, 8) != 0) return;
    if (memcmp(p + 12, "IHDR", 4) != 0) return;
    const int w = static_cast<int>(rd32be(p + 16));
    const int h = static_cast<int>(rd32be(p + 20));
    inserir_dim(m, w, h);
}

void extrair_gif(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(10);
    if (d.size() < 10) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (!(memcmp(p, "GIF87a", 6) == 0 || memcmp(p, "GIF89a", 6) == 0)) return;
    const int w = static_cast<int>(rd16le(p + 6));
    const int h = static_cast<int>(rd16le(p + 8));
    inserir_dim(m, w, h);
}

void extrair_bmp(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(32);
    if (d.size() < 26) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (p[0] != 'B' || p[1] != 'M') return;
    const int w = static_cast<int>(rd32le(p + 18));
    int h = static_cast<int>(rd32le(p + 22));
    if (h < 0) h = -h;  // top-down bitmap
    inserir_dim(m, w, h);
}

void extrair_jpeg(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(4 * 1024 * 1024)));
    if (d.size() < 4) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (p[0] != 0xFF || p[1] != 0xD8) return;

    int i = 2;
    while (i + 3 < d.size()) {
        if (p[i] != 0xFF) {
            ++i;
            continue;
        }
        while (i < d.size() && p[i] == 0xFF) ++i;
        if (i >= d.size()) break;

        const uchar marker = p[i++];
        if (marker == 0xD8 || marker == 0x01) continue;
        if (marker == 0xD9 || marker == 0xDA) break;
        if (i + 1 >= d.size()) break;

        const int seg_len = (static_cast<int>(p[i]) << 8) | static_cast<int>(p[i + 1]);
        if (seg_len < 2 || i + seg_len > d.size()) break;

        const bool sof =
            (marker >= 0xC0 && marker <= 0xCF) &&
            marker != 0xC4 && marker != 0xC8 && marker != 0xCC;
        if (sof && i + 7 < d.size()) {
            const int h = (static_cast<int>(p[i + 3]) << 8) | static_cast<int>(p[i + 4]);
            const int w = (static_cast<int>(p[i + 5]) << 8) | static_cast<int>(p[i + 6]);
            inserir_dim(m, w, h);
            return;
        }

        i += seg_len;
    }
}

void extrair_webp(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(64);
    if (d.size() < 30) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "RIFF", 4) != 0 || memcmp(p + 8, "WEBP", 4) != 0) return;
    const QByteArray chunk = d.mid(12, 4);

    if (chunk == "VP8X" && d.size() >= 30) {
        const int w = 1 + (static_cast<int>(p[24]) | (static_cast<int>(p[25]) << 8) | (static_cast<int>(p[26]) << 16));
        const int h = 1 + (static_cast<int>(p[27]) | (static_cast<int>(p[28]) << 8) | (static_cast<int>(p[29]) << 16));
        inserir_dim(m, w, h);
        return;
    }
    if (chunk == "VP8L" && d.size() >= 25 && static_cast<uchar>(p[20]) == 0x2F) {
        const quint32 bits =
            static_cast<quint32>(p[21]) |
            (static_cast<quint32>(p[22]) << 8) |
            (static_cast<quint32>(p[23]) << 16) |
            (static_cast<quint32>(p[24]) << 24);
        const int w = static_cast<int>((bits & 0x3FFF) + 1);
        const int h = static_cast<int>(((bits >> 14) & 0x3FFF) + 1);
        inserir_dim(m, w, h);
        return;
    }
    if (chunk == "VP8 " && d.size() >= 30) {
        if (p[23] == 0x9D && p[24] == 0x01 && p[25] == 0x2A) {
            const int w = static_cast<int>(rd16le(p + 26) & 0x3FFF);
            const int h = static_cast<int>(rd16le(p + 28) & 0x3FFF);
            inserir_dim(m, w, h);
        }
    }
}

void extrair_ico(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(16);
    if (d.size() < 8) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (rd16le(p + 0) != 0 || rd16le(p + 2) != 1) return;  // reserved/type
    const int count = static_cast<int>(rd16le(p + 4));
    if (count <= 0 || d.size() < 8) return;
    int w = static_cast<int>(static_cast<uchar>(p[6]));
    int h = static_cast<int>(static_cast<uchar>(p[7]));
    if (w == 0) w = 256;
    if (h == 0) h = 256;
    inserir_dim(m, w, h);
}

void extrair_icns(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(2 * 1024 * 1024)));
    if (d.size() < 8) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "icns", 4) != 0) return;

    int maior = 0;
    qint64 off = 8;
    while (off + 8 <= d.size()) {
        const QByteArray type = d.mid(static_cast<int>(off), 4);
        const quint32 len = rd32be(p + off + 4);
        if (len < 8 || off + len > d.size()) break;

        if (type == "icp4") maior = qMax(maior, 16);
        else if (type == "icp5") maior = qMax(maior, 32);
        else if (type == "icp6") maior = qMax(maior, 64);
        else if (type == "ic07") maior = qMax(maior, 128);
        else if (type == "ic08") maior = qMax(maior, 256);
        else if (type == "ic09") maior = qMax(maior, 512);
        else if (type == "ic10") maior = qMax(maior, 1024);

        off += len;
    }
    inserir_dim(m, maior, maior);
}

void extrair_sgi(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(12);
    if (d.size() < 12) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (rd16be(p) != 474) return;  // 0x01DA
    const int w = static_cast<int>(rd16be(p + 6));
    const int h = static_cast<int>(rd16be(p + 8));
    inserir_dim(m, w, h);
}

void extrair_sunras(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(16);
    if (d.size() < 12) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (rd32be(p) != 0x59A66A95u) return;
    const int w = static_cast<int>(rd32be(p + 4));
    const int h = static_cast<int>(rd32be(p + 8));
    inserir_dim(m, w, h);
}

void extrair_xwd(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(32);
    if (d.size() < 24) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    const quint32 header_size = rd32be(p);
    if (header_size < 24) return;
    const int w = static_cast<int>(rd32be(p + 16));
    const int h = static_cast<int>(rd32be(p + 20));
    inserir_dim(m, w, h);
}

void extrair_svg(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(256 * 1024)));
    if (d.isEmpty()) return;
    const QString txt = QString::fromUtf8(d);
    if (!txt.contains(QStringLiteral("<svg"), Qt::CaseInsensitive)) return;

    auto parse_number = [](const QString& s) -> int {
        bool ok = false;
        const double v = s.toDouble(&ok);
        if (!ok || v <= 0.0) return 0;
        return static_cast<int>(v + 0.5);
    };

    const QRegularExpression rw(QStringLiteral("\\bwidth\\s*=\\s*['\\\"]\\s*([0-9]+(?:\\.[0-9]+)?)"),
                                QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression rh(QStringLiteral("\\bheight\\s*=\\s*['\\\"]\\s*([0-9]+(?:\\.[0-9]+)?)"),
                                QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression rvb(
        QStringLiteral("\\bviewBox\\s*=\\s*['\\\"]\\s*[-+0-9.eE]+\\s+[-+0-9.eE]+\\s+([-+0-9.eE]+)\\s+([-+0-9.eE]+)"),
        QRegularExpression::CaseInsensitiveOption);

    int w = 0;
    int h = 0;
    const auto mw = rw.match(txt);
    const auto mh = rh.match(txt);
    if (mw.hasMatch() && mh.hasMatch()) {
        w = parse_number(mw.captured(1));
        h = parse_number(mh.captured(1));
    }

    if (w <= 0 || h <= 0) {
        const auto mvb = rvb.match(txt);
        if (mvb.hasMatch()) {
            w = parse_number(mvb.captured(1));
            h = parse_number(mvb.captured(2));
        }
    }

    inserir_dim(m, w, h);
}

void extrair_fits(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(128 * 1024)));
    if (d.size() < 80) return;
    const QString txt = QString::fromLatin1(d);
    if (!txt.startsWith(QStringLiteral("SIMPLE"))) return;

    const QRegularExpression r1(QStringLiteral("\\bNAXIS1\\s*=\\s*([0-9]+)"));
    const QRegularExpression r2(QStringLiteral("\\bNAXIS2\\s*=\\s*([0-9]+)"));
    const auto m1 = r1.match(txt);
    const auto m2 = r2.match(txt);
    if (!m1.hasMatch() || !m2.hasMatch()) return;
    inserir_dim(m, m1.captured(1).toInt(), m2.captured(1).toInt());
}

void extrair_wbmp(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(64);
    if (d.size() < 4) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (p[0] != 0x00 || p[1] != 0x00) return;

    int idx = 2;
    auto ler_mbi = [&](int& out) -> bool {
        out = 0;
        for (int i = 0; i < 5 && idx < d.size(); ++i) {
            const uchar b = static_cast<uchar>(p[idx++]);
            out = (out << 7) | (b & 0x7F);
            if ((b & 0x80) == 0) return true;
        }
        return false;
    };

    int w = 0;
    int h = 0;
    if (ler_mbi(w) && ler_mbi(h)) inserir_dim(m, w, h);
}

void extrair_xbm(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(128 * 1024)));
    if (d.isEmpty()) return;
    const QString txt = QString::fromLatin1(d);
    const QRegularExpression rw(QStringLiteral("#define\\s+\\w+_width\\s+(\\d+)"));
    const QRegularExpression rh(QStringLiteral("#define\\s+\\w+_height\\s+(\\d+)"));
    const auto mw = rw.match(txt);
    const auto mh = rh.match(txt);
    if (mw.hasMatch() && mh.hasMatch()) {
        inserir_dim(m, mw.captured(1).toInt(), mh.captured(1).toInt());
    }
}

void extrair_xpm(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(128 * 1024)));
    if (d.isEmpty()) return;
    const QString txt = QString::fromLatin1(d);
    const QRegularExpression r(QStringLiteral("\"\\s*(\\d+)\\s+(\\d+)\\s+\\d+\\s+\\d+\\s*\""));
    const auto mm = r.match(txt);
    if (mm.hasMatch()) inserir_dim(m, mm.captured(1).toInt(), mm.captured(2).toInt());
}

// ─── TIFF IFD parser (supports classic TIFF + BigTIFF) ─────────────────────────────

quint16 tiff_r16(const uchar* p, bool le) {
    return le ? qFromLittleEndian<quint16>(p) : qFromBigEndian<quint16>(p);
}

quint32 tiff_r32(const uchar* p, bool le) {
    return le ? qFromLittleEndian<quint32>(p) : qFromBigEndian<quint32>(p);
}

quint64 tiff_r64(const uchar* p, bool le) {
    return le ? qFromLittleEndian<quint64>(p) : qFromBigEndian<quint64>(p);
}

int tiff_type_size(quint16 type) {
    switch (type) {
        case 1:   // BYTE
        case 2:   // ASCII
        case 6:   // SBYTE
        case 7:   // UNDEFINED
            return 1;
        case 3:   // SHORT
        case 8:   // SSHORT
            return 2;
        case 4:   // LONG
        case 9:   // SLONG
        case 11:  // FLOAT
            return 4;
        case 5:   // RATIONAL
        case 10:  // SRATIONAL
        case 12:  // DOUBLE
        case 16:  // LONG8 (BigTIFF)
        case 17:  // SLONG8 (BigTIFF)
        case 18:  // IFD8 (BigTIFF)
            return 8;
        default:
            return 0;
    }
}

bool tiff_read_bytes(QFile& f, qint64 offset, qint64 len, QByteArray& out) {
    if (offset < 0 || len <= 0) return false;
    if (offset > f.size() || len > (f.size() - offset)) return false;
    if (!f.seek(offset)) return false;
    out = f.read(len);
    return out.size() == len;
}

quint64 tiff_read_scalar(const uchar* p, quint16 type, bool le) {
    switch (type) {
        case 1:
        case 2:
        case 6:
        case 7:
            return static_cast<quint64>(p[0]);
        case 3:
            return static_cast<quint64>(tiff_r16(p, le));
        case 4:
            return static_cast<quint64>(tiff_r32(p, le));
        case 5: {
            const quint32 num = tiff_r32(p, le);
            const quint32 den = tiff_r32(p + 4, le);
            return den == 0 ? 0 : static_cast<quint64>(num / den);
        }
        case 8:
            return static_cast<quint64>(static_cast<qint16>(tiff_r16(p, le)));
        case 9:
            return static_cast<quint64>(static_cast<qint32>(tiff_r32(p, le)));
        case 10: {
            const qint32 num = static_cast<qint32>(tiff_r32(p, le));
            const qint32 den = static_cast<qint32>(tiff_r32(p + 4, le));
            return den == 0 ? 0 : static_cast<quint64>(num / den);
        }
        case 11:
        case 12:
            return 0;  // not expected for dimensions
        case 16:
        case 18:
            return tiff_r64(p, le);
        case 17:
            return static_cast<quint64>(static_cast<qint64>(tiff_r64(p, le)));
        default:
            return 0;
    }
}

bool tiff_resolver_tag_valor(QFile& f,
                             quint16 tag,
                             quint16 type,
                             quint64 count,
                             const uchar* inline_ptr,
                             quint64 offset_ptr,
                             int inline_capacity_bytes,
                             bool le,
                             int& w,
                             int& h) {
    if (count == 0) return false;
    const int ts = tiff_type_size(type);
    if (ts <= 0) return false;
    if (count > (std::numeric_limits<quint64>::max() / static_cast<quint64>(ts))) return false;
    const quint64 total_bytes = count * static_cast<quint64>(ts);

    quint64 valor = 0;
    if (total_bytes <= static_cast<quint64>(inline_capacity_bytes)) {
        valor = tiff_read_scalar(inline_ptr, type, le);
    } else {
        const qint64 off = static_cast<qint64>(offset_ptr);
        QByteArray raw;
        if (!tiff_read_bytes(f, off, ts, raw)) return false;
        valor = tiff_read_scalar(reinterpret_cast<const uchar*>(raw.constData()), type, le);
    }

    if ((tag == 256 || tag == 0xA002) && valor > 0 && valor <= static_cast<quint64>(std::numeric_limits<int>::max())) {
        w = static_cast<int>(valor);
    } else if ((tag == 257 || tag == 0xA003) && valor > 0 && valor <= static_cast<quint64>(std::numeric_limits<int>::max())) {
        h = static_cast<int>(valor);
    }
    return (w > 0 && h > 0);
}

void extrair_tiff_ifd(QFile& f, QVariantMap& m) {
    QByteArray header;
    if (!tiff_read_bytes(f, 0, 16, header) || header.size() < 8) return;
    const uchar* h = reinterpret_cast<const uchar*>(header.constData());

    const bool le = (h[0] == 'I' && h[1] == 'I');
    const bool be = (h[0] == 'M' && h[1] == 'M');
    if (!le && !be) return;

    const quint16 magic = tiff_r16(h + 2, le);
    int w = 0;
    int hgt = 0;

    if (magic == 42) {
        quint32 ifd_off = tiff_r32(h + 4, le);
        for (int ifd_pass = 0; ifd_pass < 8 && ifd_off > 0; ++ifd_pass) {
            QByteArray cbuf;
            if (!tiff_read_bytes(f, static_cast<qint64>(ifd_off), 2, cbuf)) break;
            const quint16 count = tiff_r16(reinterpret_cast<const uchar*>(cbuf.constData()), le);

            const quint64 entries_bytes = static_cast<quint64>(count) * 12ull;
            if (entries_bytes > static_cast<quint64>(std::numeric_limits<qint64>::max() - 4)) break;

            QByteArray block;
            if (!tiff_read_bytes(f, static_cast<qint64>(ifd_off) + 2, static_cast<qint64>(entries_bytes + 4), block)) break;
            const uchar* b = reinterpret_cast<const uchar*>(block.constData());

            for (quint16 i = 0; i < count; ++i) {
                const uchar* e = b + (static_cast<qint64>(i) * 12);
                const quint16 tag = tiff_r16(e, le);
                const quint16 type = tiff_r16(e + 2, le);
                const quint64 val_count = static_cast<quint64>(tiff_r32(e + 4, le));
                const quint64 val_off = static_cast<quint64>(tiff_r32(e + 8, le));
                if (tiff_resolver_tag_valor(f, tag, type, val_count, e + 8, val_off, 4, le, w, hgt)) {
                    inserir_dim(m, w, hgt);
                    return;
                }
            }

            ifd_off = tiff_r32(b + entries_bytes, le);
        }
    } else if (magic == 43) {  // BigTIFF
        if (header.size() < 16) return;
        const quint16 off_size = tiff_r16(h + 4, le);
        if (off_size != 8) return;
        quint64 ifd_off = tiff_r64(h + 8, le);

        for (int ifd_pass = 0; ifd_pass < 8 && ifd_off > 0; ++ifd_pass) {
            QByteArray cbuf;
            if (!tiff_read_bytes(f, static_cast<qint64>(ifd_off), 8, cbuf)) break;
            quint64 count = tiff_r64(reinterpret_cast<const uchar*>(cbuf.constData()), le);
            if (count > 50000) count = 50000;  // safety cap

            const quint64 entries_bytes = count * 20ull;
            if (entries_bytes > static_cast<quint64>(std::numeric_limits<qint64>::max() - 8)) break;

            QByteArray block;
            if (!tiff_read_bytes(f, static_cast<qint64>(ifd_off) + 8, static_cast<qint64>(entries_bytes + 8), block)) break;
            const uchar* b = reinterpret_cast<const uchar*>(block.constData());

            for (quint64 i = 0; i < count; ++i) {
                const uchar* e = b + static_cast<qint64>(i * 20ull);
                const quint16 tag = tiff_r16(e, le);
                const quint16 type = tiff_r16(e + 2, le);
                const quint64 val_count = tiff_r64(e + 4, le);
                const quint64 val_off = tiff_r64(e + 12, le);
                if (tiff_resolver_tag_valor(f, tag, type, val_count, e + 12, val_off, 8, le, w, hgt)) {
                    inserir_dim(m, w, hgt);
                    return;
                }
            }

            ifd_off = tiff_r64(b + entries_bytes, le);
        }
    }

    inserir_dim(m, w, hgt);
}

// ─── PSD ──────────────────────────────────────────────────

void extrair_psd(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(22);
    if (d.size() < 22) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "8BPS", 4) != 0) return;
    int h = static_cast<int>(rd32be(p + 14));
    int w = static_cast<int>(rd32be(p + 18));
    inserir_dim(m, w, h);
}

// ─── TGA ──────────────────────────────────────────────────

void extrair_tga(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(18);
    if (d.size() < 18) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    int w = rd16le(p + 12);
    int h = rd16le(p + 14);
    inserir_dim(m, w, h);
}

// ─── ISOBMFF (HEIC / HEIF / AVIF) ────────────────────────

bool isobmff_find_ispe(const QByteArray& d, qint64 off, qint64 end, int& w, int& h) {
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    while (off + 8 <= end && off + 8 <= d.size()) {
        quint64 box_sz = rd32be(base + off);
        QByteArray type = d.mid(static_cast<int>(off + 4), 4);
        qint64 hdr = 8;
        if (box_sz == 1 && off + 16 <= d.size()) { box_sz = rd64be(base + off + 8); hdr = 16; }
        if (box_sz == 0) box_sz = static_cast<quint64>(end - off);
        if (box_sz < static_cast<quint64>(hdr) || off + static_cast<qint64>(box_sz) > end) break;
        qint64 payload = off + hdr;
        qint64 box_end = off + static_cast<qint64>(box_sz);
        if (type == "meta") {
            if (payload + 4 <= d.size()) payload += 4; // skip version/flags
            if (isobmff_find_ispe(d, payload, box_end, w, h)) return true;
        } else if (type == "iprp" || type == "ipco") {
            if (isobmff_find_ispe(d, payload, box_end, w, h)) return true;
        } else if (type == "ispe" && payload + 12 <= d.size()) {
            // version (4) + width (4) + height (4)
            w = static_cast<int>(rd32be(base + payload + 4));
            h = static_cast<int>(rd32be(base + payload + 8));
            if (w > 0 && h > 0) return true;
        }
        off = box_end;
    }
    return false;
}

void extrair_heic(QFile& f, QVariantMap& m) {
    f.seek(0);
    const qint64 read_sz = qMin(f.size(), static_cast<qint64>(4 * 1024 * 1024));
    const QByteArray d = f.read(read_sz);
    if (d.size() < 12) return;
    int w = 0, h = 0;
    if (isobmff_find_ispe(d, 0, d.size(), w, h)) inserir_dim(m, w, h);
}

// ─── EXR (OpenEXR) ────────────────────────────────────────

void extrair_exr(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(4096)));
    if (d.size() < 12) return;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    // Magic: 0x762F3101
    if (base[0] != 0x76 || base[1] != 0x2F || base[2] != 0x31 || base[3] != 0x01) return;
    // Scan for "dataWindow" attribute followed by box2i type
    int pos = d.indexOf("dataWindow");
    if (pos < 0) return;
    pos += 10; // skip name
    if (pos >= d.size()) return;
    // Skip null terminator
    while (pos < d.size() && d[pos] != '\0') ++pos;
    ++pos;
    // Now look for type "box2i" followed by null, then size (4LE), then xmin/ymin/xmax/ymax (each 4LE)
    int tpos = d.indexOf("box2i", pos);
    if (tpos < 0 || tpos + 5 + 1 + 4 + 16 > d.size()) return;
    const uchar* box = base + tpos + 5 + 1 + 4; // skip "box2i\0" + size
    int xmin = static_cast<int>(rd32le(box));
    int ymin = static_cast<int>(rd32le(box + 4));
    int xmax = static_cast<int>(rd32le(box + 8));
    int ymax = static_cast<int>(rd32le(box + 12));
    int w = xmax - xmin + 1;
    int h = ymax - ymin + 1;
    inserir_dim(m, w, h);
}

// ─── HDR (Radiance) ──────────────────────────────────────

void extrair_hdr(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(4096)));
    // Look for "-Y <height> +X <width>" or "+Y <height> +X <width>"
    int pos = d.indexOf("-Y ");
    if (pos < 0) pos = d.indexOf("+Y ");
    if (pos < 0) return;
    QString line = QString::fromLatin1(d.mid(pos, 64)).split('\n').first().trimmed();
    QStringList parts = line.split(QRegularExpression("\\s+"));
    // Expected: "+/-Y H +/-X W"
    if (parts.size() >= 4) {
        int h = parts[1].toInt();
        int w = parts[3].toInt();
        inserir_dim(m, w, h);
    }
}

// ─── PCX ──────────────────────────────────────────────────

void extrair_pcx(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(12);
    if (d.size() < 12) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (p[0] != 0x0A) return; // PCX magic
    int xmin = rd16le(p + 4), ymin = rd16le(p + 6);
    int xmax = rd16le(p + 8), ymax = rd16le(p + 10);
    inserir_dim(m, xmax - xmin + 1, ymax - ymin + 1);
}

// ─── JPEG2000 (JP2 / J2K) ────────────────────────────────

void extrair_jp2(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(256 * 1024)));
    if (d.size() < 16) return;
    // Search for SIZ marker (0xFF51) in the codestream
    for (int i = 0; i + 14 < d.size(); ++i) {
        const uchar* p = reinterpret_cast<const uchar*>(d.constData()) + i;
        if (p[0] == 0xFF && p[1] == 0x51) {
            // SIZ marker: skip 2-byte marker + 2-byte length, then Rsiz(2), Xsiz(4), Ysiz(4), XOsiz(4), YOsiz(4)
            if (i + 2 + 2 + 2 + 4 + 4 + 4 + 4 > d.size()) break;
            int w = static_cast<int>(rd32be(p + 6));
            int h = static_cast<int>(rd32be(p + 10));
            int xo = static_cast<int>(rd32be(p + 14));
            int yo = static_cast<int>(rd32be(p + 18));
            inserir_dim(m, w - xo, h - yo);
            return;
        }
    }
}

// ─── DDS (DirectDraw Surface) ─────────────────────────────

void extrair_dds(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(128);
    if (d.size() < 20) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "DDS ", 4) != 0) return;
    int h = static_cast<int>(rd32le(p + 12));
    int w = static_cast<int>(rd32le(p + 16));
    inserir_dim(m, w, h);
}

// ─── QOI (Quite OK Image) ────────────────────────────────

void extrair_qoi(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(14);
    if (d.size() < 14) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "qoif", 4) != 0) return;
    int w = static_cast<int>(rd32be(p + 4));
    int h = static_cast<int>(rd32be(p + 8));
    inserir_dim(m, w, h);
}

// ─── PPM / PGM / PBM (Netpbm) ────────────────────────────

void extrair_pnm(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(256);
    if (d.size() < 3) return;
    // Format: P[1-6]\n<width> <height>\n...
    if (d[0] != 'P') return;
    QString header = QString::fromLatin1(d);
    // Skip comments and find width/height
    QStringList lines = header.split('\n');
    for (int i = 1; i < lines.size(); ++i) {
        const QString& line = lines[i];
        if (line.startsWith('#')) continue;
        QStringList parts = line.trimmed().split(QRegularExpression("\\s+"));
        if (parts.size() >= 2) {
            inserir_dim(m, parts[0].toInt(), parts[1].toInt());
            return;
        }
    }
}

}  // namespace

QVariantMap extrair_metadados_imagem(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap metadados;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return metadados;

        // First try QImageReader (handles JPG, PNG, BMP, GIF, TIFF, WebP, SVG, ICO, etc.)
        QImageReader r(caminho);
        const QSize s = r.size();
        if (s.isValid()) {
            metadados.insert("dimensoes", QStringLiteral("%1x%2").arg(s.width()).arg(s.height()));
            return metadados;
        }

        // Fall back to format-specific binary parsers
        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) return metadados;

        // RAW camera formats (TIFF-based)
        static const QSet<QString> tiff_exts = {
            ".tif", ".tiff", ".cr2", ".cr3", ".nef", ".dng", ".arw", ".orf", ".rw2",
            ".raf", ".srw", ".pef", ".x3f", ".erf", ".mrw", ".dcr", ".kdc", ".raw",
            ".3fr", ".mef", ".nrw", ".rwl", ".sr2", ".srf", ".rwz", ".ari", ".bay",
            ".cap", ".cin", ".fff", ".gpr", ".iiq", ".k25", ".mdc", ".mos", ".ptx",
            ".pxn", ".r3d"
        };
        // HEIC/HEIF/AVIF (ISOBMFF)
        static const QSet<QString> heic_exts = {".heic", ".heif", ".avif", ".hif"};
        static const QSet<QString> jpeg_exts = {".jpg", ".jpeg", ".jpe", ".jfif", ".jif", ".pjpeg", ".mpo"};
        static const QSet<QString> png_exts = {".png", ".apng"};
        static const QSet<QString> bmp_exts = {".bmp", ".dib"};
        static const QSet<QString> ico_exts = {".ico"};
        static const QSet<QString> pnm_exts = {".ppm", ".pgm", ".pbm", ".pnm", ".pam"};

        if (jpeg_exts.contains(ext)) {
            extrair_jpeg(f, metadados);
        } else if (png_exts.contains(ext)) {
            extrair_png(f, metadados);
        } else if (ext == ".gif") {
            extrair_gif(f, metadados);
        } else if (bmp_exts.contains(ext)) {
            extrair_bmp(f, metadados);
        } else if (ext == ".webp") {
            extrair_webp(f, metadados);
        } else if (ico_exts.contains(ext)) {
            extrair_ico(f, metadados);
        } else if (ext == ".icns") {
            extrair_icns(f, metadados);
        } else if (ext == ".svg") {
            extrair_svg(f, metadados);
        } else if (ext == ".xbm") {
            extrair_xbm(f, metadados);
        } else if (ext == ".xpm") {
            extrair_xpm(f, metadados);
        } else if (ext == ".sgi" || ext == ".rgb" || ext == ".rgba") {
            extrair_sgi(f, metadados);
        } else if (ext == ".ras" || ext == ".sun") {
            extrair_sunras(f, metadados);
        } else if (ext == ".xwd") {
            extrair_xwd(f, metadados);
        } else if (ext == ".fits") {
            extrair_fits(f, metadados);
        } else if (ext == ".wbmp") {
            extrair_wbmp(f, metadados);
        } else if (tiff_exts.contains(ext)) {
            extrair_tiff_ifd(f, metadados);
        } else if (heic_exts.contains(ext)) {
            extrair_heic(f, metadados);
        } else if (ext == ".psd" || ext == ".psb") {
            extrair_psd(f, metadados);
        } else if (ext == ".tga" || ext == ".targa" || ext == ".icb" || ext == ".vda" || ext == ".vst") {
            extrair_tga(f, metadados);
        } else if (ext == ".exr") {
            extrair_exr(f, metadados);
        } else if (ext == ".hdr") {
            extrair_hdr(f, metadados);
        } else if (ext == ".pcx") {
            extrair_pcx(f, metadados);
        } else if (ext == ".jp2" || ext == ".j2k" || ext == ".j2c" || ext == ".jpc") {
            extrair_jp2(f, metadados);
        } else if (ext == ".dds") {
            extrair_dds(f, metadados);
        } else if (ext == ".qoi") {
            extrair_qoi(f, metadados);
        } else if (pnm_exts.contains(ext)) {
            extrair_pnm(f, metadados);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados da imagem", true);
    }
    return metadados;
}

}  // namespace source::services::GerenciamentoMetadados
