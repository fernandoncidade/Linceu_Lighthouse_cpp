#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QCryptographicHash>
#include <QByteArrayView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QVariantMap>
#include <QVariantList>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <Softpub.h>
#endif

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/risk_01_score_risco.h"
#include "source/security/agre_01_motor_regras.h"
#include "source/security/base_01_baseline_criptografico.h"
#include "source/security/exec_01_inteligencia_executaveis.h"
#include "source/security/quar_01_quarentena.h"
#include "source/security/evid_01_exportacao_evidencia.h"
#include "source/security/secore_01_postgres_utils.h"
#include "source/services/GerenciamentoMetadados/gmet_10_ExtrairMetadadosExecutavel.h"

namespace {

std::atomic<bool> g_stop_requested{false};

#if defined(_WIN32)
BOOL WINAPI consoleCtrlHandler(DWORD ctrlType) {
    if (ctrlType == CTRL_C_EVENT || ctrlType == CTRL_BREAK_EVENT || ctrlType == CTRL_CLOSE_EVENT) {
        g_stop_requested.store(true);
        std::cout << "\n[*] Stopping observation cleanly...\n";
        return TRUE;
    }
    return FALSE;
}
#endif

static std::string toUtf8(const QString& str) {
    return str.toUtf8().toStdString();
}

static std::string escapeCsv(const QString& val) {
    QString s = val;
    if (s.contains('"') || s.contains(',') || s.contains('\n') || s.contains('\r')) {
        s.replace('"', "\"\"");
        return "\"" + s.toUtf8().toStdString() + "\"";
    }
    return s.toUtf8().toStdString();
}

static QString formatFileSize(qint64 bytes) {
    if (bytes < 1024) {
        return QStringLiteral("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(QString::number(bytes / 1024.0, 'f', 2));
    }
    if (bytes < 1024 * 1024 * 1024) {
        return QStringLiteral("%1 MB").arg(QString::number(bytes / (1024.0 * 1024.0), 'f', 2));
    }
    return QStringLiteral("%1 GB").arg(QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2));
}

static QString computeSha256(const QString& filePath, qint64 maxBytes = 250 * 1024 * 1024) {
    QFileInfo fi(filePath);
    if (!fi.exists() || !fi.isFile()) {
        return QString();
    }
    if (fi.size() > maxBytes) {
        return QStringLiteral("SKIPPED_LARGE_FILE");
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QStringLiteral("ACCESS_DENIED");
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    char buf[65536];
    while (!file.atEnd()) {
        const qint64 read = file.read(buf, sizeof(buf));
        if (read > 0) {
            hash.addData(QByteArrayView(buf, read));
        }
    }
    return QString::fromLatin1(hash.result().toHex());
}

static QString computeSha1(const QString& filePath, qint64 maxBytes = 250 * 1024 * 1024) {
    QFileInfo fi(filePath);
    if (!fi.exists() || !fi.isFile()) {
        return QString();
    }
    if (fi.size() > maxBytes) {
        return QStringLiteral("SKIPPED_LARGE_FILE");
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QStringLiteral("ACCESS_DENIED");
    }
    QCryptographicHash hash(QCryptographicHash::Sha1);
    char buf[65536];
    while (!file.atEnd()) {
        const qint64 read = file.read(buf, sizeof(buf));
        if (read > 0) {
            hash.addData(QByteArrayView(buf, read));
        }
    }
    return QString::fromLatin1(hash.result().toHex());
}

static QString classifyFile(const QString& filePath) {
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    static const QHash<QString, QString> categories = {
        {QStringLiteral("exe"), QStringLiteral("Executable")},
        {QStringLiteral("dll"), QStringLiteral("Dynamic Library")},
        {QStringLiteral("sys"), QStringLiteral("System Driver")},
        {QStringLiteral("drv"), QStringLiteral("Driver")},
        {QStringLiteral("com"), QStringLiteral("Executable")},
        {QStringLiteral("scr"), QStringLiteral("Screensaver/Binary")},
        {QStringLiteral("msi"), QStringLiteral("Windows Installer")},
        {QStringLiteral("bat"), QStringLiteral("Batch Script")},
        {QStringLiteral("cmd"), QStringLiteral("Batch Script")},
        {QStringLiteral("ps1"), QStringLiteral("PowerShell Script")},
        {QStringLiteral("psm1"), QStringLiteral("PowerShell Module")},
        {QStringLiteral("vbs"), QStringLiteral("VBScript")},
        {QStringLiteral("js"), QStringLiteral("JavaScript")},
        {QStringLiteral("py"), QStringLiteral("Python Script")},
        {QStringLiteral("sh"), QStringLiteral("Shell Script")},
        {QStringLiteral("pdf"), QStringLiteral("PDF Document")},
        {QStringLiteral("docx"), QStringLiteral("Word Document")},
        {QStringLiteral("doc"), QStringLiteral("Word Document")},
        {QStringLiteral("xlsx"), QStringLiteral("Excel Spreadsheet")},
        {QStringLiteral("xls"), QStringLiteral("Excel Spreadsheet")},
        {QStringLiteral("pptx"), QStringLiteral("PowerPoint Presentation")},
        {QStringLiteral("txt"), QStringLiteral("Text Document")},
        {QStringLiteral("csv"), QStringLiteral("CSV Data")},
        {QStringLiteral("json"), QStringLiteral("JSON Document")},
        {QStringLiteral("xml"), QStringLiteral("XML Document")},
        {QStringLiteral("yaml"), QStringLiteral("YAML Configuration")},
        {QStringLiteral("yml"), QStringLiteral("YAML Configuration")},
        {QStringLiteral("ini"), QStringLiteral("Configuration File")},
        {QStringLiteral("cfg"), QStringLiteral("Configuration File")},
        {QStringLiteral("zip"), QStringLiteral("Zip Archive")},
        {QStringLiteral("7z"), QStringLiteral("7-Zip Archive")},
        {QStringLiteral("tar"), QStringLiteral("Tar Archive")},
        {QStringLiteral("gz"), QStringLiteral("Gzip Archive")},
        {QStringLiteral("rar"), QStringLiteral("RAR Archive")},
        {QStringLiteral("db"), QStringLiteral("Database")},
        {QStringLiteral("sqlite"), QStringLiteral("SQLite Database")},
        {QStringLiteral("sqlite3"), QStringLiteral("SQLite Database")},
        {QStringLiteral("cpp"), QStringLiteral("C++ Source Code")},
        {QStringLiteral("c"), QStringLiteral("C Source Code")},
        {QStringLiteral("h"), QStringLiteral("C/C++ Header")},
        {QStringLiteral("hpp"), QStringLiteral("C++ Header")},
        {QStringLiteral("cs"), QStringLiteral("C# Source Code")},
        {QStringLiteral("java"), QStringLiteral("Java Source Code")},
        {QStringLiteral("png"), QStringLiteral("PNG Image")},
        {QStringLiteral("jpg"), QStringLiteral("JPEG Image")},
        {QStringLiteral("jpeg"), QStringLiteral("JPEG Image")},
        {QStringLiteral("ico"), QStringLiteral("Icon Image")},
        {QStringLiteral("mp3"), QStringLiteral("Audio")},
        {QStringLiteral("mp4"), QStringLiteral("Video")},
    };
    return categories.value(suffix, QStringLiteral("Generic File"));
}

static void printBanner() {
    std::cout << "===============================================================\n";
    std::cout << "  Linceu Lighthouse CLI — File Integrity & Security Auditor   \n";
    std::cout << "  Version 2026.10.21.0 | C++17 Security & Monitoring Engine    \n";
    std::cout << "===============================================================\n\n";
}

static void printHelp() {
    printBanner();
    std::cout << "Usage:\n";
    std::cout << "  linceu-lighthouse-cli.exe <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  scan <path>              Scan directory for files, hashes, and security risks\n";
    std::cout << "  monitor <path>           Real-time filesystem observation and live security alerting\n";
    std::cout << "  metadata <file>          Extract deep technical metadata (PE, hashes, timestamps)\n";
    std::cout << "  baseline <path>          Verify files against stored cryptographic baseline or detect drift\n";
    std::cout << "  alerts                   List security alerts and rule violations from database\n";
    std::cout << "  quarantine <action>      Quarantine actions: 'list' or 'restore <id> <destination>'\n";
    std::cout << "  sessions                 List observation and audit sessions from database\n";
    std::cout << "  --version, -v            Show official version\n";
    std::cout << "  --help, -h               Show this help message\n\n";
    std::cout << "Options:\n";
    std::cout << "  --format <text|json|csv> Output format (default: text)\n";
    std::cout << "  --depth <n>              Scan recursion depth limit (default: unlimited, 0 = target only)\n";
    std::cout << "  --risks-only             Only output files/events with warnings or security risks\n";
    std::cout << "  --duration <seconds>     Duration limit for monitoring in seconds (default: 0 = until Ctrl+C)\n";
    std::cout << "  --update                 Update/create cryptographic baseline entries during baseline check\n";
    std::cout << "  --no-banner              Suppress banner header in text mode\n\n";
    std::cout << "Examples:\n";
    std::cout << "  linceu-lighthouse-cli.exe scan \"C:\\inetpub\\wwwroot\" --format json\n";
    std::cout << "  linceu-lighthouse-cli.exe scan \"D:\\Finance\" --depth 3 --risks-only\n";
    std::cout << "  linceu-lighthouse-cli.exe monitor \"C:\\Windows\\Temp\" --duration 60\n";
    std::cout << "  linceu-lighthouse-cli.exe metadata \"C:\\Windows\\System32\\cmd.exe\"\n";
    std::cout << "  linceu-lighthouse-cli.exe baseline \"D:\\ImportantDocs\" --format csv\n";
    std::cout << "  linceu-lighthouse-cli.exe alerts --risks-only\n";
    std::cout << "  linceu-lighthouse-cli.exe quarantine list\n";
    std::cout << "  linceu-lighthouse-cli.exe quarantine restore 12 \"C:\\Restored\\file.bin\"\n";
}

struct ScannedItem {
    QString relativePath;
    QString fullPath;
    qint64 sizeBytes;
    QString modifiedUtc;
    QString extension;
    QString category;
    QString sha256;
    bool isSigned;
    int riskScore;
    QString riskLevel;
    QString riskReasons;
};

static int handleScan(const QString& targetPath, int depth, const QString& format, bool risksOnly, bool noBanner) {
    const QDir rootDir(targetPath);
    if (!rootDir.exists()) {
        std::cerr << "[ERROR] Target directory does not exist or is inaccessible: " << toUtf8(targetPath) << "\n";
        return 2;
    }

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Scanning target: " << toUtf8(rootDir.absolutePath())
                  << " (Depth: " << (depth < 0 ? "unlimited" : std::to_string(depth)) << ")...\n\n";
    }

    source::security::ScoreRisco riskScorer;
    std::vector<ScannedItem> items;
    qint64 totalBytes = 0;
    int criticalCount = 0;
    int highCount = 0;
    int mediumCount = 0;
    int lowCount = 0;

    const QDir::Filters filters = QDir::Files | QDir::Hidden | QDir::NoSymLinks;
    const QDirIterator::IteratorFlags flags = (depth == 0)
        ? QDirIterator::NoIteratorFlags
        : QDirIterator::Subdirectories;

    QDirIterator it(rootDir.absolutePath(), filters, flags);
    const QString rootCanonical = QDir::cleanPath(rootDir.absolutePath());

    while (it.hasNext()) {
        const QString filePath = it.next();
        const QFileInfo fi = it.fileInfo();
        if (!fi.isFile()) continue;

        // Check depth limit if specified
        if (depth >= 0) {
            const QString rel = rootDir.relativeFilePath(filePath);
            const int currentDepth = rel.count('/') + rel.count('\\');
            if (currentDepth > depth) continue;
        }

        ScannedItem item;
        item.fullPath = QDir::cleanPath(filePath);
        item.relativePath = rootDir.relativeFilePath(filePath);
        item.sizeBytes = fi.size();
        totalBytes += item.sizeBytes;
        item.modifiedUtc = fi.lastModified().toUTC().toString(Qt::ISODateWithMs);
        item.extension = fi.suffix().toLower();
        item.category = classifyFile(filePath);
        item.sha256 = computeSha256(filePath);

        // Authenticode check for executables
        item.isSigned = false;
        if (item.category == QStringLiteral("Executable") || item.category == QStringLiteral("Dynamic Library")) {
            item.isSigned = source::services::GerenciamentoMetadados::verificar_assinatura_executavel(filePath);
        }

        // Risk calculation
        QVariantMap evt;
        evt.insert(QStringLiteral("dir_atual"), filePath);
        evt.insert(QStringLiteral("nome"), fi.fileName());
        evt.insert(QStringLiteral("tipo_operacao"), QStringLiteral("scan"));
        evt.insert(QStringLiteral("timestamp"), fi.lastModified().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")));
        evt.insert(QStringLiteral("signature_state"), item.isSigned ? QStringLiteral("yes") : QStringLiteral("no"));

        const QVariantMap riskData = riskScorer.score(evt, {});
        item.riskScore = riskData.value(QStringLiteral("risk_score")).toInt();
        item.riskLevel = riskData.value(QStringLiteral("risk_level")).toString();
        item.riskReasons = riskData.value(QStringLiteral("risk_reasons")).toString();

        if (item.riskLevel == QStringLiteral("critical")) {
            criticalCount++;
        } else if (item.riskLevel == QStringLiteral("high")) {
            highCount++;
        } else if (item.riskLevel == QStringLiteral("medium")) {
            mediumCount++;
        } else {
            lowCount++;
        }

        if (risksOnly && item.riskScore < 45) {
            continue;
        }

        items.push_back(std::move(item));
    }

    const int overallScore = items.empty() ? 0 : std::min(100, (criticalCount * 35 + highCount * 15 + mediumCount * 5));

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("targetPath"), rootDir.absolutePath());
        root.insert(QStringLiteral("totalFiles"), static_cast<int>(items.size()));
        root.insert(QStringLiteral("totalBytes"), totalBytes);
        root.insert(QStringLiteral("formattedTotalSize"), formatFileSize(totalBytes));
        root.insert(QStringLiteral("overallRiskScore"), overallScore);
        root.insert(QStringLiteral("criticalRisks"), criticalCount);
        root.insert(QStringLiteral("highRisks"), highCount);
        root.insert(QStringLiteral("mediumRisks"), mediumCount);
        root.insert(QStringLiteral("lowRisks"), lowCount);

        QJsonArray filesArr;
        for (const auto& item : items) {
            QJsonObject fObj;
            fObj.insert(QStringLiteral("relativePath"), item.relativePath);
            fObj.insert(QStringLiteral("fullPath"), item.fullPath);
            fObj.insert(QStringLiteral("sizeBytes"), item.sizeBytes);
            fObj.insert(QStringLiteral("formattedSize"), formatFileSize(item.sizeBytes));
            fObj.insert(QStringLiteral("modifiedUtc"), item.modifiedUtc);
            fObj.insert(QStringLiteral("extension"), item.extension);
            fObj.insert(QStringLiteral("category"), item.category);
            fObj.insert(QStringLiteral("sha256"), item.sha256);
            fObj.insert(QStringLiteral("isSigned"), item.isSigned);
            fObj.insert(QStringLiteral("riskScore"), item.riskScore);
            fObj.insert(QStringLiteral("riskLevel"), item.riskLevel);
            fObj.insert(QStringLiteral("riskReasons"), item.riskReasons);
            filesArr.append(fObj);
        }
        root.insert(QStringLiteral("files"), filesArr);

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return (criticalCount > 0 || highCount > 0) ? 1 : 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "RelativePath,FullPath,SizeBytes,FormattedSize,ModifiedUtc,Extension,Category,SHA256,IsSigned,RiskScore,RiskLevel,RiskReasons\n";
        for (const auto& item : items) {
            std::cout << escapeCsv(item.relativePath) << ","
                      << escapeCsv(item.fullPath) << ","
                      << item.sizeBytes << ","
                      << escapeCsv(formatFileSize(item.sizeBytes)) << ","
                      << escapeCsv(item.modifiedUtc) << ","
                      << escapeCsv(item.extension) << ","
                      << escapeCsv(item.category) << ","
                      << escapeCsv(item.sha256) << ","
                      << (item.isSigned ? "true" : "false") << ","
                      << item.riskScore << ","
                      << escapeCsv(item.riskLevel) << ","
                      << escapeCsv(item.riskReasons) << "\n";
        }
        return (criticalCount > 0 || highCount > 0) ? 1 : 0;
    }

