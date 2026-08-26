#include "gmet_38_GetMetadados.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMutexLocker>
#include <QSet>
#include <QStringList>

#include "gmet_02_ExtrairMetadadosCodigoFonte.h"
#include "gmet_03_ExtrairMetadadosImagem.h"
#include "gmet_04_ExtrairMetadadosAudio.h"
#include "gmet_05_ExtrairMetadadosVideo.h"
#include "gmet_06_ExtrairMetadadosDocumento.h"
#include "gmet_07_ExtrairMetadadosPlanilha.h"
#include "gmet_08_ExtrairMetadadosApresentacao.h"
#include "gmet_09_ExtrairMetadadosBancoDados.h"
#include "gmet_10_ExtrairMetadadosExecutavel.h"
#include "gmet_11_ExtrairMetadadosTemporario.h"
#include "gmet_12_ExtrairMetadadosCompactados.h"
#include "gmet_13_ExtrairMetadadosBackup.h"
#include "gmet_14_ExtrairMetadadosLog.h"
#include "gmet_15_ExtrairMetadadosConfig.h"
#include "gmet_17_ExtrairMetadadosDadosEstruturados.h"
#include "gmet_19_GetTipoArquivo.h"
#include "gmet_20_GetTamanhoDiretorioArquivo.h"
#include "gmet_21_GetFormataTamanho.h"
#include "gmet_22_GetAtributosArquivo.h"
#include "gmet_23_GetAutorArquivo.h"
#include "gmet_27_GetProtecaoArquivo.h"
#include "gmet_47_GetTamanhoBytes.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {
const QSet<QString> kSpreadsheetExts = {
    ".xls", ".xlsx", ".xlsm", ".ods", ".csv", ".tsv", ".numbers", ".xltx", ".xltm", ".xlsb", ".xlr",
    ".xlt", ".xml", ".sxc", ".slk", ".dif", ".prn", ".gsheet", ".123", ".wk1", ".wk3", ".wk4", ".wks",
    ".wb1", ".wb2", ".wb3", ".ab2", ".ab3", ".aws", ".bcsv", ".clf", ".et", ".fm", ".gnumeric", ".mar",
    ".nmbtemplate", ".qpw", ".sdc", ".stc", ".vc", ".xlw"
};

const QSet<QString> kImageExts = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".tif", ".psd", ".psb", ".webp",
    ".raw", ".heic", ".heif", ".cr2", ".cr3", ".nef", ".arw", ".ico", ".svg", ".ai",
    ".indd", ".eps", ".jfif", ".dib", ".pbm", ".pgm", ".ppm", ".xbm", ".xpm", ".dds",
    ".emf", ".wmf", ".apng", ".avif", ".exr", ".hdr", ".icns", ".jpe", ".jif", ".jxr",
    ".pcx", ".pic", ".pct", ".ras", ".sgi", ".sun", ".viff", ".wbmp", ".3fr", ".ari",
    ".bay", ".cap", ".cin", ".dcr", ".dng", ".erf", ".fff", ".gpr", ".iiq", ".k25",
    ".kdc", ".mdc", ".mef", ".mos", ".mrw", ".nrw", ".orf", ".pef", ".ptx", ".pxn",
    ".r3d", ".raf", ".rwl", ".rw2", ".sr2", ".srf", ".srw", ".x3f", ".art", ".bpg",
    ".cd5", ".cpt", ".dpx", ".ecw", ".fpx", ".jbig", ".jp2", ".j2k", ".j2c", ".jpc",
    ".jps", ".mpo", ".pat", ".tga", ".targa", ".vtf", ".cif", ".cmyk", ".dcx", ".djvu",
    ".drw", ".fig", ".flc", ".fli", ".flic", ".fits", ".gbr", ".iff", ".lbm", ".mac",
    ".mng", ".msp", ".nitf", ".otb", ".pcd", ".pict", ".pjpeg", ".qti", ".qtif", ".rgb",
    ".rgba", ".rle", ".sid", ".xwd", ".hif", ".qoi", ".pnm", ".pam", ".rwz"
};

