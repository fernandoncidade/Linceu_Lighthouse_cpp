#include "gmet_19_GetTipoArquivo.h"

#include <QFileInfo>
#include <QSet>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {
bool has_ext(const QString& nome_lower, const QSet<QString>& exts) {
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext)) return true;
    }
    return false;
}
}  // namespace

QString identificar_tipo_arquivo(const QString& caminho, source::ui::LocalizadorQt* loc, const QString& nome_arquivo) {
    try {
        const QString nome = nome_arquivo.isEmpty() ? QFileInfo(caminho).fileName() : nome_arquivo;
        const QString nome_lower = nome.toLower();
        const QString ext = QFileInfo(nome).suffix().toLower();
        const QString e = ext.isEmpty() ? QString() : QStringLiteral(".") + ext;

        // Keep "tipo" consistent: .tmp/.TMP files should report their extension.
        if (ext == QStringLiteral("tmp")) return ext;

        const auto t = [&](const QString& key, const QString& fallback) { return loc ? loc->get_text(key) : fallback; };

        const QSet<QString> temp = {".tmp", ".temp", ".~", ".swp", ".swo", ".$$", ".old", ".part", ".cache", ".crdownload", ".download", ".partial", ".lock", ".thumb"};
        if (temp.contains(e) || nome.startsWith("~$") || nome.startsWith("._") || nome.startsWith(".#")) return t("file_temp", "file_temp");

        static const QSet<QString> img = {
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
        if (has_ext(nome_lower, img)) return t("file_image", "file_image");

        const QSet<QString> aud = {
            ".wav", ".wave", ".mp3", ".aac", ".flac", ".ogg", ".oga", ".opus",
            ".aiff", ".aif", ".aifc", ".wma", ".m4a", ".m4b", ".m4r", ".alac",
            ".mka", ".ape", ".wv", ".ac3", ".eac3", ".dts", ".dtshd",
            ".amr", ".awb", ".au", ".snd", ".tta", ".mpc", ".mp+", ".mpp",
            ".mid", ".midi", ".kar", ".dsf", ".spx", ".weba", ".3ga",
            ".ra", ".ram", ".caf", ".bwf", ".rf64"
        };
        if (has_ext(nome_lower, aud)) return t("file_audio", "file_audio");

        const QSet<QString> vid = {
            ".mp4", ".mov", ".avi", ".mkv", ".wmv", ".flv", ".webm", ".mpeg", ".mpg",
            ".m4v", ".mts", ".m2ts", ".ts", ".vob", ".3gp", ".3g2", ".f4v",
            ".rm", ".rmvb", ".divx", ".ogv", ".asf", ".m2v", ".mpv", ".m1v",
            ".mpe", ".dat", ".tp", ".trp", ".qt"
        };
        if (has_ext(nome_lower, vid)) return t("file_video", "file_video");

        static const QSet<QString> spreadsheet_exts = {
            ".xls", ".xlsx", ".xlsm", ".ods", ".csv", ".tsv", ".numbers", ".xltx", ".xltm", ".xlsb", ".xlr",
            ".xlt", ".xml", ".sxc", ".slk", ".dif", ".prn", ".gsheet", ".123", ".wk1", ".wk3", ".wk4", ".wks",
            ".wb1", ".wb2", ".wb3", ".ab2", ".ab3", ".aws", ".bcsv", ".clf", ".et", ".fm", ".gnumeric", ".mar",
            ".nmbtemplate", ".qpw", ".sdc", ".stc", ".vc", ".xlw"
        };
        static const QSet<QString> presentation_exts = {
            ".ppt", ".pptx", ".odp", ".key", ".potx", ".ppsx", ".pptm", ".potm", ".ppsm", ".pps", ".pot", ".sxi",
            ".gslides", ".sti", ".sdd", ".sdp", ".fods", ".keynote", ".nb", ".odg", ".otp", ".pez", ".shf", ".show", ".sxd", ".watch"
        };
        static const QSet<QString> document_exts = {
            ".doc", ".docx", ".pdf", ".rtf", ".txt", ".odt", ".wpd", ".pages", ".map", ".md5", ".dotx", ".docm", ".dotm",
            ".msg", ".pst", ".ost", ".pub", ".tex", ".latex", ".djvu", ".epub", ".fb2", ".mobi", ".azw", ".tsv",
            ".rtfd", ".abw", ".sxw", ".sdw", ".gdoc", ".gslides", ".gdraw", ".gtable", ".gform", ".gmap", ".xps", ".oxps",
            ".lit", ".prc", ".pdb", ".ps", ".psw", ".log", ".nfo", ".me", ".1st", ".srt", ".sub", ".lrc", ".dvi", ".ans",
            ".azw3", ".ceb", ".chm", ".cwk", ".dox", ".eml", ".etf", ".ibooks", ".kwd", ".lwp", ".mbp", ".mcw", ".nb",
            ".odm", ".pml", ".stw", ".tcr", ".wn", ".wps", ".zrtf", ".asc", ".ascii", ".bib", ".diz", ".err",
            ".exc", ".faq", ".idx", ".info", ".man", ".readme", ".rel", ".sig", ".uue", ".vcf", ".vcard", ".wp", ".wp5", ".wp6", ".wri"
        };
        static const QSet<QString> source_exts = {
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
        static const QSet<QString> database_exts = {
            ".dat", ".db", ".sqlite", ".mdb", ".accdb", ".sav", ".spss", ".db-journal", ".sql", ".dbf", ".ndb", ".frm",
            ".myd", ".myi", ".sqlite3", ".sl3", ".sdb", ".cdb", ".pdb", ".gdb", ".ora", ".dmp", ".par", ".mdf", ".ldf",
            ".idb", ".ibd", ".sqlite2", ".4db", ".4dd", ".adf", ".adp", ".alf", ".ask", ".btr", ".cat", ".daconnections",
            ".daschema", ".db2", ".db3", ".dbc", ".kexi", ".kexic", ".kexis", ".lirs", ".ndf", ".nwdb", ".pan", ".pdm",
            ".pdi", ".pnz", ".rpd", ".rsd", ".sbf", ".sds", ".sqlitedb", ".udl", ".wdb", ".abs", ".abx", ".adb", ".dta"
        };
        static const QSet<QString> executable_exts = {
            ".exe", ".dll", ".bin", ".app", ".apk", ".msi", ".run", ".bat", ".cmd", ".pma", ".pbtxt", ".binarypb", ".tflite",
            ".fst", ".otf", ".ldb", ".leveldb", ".blob", ".lnk", ".hyb", ".msix", ".com", ".scr", ".jar", ".wsf", ".vbs",
            ".gadget", ".pif", ".cpl", ".sys", ".drv", ".bpl", ".ocx", ".so", ".out", ".elf", ".a", ".dylib", ".prg",
            ".appimage", ".ipa", ".deb", ".rpm", ".pkg", ".msu", ".cab", ".bsp", ".mod", ".xpi", ".crx", ".xap", ".action",
            ".cgi", ".dek", ".ear", ".ex_", ".ex4", ".fpi", ".fxp", ".gs", ".ham", ".ipf", ".isu", ".jse", ".mel", ".mrc",
            ".ms", ".mxe", ".o", ".obs", ".paf.exe", ".phar", ".pyc", ".pyo", ".rgs", ".scb", ".scar", ".scpt", ".sfx",
            ".shb", ".u3p", ".udf", ".upx", ".vpm", ".widget", ".workflow", ".ws", ".zl9"
        };
        static const QSet<QString> compressed_exts = {
            ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".cab", ".pb", ".pb.gz", ".tgz", ".tar.gz", ".tar.bz2",
            ".tar.xz", ".ace", ".arj", ".lzh", ".lha", ".z", ".jar", ".war", ".ear", ".apk", ".deb", ".rpm", ".pkg", ".img",
            ".iso", ".dmg", ".cso", ".hqx", ".sit", ".sitx", ".bin", ".cue", ".001", ".002", ".alz", ".arc", ".ark", ".b1",
            ".ba", ".bh", ".car", ".cbr", ".cbz", ".pak", ".partimg", ".pea", ".s7z", ".sen", ".tbz2",
            ".tlz", ".uc2", ".uha", ".wim", ".xar", ".zipx", ".a00", ".a01", ".a02", ".ain", ".apa", ".cfs", ".dar", ".dd",
            ".egg", ".ess", ".f", ".gbs", ".gza", ".ha", ".ice", ".ipg", ".kgb", ".lbr", ".lqr", ".lz", ".lzo", ".lzx",
            ".mbw", ".mpq", ".nth", ".p7m", ".part", ".puz", ".r00", ".r01", ".r02", ".rk", ".sda", ".sea", ".sfx",
            ".shk", ".shar", ".shr", ".tar.z", ".taz", ".tbz", ".uc", ".vsi", ".wad", ".xef", ".zlib", ".zoo"
        };
        static const QSet<QString> backup_exts = {
            ".bak", ".bkp", ".backup", ".old", ".orig", ".save", ".sav", ".auto", ".abk", ".arc", ".gho", ".ibackup",
            ".tib", ".bkf", ".bup", ".swm", ".sna", ".sn1", ".sn2", ".sn3", ".wbk", ".mbk", ".sbk", ".fbk", ".rbk",
            ".~bk", ".$bk", ".$$$", ".@@@", ".b@k", ".bac", ".sik", ".tmp"
        };
        static const QSet<QString> log_exts = {
            ".log", ".trace", ".dmp", ".dump", ".hprof", ".core", ".err", ".out", ".blg", ".etl", ".evtx", ".glog",
            ".lst", ".prn", ".trc", ".wrn", ".clg", ".audit", ".journal", ".lg", ".txt", ".dat", ".svclog", ".history", ".evt"
        };
        static const QSet<QString> config_exts = {
            ".ini", ".cfg", ".config", ".conf", ".properties", ".plist", ".toml", ".settings", ".rc", ".json", ".yaml",
            ".yml", ".env", ".desktop", ".profile", ".bashrc", ".zshrc", ".cshrc", ".login", ".logout", ".npmrc", ".editorconfig",
            ".cf", ".cnf", ".inf", ".reg", ".scr", ".sys", ".xml", ".prefs", ".opt", ".options", ".ds_store", ".gitconfig",
            ".htaccess", ".htpasswd", ".manifest", ".pro", ".props", ".sln", ".user", ".vcxproj", ".xcu", ".xmc"
        };
        static const QSet<QString> font_exts = {
            ".fnt", ".fon", ".otf", ".ttf", ".dfont", ".eot", ".pfb", ".pfm", ".woff", ".woff2", ".afm", ".bdf", ".pcf", ".snf"
        };
        static const QSet<QString> system_exts = {
            ".acm", ".ax", ".bpl", ".com", ".cpl", ".cur", ".ani", ".drv", ".ds", ".efi", ".grd", ".icl", ".ime", ".job",
            ".kbd", ".mui", ".nls", ".rom", ".rsrc", ".sdi", ".sfc", ".spl", ".sve", ".theme", ".vxd", ".win", ".cer", ".p7b",
            ".der", ".crt", ".icls", ".plt", ".dwg", ".dcm"
        };
        static const QSet<QString> gis_exts = {
            ".shp", ".shx", ".dbf", ".prj", ".sbn", ".sbx", ".gpx", ".kml", ".kmz", ".geojson", ".topojson", ".gdb",
            ".mxd", ".aprx", ".lyr", ".lyrx", ".qgs", ".qgz", ".mbtiles", ".geotiff", ".las", ".laz"
        };
        static const QSet<QString> virtual_disk_exts = {
            ".vdi", ".vhd", ".vhdx", ".vmdk", ".hdd", ".hds", ".pvm", ".qcow", ".qcow2", ".qed", ".vbox", ".vbox-prev"
        };

        if (has_ext(nome_lower, spreadsheet_exts)) return t("file_spreadsheet", "file_spreadsheet");
        if (has_ext(nome_lower, presentation_exts)) return t("file_presentation", "file_presentation");
        if (has_ext(nome_lower, database_exts) || has_ext(nome_lower, gis_exts)) return t("file_database", "file_database");
        if (has_ext(nome_lower, executable_exts) || has_ext(nome_lower, font_exts) || has_ext(nome_lower, system_exts)) return t("file_executable", "file_executable");
        if (has_ext(nome_lower, compressed_exts) || has_ext(nome_lower, virtual_disk_exts)) return t("file_compressed", "file_compressed");
        if (has_ext(nome_lower, backup_exts)) return t("file_backup", "file_backup");
        if (e == QStringLiteral(".log") || nome_lower.contains("log")) return t("file_log", "file_log");
        if (has_ext(nome_lower, document_exts)) return t("file_document", "file_document");
        if (has_ext(nome_lower, config_exts)) return t("file_config", "file_config");
        if (has_ext(nome_lower, log_exts)) return t("file_log", "file_log");

        if (has_ext(nome_lower, source_exts)) return t("file_source_code", "file_source_code");

        if (QFileInfo(caminho).isDir()) return t("folder", "folder");
        return t("file_unknown", "file_unknown");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao identificar tipo de arquivo", true);
        return QStringLiteral("file_unknown");
    }
}

}  // namespace source::services::GerenciamentoMetadados
