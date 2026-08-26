#include "gmet_04_ExtrairMetadadosAudio.h"

#include <QtEndian>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <cstring>
#include <cmath>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

// ─── Helpers ───────────────────────────────────────────────

inline quint32 rd32be(const uchar* p) { return qFromBigEndian<quint32>(p); }
inline quint16 rd16be(const uchar* p) { return qFromBigEndian<quint16>(p); }
inline quint64 rd64be(const uchar* p) { return qFromBigEndian<quint64>(p); }
inline quint32 rd32le(const uchar* p) { return qFromLittleEndian<quint32>(p); }
inline quint16 rd16le(const uchar* p) { return qFromLittleEndian<quint16>(p); }
inline quint64 rd64le(const uchar* p) { return qFromLittleEndian<quint64>(p); }

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

QString limpar_tag_id3(const QByteArray& dados) {
    QString texto = QString::fromLatin1(dados).trimmed();
    while (!texto.isEmpty() && texto.endsWith(QChar('\0'))) texto.chop(1);
    return texto.trimmed();
}

// ─── WAV ───────────────────────────────────────────────────

void extrair_wav(QFile& f, QVariantMap& m) {
    if (f.size() < 44) return;
    f.seek(0);
    const QByteArray cab = f.read(44);
    if (cab.size() < 44) return;
    const uchar* p = reinterpret_cast<const uchar*>(cab.constData());
    if (memcmp(p, "RIFF", 4) != 0 || memcmp(p + 8, "WAVE", 4) != 0) return;
    const quint32 byte_rate = rd32le(p + 28);
    const qint64 tamanho_dados = qMax<qint64>(0, f.size() - 44);
    if (byte_rate > 0) {
        const qint64 dur = tamanho_dados / static_cast<qint64>(byte_rate);
        m.insert("duracao", formatar_duracao(dur));
        const int kbps = static_cast<int>((static_cast<quint64>(byte_rate) * 8ULL) / 1000ULL);
        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
    }
}

// ─── MP3 ───────────────────────────────────────────────────

void extrair_mp3(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray all = f.read(qMin(f.size(), static_cast<qint64>(512 * 1024)));
    if (all.size() < 10) return;
    const uchar* base = reinterpret_cast<const uchar*>(all.constData());

    // Skip ID3v2 tag if present
    qint64 data_start = 0;
    if (all.size() >= 10 && base[0] == 'I' && base[1] == 'D' && base[2] == '3') {
        quint32 tag_sz = ((base[6] & 0x7F) << 21) | ((base[7] & 0x7F) << 14) |
                         ((base[8] & 0x7F) << 7) | (base[9] & 0x7F);
        data_start = 10 + tag_sz;
    }

    // ID3v1 tags for author
    if (f.size() >= 128) {
        f.seek(f.size() - 128);
        const QByteArray tag = f.read(128);
        if (tag.size() == 128 && tag.mid(0, 3) == "TAG") {
            const QString artist = limpar_tag_id3(tag.mid(33, 30));
            if (!artist.isEmpty()) m.insert("autor", artist);
        }
    }

    // Find first valid MPEG frame header and check for Xing/VBRI
    static const int mp3_bitrates_v1_l3[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
    static const int mp3_bitrates_v1_l2[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0};
    static const int mp3_bitrates_v1_l1[] = {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0};
    static const int mp3_bitrates_v2_l1[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0};
    static const int mp3_bitrates_v2_l23[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0};
    static const int mp3_samplerates_v1[] = {44100, 48000, 32000, 0};
    static const int mp3_samplerates_v2[] = {22050, 24000, 16000, 0};
    static const int mp3_samplerates_v25[] = {11025, 12000, 8000, 0};

    for (qint64 pos = data_start; pos + 4 < all.size() && pos < data_start + 16384; ++pos) {
        const uchar* h = base + pos;
        if (h[0] != 0xFF || (h[1] & 0xE0) != 0xE0) continue;
        int ver = (h[1] >> 3) & 0x03; // 0=2.5, 2=2, 3=1
        int layer = (h[1] >> 1) & 0x03; // 1=L3, 2=L2, 3=L1
        int br_idx = (h[2] >> 4) & 0x0F;
        int sr_idx = (h[2] >> 2) & 0x03;
        if (ver == 1 || layer == 0 || br_idx == 0 || br_idx == 15 || sr_idx == 3) continue;

        const int* br_table = nullptr;
        const int* sr_table = nullptr;
        int samples_per_frame = 0;
        if (ver == 3) { // MPEG1
            sr_table = mp3_samplerates_v1;
            if (layer == 3) { br_table = mp3_bitrates_v1_l1; samples_per_frame = 384; }
            else if (layer == 2) { br_table = mp3_bitrates_v1_l2; samples_per_frame = 1152; }
            else { br_table = mp3_bitrates_v1_l3; samples_per_frame = 1152; }
        } else {
            sr_table = (ver == 0) ? mp3_samplerates_v25 : mp3_samplerates_v2;
            if (layer == 3) { br_table = mp3_bitrates_v2_l1; samples_per_frame = 384; }
            else { br_table = mp3_bitrates_v2_l23; samples_per_frame = (layer == 1) ? 576 : 1152; }
        }
        if (!br_table || !sr_table) continue;
        int bitrate = br_table[br_idx] * 1000;
        int samplerate = sr_table[sr_idx];
        if (bitrate == 0 || samplerate == 0) continue;

        // Check for Xing/VBRI header in this frame for VBR info
        int side_info_size = 0;
        if (ver == 3) side_info_size = ((h[3] >> 6) & 0x03) == 3 ? 17 : 32;
        else side_info_size = ((h[3] >> 6) & 0x03) == 3 ? 9 : 17;

        qint64 xing_off = pos + 4 + side_info_size;
        bool found_duration = false;
        if (xing_off + 12 < all.size()) {
            const uchar* xp = base + xing_off;
            if ((memcmp(xp, "Xing", 4) == 0 || memcmp(xp, "Info", 4) == 0) && (xp[7] & 0x01)) {
                quint32 total_frames = rd32be(xp + 8);
                if (total_frames > 0 && samplerate > 0) {
                    qint64 dur = static_cast<qint64>(static_cast<double>(total_frames) * samples_per_frame / samplerate);
                    m.insert("duracao", formatar_duracao(dur));
                    if (dur > 0) {
                        int avg_kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
                        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(avg_kbps));
                    }
                    found_duration = true;
                }
            }
        }
        // VBRI header at fixed offset 36 bytes after frame header
        if (!found_duration && pos + 4 + 36 + 26 < all.size()) {
            const uchar* vp = base + pos + 4 + 36;
            if (memcmp(vp, "VBRI", 4) == 0) {
                quint32 total_frames = rd32be(vp + 14);
                if (total_frames > 0 && samplerate > 0) {
                    qint64 dur = static_cast<qint64>(static_cast<double>(total_frames) * samples_per_frame / samplerate);
                    m.insert("duracao", formatar_duracao(dur));
                    if (dur > 0) {
                        int avg_kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
                        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(avg_kbps));
                    }
                    found_duration = true;
                }
            }
        }
        if (!found_duration) {
            // CBR estimation
            qint64 audio_size = f.size() - data_start;
            qint64 dur = (audio_size * 8) / bitrate;
            m.insert("duracao", formatar_duracao(dur));
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate / 1000));
        }
        break;
    }
}