const QSet<QString> kSourceCodeExts = {
    ".py", ".java", ".cpp", ".c", ".h", ".hpp", ".cs", ".js", ".html", ".htm", ".mht", ".xhtml", ".mhtml", ".css",
    ".php", ".sql", ".json", ".xml", ".yaml", ".yml", ".md", ".rst", ".syms", ".svg", ".bat", ".sh", ".ps1",
    ".psm1", ".psd1", ".ps1xml", ".pssc", ".psc1", ".rb", ".pl", ".pm", ".go", ".rs", ".swift", ".kt", ".dart",
    ".ts", ".jsx", ".tsx", ".vb", ".vbs", ".lua", ".r", ".m", ".scala", ".groovy", ".erl", ".hs", ".clj", ".coffee",
    ".f90", ".for", ".ada", ".pas", ".d", ".asm", ".s", ".v", ".vhdl", ".tcl", ".scm", ".lisp", ".pro",
    ".db2", ".cob", ".jsp", ".asp", ".aspx", ".inc", ".hxx", ".cxx", ".cc", ".mm", ".mjs", ".fs",
    ".fsx", ".fsi", ".ml", ".mli", ".cl", ".cu", ".ipynb", ".jl", ".nim", ".vala", ".cr", ".hx", ".ex",
    ".exs", ".elm", ".purs", ".rkt", ".ss", ".sc", ".sbt", ".kts", ".psql", ".prg", ".abap", ".ahk", ".au3",
    ".bas", ".cljs", ".e", ".ecl", ".el", ".fth", ".gml", ".ino", ".l", ".lgt", ".lsp", ".n", ".p", ".pp", ".red",
    ".sb2", ".sb3", ".scpt", ".vbscript", ".y", ".cgi", ".fcgi", ".plx", ".pmc", ".pod", ".psgi", ".phtml"
};

