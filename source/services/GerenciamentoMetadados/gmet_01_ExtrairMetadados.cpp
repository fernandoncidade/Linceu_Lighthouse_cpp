#include "gmet_01_ExtrairMetadados.h"

#include <QDateTime>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMimeType>
#include <QSet>

#include "gmet_03_ExtrairMetadadosImagem.h"
#include "gmet_04_ExtrairMetadadosAudio.h"
#include "gmet_05_ExtrairMetadadosVideo.h"
#include "gmet_06_ExtrairMetadadosDocumento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QPair<QString, QVariantMap> extrair_metadados(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap metadados;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return {caminho, metadados};
        QMimeDatabase mime_db;
        const QMimeType mime = mime_db.mimeTypeForFile(fi);
        metadados.insert("tamanho", fi.size());
        metadados.insert("mime", mime.isValid() ? mime.name() : QString());
        metadados.insert("data_acesso", fi.lastRead().toSecsSinceEpoch());
        metadados.insert("data_modificacao", fi.lastModified().toSecsSinceEpoch());
        metadados.insert("data_criacao", fi.birthTime().toSecsSinceEpoch());

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();

        static const QSet<QString> img_exts = {
            ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".webp", ".svg", ".ico",
            ".psd", ".psb", ".raw", ".cr2", ".cr3", ".nef", ".dng", ".arw", ".orf", ".rw2",
            ".raf", ".srw", ".pef", ".x3f", ".erf", ".mrw", ".dcr", ".kdc",
            ".heic", ".heif", ".avif", ".hif", ".tga", ".exr", ".hdr", ".pcx",
            ".jp2", ".j2k", ".dds", ".qoi", ".ppm", ".pgm", ".pbm", ".pnm", ".pam"
        };
        static const QSet<QString> aud_exts = {
            ".mp3", ".wav", ".wave", ".aac", ".flac", ".ogg", ".oga", ".opus", ".m4a", ".m4b",
            ".m4r", ".alac", ".aiff", ".aif", ".aifc", ".wma", ".mka", ".ape", ".wv",
            ".ac3", ".eac3", ".dts", ".dtshd", ".amr", ".awb", ".au", ".snd",
            ".tta", ".mpc", ".mp+", ".mpp", ".mid", ".midi", ".kar", ".dsf",
            ".spx", ".weba", ".3ga", ".ra", ".ram", ".caf", ".bwf", ".rf64"
        };
        static const QSet<QString> vid_exts = {
            ".mp4", ".mov", ".avi", ".mkv", ".wmv", ".flv", ".webm", ".mpeg", ".mpg",
            ".m4v", ".mts", ".m2ts", ".ts", ".vob", ".3gp", ".3g2", ".f4v",
            ".rm", ".rmvb", ".divx", ".ogv", ".asf", ".m2v", ".mpv", ".m1v",
            ".mpe", ".dat", ".tp", ".trp", ".qt"
        };
        static const QSet<QString> doc_exts = {
            ".doc", ".docx", ".pdf", ".rtf", ".txt", ".odt", ".wpd", ".pages", ".map", ".md5", ".dotx", ".docm", ".dotm",
            ".msg", ".pst", ".ost", ".pub", ".tex", ".latex", ".djvu", ".epub", ".fb2", ".mobi", ".azw", ".tsv",
            ".rtfd", ".abw", ".sxw", ".sdw", ".gdoc", ".gslides", ".gdraw", ".gtable", ".gform", ".gmap", ".xps", ".oxps",
            ".lit", ".prc", ".pdb", ".ps", ".psw", ".log", ".nfo", ".me", ".1st", ".srt", ".sub", ".lrc", ".dvi", ".ans",
            ".azw3", ".ceb", ".chm", ".cwk", ".dox", ".eml", ".etf", ".ibooks", ".kwd", ".lwp", ".mbp", ".mcw", ".nb",
            ".odm", ".pml", ".stw", ".tcr", ".wn", ".wps", ".zrtf", ".asc", ".ascii", ".bib", ".diz", ".err",
            ".exc", ".faq", ".idx", ".info", ".man", ".readme", ".rel", ".sig", ".uue", ".vcf", ".vcard", ".wp", ".wp5", ".wp6", ".wri"
        };

        if (img_exts.contains(ext)) {
            const QVariantMap img = extrair_metadados_imagem(caminho, loc);
            for (auto it = img.constBegin(); it != img.constEnd(); ++it) metadados.insert(it.key(), it.value());
        } else if (aud_exts.contains(ext)) {
            const QVariantMap audio = extrair_metadados_audio(caminho, loc);
            for (auto it = audio.constBegin(); it != audio.constEnd(); ++it) metadados.insert(it.key(), it.value());
        } else if (vid_exts.contains(ext)) {
            const QVariantMap video = extrair_metadados_video(caminho, loc);
            for (auto it = video.constBegin(); it != video.constEnd(); ++it) metadados.insert(it.key(), it.value());
        } else if (doc_exts.contains(ext)) {
            const QVariantMap doc = extrair_metadados_documento(caminho, loc);
            for (auto it = doc.constBegin(); it != doc.constEnd(); ++it) metadados.insert(it.key(), it.value());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro extraindo metadados", true);
    }
    return {caminho, metadados};
}

}  // namespace source::services::GerenciamentoMetadados