// ─── ISOBMFF (AAC / M4A / ALAC) ───────────────────────────

void isobmff_audio_scan(const QByteArray& d, qint64 off, qint64 end,
                        quint32& timescale, quint64& duration) {
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
            type == "udta" || type == "meta" || type == "edts") {
            qint64 sub = payload;
            if (type == "meta" && sub + 4 <= sz) sub += 4;
            isobmff_audio_scan(d, sub, box_end, timescale, duration);
        } else if (type == "mvhd" && payload + 20 <= sz) {
            const uchar ver = base[payload];
            if (ver == 0 && payload + 24 <= sz) {
                timescale = rd32be(base + payload + 12);
                duration = rd32be(base + payload + 16);
            } else if (ver == 1 && payload + 32 <= sz) {
                timescale = rd32be(base + payload + 20);
                duration = rd64be(base + payload + 24);
            }
        }
        off = box_end;
    }
}

void extrair_isobmff_audio(QFile& f, QVariantMap& m) {
    f.seek(0);
    const qint64 read_sz = qMin(f.size(), static_cast<qint64>(16 * 1024 * 1024));
    const QByteArray d = f.read(read_sz);
    if (d.size() < 8) return;
    quint32 ts = 0; quint64 dur = 0;
    isobmff_audio_scan(d, 0, d.size(), ts, dur);
    if (ts > 0 && dur > 0) {
        qint64 dur_sec = static_cast<qint64>(dur / ts);
        m.insert("duracao", formatar_duracao(dur_sec));
        if (dur_sec > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur_sec);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── FLAC ──────────────────────────────────────────────────

void extrair_flac(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray hdr = f.read(46);
    if (hdr.size() < 46) return;
    const uchar* p = reinterpret_cast<const uchar*>(hdr.constData());
    // fLaC magic
    if (memcmp(p, "fLaC", 4) != 0) return;
    // First metadata block: STREAMINFO (type 0)
    // p[4] = block header byte (bit 7 = last flag, bits 0-6 = type)
    // p[5..7] = block length (3 bytes BE)
    // STREAMINFO: 34 bytes
    // Bytes 10..11: min block size, 12..13: max block size
    // 14..16: min frame size (3 bytes), 17..19: max frame size (3 bytes)
    // 20..23: sample rate (20 bits), channels (3 bits), bps (5 bits), total samples (36 bits)
    // Bytes 20-23: [SSSSSSSS SSSSSSSS SSSSCCCC BBBBBTT...]
    if ((p[4] & 0x7F) != 0) return; // not STREAMINFO
    const quint32 sr = (static_cast<quint32>(p[18]) << 12) |
                       (static_cast<quint32>(p[19]) << 4) |
                       (static_cast<quint32>(p[20]) >> 4);
    // total samples: bits 4..39 starting from byte 21
    const quint64 total_samples = (static_cast<quint64>(p[21] & 0x0F) << 32) |
                                  (static_cast<quint64>(p[22]) << 24) |
                                  (static_cast<quint64>(p[23]) << 16) |
                                  (static_cast<quint64>(p[24]) << 8) |
                                  static_cast<quint64>(p[25]);
    if (sr > 0 && total_samples > 0) {
        qint64 dur = static_cast<qint64>(total_samples / sr);
        m.insert("duracao", formatar_duracao(dur));
        if (dur > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
    // Try to read Vorbis comment for author
    // After STREAMINFO, scan subsequent metadata blocks for VORBIS_COMMENT (type 4)
    f.seek(4); // after fLaC
    for (int block = 0; block < 50; ++block) {
        QByteArray bhdr = f.read(4);
        if (bhdr.size() < 4) break;
        const uchar* bh = reinterpret_cast<const uchar*>(bhdr.constData());
        bool last = (bh[0] & 0x80) != 0;
        int btype = bh[0] & 0x7F;
        quint32 blen = (static_cast<quint32>(bh[1]) << 16) | (static_cast<quint32>(bh[2]) << 8) | bh[3];
        if (btype == 4 && blen > 0 && blen < 1024 * 1024) {
            QByteArray vc = f.read(blen);
            // Vorbis comment: vendor string length (LE32), vendor string, comment count (LE32), comments
            if (vc.size() >= 8) {
                const uchar* vp = reinterpret_cast<const uchar*>(vc.constData());
                quint32 vendor_len = rd32le(vp);
                qint64 off = 4 + vendor_len;
                if (off + 4 <= vc.size()) {
                    quint32 count = rd32le(vp + off); off += 4;
                    for (quint32 i = 0; i < count && off + 4 <= vc.size(); ++i) {
                        quint32 clen = rd32le(vp + off); off += 4;
                        if (off + clen > vc.size()) break;
                        QString comment = QString::fromUtf8(vc.mid(static_cast<int>(off), clen));
                        off += clen;
                        if (comment.startsWith("ARTIST=", Qt::CaseInsensitive))
                            m.insert("autor", comment.mid(7).trimmed());
                        else if (comment.startsWith("artist=", Qt::CaseInsensitive))
                            m.insert("autor", comment.mid(7).trimmed());
                    }
                }
            }
            break;
        }
        f.seek(f.pos() + blen);
        if (last) break;
    }
}

// ─── OGG (Vorbis / Opus / Speex) ──────────────────────────

void extrair_ogg_audio(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(256 * 1024)));
    if (d.size() < 58) return;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    // Verify OGG page magic "OggS"
    if (memcmp(base, "OggS", 4) != 0) return;

    int samplerate = 0;
    int channels = 0;
    int bitrate_nominal = 0;

    // Search for Vorbis identification header: "\x01vorbis"
    int vpos = d.indexOf(QByteArray("\x01vorbis", 7));
    if (vpos >= 0 && vpos + 30 <= d.size()) {
        const uchar* v = base + vpos + 7;
        // version (4), channels (1), sample_rate (4LE), bitrate_max (4LE), bitrate_nominal (4LE)
        channels = v[4];
        samplerate = static_cast<int>(rd32le(v + 5));
        bitrate_nominal = static_cast<int>(rd32le(v + 13)); // bitrate_nominal
    }

    // Search for Opus identification header: "OpusHead"
    int opos = d.indexOf("OpusHead");
    if (opos >= 0 && opos + 19 <= d.size()) {
        const uchar* o = base + opos + 8;
        // version (1), channels (1), pre-skip (2LE), input_sample_rate (4LE)
        channels = o[1];
        samplerate = static_cast<int>(rd32le(o + 4)); // original sample rate
        if (samplerate == 0) samplerate = 48000; // Opus always decodes at 48kHz
    }

    // Search for Speex identification header: "Speex   "
    int spos = d.indexOf("Speex   ");
    if (spos >= 0 && spos + 80 <= d.size()) {
        const uchar* s = base + spos;
        // offset 36: sample rate (4LE), 40: mode, 44: mode_bitstream_version, 48: nb_channels
        samplerate = static_cast<int>(rd32le(s + 36));
        channels = static_cast<int>(rd32le(s + 48));
        bitrate_nominal = static_cast<int>(rd32le(s + 52));
    }

    Q_UNUSED(channels)

    // Find last OGG page to get granule position for duration
    // Scan backwards for "OggS" then read granule_position
    qint64 last_granule = -1;
    if (f.size() > 65536) {
        f.seek(qMax(static_cast<qint64>(0), f.size() - 65536));
    } else {
        f.seek(0);
    }
    const QByteArray tail = f.readAll();
    for (int i = tail.size() - 27; i >= 0; --i) {
        const uchar* tp = reinterpret_cast<const uchar*>(tail.constData()) + i;
        if (tp[0] == 'O' && tp[1] == 'g' && tp[2] == 'g' && tp[3] == 'S') {
            last_granule = static_cast<qint64>(rd64le(tp + 6));
            break;
        }
    }

    if (samplerate > 0 && last_granule > 0) {
        qint64 dur = last_granule / samplerate;
        m.insert("duracao", formatar_duracao(dur));
        if (dur > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    } else if (bitrate_nominal > 0) {
        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate_nominal / 1000));
        qint64 dur = (f.size() * 8) / bitrate_nominal;
        if (dur > 0) m.insert("duracao", formatar_duracao(dur));
    }
}

// ─── AIFF / AIFC ──────────────────────────────────────────

void extrair_aiff(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(256 * 1024)));
    if (d.size() < 54) return;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(base, "FORM", 4) != 0) return;
    if (memcmp(base + 8, "AIFF", 4) != 0 && memcmp(base + 8, "AIFC", 4) != 0) return;

    // Scan chunks for COMM (Common)
    qint64 pos = 12;
    while (pos + 8 <= d.size()) {
        const uchar* ch = base + pos;
        quint32 chunk_sz = rd32be(ch + 4);
        if (memcmp(ch, "COMM", 4) == 0 && pos + 8 + 18 <= d.size()) {
            // channels (2BE), numSampleFrames (4BE), sampleSize (2BE), sampleRate (10-byte IEEE 754 extended)
            quint32 num_frames = rd32be(ch + 10);
            // Parse 80-bit IEEE 754 extended float for sample rate
            const uchar* sr_bytes = ch + 16;
            int exponent = ((sr_bytes[0] & 0x7F) << 8) | sr_bytes[1];
            quint64 mantissa = rd64be(sr_bytes + 2);
            double sr = 0.0;
            if (exponent == 0 && mantissa == 0) sr = 0.0;
            else {
                exponent -= 16383;
                sr = static_cast<double>(mantissa) * std::pow(2.0, exponent - 63);
            }
            if (sr > 0 && num_frames > 0) {
                qint64 dur = static_cast<qint64>(num_frames / sr);
                m.insert("duracao", formatar_duracao(dur));
                if (dur > 0) {
                    int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
                    m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
                }
            }
            break;
        }
        pos += 8 + chunk_sz;
        if (chunk_sz & 1) ++pos; // pad byte
    }
}