const QSet<QString> kPresentationExts = {
    ".ppt", ".pptx", ".odp", ".key", ".potx", ".ppsx", ".pptm", ".potm", ".ppsm", ".pps", ".pot", ".sxi",
    ".gslides", ".sti", ".sdd", ".sdp", ".fods", ".keynote", ".nb", ".odg", ".otp", ".pez", ".shf", ".show", ".sxd", ".watch"
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

const QSet<QString> kDatabaseExts = {
    ".dat", ".db", ".sqlite", ".mdb", ".accdb", ".sav", ".spss", ".db-journal", ".sql", ".dbf", ".ndb", ".frm",
    ".myd", ".myi", ".sqlite3", ".sl3", ".sdb", ".cdb", ".pdb", ".gdb", ".ora", ".dmp", ".par", ".mdf", ".ldf",
    ".idb", ".ibd", ".sqlite2", ".4db", ".4dd", ".adf", ".adp", ".alf", ".ask", ".btr", ".cat", ".daconnections",
    ".daschema", ".db2", ".db3", ".dbc", ".kexi", ".kexic", ".kexis", ".lirs", ".ndf", ".nwdb", ".pan", ".pdm",
    ".pdi", ".pnz", ".rpd", ".rsd", ".sbf", ".sds", ".sqlitedb", ".udl", ".wdb", ".abs", ".abx", ".adb", ".dta"
};

const QSet<QString> kExecutableExts = {
    ".exe", ".dll", ".bin", ".app", ".apk", ".msi", ".run", ".bat", ".cmd", ".pma", ".pbtxt", ".binarypb", ".tflite",
    ".fst", ".otf", ".ldb", ".leveldb", ".blob", ".lnk", ".hyb", ".msix", ".com", ".scr", ".jar", ".wsf", ".vbs",
    ".gadget", ".pif", ".cpl", ".sys", ".drv", ".bpl", ".ocx", ".so", ".out", ".elf", ".a", ".dylib", ".prg",
    ".appimage", ".ipa", ".deb", ".rpm", ".pkg", ".msu", ".cab", ".bsp", ".mod", ".xpi", ".crx", ".xap", ".action",
    ".cgi", ".dek", ".ear", ".ex_", ".ex4", ".fpi", ".fxp", ".gs", ".ham", ".ipf", ".isu", ".jse", ".mel", ".mrc",
    ".ms", ".mxe", ".o", ".obs", ".paf.exe", ".phar", ".pyc", ".pyo", ".rgs", ".scb", ".scar", ".scpt", ".sfx",
    ".shb", ".u3p", ".udf", ".upx", ".vpm", ".widget", ".workflow", ".ws", ".zl9"
};

const QSet<QString> kCompressedExts = {
    ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".cab", ".pb", ".pb.gz", ".tgz", ".tar.gz", ".tar.bz2",
    ".tar.xz", ".ace", ".arj", ".lzh", ".lha", ".z", ".jar", ".war", ".ear", ".apk", ".deb", ".rpm", ".pkg", ".img",
    ".iso", ".dmg", ".cso", ".hqx", ".sit", ".sitx", ".bin", ".cue", ".001", ".002", ".alz", ".arc", ".ark", ".b1",
    ".ba", ".bh", ".car", ".cbr", ".cbz", ".pak", ".partimg", ".pea", ".s7z", ".sen", ".tbz2",
    ".tlz", ".uc2", ".uha", ".wim", ".xar", ".zipx", ".a00", ".a01", ".a02", ".ain", ".apa", ".cfs", ".dar", ".dd",
    ".egg", ".ess", ".f", ".gbs", ".gza", ".ha", ".ice", ".ipg", ".kgb", ".lbr", ".lqr", ".lz", ".lzo", ".lzx",
    ".mbw", ".mpq", ".nth", ".p7m", ".part", ".puz", ".r00", ".r01", ".r02", ".rk", ".sda", ".sea", ".sfx",
    ".shk", ".shar", ".shr", ".tar.z", ".taz", ".tbz", ".uc", ".vsi", ".wad", ".xef", ".zlib", ".zoo"
};

const QSet<QString> kBackupExts = {
    ".bak", ".bkp", ".backup", ".old", ".orig", ".save", ".sav", ".auto", ".abk", ".arc", ".gho", ".ibackup",
    ".tib", ".bkf", ".bup", ".swm", ".sna", ".sn1", ".sn2", ".sn3", ".wbk", ".mbk", ".sbk", ".fbk", ".rbk",
    ".~bk", ".$bk", ".$$$", ".@@@", ".b@k", ".bac", ".sik", ".tmp"
};

const QSet<QString> kLogExts = {
    ".log", ".trace", ".dmp", ".dump", ".hprof", ".core", ".err", ".out", ".blg", ".etl", ".evtx", ".glog",
    ".lst", ".prn", ".trc", ".wrn", ".clg", ".audit", ".journal", ".lg", ".txt", ".dat", ".svclog", ".history", ".evt"
};

const QSet<QString> kConfigExts = {
    ".ini", ".cfg", ".config", ".conf", ".properties", ".plist", ".toml", ".settings", ".rc", ".json", ".yaml",
    ".yml", ".env", ".desktop", ".profile", ".bashrc", ".zshrc", ".cshrc", ".login", ".logout", ".npmrc", ".editorconfig",
    ".cf", ".cnf", ".inf", ".reg", ".scr", ".sys", ".xml", ".prefs", ".opt", ".options", ".ds_store", ".gitconfig",
    ".htaccess", ".htpasswd", ".manifest", ".pro", ".props", ".sln", ".user", ".vcxproj", ".xcu", ".xmc"
};

const QSet<QString> kFontExts = {
    ".fnt", ".fon", ".otf", ".ttf", ".dfont", ".eot", ".pfb", ".pfm", ".woff", ".woff2", ".afm", ".bdf", ".pcf", ".snf"
};

const QSet<QString> kSystemExts = {
    ".acm", ".ax", ".bpl", ".com", ".cpl", ".cur", ".ani", ".drv", ".ds", ".efi", ".grd", ".icl", ".ime", ".job",
    ".kbd", ".mui", ".nls", ".rom", ".rsrc", ".sdi", ".sfc", ".spl", ".sve", ".theme", ".vxd", ".win", ".cer", ".p7b",
    ".der", ".crt", ".icls", ".plt", ".dwg", ".dcm"
};

const QSet<QString> kGisExts = {
    ".shp", ".shx", ".dbf", ".prj", ".sbn", ".sbx", ".gpx", ".kml", ".kmz", ".geojson", ".topojson", ".gdb",
    ".mxd", ".aprx", ".lyr", ".lyrx", ".qgs", ".qgz", ".mbtiles", ".geotiff", ".las", ".laz"
};

const QSet<QString> kVirtualDiskExts = {
    ".vdi", ".vhd", ".vhdx", ".vmdk", ".hdd", ".hds", ".pvm", ".qcow", ".qcow2", ".qed", ".vbox", ".vbox-prev"
};

bool has_ext(const QString& nome_arquivo_lower, const QSet<QString>& exts) {
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_arquivo_lower.endsWith(ext)) return true;
    }
    return false;
}

