#include "gmet_05_ExtrairMetadadosVideo.h"

#include <QtEndian>
#include <QFile>
#include <QFileInfo>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

// ─── Helpers ───────────────────────────────────────────────

QString formatar_duracao(qint64 total_segundos) {
    if (total_segundos <= 0) return QString();
    const qint64 horas = total_segundos / 3600;
    const qint64 minutos = (total_segundos % 3600) / 60;
    const qint64 segundos = total_segundos % 60;
    return QStringLiteral("%1:%2:%3")
        .arg(horas, 2, 10, QLatin1Char('0'))
        .arg(minutos, 2, 10, QLatin1Char('0'))
        .arg(segundos, 2, 10, QLatin1Char('0'));
}

void inserir_bitrate(QVariantMap& m, qint64 file_size, qint64 duracao_seg) {
    if (duracao_seg > 0) {
        const int kbps = static_cast<int>((file_size * 8.0 / 1000.0) / static_cast<double>(duracao_seg));
        if (kbps > 0) m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
    }
}

inline quint32 rd32be(const uchar* p) { return qFromBigEndian<quint32>(p); }
inline quint16 rd16be(const uchar* p) { return qFromBigEndian<quint16>(p); }
inline quint64 rd64be(const uchar* p) { return qFromBigEndian<quint64>(p); }
inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }
inline quint64 rd64le(const uchar* p) { return qFromLittleEndian<quint64>(p); }

QByteArray carregar_bloco_isobmff(QFile& f) {
    const qint64 tamanho_arquivo = f.size();
    if (tamanho_arquivo <= 0) return {};

    // Tenta localizar a box "moov" em nível superior. Em muitos MP4 o "moov"
    // fica no final do arquivo; ler apenas o início perde duração/dimensões.
    qint64 offset = 0;
    while (offset + 8 <= tamanho_arquivo) {
        f.seek(offset);
        const QByteArray header = f.read(8);
        if (header.size() < 8) break;

        const uchar* hp = reinterpret_cast<const uchar*>(header.constData());
        quint64 box_size = rd32be(hp);
        const QByteArray box_type = header.mid(4, 4);
        qint64 header_size = 8;

        if (box_size == 1) {
            const QByteArray ext = f.read(8);
            if (ext.size() < 8) break;
            box_size = rd64be(reinterpret_cast<const uchar*>(ext.constData()));
            header_size = 16;
        } else if (box_size == 0) {
            box_size = static_cast<quint64>(tamanho_arquivo - offset);
        }

        if (box_size < static_cast<quint64>(header_size) ||
            offset + static_cast<qint64>(box_size) > tamanho_arquivo) {
            break;
        }

        if (box_type == "moov") {
            // Mantém limite de leitura para segurança de memória.
            const qint64 leitura_max = 128ll * 1024ll * 1024ll;
            const qint64 leitura = qMin(static_cast<qint64>(box_size), leitura_max);
            f.seek(offset);
            return f.read(leitura);
        }

        offset += static_cast<qint64>(box_size);
    }

    // Fallback para arquivos fora do padrão: lê o início.
    f.seek(0);
    const qint64 leitura_fallback = qMin(tamanho_arquivo, static_cast<qint64>(32ll * 1024ll * 1024ll));
    return f.read(leitura_fallback);
}

// ─── AVI ───────────────────────────────────────────────────

bool extrair_avi(const QByteArray& dados, int& w, int& h, qint64& dur) {
    w = h = 0; dur = 0;
    const int pos = dados.indexOf("avih");
    if (pos < 0 || pos + 4 + 56 > dados.size()) return false;
    const uchar* p = reinterpret_cast<const uchar*>(dados.constData()) + pos + 4;
    const quint32 us_frame = rd32le(p);
    const quint32 frames = rd32le(p + 16);
    w = static_cast<int>(rd32le(p + 32));
    h = static_cast<int>(rd32le(p + 36));
    if (us_frame > 0 && frames > 0)
        dur = static_cast<qint64>((static_cast<double>(us_frame) * frames) / 1e6 + 0.5);
    return true;
}