// ─── ASF (WMA) ────────────────────────────────────────────

bool asf_guid_eq(const uchar* a, const uchar* b) { return memcmp(a, b, 16) == 0; }

void extrair_wma(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(qMin(f.size(), static_cast<qint64>(512 * 1024)));
    if (d.size() < 30) return;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    static const uchar hdr_guid[] = {0x30,0x26,0xB2,0x75,0x8E,0x66,0xCF,0x11,0xA6,0xD9,0x00,0xAA,0x00,0x62,0xCE,0x6C};
    if (!asf_guid_eq(base, hdr_guid)) return;
    const quint64 hdr_sz = rd64le(base + 16);
    if (hdr_sz < 30 || static_cast<qint64>(hdr_sz) > d.size()) return;

    static const uchar fp_guid[] = {0xA1,0xDC,0xAB,0x8C,0x47,0xA9,0xCF,0x11,0x8E,0xE4,0x00,0xC0,0x0C,0x20,0x53,0x65};

    qint64 pos = 30;
    while (pos + 24 <= static_cast<qint64>(hdr_sz) && pos + 24 <= d.size()) {
        const uchar* obj = base + pos;
        const quint64 obj_sz = rd64le(obj + 16);
        if (obj_sz < 24 || pos + static_cast<qint64>(obj_sz) > d.size()) break;
        if (asf_guid_eq(obj, fp_guid) && obj_sz >= 104) {
            const quint64 dur_100ns = rd64le(obj + 64);
            const quint64 preroll_ms = rd64le(obj + 80);
            qint64 dur_sec = static_cast<qint64>(dur_100ns / 10000000ULL);
            if (dur_sec > static_cast<qint64>(preroll_ms / 1000)) dur_sec -= static_cast<qint64>(preroll_ms / 1000);
            const qint64 bitrate = static_cast<qint64>(rd32le(obj + 100));
            m.insert("duracao", formatar_duracao(dur_sec));
            if (bitrate > 0) m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate / 1000));
            break;
        }
        pos += static_cast<qint64>(obj_sz);
    }
}