    // Default: text output
    std::cout << "[+] Scan completed successfully.\n";
    std::cout << "    - Target Path:         " << toUtf8(rootDir.absolutePath()) << "\n";
    std::cout << "    - Total Files:         " << items.size() << "\n";
    std::cout << "    - Total Volume:        " << toUtf8(formatFileSize(totalBytes)) << " (" << totalBytes << " bytes)\n";
    std::cout << "    - Overall Risk Score:  " << overallScore << " / 100\n";
    std::cout << "    - Critical Findings:   " << criticalCount << "\n";
    std::cout << "    - High Risk Findings:  " << highCount << "\n";
    std::cout << "    - Medium Risks:        " << mediumCount << "\n";
    std::cout << "    - Low/Normal Items:    " << lowCount << "\n\n";

    if (!items.empty()) {
        std::cout << "--- DETAILED FINDINGS ---\n";
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& item = items[i];
            if (risksOnly && item.riskScore < 45) continue;

            std::cout << "[" << (i + 1) << "] [" << toUtf8(item.riskLevel.toUpper()) << "] "
                      << toUtf8(item.relativePath) << "\n";
            std::cout << "    Size:     " << toUtf8(formatFileSize(item.sizeBytes)) << " (" << item.sizeBytes << " B)\n";
            std::cout << "    Type:     " << toUtf8(item.category) << " (." << toUtf8(item.extension) << ")\n";
            std::cout << "    SHA-256:  " << toUtf8(item.sha256) << "\n";
            if (item.category == QStringLiteral("Executable") || item.category == QStringLiteral("Dynamic Library")) {
                std::cout << "    Signed:   " << (item.isSigned ? "YES [Valid Authenticode]" : "NO [Unsigned Binary]") << "\n";
            }
            if (!item.riskReasons.isEmpty()) {
                std::cout << "    Reasons:  " << toUtf8(item.riskReasons) << "\n";
            }
            std::cout << "\n";
        }
    }

    return (criticalCount > 0 || highCount > 0) ? 1 : 0;
}

static int handleMetadata(const QString& targetFile, const QString& format, bool noBanner) {
    const QFileInfo fi(targetFile);
    if (!fi.exists()) {
        std::cerr << "[ERROR] Target file does not exist: " << toUtf8(targetFile) << "\n";
        return 2;
    }

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Extracting technical metadata for: " << toUtf8(fi.absoluteFilePath()) << "...\n\n";
    }

    const QString sha256 = computeSha256(fi.absoluteFilePath());
    const QString sha1 = computeSha1(fi.absoluteFilePath());
    const QString category = classifyFile(fi.absoluteFilePath());

    // Attribute flags
    const DWORD attrs = GetFileAttributesW(reinterpret_cast<LPCWSTR>(fi.absoluteFilePath().utf16()));
    const bool isReadOnly = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_READONLY);
    const bool isHidden = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_HIDDEN);
    const bool isSystem = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_SYSTEM);
    const bool isArchive = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_ARCHIVE);
    const bool isCompressed = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_COMPRESSED);
    const bool isEncrypted = (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_ENCRYPTED);

    // Deep executable metadata if binary
    bool isSigned = false;
    QVariantMap peMeta;
    if (category == QStringLiteral("Executable") || category == QStringLiteral("Dynamic Library") ||
        category == QStringLiteral("System Driver")) {
        isSigned = source::services::GerenciamentoMetadados::verificar_assinatura_executavel(fi.absoluteFilePath());
        peMeta = source::services::GerenciamentoMetadados::extrair_metadados_executavel(fi.absoluteFilePath(), nullptr);
    }

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("fileName"), fi.fileName());
        root.insert(QStringLiteral("fullPath"), fi.absoluteFilePath());
        root.insert(QStringLiteral("directory"), fi.absolutePath());
        root.insert(QStringLiteral("sizeBytes"), fi.size());
        root.insert(QStringLiteral("formattedSize"), formatFileSize(fi.size()));
        root.insert(QStringLiteral("createdUtc"), fi.birthTime().toUTC().toString(Qt::ISODateWithMs));
        root.insert(QStringLiteral("modifiedUtc"), fi.lastModified().toUTC().toString(Qt::ISODateWithMs));
        root.insert(QStringLiteral("accessedUtc"), fi.lastRead().toUTC().toString(Qt::ISODateWithMs));
        root.insert(QStringLiteral("extension"), fi.suffix().toLower());
        root.insert(QStringLiteral("category"), category);
        root.insert(QStringLiteral("sha256"), sha256);
        root.insert(QStringLiteral("sha1"), sha1);

        QJsonObject attrObj;
        attrObj.insert(QStringLiteral("readOnly"), isReadOnly);
        attrObj.insert(QStringLiteral("hidden"), isHidden);
        attrObj.insert(QStringLiteral("system"), isSystem);
        attrObj.insert(QStringLiteral("archive"), isArchive);
        attrObj.insert(QStringLiteral("compressed"), isCompressed);
        attrObj.insert(QStringLiteral("encrypted"), isEncrypted);
        root.insert(QStringLiteral("attributes"), attrObj);

        if (!peMeta.isEmpty()) {
            QJsonObject peObj;
            peObj.insert(QStringLiteral("isSigned"), isSigned);
            peObj.insert(QStringLiteral("binaryFormat"), peMeta.value(QStringLiteral("formato")).toString());
            peObj.insert(QStringLiteral("architecture"), peMeta.value(QStringLiteral("arquitetura")).toString());
            peObj.insert(QStringLiteral("compiler"), peMeta.value(QStringLiteral("compilador")).toString());
            peObj.insert(QStringLiteral("subsystem"), peMeta.value(QStringLiteral("subsistema")).toString());
            root.insert(QStringLiteral("peDetails"), peObj);
        }

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "Property,Value\n";
        std::cout << "FileName," << escapeCsv(fi.fileName()) << "\n";
        std::cout << "FullPath," << escapeCsv(fi.absoluteFilePath()) << "\n";
        std::cout << "Directory," << escapeCsv(fi.absolutePath()) << "\n";
        std::cout << "SizeBytes," << fi.size() << "\n";
        std::cout << "FormattedSize," << escapeCsv(formatFileSize(fi.size())) << "\n";
        std::cout << "CreatedUtc," << escapeCsv(fi.birthTime().toUTC().toString(Qt::ISODateWithMs)) << "\n";
        std::cout << "ModifiedUtc," << escapeCsv(fi.lastModified().toUTC().toString(Qt::ISODateWithMs)) << "\n";
        std::cout << "AccessedUtc," << escapeCsv(fi.lastRead().toUTC().toString(Qt::ISODateWithMs)) << "\n";
        std::cout << "Extension," << escapeCsv(fi.suffix().toLower()) << "\n";
        std::cout << "Category," << escapeCsv(category) << "\n";
        std::cout << "SHA256," << escapeCsv(sha256) << "\n";
        std::cout << "SHA1," << escapeCsv(sha1) << "\n";
        std::cout << "IsReadOnly," << (isReadOnly ? "true" : "false") << "\n";
        std::cout << "IsHidden," << (isHidden ? "true" : "false") << "\n";
        std::cout << "IsSystem," << (isSystem ? "true" : "false") << "\n";
        std::cout << "IsSigned," << (isSigned ? "true" : "false") << "\n";
        return 0;
    }

    // Default: text output
    std::cout << "File Details:\n";
    std::cout << "  - File Name:       " << toUtf8(fi.fileName()) << "\n";
    std::cout << "  - Full Path:       " << toUtf8(fi.absoluteFilePath()) << "\n";
    std::cout << "  - Directory:       " << toUtf8(fi.absolutePath()) << "\n";
    std::cout << "  - Size:            " << toUtf8(formatFileSize(fi.size())) << " (" << fi.size() << " bytes)\n";
    std::cout << "  - Classification:  " << toUtf8(category) << " (." << toUtf8(fi.suffix().toLower()) << ")\n";
    std::cout << "\nTimestamps (UTC):\n";
    std::cout << "  - Created:         " << toUtf8(fi.birthTime().toUTC().toString(Qt::ISODateWithMs)) << "\n";
    std::cout << "  - Last Modified:   " << toUtf8(fi.lastModified().toUTC().toString(Qt::ISODateWithMs)) << "\n";
    std::cout << "  - Last Accessed:   " << toUtf8(fi.lastRead().toUTC().toString(Qt::ISODateWithMs)) << "\n";
    std::cout << "\nCryptographic Hashes:\n";
    std::cout << "  - SHA-256:         " << toUtf8(sha256) << "\n";
    std::cout << "  - SHA-1:           " << toUtf8(sha1) << "\n";
    std::cout << "\nFile Attributes:\n";
    std::cout << "  - Read-Only:       " << (isReadOnly ? "YES" : "NO") << "\n";
    std::cout << "  - Hidden:          " << (isHidden ? "YES" : "NO") << "\n";
    std::cout << "  - System:          " << (isSystem ? "YES" : "NO") << "\n";
    std::cout << "  - Archive:         " << (isArchive ? "YES" : "NO") << "\n";
    std::cout << "  - Compressed:      " << (isCompressed ? "YES" : "NO") << "\n";
    std::cout << "  - Encrypted:       " << (isEncrypted ? "YES" : "NO") << "\n";

    if (!peMeta.isEmpty()) {
        std::cout << "\nPE Executable Intelligence:\n";
        std::cout << "  - Format:          " << toUtf8(peMeta.value(QStringLiteral("formato")).toString()) << "\n";
        std::cout << "  - Architecture:    " << toUtf8(peMeta.value(QStringLiteral("arquitetura")).toString()) << "\n";
        std::cout << "  - Authenticode:    " << (isSigned ? "VALID [Digitally Signed]" : "NO [Unsigned Binary]") << "\n";
        const QString compiler = peMeta.value(QStringLiteral("compilador")).toString();
        if (!compiler.isEmpty()) {
            std::cout << "  - Compiler/Linker: " << toUtf8(compiler) << "\n";
        }
        const QString sub = peMeta.value(QStringLiteral("subsistema")).toString();
        if (!sub.isEmpty()) {
            std::cout << "  - Subsystem:       " << toUtf8(sub) << "\n";
        }
    }
    std::cout << "\n";
    return 0;
}

static int handleBaseline(const QString& targetPath, bool updateBaseline, const QString& format, bool risksOnly, bool noBanner) {
    const QDir rootDir(targetPath);
    if (!rootDir.exists()) {
        std::cerr << "[ERROR] Target directory does not exist: " << toUtf8(targetPath) << "\n";
        return 2;
    }

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Evaluating cryptographic baseline for: " << toUtf8(rootDir.absolutePath()) << "...\n\n";
    }

    source::security::SecurityManager sec(nullptr);
    sec.ensure_schema();

    // Query existing baseline entries from database
    const QVariantList baselineRows = sec.list_baseline_entries(50000);
    QHash<QString, QString> storedBaseline; // normalized path -> sha256
    for (const auto& var : baselineRows) {
        const QVariantMap m = var.toMap();
        const QString p = QDir::cleanPath(m.value(QStringLiteral("caminho")).toString());
        const QString h = m.value(QStringLiteral("sha256")).toString();
        if (!p.isEmpty() && !h.isEmpty()) {
            storedBaseline.insert(p, h);
        }
    }

    // Traverse directory on disk
    struct BaselineDiffItem {
        QString fullPath;
        QString relativePath;
        QString status; // MATCH, CHANGED, NEW, MISSING
        QString currentSha256;
        QString baselineSha256;
        qint64 sizeBytes;
    };

    std::vector<BaselineDiffItem> results;
    QSet<QString> diskPaths;
    int matchCount = 0;
    int changedCount = 0;
    int newCount = 0;
    int missingCount = 0;

    QDirIterator it(rootDir.absolutePath(), QDir::Files | QDir::Hidden | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString filePath = it.next();
        const QFileInfo fi = it.fileInfo();
        if (!fi.isFile()) continue;

        const QString clean = QDir::cleanPath(filePath);
        diskPaths.insert(clean);

        BaselineDiffItem item;
        item.fullPath = clean;
        item.relativePath = rootDir.relativeFilePath(filePath);
        item.sizeBytes = fi.size();
        item.currentSha256 = computeSha256(filePath);

        if (storedBaseline.contains(clean)) {
            item.baselineSha256 = storedBaseline.value(clean);
            if (item.currentSha256.compare(item.baselineSha256, Qt::CaseInsensitive) == 0) {
                item.status = QStringLiteral("MATCH");
                matchCount++;
            } else {
                item.status = QStringLiteral("CHANGED");
                changedCount++;
            }
        } else {
            item.status = QStringLiteral("NEW");
            newCount++;
        }

        if (risksOnly && item.status == QStringLiteral("MATCH")) {
            continue;
        }
        results.push_back(std::move(item));
    }

    // Check for missing files in baseline that no longer exist on disk
    for (auto bit = storedBaseline.constBegin(); bit != storedBaseline.constEnd(); ++bit) {
        const QString bPath = bit.key();
        if (bPath.startsWith(rootDir.absolutePath(), Qt::CaseInsensitive) && !diskPaths.contains(bPath)) {
            BaselineDiffItem item;
            item.fullPath = bPath;
            item.relativePath = rootDir.relativeFilePath(bPath);
            item.sizeBytes = 0;
            item.currentSha256 = QStringLiteral("DELETED");
            item.baselineSha256 = bit.value();
            item.status = QStringLiteral("MISSING");
            missingCount++;
            results.push_back(std::move(item));
        }
    }

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("targetPath"), rootDir.absolutePath());
        root.insert(QStringLiteral("totalEvaluated"), static_cast<int>(results.size()));
        root.insert(QStringLiteral("matches"), matchCount);
        root.insert(QStringLiteral("changed"), changedCount);
        root.insert(QStringLiteral("newFiles"), newCount);
        root.insert(QStringLiteral("missingFiles"), missingCount);
        root.insert(QStringLiteral("driftDetected"), (changedCount > 0 || missingCount > 0));

        QJsonArray itemsArr;
        for (const auto& item : results) {
            QJsonObject itObj;
            itObj.insert(QStringLiteral("relativePath"), item.relativePath);
            itObj.insert(QStringLiteral("fullPath"), item.fullPath);
            itObj.insert(QStringLiteral("status"), item.status);
            itObj.insert(QStringLiteral("currentSha256"), item.currentSha256);
            itObj.insert(QStringLiteral("baselineSha256"), item.baselineSha256);
            itObj.insert(QStringLiteral("sizeBytes"), item.sizeBytes);
            itemsArr.append(itObj);
        }
        root.insert(QStringLiteral("baselineItems"), itemsArr);

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return (changedCount > 0 || missingCount > 0) ? 1 : 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "Status,RelativePath,FullPath,SizeBytes,CurrentSHA256,BaselineSHA256\n";
        for (const auto& item : results) {
            std::cout << escapeCsv(item.status) << ","
                      << escapeCsv(item.relativePath) << ","
                      << escapeCsv(item.fullPath) << ","
                      << item.sizeBytes << ","
                      << escapeCsv(item.currentSha256) << ","
                      << escapeCsv(item.baselineSha256) << "\n";
        }
        return (changedCount > 0 || missingCount > 0) ? 1 : 0;
    }

    // Default: text output
    std::cout << "[+] Baseline audit completed for: " << toUtf8(rootDir.absolutePath()) << "\n";
    std::cout << "    - Unchanged (MATCH):   " << matchCount << "\n";
    std::cout << "    - Drift (CHANGED):     " << changedCount << "\n";
    std::cout << "    - Untracked (NEW):     " << newCount << "\n";
    std::cout << "    - Deleted (MISSING):   " << missingCount << "\n\n";

    if (changedCount > 0 || missingCount > 0) {
        std::cout << "[!] INTEGRITY DRIFT DETECTED:\n";
        for (const auto& item : results) {
            if (item.status == QStringLiteral("CHANGED")) {
                std::cout << "  [MODIFIED] " << toUtf8(item.relativePath) << "\n";
                std::cout << "    Baseline: " << toUtf8(item.baselineSha256) << "\n";
                std::cout << "    Current:  " << toUtf8(item.currentSha256) << "\n\n";
            } else if (item.status == QStringLiteral("MISSING")) {
                std::cout << "  [DELETED]  " << toUtf8(item.relativePath) << " (was in baseline)\n\n";
            }
        }
    } else {
        std::cout << "[+] All files match stored cryptographic baseline with 100% integrity.\n";
    }

    return (changedCount > 0 || missingCount > 0) ? 1 : 0;
}

static int handleAlerts(const QString& format, bool risksOnly, bool noBanner) {
    source::security::SecurityManager sec(nullptr);
    sec.ensure_schema();

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Querying security alert center...\n\n";
    }

    const QVariantList alerts = sec.list_alerts(500);

    QVariantList filtered;
    for (const auto& var : alerts) {
        const QVariantMap m = var.toMap();
        const QString level = m.value(QStringLiteral("risk_level")).toString().toLower();
        if (risksOnly && (level != QStringLiteral("critical") && level != QStringLiteral("high"))) {
            continue;
        }
        filtered.append(m);
    }

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("totalAlerts"), static_cast<int>(filtered.size()));

        QJsonArray arr;
        for (const auto& var : filtered) {
            const QVariantMap m = var.toMap();
            QJsonObject aObj;
            aObj.insert(QStringLiteral("id"), m.value(QStringLiteral("id")).toLongLong());
            aObj.insert(QStringLiteral("createdAt"), m.value(QStringLiteral("created_at")).toString());
            aObj.insert(QStringLiteral("fileName"), m.value(QStringLiteral("nome")).toString());
            aObj.insert(QStringLiteral("path"), m.value(QStringLiteral("caminho")).toString());
            aObj.insert(QStringLiteral("riskLevel"), m.value(QStringLiteral("risk_level")).toString());
            aObj.insert(QStringLiteral("rules"), m.value(QStringLiteral("rules")).toString());
            aObj.insert(QStringLiteral("summary"), m.value(QStringLiteral("summary")).toString());
            arr.append(aObj);
        }
        root.insert(QStringLiteral("alerts"), arr);

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "ID,CreatedAt,RiskLevel,FileName,Path,Rules,Summary\n";
        for (const auto& var : filtered) {
            const QVariantMap m = var.toMap();
            std::cout << m.value(QStringLiteral("id")).toLongLong() << ","
                      << escapeCsv(m.value(QStringLiteral("created_at")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("risk_level")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("nome")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("caminho")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("rules")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("summary")).toString()) << "\n";
        }
        return 0;
    }

    // Default: text output
    if (filtered.empty()) {
        std::cout << "[+] No security alerts found in database.\n";
        return 0;
    }

    std::cout << "Total Security Alerts: " << filtered.size() << "\n\n";
    for (int i = 0; i < filtered.size(); ++i) {
        const QVariantMap m = filtered[i].toMap();
        std::cout << "[" << (i + 1) << "] [" << toUtf8(m.value(QStringLiteral("risk_level")).toString().toUpper())
                  << "] " << toUtf8(m.value(QStringLiteral("summary")).toString()) << "\n";
        std::cout << "    ID:        #" << m.value(QStringLiteral("id")).toLongLong() << "\n";
        std::cout << "    Timestamp: " << toUtf8(m.value(QStringLiteral("created_at")).toString()) << "\n";
        std::cout << "    File:      " << toUtf8(m.value(QStringLiteral("nome")).toString()) << "\n";
        std::cout << "    Path:      " << toUtf8(m.value(QStringLiteral("caminho")).toString()) << "\n";
        const QString rules = m.value(QStringLiteral("rules")).toString();
        if (!rules.isEmpty()) {
            std::cout << "    Rules:     " << toUtf8(rules) << "\n";
        }
        std::cout << "\n";
    }

    return 0;
}

static int handleQuarantine(const QString& action, qint64 restoreId, const QString& destPath,
                            const QString& format, bool noBanner) {
    source::security::SecurityManager sec(nullptr);
    sec.ensure_schema();

    if (action == QStringLiteral("restore")) {
        if (restoreId <= 0 || destPath.isEmpty()) {
            std::cerr << "[ERROR] Quarantine restore requires item <id> and <destination_path>.\n";
            std::cerr << "Example: linceu-lighthouse-cli.exe quarantine restore 5 \"C:\\Restored\\file.bin\"\n";
            return 3;
        }

        QString errorMsg;
        const bool ok = sec.restore_quarantine_item(restoreId, destPath, &errorMsg);
        if (ok) {
            std::cout << "[+] Item #" << restoreId << " successfully restored from quarantine to:\n";
            std::cout << "    " << toUtf8(destPath) << "\n";
            return 0;
        } else {
            std::cerr << "[ERROR] Failed to restore quarantine item #" << restoreId << ": "
                      << toUtf8(errorMsg) << "\n";
            return 1;
        }
    }

    // Default action: list
    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Listing quarantined items in security vault...\n\n";
    }

    const QVariantList items = sec.list_quarantine_items(500);

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("totalQuarantined"), static_cast<int>(items.size()));

        QJsonArray arr;
        for (const auto& var : items) {
            const QVariantMap m = var.toMap();
            QJsonObject qObj;
            qObj.insert(QStringLiteral("id"), m.value(QStringLiteral("id")).toLongLong());
            qObj.insert(QStringLiteral("createdAt"), m.value(QStringLiteral("created_at")).toString());
            qObj.insert(QStringLiteral("originalPath"), m.value(QStringLiteral("original_path")).toString());
            qObj.insert(QStringLiteral("quarantinePath"), m.value(QStringLiteral("quarantine_path")).toString());
            qObj.insert(QStringLiteral("sha256"), m.value(QStringLiteral("sha256")).toString());
            qObj.insert(QStringLiteral("status"), m.value(QStringLiteral("status")).toString());
            arr.append(qObj);
        }
        root.insert(QStringLiteral("items"), arr);

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "ID,CreatedAt,OriginalPath,QuarantinePath,SHA256,Status\n";
        for (const auto& var : items) {
            const QVariantMap m = var.toMap();
            std::cout << m.value(QStringLiteral("id")).toLongLong() << ","
                      << escapeCsv(m.value(QStringLiteral("created_at")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("original_path")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("quarantine_path")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("sha256")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("status")).toString()) << "\n";
        }
        return 0;
    }

    // Default: text output
    if (items.empty()) {
        std::cout << "[+] No files currently in quarantine.\n";
        return 0;
    }

    std::cout << "Quarantine Vault (" << items.size() << " items):\n\n";
    for (int i = 0; i < items.size(); ++i) {
        const QVariantMap m = items[i].toMap();
        std::cout << "[" << (i + 1) << "] Item #" << m.value(QStringLiteral("id")).toLongLong() << "\n";
        std::cout << "    Original Path:   " << toUtf8(m.value(QStringLiteral("original_path")).toString()) << "\n";
        std::cout << "    Quarantine File: " << toUtf8(m.value(QStringLiteral("quarantine_path")).toString()) << "\n";
        std::cout << "    SHA-256:         " << toUtf8(m.value(QStringLiteral("sha256")).toString()) << "\n";
        std::cout << "    Captured At:     " << toUtf8(m.value(QStringLiteral("created_at")).toString()) << "\n";
        std::cout << "    Status:          " << toUtf8(m.value(QStringLiteral("status")).toString()) << "\n\n";
    }

    return 0;
}

static int handleSessions(const QString& format, bool noBanner) {
    source::security::SecurityManager sec(nullptr);
    sec.ensure_schema();

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Querying observation and security sessions...\n\n";
    }

    const QVariantList sessions = sec.list_sessions(500);

    if (format == QStringLiteral("json")) {
        QJsonObject root;
        root.insert(QStringLiteral("totalSessions"), static_cast<int>(sessions.size()));

        QJsonArray arr;
        for (const auto& var : sessions) {
            const QVariantMap m = var.toMap();
            QJsonObject sObj;
            sObj.insert(QStringLiteral("sessionId"), m.value(QStringLiteral("session_id")).toString());
            sObj.insert(QStringLiteral("profileId"), m.value(QStringLiteral("profile_id")).toString());
            sObj.insert(QStringLiteral("label"), m.value(QStringLiteral("label")).toString());
            sObj.insert(QStringLiteral("startedAt"), m.value(QStringLiteral("started_at")).toString());
            sObj.insert(QStringLiteral("updatedAt"), m.value(QStringLiteral("updated_at")).toString());
            sObj.insert(QStringLiteral("totalEvents"), m.value(QStringLiteral("total_eventos")).toLongLong());
            sObj.insert(QStringLiteral("directories"), m.value(QStringLiteral("diretorios")).toString());
            arr.append(sObj);
        }
        root.insert(QStringLiteral("sessions"), arr);

        QJsonDocument doc(root);
        std::cout << doc.toJson(QJsonDocument::Indented).toStdString() << std::endl;
        return 0;
    }

    if (format == QStringLiteral("csv")) {
        std::cout << "SessionID,ProfileID,Label,StartedAt,UpdatedAt,TotalEvents,Directories\n";
        for (const auto& var : sessions) {
            const QVariantMap m = var.toMap();
            std::cout << escapeCsv(m.value(QStringLiteral("session_id")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("profile_id")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("label")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("started_at")).toString()) << ","
                      << escapeCsv(m.value(QStringLiteral("updated_at")).toString()) << ","
                      << m.value(QStringLiteral("total_eventos")).toLongLong() << ","
                      << escapeCsv(m.value(QStringLiteral("diretorios")).toString()) << "\n";
        }
        return 0;
    }

    // Default: text output
    if (sessions.empty()) {
        std::cout << "[+] No observation sessions found in database.\n";
        return 0;
    }

    std::cout << "Active and Historical Sessions (" << sessions.size() << "):\n\n";
    for (int i = 0; i < sessions.size(); ++i) {
        const QVariantMap m = sessions[i].toMap();
        std::cout << "[" << (i + 1) << "] Session: " << toUtf8(m.value(QStringLiteral("label")).toString()) << "\n";
        std::cout << "    ID:           " << toUtf8(m.value(QStringLiteral("session_id")).toString()) << "\n";
        std::cout << "    Started:      " << toUtf8(m.value(QStringLiteral("started_at")).toString()) << "\n";
        std::cout << "    Last Updated: " << toUtf8(m.value(QStringLiteral("updated_at")).toString()) << "\n";
        std::cout << "    Total Events: " << m.value(QStringLiteral("total_eventos")).toLongLong() << "\n";
        std::cout << "    Directories:  " << toUtf8(m.value(QStringLiteral("diretorios")).toString()) << "\n\n";
    }

    return 0;
}

static int handleMonitor(const QString& targetPath, int durationSeconds, const QString& format, bool risksOnly, bool noBanner) {
    const QDir rootDir(targetPath);
    if (!rootDir.exists()) {
        std::cerr << "[ERROR] Target directory does not exist: " << toUtf8(targetPath) << "\n";
        return 2;
    }

#if defined(_WIN32)
    SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);
#endif

    if (!noBanner && format == QStringLiteral("text")) {
        printBanner();
        std::cout << "[*] Starting real-time observation on: " << toUtf8(rootDir.absolutePath()) << "\n";
        if (durationSeconds > 0) {
            std::cout << "[*] Monitoring duration set to: " << durationSeconds << " seconds.\n";
        } else {
            std::cout << "[*] Continuous monitoring mode. Press Ctrl+C to terminate.\n";
        }
        std::cout << "---------------------------------------------------------------\n";
    }

#if defined(_WIN32)
    HANDLE hDir = CreateFileW(
        reinterpret_cast<LPCWSTR>(rootDir.absolutePath().utf16()),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] Failed to open directory handle for observation (Error code: " << GetLastError() << ")\n";
        return 2;
    }

    source::security::ScoreRisco riskScorer;
    const auto startTime = std::chrono::steady_clock::now();
    BYTE buffer[32768];
    DWORD bytesReturned = 0;
    qint64 eventCounter = 0;

    while (!g_stop_requested.load()) {
        if (durationSeconds > 0) {
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - startTime).count();
            if (elapsed >= durationSeconds) {
                break;
            }
        }

        // Set timeout via ReadDirectoryChangesW
        const BOOL success = ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            nullptr,
            nullptr
        );

        if (!success || bytesReturned == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
        while (pNotify != nullptr) {
            std::wstring wfileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
            const QString relativeName = QString::fromStdWString(wfileName);
            const QString fullItemPath = QDir::cleanPath(rootDir.filePath(relativeName));

            QString actionStr = QStringLiteral("MODIFIED");
            switch (pNotify->Action) {
                case FILE_ACTION_ADDED: actionStr = QStringLiteral("ADDED"); break;
                case FILE_ACTION_REMOVED: actionStr = QStringLiteral("DELETED"); break;
                case FILE_ACTION_MODIFIED: actionStr = QStringLiteral("MODIFIED"); break;
                case FILE_ACTION_RENAMED_OLD_NAME: actionStr = QStringLiteral("RENAMED_OLD"); break;
                case FILE_ACTION_RENAMED_NEW_NAME: actionStr = QStringLiteral("RENAMED_NEW"); break;
            }

            const QString timeNow = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

            // Risk scoring
            QVariantMap evt;
            evt.insert(QStringLiteral("dir_atual"), fullItemPath);
            evt.insert(QStringLiteral("nome"), QFileInfo(fullItemPath).fileName());
            evt.insert(QStringLiteral("tipo_operacao"), actionStr);
            evt.insert(QStringLiteral("timestamp"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")));

            const QVariantMap riskData = riskScorer.score(evt, {});
            const int score = riskData.value(QStringLiteral("risk_score")).toInt();
            const QString level = riskData.value(QStringLiteral("risk_level")).toString();
            const QString reasons = riskData.value(QStringLiteral("risk_reasons")).toString();

            eventCounter++;

            if (!risksOnly || score >= 45) {
                if (format == QStringLiteral("json")) {
                    QJsonObject evtObj;
                    evtObj.insert(QStringLiteral("eventIndex"), eventCounter);
                    evtObj.insert(QStringLiteral("timestampUtc"), timeNow);
                    evtObj.insert(QStringLiteral("action"), actionStr);
                    evtObj.insert(QStringLiteral("path"), fullItemPath);
                    evtObj.insert(QStringLiteral("riskScore"), score);
                    evtObj.insert(QStringLiteral("riskLevel"), level);
                    evtObj.insert(QStringLiteral("riskReasons"), reasons);
                    QJsonDocument d(evtObj);
                    std::cout << d.toJson(QJsonDocument::Compact).toStdString() << std::endl;
                } else if (format == QStringLiteral("csv")) {
                    std::cout << eventCounter << ","
                              << escapeCsv(timeNow) << ","
                              << escapeCsv(actionStr) << ","
                              << escapeCsv(fullItemPath) << ","
                              << score << ","
                              << escapeCsv(level) << ","
                              << escapeCsv(reasons) << std::endl;
                } else {
                    std::cout << "[" << toUtf8(timeNow) << "] ["
                              << std::left << std::setw(8) << toUtf8(actionStr) << "] "
                              << "[" << toUtf8(level.toUpper()) << " (" << score << "/100)] "
                              << toUtf8(fullItemPath);
                    if (!reasons.isEmpty()) {
                        std::cout << " (" << toUtf8(reasons) << ")";
                    }
                    std::cout << std::endl;
                }
            }

            if (pNotify->NextEntryOffset == 0) break;
            pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset);
        }
    }

    CloseHandle(hDir);
    std::cout << "\n[+] Monitoring ended. Total events processed: " << eventCounter << "\n";
    return 0;
#else
    std::cerr << "[ERROR] Real-time filesystem observation requires Windows platform APIs.\n";
    return 1;
#endif
}

} // namespace

int main(int argc, char* argv[]) {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("LinceuLighthouseCLI"));
    app.setApplicationVersion(QStringLiteral("2026.10.21.0"));

    if (argc < 2) {
        printHelp();
        return 0;
    }

    const QString cmdArg = QString::fromLocal8Bit(argv[1]).trimmed();
    if (cmdArg == QStringLiteral("--help") || cmdArg == QStringLiteral("-h") ||
        cmdArg == QStringLiteral("/?") || cmdArg == QStringLiteral("help")) {
        printHelp();
        return 0;
    }

    if (cmdArg == QStringLiteral("--version") || cmdArg == QStringLiteral("-v") ||
        cmdArg == QStringLiteral("version")) {
        std::cout << "Linceu Lighthouse CLI v2026.10.21.0\n";
        return 0;
    }

    QString targetPath;
    QString format = QStringLiteral("text");
    int depth = -1; // -1 = unlimited
    int duration = 0; // 0 = continuous
    bool risksOnly = false;
    bool updateBaseline = false;
    bool noBanner = false;
    QString subAction = QStringLiteral("list");
    qint64 restoreId = 0;
    QString restoreDest;

    for (int i = 2; i < argc; ++i) {
        const QString arg = QString::fromLocal8Bit(argv[i]).trimmed();
        if (arg == QStringLiteral("--format") && i + 1 < argc) {
            format = QString::fromLocal8Bit(argv[++i]).toLower().trimmed();
        } else if (arg == QStringLiteral("--depth") && i + 1 < argc) {
            depth = QString::fromLocal8Bit(argv[++i]).toInt();
            if (depth < 0) depth = 0;
        } else if (arg == QStringLiteral("--duration") && i + 1 < argc) {
            duration = QString::fromLocal8Bit(argv[++i]).toInt();
            if (duration < 0) duration = 0;
        } else if (arg == QStringLiteral("--risks-only")) {
            risksOnly = true;
        } else if (arg == QStringLiteral("--update")) {
            updateBaseline = true;
        } else if (arg == QStringLiteral("--no-banner")) {
            noBanner = true;
        } else if (cmdArg == QStringLiteral("quarantine") && subAction == QStringLiteral("list") &&
                   (arg == QStringLiteral("restore") || arg == QStringLiteral("list"))) {
            subAction = arg;
            if (subAction == QStringLiteral("restore")) {
                if (i + 1 < argc) restoreId = QString::fromLocal8Bit(argv[++i]).toLongLong();
                if (i + 1 < argc) restoreDest = QString::fromLocal8Bit(argv[++i]);
            }
        } else if (targetPath.isEmpty() && !arg.startsWith(QStringLiteral("--"))) {
            targetPath = arg;
        }
    }

    if (cmdArg == QStringLiteral("scan")) {
        if (targetPath.isEmpty()) {
            std::cerr << "[ERROR] Missing required <path> argument for 'scan' command.\n";
            std::cerr << "Run 'linceu-lighthouse-cli.exe --help' for usage.\n";
            return 3;
        }
        return handleScan(targetPath, depth, format, risksOnly, noBanner);
    } else if (cmdArg == QStringLiteral("metadata")) {
        if (targetPath.isEmpty()) {
            std::cerr << "[ERROR] Missing required <file> argument for 'metadata' command.\n";
            return 3;
        }
        return handleMetadata(targetPath, format, noBanner);
    } else if (cmdArg == QStringLiteral("baseline")) {
        if (targetPath.isEmpty()) {
            std::cerr << "[ERROR] Missing required <path> argument for 'baseline' command.\n";
            return 3;
        }
        return handleBaseline(targetPath, updateBaseline, format, risksOnly, noBanner);
    } else if (cmdArg == QStringLiteral("alerts")) {
        return handleAlerts(format, risksOnly, noBanner);
    } else if (cmdArg == QStringLiteral("quarantine")) {
        return handleQuarantine(subAction, restoreId, restoreDest, format, noBanner);
    } else if (cmdArg == QStringLiteral("sessions")) {
        return handleSessions(format, noBanner);
    } else if (cmdArg == QStringLiteral("monitor")) {
        if (targetPath.isEmpty()) {
            std::cerr << "[ERROR] Missing required <path> argument for 'monitor' command.\n";
            return 3;
        }
        return handleMonitor(targetPath, duration, format, risksOnly, noBanner);
    } else {
        std::cerr << "[ERROR] Unknown command: " << toUtf8(cmdArg) << "\n";
        printHelp();
        return 3;
    }
}