// ─── ISOBMFF (MP4 / MOV / M4V / 3GP / 3G2 / F4V) ─────────

struct IsobmffResult { int w; int h; qint64 dur_sec; bool ok; };

void isobmff_scan(const QByteArray& d, qint64 off, qint64 end, int depth,
                  quint32& timescale, quint64& duration, int& w, int& h, bool& got_video) {
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    const qint64 sz = d.size();
    while (off + 8 <= end && off + 8 <= sz) {
        quint64 box_sz = rd32be(base + off);
        const QByteArray type = d.mid(static_cast<int>(off + 4), 4);
        qint64 hdr = 8;
        if (box_sz == 1 && off + 16 <= sz) { box_sz = rd64be(base + off + 8); hdr = 16; }
        if (box_sz == 0) box_sz = static_cast<quint64>(end - off);
        if (box_sz < static_cast<quint64>(hdr) || off + static_cast<qint64>(box_sz) > end) break;
        const qint64 payload = off + hdr;
        const qint64 box_end = off + static_cast<qint64>(box_sz);

        if (type == "moov" || type == "trak" || type == "mdia" || type == "minf" || type == "stbl" ||
            type == "udta" || type == "meta" || type == "dinf" || type == "edts") {
            qint64 sub_off = payload;
            if (type == "meta" && sub_off + 4 <= sz) sub_off += 4; // version+flags
            isobmff_scan(d, sub_off, box_end, depth + 1, timescale, duration, w, h, got_video);
        } else if (type == "mvhd" && payload + 20 <= sz) {
            const uchar ver = base[payload];
            if (ver == 0 && payload + 24 <= sz) {
                timescale = rd32be(base + payload + 12);
                duration = rd32be(base + payload + 16);
            } else if (ver == 1 && payload + 32 <= sz) {
                timescale = rd32be(base + payload + 20);
                duration = rd64be(base + payload + 24);
            }
        } else if (type == "tkhd" && !got_video && payload + 84 <= sz) {
            const uchar ver = base[payload];
            int off_w = 0;
            if (ver == 0 && payload + 84 <= sz) off_w = 76;
            else if (ver == 1 && payload + 96 <= sz) off_w = 88;
            if (off_w > 0) {
                const quint32 fw = rd32be(base + payload + off_w);
                const quint32 fh = rd32be(base + payload + off_w + 4);
                const int tw = static_cast<int>(fw >> 16);
                const int th = static_cast<int>(fh >> 16);
                if (tw > 0 && th > 0) { w = tw; h = th; got_video = true; }
            }
        } else if (type == "stsd" && payload + 16 <= sz) {
            // Try to find avc1/hvc1/mp4v visual sample entry for dimensions
            qint64 p = payload + 8; // skip version+flags + entry_count
            if (p + 8 <= box_end && p + 8 <= sz) {
                quint32 entry_sz = rd32be(base + p);
                QByteArray entry_type = d.mid(static_cast<int>(p + 4), 4);
                // VisualSampleEntry: width/height ficam em offsets fixos (+24/+26) a partir
                // do início da entry visual (após size+type).
                const qint64 visual_entry = p + 8;
                if ((entry_type == "avc1" || entry_type == "hvc1" || entry_type == "hev1" ||
                     entry_type == "mp4v" || entry_type == "encv" || entry_type == "vp08" ||
                     entry_type == "vp09" || entry_type == "av01") && visual_entry + 28 <= sz) {
                    const int sw = static_cast<int>(rd16be(base + visual_entry + 24));
                    const int sh = static_cast<int>(rd16be(base + visual_entry + 26));
                    if (sw > 0 && sh > 0 && !got_video) { w = sw; h = sh; got_video = true; }
                }
                Q_UNUSED(entry_sz)
            }
        }
        off = box_end;
    }
}

IsobmffResult extrair_isobmff(QFile& f) {
    IsobmffResult r = {0, 0, 0, false};
    const QByteArray d = carregar_bloco_isobmff(f);
    if (d.size() < 8) return r;
    quint32 ts = 0; quint64 dur = 0; int w = 0, h = 0; bool gv = false;
    isobmff_scan(d, 0, d.size(), 0, ts, dur, w, h, gv);
    if (ts > 0 && dur > 0) r.dur_sec = static_cast<qint64>(dur / ts);
    r.w = w; r.h = h; r.ok = (r.dur_sec > 0 || r.w > 0);
    return r;
}

// ─── EBML / Matroska (MKV / WebM) ─────────────────────────

struct EbmlResult { int w; int h; qint64 dur_sec; bool ok; };

quint64 ebml_read_id(const uchar* p, qint64 avail, int& len) {
    len = 0;
    if (avail < 1) return 0;
    quint8 first = p[0];
    if (first & 0x80) len = 1;
    else if (first & 0x40) len = 2;
    else if (first & 0x20) len = 3;
    else if (first & 0x10) len = 4;
    else { len = 0; return 0; }
    if (avail < len) { len = 0; return 0; }
    quint64 val = 0;
    for (int i = 0; i < len; ++i) val = (val << 8) | p[i];
    return val;
}

quint64 ebml_read_size(const uchar* p, qint64 avail, int& len) {
    len = 0;
    if (avail < 1) return 0;
    quint8 first = p[0];
    if (first & 0x80) len = 1;
    else if (first & 0x40) len = 2;
    else if (first & 0x20) len = 3;
    else if (first & 0x10) len = 4;
    else if (first & 0x08) len = 5;
    else if (first & 0x04) len = 6;
    else if (first & 0x02) len = 7;
    else if (first & 0x01) len = 8;
    else { len = 0; return 0; }
    if (avail < len) { len = 0; return 0; }
    quint64 val = first & ((1 << (8 - len)) - 1);
    for (int i = 1; i < len; ++i) val = (val << 8) | p[i];
    // Check for "unknown size"
    quint64 all_ones = (1ULL << (7 * len)) - 1;
    if (val == all_ones) return UINT64_MAX;
    return val;
}

quint64 ebml_read_uint(const uchar* p, int sz) {
    quint64 v = 0;
    for (int i = 0; i < sz; ++i) v = (v << 8) | p[i];
    return v;
}

double ebml_read_float(const uchar* p, int sz) {
    if (sz == 4) {
        quint32 raw = rd32be(p);
        float f; memcpy(&f, &raw, 4);
        return static_cast<double>(f);
    } else if (sz == 8) {
        quint64 raw = rd64be(p);
        double d; memcpy(&d, &raw, 8);
        return d;
    }
    return 0.0;
}

void ebml_scan(const uchar* base, qint64 off, qint64 end, qint64 total,
               double& duration_ms, quint64& timecode_scale, int& w, int& h) {
    // Matroska element IDs
    const quint64 ID_Segment      = 0x18538067;
    const quint64 ID_Info         = 0x1549A966;
    const quint64 ID_TimecodeScale= 0x2AD7B1;
    const quint64 ID_Duration     = 0x4489;
    const quint64 ID_Tracks       = 0x1654AE6B;
    const quint64 ID_TrackEntry   = 0xAE;
    const quint64 ID_TrackType    = 0x83;
    const quint64 ID_Video        = 0xE0;
    const quint64 ID_PixelWidth   = 0xB0;
    const quint64 ID_PixelHeight  = 0xBA;

    while (off + 2 <= end && off + 2 <= total) {
        int id_len = 0, sz_len = 0;
        quint64 id = ebml_read_id(base + off, end - off, id_len);
        if (id_len == 0) break;
        quint64 data_sz = ebml_read_size(base + off + id_len, end - off - id_len, sz_len);
        if (sz_len == 0) break;
        qint64 data_off = off + id_len + sz_len;
        qint64 el_end = (data_sz == UINT64_MAX) ? end : data_off + static_cast<qint64>(data_sz);
        if (el_end > end) el_end = end;
        if (el_end > total) el_end = total;

        if (id == ID_Segment || id == ID_Info || id == ID_Tracks || id == ID_TrackEntry || id == ID_Video) {
            ebml_scan(base, data_off, el_end, total, duration_ms, timecode_scale, w, h);
        } else if (id == ID_TimecodeScale && data_sz <= 8 && data_off + static_cast<qint64>(data_sz) <= total) {
            timecode_scale = ebml_read_uint(base + data_off, static_cast<int>(data_sz));
        } else if (id == ID_Duration && (data_sz == 4 || data_sz == 8) && data_off + static_cast<qint64>(data_sz) <= total) {
            duration_ms = ebml_read_float(base + data_off, static_cast<int>(data_sz));
        } else if (id == ID_PixelWidth && data_sz <= 4 && data_off + static_cast<qint64>(data_sz) <= total) {
            w = static_cast<int>(ebml_read_uint(base + data_off, static_cast<int>(data_sz)));
        } else if (id == ID_PixelHeight && data_sz <= 4 && data_off + static_cast<qint64>(data_sz) <= total) {
            h = static_cast<int>(ebml_read_uint(base + data_off, static_cast<int>(data_sz)));
        }
        off = el_end;
    }
}