// ─── EBML / Matroska (MKA) ────────────────────────────────

quint64 ebml_read_id(const uchar* p, qint64 avail, int& len) {
    len = 0;
    if (avail < 1) return 0;
    if (p[0] & 0x80) len = 1;
    else if (p[0] & 0x40) len = 2;
    else if (p[0] & 0x20) len = 3;
    else if (p[0] & 0x10) len = 4;
    else return 0;
    if (avail < len) { len = 0; return 0; }
    quint64 val = 0;
    for (int i = 0; i < len; ++i) val = (val << 8) | p[i];
    return val;
}

quint64 ebml_read_size(const uchar* p, qint64 avail, int& len) {
    len = 0;
    if (avail < 1) return 0;
    for (int i = 0; i < 8; ++i) { if (p[0] & (0x80 >> i)) { len = i + 1; break; } }
    if (len == 0 || avail < len) { len = 0; return 0; }
    quint64 val = p[0] & ((1 << (8 - len)) - 1);
    for (int i = 1; i < len; ++i) val = (val << 8) | p[i];
    quint64 all_ones = (1ULL << (7 * len)) - 1;
    if (val == all_ones) return UINT64_MAX;
    return val;
}

quint64 ebml_uint(const uchar* p, int sz) {
    quint64 v = 0;
    for (int i = 0; i < sz; ++i) v = (v << 8) | p[i];
    return v;
}

