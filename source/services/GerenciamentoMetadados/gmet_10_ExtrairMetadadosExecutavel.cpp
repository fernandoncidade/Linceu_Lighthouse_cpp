#include "gmet_10_ExtrairMetadadosExecutavel.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>

#ifdef _WIN32
#include <windows.h>
#include <wintrust.h>
#include <Softpub.h>
#include <wincrypt.h>
#include <QStringList>
#endif

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

namespace {

const QSet<QString> kExecutableLikeExts = {
    ".exe", ".dll", ".bin", ".app", ".apk", ".msi", ".run", ".bat", ".cmd", ".pma", ".pbtxt", ".binarypb", ".tflite",
    ".fst", ".otf", ".ldb", ".leveldb", ".blob", ".lnk", ".hyb", ".msix", ".com", ".scr", ".jar", ".wsf", ".vbs",
    ".gadget", ".pif", ".cpl", ".sys", ".drv", ".bpl", ".ocx", ".so", ".out", ".elf", ".a", ".dylib", ".prg",
    ".appimage", ".ipa", ".deb", ".rpm", ".pkg", ".msu", ".cab", ".bsp", ".mod", ".xpi", ".crx", ".xap", ".action",
    ".cgi", ".dek", ".ear", ".ex_", ".ex4", ".fpi", ".fxp", ".gs", ".ham", ".ipf", ".isu", ".jse", ".mel", ".mrc",
    ".ms", ".mxe", ".o", ".obs", ".paf.exe", ".phar", ".pyc", ".pyo", ".rgs", ".scb", ".scar", ".scpt", ".sfx",
    ".shb", ".u3p", ".udf", ".upx", ".vpm", ".widget", ".workflow", ".ws", ".zl9",
    ".fnt", ".fon", ".ttf", ".dfont", ".eot", ".pfb", ".pfm", ".woff", ".woff2", ".afm", ".bdf", ".pcf", ".snf",
    ".acm", ".ax", ".cur", ".ani", ".ds", ".efi", ".grd", ".icl", ".ime", ".job", ".kbd", ".mui", ".nls", ".rom",
    ".rsrc", ".sdi", ".sfc", ".spl", ".sve", ".theme", ".vxd", ".win", ".cer", ".p7b", ".der", ".crt", ".icls", ".plt", ".dwg", ".dcm"
};

bool has_ext(const QString& nome_lower, const QSet<QString>& exts) {
    for (const QString& ext : exts) {
        if (!ext.isEmpty() && nome_lower.endsWith(ext)) return true;
    }
    return false;
}

QString detectar_formato_binario(const QByteArray& header) {
    if (header.size() >= 2 && header.left(2) == "MZ") return QStringLiteral("PE");
    if (header.size() >= 4 && static_cast<uchar>(header[0]) == 0x7F &&
        header[1] == 'E' && header[2] == 'L' && header[3] == 'F') {
        return QStringLiteral("ELF");
    }
    if (header.size() >= 4) {
        const quint32 magic = (static_cast<quint32>(static_cast<uchar>(header[0])) << 24) |
                              (static_cast<quint32>(static_cast<uchar>(header[1])) << 16) |
                              (static_cast<quint32>(static_cast<uchar>(header[2])) << 8) |
                              (static_cast<quint32>(static_cast<uchar>(header[3])));
        if (magic == 0xFEEDFACE || magic == 0xFEEDFACF ||
            magic == 0xCEFAEDFE || magic == 0xCFFAEDFE) {
            return QStringLiteral("Mach-O");
        }
    }
    if (header.size() >= 2 && header.left(2) == "#!") return QStringLiteral("Script");
    return QStringLiteral("Genérico");
}

}  // namespace

bool verificar_assinatura_executavel(const QString& caminho) {
#ifdef _WIN32
    const std::wstring caminho_w = caminho.toStdWString();

    WINTRUST_FILE_INFO file_info{};
    file_info.cbStruct = sizeof(WINTRUST_FILE_INFO);
    file_info.pcwszFilePath = caminho_w.c_str();

    WINTRUST_DATA trust_data{};
    trust_data.cbStruct = sizeof(WINTRUST_DATA);
    trust_data.dwUIChoice = WTD_UI_NONE;
    trust_data.fdwRevocationChecks = WTD_REVOKE_NONE;
    trust_data.dwUnionChoice = WTD_CHOICE_FILE;
    trust_data.pFile = &file_info;
    trust_data.dwStateAction = WTD_STATEACTION_VERIFY;
    trust_data.dwProvFlags = WTD_SAFER_FLAG;

    GUID policy_guid = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    const LONG status = WinVerifyTrust(nullptr, &policy_guid, &trust_data);

    trust_data.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policy_guid, &trust_data);

    return status == ERROR_SUCCESS;