EbmlResult extrair_ebml(QFile& f) {
    EbmlResult r = {0, 0, 0, false};
    const qint64 read_sz = qMin(f.size(), static_cast<qint64>(32 * 1024 * 1024));
    const QByteArray d = f.read(read_sz);
    if (d.size() < 4) return r;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    // Verify EBML header
    if (!(base[0] == 0x1A && base[1] == 0x45 && base[2] == 0xDF && base[3] == 0xA3)) return r;
    double dur_ms = 0; quint64 ts = 1000000; int w = 0, h = 0;
    ebml_scan(base, 0, d.size(), d.size(), dur_ms, ts, w, h);
    if (dur_ms > 0 && ts > 0) r.dur_sec = static_cast<qint64>((dur_ms * static_cast<double>(ts)) / 1e9 + 0.5);
    r.w = w; r.h = h; r.ok = (r.dur_sec > 0 || r.w > 0);
    return r;
}

// ─── ASF (WMV / ASF) ──────────────────────────────────────

struct AsfResult { int w; int h; qint64 dur_sec; qint64 bitrate; bool ok; };

bool asf_guid_eq(const uchar* a, const uchar* b) { return memcmp(a, b, 16) == 0; }

AsfResult extrair_asf(QFile& f) {
    AsfResult r = {0, 0, 0, 0, false};
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(1024 * 1024)));
    if (d.size() < 30) return r;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    // ASF Header Object GUID
    static const uchar hdr_guid[] = {0x30,0x26,0xB2,0x75,0x8E,0x66,0xCF,0x11,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C};
    if (!asf_guid_eq(base, hdr_guid)) return r;
    const quint64 hdr_sz = rd64le(base + 16);
    if (hdr_sz < 30 || static_cast<qint64>(hdr_sz) > d.size()) return r;
    const quint32 n_headers = rd32le(base + 24);

    // File Properties Object GUID
    static const uchar fp_guid[] = {0xA1,0xDC,0xAB,0x8C,0x47,0xA9,0xCF,0x11,0x8E,0xE4,0x00,0xC0,0x0C,0x20,0x53,0x65};
    // Stream Properties Object GUID
    static const uchar sp_guid[] = {0x91,0x07,0xDC,0xB7,0xB7,0xA9,0xCF,0x11,0x8E,0xE6,0x00,0xC0,0x0C,0x20,0x53,0x65};
    // Video Media GUID
    static const uchar vid_guid[] = {0xC0,0xEF,0x19,0xBC,0x4D,0x5B,0xCF,0x11,0xA8,0xFD,0x00,0x80,0x5F,0x5C,0x44,0x2B};

    qint64 pos = 30;
    const qint64 hdr_end = static_cast<qint64>(hdr_sz);
    Q_UNUSED(n_headers)
    while (pos + 24 <= hdr_end && pos + 24 <= d.size()) {
        const uchar* obj = base + pos;
        const quint64 obj_sz = rd64le(obj + 16);
        if (obj_sz < 24 || pos + static_cast<qint64>(obj_sz) > d.size()) break;

        if (asf_guid_eq(obj, fp_guid) && obj_sz >= 104) {
            const quint64 dur_100ns = rd64le(obj + 64);
            r.dur_sec = static_cast<qint64>(dur_100ns / 10000000ULL);
            // preroll in ms at offset 80
            const quint64 preroll = rd64le(obj + 80);
            if (r.dur_sec > static_cast<qint64>(preroll / 1000))
                r.dur_sec -= static_cast<qint64>(preroll / 1000);
            // avg bitrate at offset 100 (bps)
            r.bitrate = static_cast<qint64>(rd32le(obj + 100));
        } else if (asf_guid_eq(obj, sp_guid) && obj_sz >= 78) {
            const uchar* stream_type = obj + 24;
            if (asf_guid_eq(stream_type, vid_guid) && obj_sz >= 89 + 40) {
                // Type-specific data contains BITMAPINFOHEADER
                const quint32 ts_off = rd32le(obj + 64); // type-specific data length
                const quint32 ec_off = rd32le(obj + 68); // error correction data length
                Q_UNUSED(ec_off)
                qint64 bih_pos = pos + 78 + ts_off - 40;
                if (bih_pos >= 0 && bih_pos + 12 <= d.size()) {
                    // Actually, BITMAPINFOHEADER starts after stream properties header
                    qint64 tsd_pos = pos + 78;
                    if (tsd_pos + 15 <= d.size()) {
                        // Video specific: first 11 bytes are video info, then BITMAPINFOHEADER
                        qint64 bmp_pos = tsd_pos + 11;
                        if (bmp_pos + 12 <= d.size()) {
                            r.w = static_cast<int>(rd32le(base + bmp_pos + 4));
                            r.h = static_cast<int>(rd32le(base + bmp_pos + 8));
                            if (r.h < 0) r.h = -r.h;
                        }
                    }
                }
            }
        }
        pos += static_cast<qint64>(obj_sz);
    }
    r.ok = (r.dur_sec > 0 || r.w > 0);
    return r;
}

// ─── FLV ───────────────────────────────────────────────────

struct FlvResult { int w; int h; qint64 dur_sec; qint64 bitrate; bool ok; };

double flv_read_double(const uchar* p) {
    quint64 raw = rd64be(p);
    double d; memcpy(&d, &raw, 8);
    return d;
}

FlvResult extrair_flv(QFile& f) {
    FlvResult r = {0, 0, 0, 0, false};
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(1024 * 1024)));
    if (d.size() < 13) return r;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    if (base[0] != 'F' || base[1] != 'L' || base[2] != 'V') return r;
    // Parse script data (onMetaData) - search for AMF strings
    // Look for "duration", "width", "height", "videodatarate" in the AMF data
    auto find_amf_number = [&](const char* key) -> double {
        QByteArray k = QByteArray(key);
        int pos = d.indexOf(k);
        if (pos < 0) return -1.0;
        // Key is preceded by 2-byte length in AMF string format, value follows as type byte + 8 bytes double
        int val_pos = pos + k.size();
        if (val_pos + 9 <= d.size() && base[val_pos] == 0x00) {
            // AMF0 number type
            return flv_read_double(base + val_pos + 1);
        }
        return -1.0;
    };
    double dur = find_amf_number("duration");
    double width = find_amf_number("width");
    double height = find_amf_number("height");
    double vbr = find_amf_number("videodatarate");
    double abr = find_amf_number("audiodatarate");

    if (dur > 0) r.dur_sec = static_cast<qint64>(dur + 0.5);
    if (width > 0 && height > 0) { r.w = static_cast<int>(width); r.h = static_cast<int>(height); }
    if (vbr > 0 || abr > 0) r.bitrate = static_cast<qint64>((vbr > 0 ? vbr : 0) + (abr > 0 ? abr : 0));
    r.ok = (r.dur_sec > 0 || r.w > 0);
    return r;
}