double ebml_float(const uchar* p, int sz) {
    if (sz == 4) { quint32 raw = rd32be(p); float f; memcpy(&f, &raw, 4); return static_cast<double>(f); }
    if (sz == 8) { quint64 raw = rd64be(p); double d; memcpy(&d, &raw, 8); return d; }
    return 0.0;
}

void ebml_audio_scan(const uchar* base, qint64 off, qint64 end, qint64 total,
                     double& dur_ms, quint64& ts) {
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

        if (id == 0x18538067 || id == 0x1549A966) { // Segment or Info
            ebml_audio_scan(base, data_off, el_end, total, dur_ms, ts);
        } else if (id == 0x2AD7B1 && data_sz <= 8 && data_off + static_cast<qint64>(data_sz) <= total) {
            ts = ebml_uint(base + data_off, static_cast<int>(data_sz));
        } else if (id == 0x4489 && (data_sz == 4 || data_sz == 8) && data_off + static_cast<qint64>(data_sz) <= total) {
            dur_ms = ebml_float(base + data_off, static_cast<int>(data_sz));
        }
        off = el_end;
    }
}

void extrair_mka(QFile& f, QVariantMap& m) {
    f.seek(0);
    const qint64 read_sz = qMin(f.size(), static_cast<qint64>(8 * 1024 * 1024));
    const QByteArray d = f.read(read_sz);
    if (d.size() < 4) return;
    const uchar* base = reinterpret_cast<const uchar*>(d.constData());
    if (!(base[0] == 0x1A && base[1] == 0x45 && base[2] == 0xDF && base[3] == 0xA3)) return;
    double dur_ms = 0; quint64 ts = 1000000;
    ebml_audio_scan(base, 0, d.size(), d.size(), dur_ms, ts);
    if (dur_ms > 0 && ts > 0) {
        qint64 dur_sec = static_cast<qint64>((dur_ms * static_cast<double>(ts)) / 1e9 + 0.5);
        m.insert("duracao", formatar_duracao(dur_sec));
        if (dur_sec > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur_sec);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── APE (Monkey's Audio) ─────────────────────────────────

void extrair_ape(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(76);
    if (d.size() < 76) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "MAC ", 4) != 0) return;
    const quint16 ver = rd16le(p + 4);
    // Version >= 3980: new header format
    quint32 total_frames = 0, final_frame_blocks = 0, blocks_per_frame = 0;
    quint32 sample_rate = 0, channels = 0, bits = 0;
    if (ver >= 3980 && d.size() >= 76) {
        blocks_per_frame = rd32le(p + 16);
        final_frame_blocks = rd32le(p + 20);
        total_frames = rd32le(p + 24);
        bits = rd16le(p + 28);
        channels = rd16le(p + 30);
        sample_rate = rd32le(p + 32);
    } else if (ver >= 3950) {
        // Older format
        sample_rate = rd32le(p + 12);
        channels = rd16le(p + 18);
        total_frames = rd32le(p + 24);
        blocks_per_frame = rd32le(p + 28);
        final_frame_blocks = rd32le(p + 32);
    }
    Q_UNUSED(bits) Q_UNUSED(channels)
    if (sample_rate > 0 && total_frames > 0 && blocks_per_frame > 0) {
        quint64 total_blocks = static_cast<quint64>(total_frames - 1) * blocks_per_frame + final_frame_blocks;
        qint64 dur = static_cast<qint64>(total_blocks / sample_rate);
        m.insert("duracao", formatar_duracao(dur));
        if (dur > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── WavPack (WV) ─────────────────────────────────────────

void extrair_wv(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(32);
    if (d.size() < 32) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "wvpk", 4) != 0) return;
    // offset 24: total_samples (4LE), offset 23: sample_rate encoded in flags
    const quint32 flags = rd32le(p + 24);
    static const int wv_rates[] = {6000, 8000, 9600, 11025, 12000, 16000, 22050, 24000,
                                   32000, 44100, 48000, 64000, 88200, 96000, 192000, 0};
    int sr_idx = (flags >> 23) & 0x0F;
    int samplerate = (sr_idx < 15) ? wv_rates[sr_idx] : 0;
    // Total samples from header: try to find total samples
    const quint32 total_samples = rd32le(p + 12);
    if (samplerate > 0 && total_samples > 0) {
        qint64 dur = total_samples / samplerate;
        m.insert("duracao", formatar_duracao(dur));
        if (dur > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── AC3 (Dolby Digital) ──────────────────────────────────

void extrair_ac3(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(8);
    if (d.size() < 8) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    // Sync word: 0x0B77
    if (p[0] != 0x0B || p[1] != 0x77) return;
    // Frame size code at byte 4 bits 0-5
    int fscod = (p[4] >> 6) & 0x03;
    int frmsizecod = p[4] & 0x3F;
    static const int ac3_samplerates[] = {48000, 44100, 32000, 0};
    static const int ac3_bitrates[] = {32, 40, 48, 56, 64, 80, 96, 112, 128, 160,
                                        192, 224, 256, 320, 384, 448, 512, 576, 640, 0};
    int samplerate = (fscod < 3) ? ac3_samplerates[fscod] : 0;
    int bitrate_kbps = (frmsizecod / 2 < 19) ? ac3_bitrates[frmsizecod / 2] : 0;
    Q_UNUSED(samplerate)
    if (bitrate_kbps > 0) {
        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate_kbps));
        qint64 dur = (f.size() * 8) / (bitrate_kbps * 1000LL);
        if (dur > 0) m.insert("duracao", formatar_duracao(dur));
    }
}

// ─── DTS ──────────────────────────────────────────────────

void extrair_dts(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(12);
    if (d.size() < 12) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    // DTS sync: 0x7FFE8001
    if (!(p[0] == 0x7F && p[1] == 0xFE && p[2] == 0x80 && p[3] == 0x01)) return;
    // Frame size at bytes 5-6 (bits 2-15 of byte 5-6)
    int fsize = ((p[5] & 0x03) << 12) | (p[6] << 4) | (p[7] >> 4);
    fsize += 1; // frame size is stored as (size - 1)
    // Sample rate index at byte 8 bits 6-9
    int sr_idx = ((p[8] & 0x3C) >> 2);
    static const int dts_samplerates[] = {0, 8000, 16000, 32000, 0, 0, 11025, 22050, 44100, 0, 0, 12000, 24000, 48000, 0, 0};
    // Bitrate index at byte 8 bit 1..0 + byte 9 bits 7..5
    int br_idx = ((p[8] & 0x03) << 3) | ((p[9] >> 5) & 0x07);
    static const int dts_bitrates[] = {
        32, 56, 64, 96, 112, 128, 192, 224, 256, 320, 384, 448, 512, 576, 640, 768,
        896, 1024, 1152, 1280, 1344, 1408, 1411, 1472, 1536, 0, 0, 0, 0, 0, 0, 0
    };
    Q_UNUSED(fsize) Q_UNUSED(sr_idx) Q_UNUSED(dts_samplerates)
    int bitrate_kbps = (br_idx < 25) ? dts_bitrates[br_idx] : 0;
    if (bitrate_kbps > 0) {
        m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate_kbps));
        qint64 dur = (f.size() * 8) / (bitrate_kbps * 1000LL);
        if (dur > 0) m.insert("duracao", formatar_duracao(dur));
    }
}

// ─── AMR ──────────────────────────────────────────────────

void extrair_amr(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray hdr = f.read(12);
    bool wb = false;
    if (hdr.startsWith("#!AMR-WB\n")) wb = true;
    else if (hdr.startsWith("#!AMR\n")) wb = false;
    else return;
    // AMR-NB: 8kHz, 12.2kbps max; AMR-WB: 16kHz, 23.85kbps max
    int bitrate = wb ? 24 : 12;
    m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(bitrate));
    int hdr_sz = wb ? 9 : 6;
    qint64 data_sz = f.size() - hdr_sz;
    if (data_sz > 0 && bitrate > 0) {
        qint64 dur = (data_sz * 8) / (bitrate * 1000LL);
        if (dur > 0) m.insert("duracao", formatar_duracao(dur));
    }
}

// ─── AU / SND ─────────────────────────────────────────────

void extrair_au(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(24);
    if (d.size() < 24) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, ".snd", 4) != 0) return;
    quint32 data_offset = rd32be(p + 4);
    quint32 data_size = rd32be(p + 8);
    quint32 encoding = rd32be(p + 12);
    quint32 samplerate = rd32be(p + 16);
    quint32 channels = rd32be(p + 20);

    int bits = 0;
    switch (encoding) {
        case 1: bits = 8; break; // mu-law
        case 2: bits = 8; break; // linear PCM 8-bit
        case 3: bits = 16; break;
        case 4: bits = 24; break;
        case 5: bits = 32; break;
        case 6: bits = 32; break; // float
        case 7: bits = 64; break; // double
        default: bits = 16; break;
    }
    Q_UNUSED(data_offset)
    if (samplerate > 0 && channels > 0 && bits > 0) {
        quint64 byte_rate = static_cast<quint64>(samplerate) * channels * (bits / 8);
        if (data_size == 0xFFFFFFFF) data_size = static_cast<quint32>(f.size() - data_offset);
        if (byte_rate > 0) {
            qint64 dur = data_size / byte_rate;
            m.insert("duracao", formatar_duracao(dur));
            int kbps = static_cast<int>((byte_rate * 8) / 1000);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── TTA (True Audio) ─────────────────────────────────────

void extrair_tta(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(22);
    if (d.size() < 22) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "TTA1", 4) != 0) return;
    // offset 6: channels (2LE), 8: bps (2LE), 10: samplerate (4LE), 14: total_samples (4LE)
    quint32 samplerate = rd32le(p + 10);
    quint32 total_samples = rd32le(p + 14);
    if (samplerate > 0 && total_samples > 0) {
        qint64 dur = total_samples / samplerate;
        m.insert("duracao", formatar_duracao(dur));
        if (dur > 0) {
            int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
            m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
        }
    }
}