#else
    Q_UNUSED(caminho)
    return false;
#endif
}

#ifdef _WIN32
QString query_version_string(const QString& caminho, const QString& key) {
    DWORD dummy = 0;
    const std::wstring caminho_w = caminho.toStdWString();
    const DWORD sz = GetFileVersionInfoSizeW(caminho_w.c_str(), &dummy);
    if (sz == 0) return QString();

    QByteArray buffer;
    buffer.resize(static_cast<int>(sz));
    if (!GetFileVersionInfoW(caminho_w.c_str(), 0, sz, buffer.data())) return QString();

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    } *translate = nullptr;

    UINT cb_translate = 0;
    if (!VerQueryValueW(buffer.data(), L"\\VarFileInfo\\Translation", reinterpret_cast<LPVOID*>(&translate), &cb_translate) ||
        cb_translate < sizeof(LANGANDCODEPAGE)) {
        return QString();
    }

    const QString sub_block = QStringLiteral("\\StringFileInfo\\%1%2\\%3")
                                  .arg(QString::number(translate[0].wLanguage, 16).rightJustified(4, QLatin1Char('0')))
                                  .arg(QString::number(translate[0].wCodePage, 16).rightJustified(4, QLatin1Char('0')))
                                  .arg(key);

    LPVOID value_ptr = nullptr;
    UINT value_len = 0;
    const std::wstring sub_block_w = sub_block.toStdWString();
    if (VerQueryValueW(buffer.data(), sub_block_w.c_str(), &value_ptr, &value_len) && value_ptr && value_len > 0) {
        return QString::fromWCharArray(static_cast<LPCWSTR>(value_ptr)).trimmed();
    }
    return QString();
}
#endif

QVariantMap extrair_metadados_executavel(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists() || !fi.isFile()) return m;

        const QString nome_lower = fi.fileName().toLower();
        const bool extensao_reconhecida = has_ext(nome_lower, kExecutableLikeExts);

        QFile f(caminho);
        QByteArray header;
        if (f.open(QIODevice::ReadOnly)) header = f.read(16);

        const QString formato_binario = detectar_formato_binario(header);
        if (!extensao_reconhecida && formato_binario == QStringLiteral("Genérico")) return m;

        const bool pe_windows = (formato_binario == QStringLiteral("PE"));
        if (pe_windows) {
            const bool assinado = verificar_assinatura_executavel(caminho);
            m.insert("assinado", assinado ? QStringLiteral("Sim") : QStringLiteral("Não"));
        } else {
            m.insert("assinado", QStringLiteral("Desconhecido"));
        }

        m.insert("formato_binario", formato_binario);
        m.insert("executavel_generico", !pe_windows);
        m.insert("tamanho", fi.size());
        m.insert("binary_file_b", fi.size());
        m.insert("binary_file_kb", QString::number(fi.size() / 1024.0, 'f', 2));
        m.insert("binary_file_mb", QString::number(fi.size() / (1024.0 * 1024.0), 'f', 2));
        m.insert("binary_file_gb", QString::number(fi.size() / (1024.0 * 1024.0 * 1024.0), 'f', 2));
        m.insert("binary_file_tb", QString::number(fi.size() / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2));

#ifdef _WIN32
        if (pe_windows) {
            const QString company = query_version_string(caminho, QStringLiteral("CompanyName"));
            const QString desc = query_version_string(caminho, QStringLiteral("FileDescription"));
            const QString product = query_version_string(caminho, QStringLiteral("ProductName"));
            const QString copyright = query_version_string(caminho, QStringLiteral("LegalCopyright"));
            const QString version = query_version_string(caminho, QStringLiteral("FileVersion"));

            if (!company.isEmpty()) m.insert("empresa", company);
            if (!desc.isEmpty()) m.insert("descricao", desc);
            if (!product.isEmpty()) m.insert("produto", product);
            if (!copyright.isEmpty()) m.insert("copyright", copyright);
            if (!version.isEmpty()) m.insert("versao", version);
        }
#endif
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados de executável", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