// ─── MPEG-PS / MPEG elementary (MPG / MPEG / VOB / M2V) ───

struct MpegResult { int w; int h; double fps; bool ok; };

MpegResult extrair_mpeg_seq_header(const QByteArray& d) {
    MpegResult r = {0, 0, 0.0, false};
    // Scan for sequence header: 00 00 01 B3
    const int limit = qMin(d.size() - 8, 4 * 1024 * 1024);
    for (int i = 0; i < limit; ++i) {
        const uchar* p = reinterpret_cast<const uchar*>(d.constData()) + i;
        if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01 && p[3] == 0xB3) {
            if (i + 8 > d.size()) break;
            // Sequence header found
            r.w = (p[4] << 4) | (p[5] >> 4);
            r.h = ((p[5] & 0x0F) << 8) | p[6];
            const int fps_code = p[7] & 0x0F;
            static const double fps_table[] = {0, 23.976, 24, 25, 29.97, 30, 50, 59.94, 60};
            if (fps_code >= 1 && fps_code <= 8) r.fps = fps_table[fps_code];
            r.ok = (r.w > 0 && r.h > 0);
            return r;
        }
    }
    return r;
}

// ─── MPEG-TS (MTS / M2TS / TS) ────────────────────────────

MpegResult extrair_mpeg_ts(QFile& f) {
    const qint64 read_sz = qMin(f.size(), static_cast<qint64>(8 * 1024 * 1024));
    const QByteArray d = f.read(read_sz);
    // Determine packet size: 188 (TS) or 192 (M2TS with 4-byte timestamp prefix)
    int pkt_sz = 0;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    if (d.size() >= 192 * 3) {
        if (base[0] == 0x47 && base[188] == 0x47) pkt_sz = 188;
        else if (base[4] == 0x47 && base[4 + 192] == 0x47) pkt_sz = 192;
        else if (base[0] == 0x47) pkt_sz = 188;
    }
    // Extract raw payload and scan for MPEG sequence header
    if (pkt_sz > 0) {
        QByteArray payload;
        payload.reserve(d.size());
        int ts_off = (pkt_sz == 192) ? 4 : 0;
        for (qint64 pos = 0; pos + pkt_sz <= d.size(); pos += pkt_sz) {
            int hdr_start = static_cast<int>(pos + ts_off);
            if (hdr_start + 4 > d.size()) break;
            int adaptation = (base[hdr_start + 3] >> 4) & 0x03;
            int data_off = hdr_start + 4;
            if (adaptation & 0x02) { // adaptation field present
                if (data_off < d.size()) data_off += 1 + base[data_off];
            }
            if (adaptation & 0x01) { // payload present
                int payload_len = static_cast<int>(pos + pkt_sz) - data_off;
                if (payload_len > 0 && data_off + payload_len <= d.size())
                    payload.append(d.mid(data_off, payload_len));
            }
            if (payload.size() > 4 * 1024 * 1024) break;
        }
        return extrair_mpeg_seq_header(payload);
    }
    return extrair_mpeg_seq_header(d);
}

// ─── OGG + Theora (OGV) ───────────────────────────────────

struct OggVideoResult { int w; int h; double fps; bool ok; };

OggVideoResult extrair_ogg_video(QFile& f) {
    OggVideoResult r = {0, 0, 0.0, false};
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(256 * 1024)));
    // Search for Theora identification header: "\x80theora"
    int pos = d.indexOf(QByteArray("\x80theora", 7));
    if (pos >= 0 && pos + 41 <= d.size()) {
        const uchar* p = reinterpret_cast<const uchar*>(d.constData()) + pos;
        // Version major[7], minor[8], revision[9]
        // Width in macroblocks * 16: bytes 10-12 (24 bits BE) * 16 -> but actually it's picture width
        // Actually Theora header format:
        // offset 7: VMAJ, 8: VMIN, 9: VREV
        // 10-12: FMBW (frame width in macroblocks) 16-bit BE
        // 12-14: FMBH (frame height in macroblocks) 16-bit BE
        // Actually the exact layout is:
        // Bytes 10,11,12 = encoded width / 16 (3 bytes)... let's use simpler offset
        // Theora spec: after the 7-byte magic:
        // [1] VMAJ [1] VMIN [1] VREV
        // [2] FMBW [2] FMBH (in 16x16 macroblocks)
        // [3] PICW [3] PICH (actual picture dimensions in 3 bytes each) -> but these are at offset 14
        if (pos + 7 + 3 + 2 + 2 + 3 + 3 + 4 + 4 <= d.size()) {
            // pic width at offset +14 (3 bytes BE), pic height at offset +17 (3 bytes BE)
            const uchar* h_ptr = p + 7; // after magic
            int pic_w = (h_ptr[7] << 16) | (h_ptr[8] << 8) | h_ptr[9];   // offset 14 from id start
            int pic_h = (h_ptr[10] << 16) | (h_ptr[11] << 8) | h_ptr[12]; // offset 17 from id start
            quint32 fps_num = rd32be(h_ptr + 15); // offset 22 from id start
            quint32 fps_den = rd32be(h_ptr + 19); // offset 26 from id start
            if (pic_w > 0 && pic_h > 0) { r.w = pic_w; r.h = pic_h; r.ok = true; }
            if (fps_num > 0 && fps_den > 0) r.fps = static_cast<double>(fps_num) / fps_den;
        }
    }
    // Also try VP8 (for OGV with VP8)
    if (!r.ok) {
        // VP8 bitstream in OGG uses "\x4fVP80" magic in mapping header
        int vp8_pos = d.indexOf(QByteArray("VP80", 4));
        if (vp8_pos >= 0 && vp8_pos + 26 <= d.size()) {
            const uchar* p = reinterpret_cast<const uchar*>(d.constData()) + vp8_pos;
            // Simple VP8 mapping header in OGG
            if (vp8_pos + 16 <= d.size()) {
                r.w = rd16le(p + 8);
                r.h = rd16le(p + 10);
                r.ok = (r.w > 0 && r.h > 0);
            }
        }
    }
    return r;
}

// ─── RealMedia (RM / RMVB) ────────────────────────────────

struct RmResult { int w; int h; qint64 dur_sec; qint64 bitrate; bool ok; };

RmResult extrair_rm(QFile& f) {
    RmResult r = {0, 0, 0, 0, false};
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(512 * 1024)));
    if (d.size() < 18) return r;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    // RealMedia: ".RMF" magic at offset 0
    if (base[0] != '.' || base[1] != 'R' || base[2] != 'M' || base[3] != 'F') return r;
    // Scan for PROP chunk (properties header)
    int pos = d.indexOf("PROP");
    if (pos >= 0 && pos + 50 <= d.size()) {
        const uchar* p = base + pos;
        // PROP header: "PROP" + 4 size + 2 version + 4 max_bitrate + 4 avg_bitrate + ...
        // offset 8: version (2 bytes), 10: max_bit_rate (4), 14: avg_bit_rate (4)
        // 18: max_pkt_size (4), 22: avg_pkt_size (4), 26: num_packets (4), 30: duration (4 ms)
        r.bitrate = static_cast<qint64>(rd32be(p + 14)) / 1000; // convert to kbps
        const quint32 dur_ms = rd32be(p + 30);
        r.dur_sec = dur_ms / 1000;
    }
    // Scan for MDPR (media properties) for video dimensions:
    // video stream has codec_name containing "Video" and extra_data contains width/height
    // This is complex; use CONT header for basic info
    r.ok = (r.dur_sec > 0 || r.bitrate > 0);
    return r;
}

// ─── Estimação genérica via tamanho ────────────────────────

qint64 estimar_duracao_por_tamanho(qint64 file_size, int bitrate_kbps_estimado) {
    if (bitrate_kbps_estimado <= 0) return 0;
    return file_size * 8 / (bitrate_kbps_estimado * 1000LL);
}

}  // namespace

QVariantMap extrair_metadados_video(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) return m;

        // ISOBMFF containers
        static const QSet<QString> isobmff_exts = {
            ".mp4", ".m4v", ".mov", ".3gp", ".3g2", ".f4v", ".qt"
        };
        // EBML/Matroska containers
        static const QSet<QString> ebml_exts = {".mkv", ".webm"};
        // ASF containers
        static const QSet<QString> asf_exts = {".wmv", ".asf"};
        // FLV
        static const QSet<QString> flv_exts = {".flv"};
        // AVI/DivX
        static const QSet<QString> avi_exts = {".avi", ".divx"};
        // MPEG-PS
        static const QSet<QString> mpeg_ps_exts = {".mpeg", ".mpg", ".mpe", ".m2v", ".mpv", ".m1v", ".vob"};
        // MPEG-TS
        static const QSet<QString> mpeg_ts_exts = {".mts", ".m2ts", ".ts", ".tp", ".trp"};
        // OGG video
        static const QSet<QString> ogg_exts = {".ogv"};
        // RealMedia
        static const QSet<QString> rm_exts = {".rm", ".rmvb"};

        int w = 0, h = 0;
        qint64 dur_sec = 0;
        qint64 bitrate_kbps = 0;

        if (isobmff_exts.contains(ext)) {
            auto r = extrair_isobmff(f);
            w = r.w; h = r.h; dur_sec = r.dur_sec;
        } else if (ebml_exts.contains(ext)) {
            auto r = extrair_ebml(f);
            w = r.w; h = r.h; dur_sec = r.dur_sec;
        } else if (asf_exts.contains(ext)) {
            auto r = extrair_asf(f);
            w = r.w; h = r.h; dur_sec = r.dur_sec;
            bitrate_kbps = r.bitrate / 1000;
        } else if (flv_exts.contains(ext)) {
            auto r = extrair_flv(f);
            w = r.w; h = r.h; dur_sec = r.dur_sec;
            bitrate_kbps = r.bitrate;
        } else if (avi_exts.contains(ext)) {
            const QByteArray dados = f.read(1024 * 1024);
            extrair_avi(dados, w, h, dur_sec);
        } else if (mpeg_ps_exts.contains(ext)) {
            const qint64 read_sz = qMin(f.size(), static_cast<qint64>(4 * 1024 * 1024));
            const QByteArray dados = f.read(read_sz);
            auto r = extrair_mpeg_seq_header(dados);
            w = r.w; h = r.h;
            // Estimate duration: typical MPEG-1/2 bitrate ~4-8 Mbps
            if (w > 0) dur_sec = estimar_duracao_por_tamanho(fi.size(), 5000);
        } else if (mpeg_ts_exts.contains(ext)) {
            auto r = extrair_mpeg_ts(f);
            w = r.w; h = r.h;
            // Estimate duration for TS: typical ~15-25 Mbps for BluRay, ~5 Mbps for broadcast
            if (w > 0) {
                int est_br = (w >= 1920) ? 20000 : (w >= 1280 ? 10000 : 5000);
                dur_sec = estimar_duracao_por_tamanho(fi.size(), est_br);
            }
        } else if (ogg_exts.contains(ext)) {
            auto r = extrair_ogg_video(f);
            w = r.w; h = r.h;
            if (w > 0) dur_sec = estimar_duracao_por_tamanho(fi.size(), 1500);
        } else if (rm_exts.contains(ext)) {
            auto r = extrair_rm(f);
            dur_sec = r.dur_sec; bitrate_kbps = r.bitrate;
        }

        if (w > 0 && h > 0)
            m.insert("dimensoes", QStringLiteral("%1x%2").arg(w).arg(h));
        if (dur_sec > 0)
            m.insert("duracao", formatar_duracao(dur_sec));
        if (bitrate_kbps <= 0 && dur_sec > 0)
            bitrate_kbps = static_cast<qint64>((fi.size() * 8.0 / 1000.0) / static_cast<double>(dur_sec));
        if (bitrate_kbps > 0)
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate_kbps));

    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados de vídeo", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