QString normalizar_chave_cache(const QString& caminho) {
    if (caminho.trimmed().isEmpty()) {
        return {};
    }

    QString normalizado = QDir::toNativeSeparators(QDir::cleanPath(caminho));
#ifdef Q_OS_WIN
    normalizado = normalizado.toLower();
#endif
    return normalizado;
}

QStringList chaves_cache_metadados(const QVariantMap& item, const QString& caminho_preferencial) {
    QStringList chaves;
    const auto adicionar = [&chaves](const QString& valor) {
        const QString trimmed = valor.trimmed();
        if (trimmed.isEmpty()) {
            return;
        }

        const QString normalizada = normalizar_chave_cache(trimmed);
        for (const QString& chave : {trimmed, normalizada}) {
            if (!chave.isEmpty() && !chaves.contains(chave, Qt::CaseInsensitive)) {
                chaves.append(chave);
            }
        }
    };

    adicionar(caminho_preferencial);
    adicionar(item.value(QStringLiteral("dir_atual")).toString());
    adicionar(item.value(QStringLiteral("dir_anterior")).toString());
    return chaves;
}

bool buscar_cache_metadados(source::services::GerenciadorColunas* gc,
                            const QStringList& chaves,
                            QVariantMap* metadados,
                            QString* chave_encontrada = nullptr) {
    if (gc == nullptr || metadados == nullptr || chaves.isEmpty()) {
        return false;
    }

    QMutexLocker locker(&gc->lock_cache);
    for (const QString& chave : chaves) {
        const auto it = gc->cache_metadados.constFind(chave);
        if (it == gc->cache_metadados.constEnd()) {
            continue;
        }

        const QVariantMap valor = it.value().toMap();
        if (valor.isEmpty()) {
            continue;
        }

        *metadados = valor;
        if (chave_encontrada != nullptr) {
            *chave_encontrada = chave;
        }
        return true;
    }
    return false;
}

void armazenar_cache_metadados(source::services::GerenciadorColunas* gc,
                               const QString& caminho,
                               const QVariantMap& metadados) {
    if (gc == nullptr || caminho.trimmed().isEmpty() || metadados.isEmpty()) {
        return;
    }

    QMutexLocker locker(&gc->lock_cache);
    gc->cache_metadados[caminho] = metadados;
    const QString normalizado = normalizar_chave_cache(caminho);
    if (!normalizado.isEmpty()) {
        gc->cache_metadados[normalizado] = metadados;
    }
}

void merge_map(QVariantMap& destino, const QVariantMap& origem) {
    destino.insert(origem);
}

void preencher_unidades_tamanho(QVariantMap& metadados, qint64 bytes,
                                const QString& prefixo = QStringLiteral("size")) {
    metadados.insert(prefixo + QStringLiteral("_b"), bytes);
    metadados.insert(prefixo + QStringLiteral("_kb"), QString::number(bytes / 1024.0, 'f', 2));
    metadados.insert(prefixo + QStringLiteral("_mb"), QString::number(bytes / (1024.0 * 1024.0), 'f', 2));
    metadados.insert(prefixo + QStringLiteral("_gb"), QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2));
    metadados.insert(prefixo + QStringLiteral("_tb"),
                     QString::number(bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2));
}

QString detectar_mime(const QFileInfo& info, const QString& nome_fallback = QString()) {
    QMimeDatabase mime_db;
    QMimeType mime;

    if (info.exists()) {
        mime = mime_db.mimeTypeForFile(info);
    } else {
        QString nome = nome_fallback.trimmed();
        if (nome.isEmpty()) {
            nome = info.fileName();
        }
        if (!nome.isEmpty()) {
            mime = mime_db.mimeTypeForFile(nome, QMimeDatabase::MatchExtension);
        }
    }

    return mime.isValid() ? mime.name() : QString();
}

bool valor_metadado_preenchido(const QVariantMap& metadados, const QString& chave) {
    return !metadados.value(chave).toString().trimmed().isEmpty();
}

bool cache_tem_datas_base(const QVariantMap& metadados) {
    return valor_metadado_preenchido(metadados, QStringLiteral("data_criacao")) &&
           valor_metadado_preenchido(metadados, QStringLiteral("data_modificacao")) &&
           valor_metadado_preenchido(metadados, QStringLiteral("data_acesso"));
}

QDateTime primeira_data_valida(const QDateTime& principal,
                               const QDateTime& fallback_1 = QDateTime(),
                               const QDateTime& fallback_2 = QDateTime()) {
    if (principal.isValid()) return principal;
    if (fallback_1.isValid()) return fallback_1;
    if (fallback_2.isValid()) return fallback_2;
    return {};
}