// ─── MPC (Musepack) ───────────────────────────────────────

void extrair_mpc(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(32);
    if (d.size() < 8) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    // SV7: "MP+" magic, byte 3 = version (7)
    if (p[0] == 'M' && p[1] == 'P' && p[2] == '+' && (p[3] & 0x0F) == 7) {
        quint32 total_frames = rd32le(p + 4);
        // MPC SV7: 1152 samples per frame, default 44100Hz
        int samplerate = 44100;
        if (total_frames > 0) {
            qint64 dur = (static_cast<qint64>(total_frames) * 1152) / samplerate;
            m.insert("duracao", formatar_duracao(dur));
            if (dur > 0) {
                int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
                m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
            }
        }
    }
    // SV8: "MPCK" magic
    else if (p[0] == 'M' && p[1] == 'P' && p[2] == 'C' && p[3] == 'K') {
        // SV8 uses variable-length encoding: complex to parse without extra effort
        // Estimate from file size assuming ~200 kbps typical
        qint64 dur = (f.size() * 8) / (200 * 1000LL);
        if (dur > 0) m.insert("duracao", formatar_duracao(dur));
    }
}

// ─── MIDI ─────────────────────────────────────────────────

void extrair_midi(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(14);
    if (d.size() < 14) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "MThd", 4) != 0) return;
    const quint16 format_type = rd16be(p + 8);
    const quint16 ntracks = rd16be(p + 10);
    Q_UNUSED(format_type)
    Q_UNUSED(ntracks)
    // For MIDI, duration requires parsing all track events - estimate instead
    // Typical MIDI: ~3-4 minutes per 50KB
    qint64 dur = static_cast<qint64>((f.size() / 50000.0) * 210);
    if (dur > 0 && dur < 36000) m.insert("duracao", formatar_duracao(dur));
}

// ─── DSF (DSD) ────────────────────────────────────────────

void extrair_dsf(QFile& f, QVariantMap& m) {
    f.seek(0);
    const QByteArray d = f.read(92);
    if (d.size() < 92) return;
    const uchar* p = reinterpret_cast<const uchar*>(d.constData());
    if (memcmp(p, "DSD ", 4) != 0) return;
    // DSF: total file size at offset 12 (8 bytes LE)
    // fmt chunk starts at offset 28: "fmt " + size + format version
    if (memcmp(p + 28, "fmt ", 4) == 0 && d.size() >= 80) {
        quint32 samplerate = rd32le(p + 60);
        quint32 channels = rd32le(p + 64);
        quint64 total_samples = rd64le(p + 68);
        Q_UNUSED(channels)
        if (samplerate > 0 && total_samples > 0) {
            qint64 dur = static_cast<qint64>(total_samples / samplerate);
            m.insert("duracao", formatar_duracao(dur));
            if (dur > 0) {
                int kbps = static_cast<int>((f.size() * 8.0 / 1000.0) / dur);
                m.insert("taxa_bits", QStringLiteral("%1 kbps").arg(kbps));
            }
        }
    }
}

}  // namespace

QVariantMap extrair_metadados_audio(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) return m;

        // WAV / RF64 / BWF
        static const QSet<QString> wav_exts = {".wav", ".wave", ".bwf", ".rf64"};
        // MP3
        static const QSet<QString> mp3_exts = {".mp3"};
        // ISOBMFF audio (AAC/M4A/ALAC/3GA)
        static const QSet<QString> isobmff_exts = {".aac", ".m4a", ".m4b", ".m4r", ".alac", ".3ga"};
        // FLAC
        static const QSet<QString> flac_exts = {".flac"};
        // OGG (Vorbis/Opus/Speex)
        static const QSet<QString> ogg_exts = {".ogg", ".oga", ".opus", ".spx", ".weba"};
        // AIFF
        static const QSet<QString> aiff_exts = {".aiff", ".aif", ".aifc"};
        // WMA (ASF)
        static const QSet<QString> wma_exts = {".wma"};
        // MKA (EBML/Matroska)
        static const QSet<QString> mka_exts = {".mka"};
        // APE (Monkey's Audio)
        static const QSet<QString> ape_exts = {".ape"};
        // WavPack
        static const QSet<QString> wv_exts = {".wv"};
        // AC3
        static const QSet<QString> ac3_exts = {".ac3", ".eac3", ".ec3"};
        // DTS
        static const QSet<QString> dts_exts = {".dts", ".dtshd"};
        // AMR
        static const QSet<QString> amr_exts = {".amr", ".awb"};
        // AU/SND
        static const QSet<QString> au_exts = {".au", ".snd"};
        // TTA
        static const QSet<QString> tta_exts = {".tta"};
        // MPC (Musepack)
        static const QSet<QString> mpc_exts = {".mpc", ".mp+", ".mpp"};
        // MIDI
        static const QSet<QString> midi_exts = {".mid", ".midi", ".kar"};
        // DSF (DSD)
        static const QSet<QString> dsf_exts = {".dsf"};
        // RealAudio
        static const QSet<QString> ra_exts = {".ra", ".ram"};
        // CAF (Apple Core Audio Format - ISOBMFF-like but different; estimate)
        static const QSet<QString> caf_exts = {".caf"};

        if (wav_exts.contains(ext)) {
            extrair_wav(f, m);
        } else if (mp3_exts.contains(ext)) {
            extrair_mp3(f, m);
        } else if (isobmff_exts.contains(ext)) {
            extrair_isobmff_audio(f, m);
        } else if (flac_exts.contains(ext)) {
            extrair_flac(f, m);
        } else if (ogg_exts.contains(ext)) {
            extrair_ogg_audio(f, m);
        } else if (aiff_exts.contains(ext)) {
            extrair_aiff(f, m);
        } else if (wma_exts.contains(ext)) {
            extrair_wma(f, m);
        } else if (mka_exts.contains(ext)) {
            extrair_mka(f, m);
        } else if (ape_exts.contains(ext)) {
            extrair_ape(f, m);
        } else if (wv_exts.contains(ext)) {
            extrair_wv(f, m);
        } else if (ac3_exts.contains(ext)) {
            extrair_ac3(f, m);
        } else if (dts_exts.contains(ext)) {
            extrair_dts(f, m);
        } else if (amr_exts.contains(ext)) {
            extrair_amr(f, m);
        } else if (au_exts.contains(ext)) {
            extrair_au(f, m);
        } else if (tta_exts.contains(ext)) {
            extrair_tta(f, m);
        } else if (mpc_exts.contains(ext)) {
            extrair_mpc(f, m);
        } else if (midi_exts.contains(ext)) {
            extrair_midi(f, m);
        } else if (dsf_exts.contains(ext)) {
            extrair_dsf(f, m);
        } else if (ra_exts.contains(ext) || caf_exts.contains(ext)) {
            // Estimate: ~128 kbps typical
            qint64 dur = (fi.size() * 8) / (128 * 1000LL);
            if (dur > 0) {
                m.insert("duracao", formatar_duracao(dur));
                m.insert("taxa_bits", QStringLiteral("128 kbps"));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados do áudio", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