QString formatar_data_metadado(const QDateTime& data) {
    return data.isValid() ? data.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) : QString();
}
}

QVariantMap get_metadados_gc(source::services::GerenciadorColunas* gc, const QVariantMap& item) {
    try {
        if (!gc) return {};
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(gc->loc);

        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
        const bool caminho_existe = !caminho.isEmpty() && QFileInfo::exists(caminho);
        const QStringList chaves_cache = chaves_cache_metadados(item, caminho);
        QVariantMap cache_previo;
        QString chave_cache_encontrada;
        if (buscar_cache_metadados(gc, chaves_cache, &cache_previo, &chave_cache_encontrada)) {
            const QString chave_preferencial = normalizar_chave_cache(caminho);
            if (!chave_preferencial.isEmpty() &&
                chave_cache_encontrada.compare(chave_preferencial, Qt::CaseInsensitive) != 0) {
                armazenar_cache_metadados(gc, caminho, cache_previo);
            }

            if (!caminho_existe || cache_tem_datas_base(cache_previo)) {
                return cache_previo;
            }
        }

        if (caminho.isEmpty() || !caminho_existe) {
            const QString tipo = identificar_tipo_arquivo(caminho, loc, item.value("nome").toString());
            const QString mime = detectar_mime(QFileInfo(caminho), item.value("nome").toString());
            const QString autor = !item.value("autor").toString().trimmed().isEmpty()
                                      ? item.value("autor").toString()
                                      : (!qEnvironmentVariable("USERNAME").trimmed().isEmpty()
                                             ? qEnvironmentVariable("USERNAME").trimmed()
                                             : qEnvironmentVariable("USER").trimmed());

            return {
                {"tipo", tipo},
                {"mime", mime},
                {"atributos", item.value("atributos", loc ? loc->get_text("file") : QStringLiteral("file"))},
                {"autor", autor},
                {"protegido", item.value("protegido", loc ? loc->get_text("no") : QStringLiteral("no"))},
                {"dimensoes", item.value("dimensoes", QString())},
                {"duracao", item.value("duracao", QString())},
                {"taxa_bits", item.value("taxa_bits", QString())}
            };
        }

        QFileInfo fi(caminho);
        QVariantMap metadados;
        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        const QString extensao = fi.suffix().toLower();
        const QString nome_arquivo_lower = fi.fileName().toLower();
        const QString tipo_categoria = identificar_tipo_arquivo(caminho, loc, item.value("nome").toString());

        // Para arquivo, prioriza extensão na coluna "tipo"; para pasta, mantém descrição traduzida.
        metadados.insert("tipo", fi.isDir() ? tipo_categoria : (extensao.isEmpty() ? tipo_categoria : extensao));
        metadados.insert("mime", detectar_mime(fi, item.value("nome").toString()));

        const qint64 tamanho_bytes = fi.isDir() ? get_tamanho_bytes(gc, item) : fi.size();
        metadados.insert("tamanho", get_formata_tamanho(static_cast<double>(tamanho_bytes)));
        preencher_unidades_tamanho(metadados, tamanho_bytes, QStringLiteral("size"));

        if (fi.isDir()) {
            qint64 arquivos = 0;
            bool stats_cache_pronto = false;
            {
                QMutexLocker locker(&gc->lock_cache);
                const QVariantMap cache_dir = gc->cache_metadados.value(caminho).toMap();
                stats_cache_pronto = cache_dir.value(QStringLiteral("_dir_stats_ready")).toBool();
                arquivos = cache_dir.value(QStringLiteral("arquivos")).toLongLong();
            }

            if (!stats_cache_pronto) {
                QDirIterator it(caminho, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    it.next();
                    ++arquivos;
                }
            }
            metadados.insert("arquivos", arquivos);
        }

        const QDateTime data_modificacao = primeira_data_valida(fi.lastModified(), fi.metadataChangeTime());
        const QDateTime data_criacao = primeira_data_valida(fi.birthTime(), fi.metadataChangeTime(), data_modificacao);
        const QDateTime data_acesso = primeira_data_valida(fi.lastRead(), data_modificacao, data_criacao);
        metadados.insert("data_acesso", formatar_data_metadado(data_acesso));
        metadados.insert("data_modificacao", formatar_data_metadado(data_modificacao));
        metadados.insert("data_criacao", formatar_data_metadado(data_criacao));
        metadados.insert("atributos", get_atributos_arquivo(item, loc));
        metadados.insert("autor", get_autor_arquivo(item, loc));
        metadados.insert("protegido", get_protecao_arquivo(gc, item, loc));

        static const QSet<QString> aud_exts = {
            ".wav", ".wave", ".mp3", ".aac", ".flac", ".ogg", ".oga", ".opus", ".m4a", ".m4b",
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

        const bool is_source = has_ext(nome_arquivo_lower, kSourceCodeExts);
        const bool is_image = has_ext(nome_arquivo_lower, kImageExts);
        const bool is_audio = has_ext(nome_arquivo_lower, aud_exts);
        const bool is_video = has_ext(nome_arquivo_lower, vid_exts);
        const bool is_spreadsheet = has_ext(nome_arquivo_lower, kSpreadsheetExts);
        const bool is_presentation = has_ext(nome_arquivo_lower, kPresentationExts);
        const bool is_document = has_ext(nome_arquivo_lower, kDocumentExts);
        const bool is_database = has_ext(nome_arquivo_lower, kDatabaseExts);
        const bool is_executable = has_ext(nome_arquivo_lower, kExecutableExts);
        const bool is_compressed = has_ext(nome_arquivo_lower, kCompressedExts);
        const bool is_backup = has_ext(nome_arquivo_lower, kBackupExts);
        const bool is_log = has_ext(nome_arquivo_lower, kLogExts) || ext == QStringLiteral(".log");
        const bool is_config = has_ext(nome_arquivo_lower, kConfigExts);
        const bool is_font = has_ext(nome_arquivo_lower, kFontExts);
        const bool is_system = has_ext(nome_arquivo_lower, kSystemExts);
        const bool is_gis = has_ext(nome_arquivo_lower, kGisExts);
        const bool is_virtual_disk = has_ext(nome_arquivo_lower, kVirtualDiskExts);
        const bool is_temp = QSet<QString>{".tmp", ".temp", ".~", ".swp", ".swo", ".$$", ".old", ".part", ".cache", ".crdownload", ".download", ".partial", ".lock", ".thumb"}.contains(ext);

        if (is_source) merge_map(metadados, extrair_metadados_codigo_fonte(caminho, loc));
        if (is_image) merge_map(metadados, extrair_metadados_imagem(caminho, loc));
        else if (is_audio) merge_map(metadados, extrair_metadados_audio(caminho, loc));
        else if (is_video) merge_map(metadados, extrair_metadados_video(caminho, loc));
        else if (is_spreadsheet) merge_map(metadados, extrair_metadados_planilha(caminho, loc));
        else if (is_presentation) merge_map(metadados, extrair_metadados_apresentacao(caminho, loc));
        else if (is_document) merge_map(metadados, extrair_metadados_documento(caminho, loc));
        if (is_database || is_gis) merge_map(metadados, extrair_metadados_banco_dados(caminho, loc));
        if (is_executable || is_font || is_system) merge_map(metadados, extrair_metadados_executavel(caminho, loc));
        if (is_temp) merge_map(metadados, extrair_metadados_temporario(caminho, loc));
        if (is_compressed || is_virtual_disk) merge_map(metadados, extrair_metadados_compactados(caminho, loc));
        if (is_backup) merge_map(metadados, extrair_metadados_backup(caminho, loc));
        if (is_log) merge_map(metadados, extrair_metadados_log(caminho, loc));
        if (is_config) merge_map(metadados, extrair_metadados_config(caminho, loc));
        if (ext == QStringLiteral(".dat") && !is_database && !is_log) {
            merge_map(metadados, extrair_metadados_dados_estruturados(caminho, loc));
        }

        // Normaliza metadados compactados para colunas esperadas na tabela/banco.
        if (metadados.contains("tamanho_descompactado_bytes")) {
            const qint64 unzipped_b = metadados.value("tamanho_descompactado_bytes").toLongLong();
            preencher_unidades_tamanho(metadados, unzipped_b, QStringLiteral("unzipped"));
        }

        // Garante chaves de tamanho para arquivos binários/executáveis.
        if (is_executable || is_font || is_system) {
            preencher_unidades_tamanho(metadados, fi.size(), QStringLiteral("binary_file"));
        }

        armazenar_cache_metadados(gc, caminho, metadados);
        return metadados;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter metadados", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoMetadados
