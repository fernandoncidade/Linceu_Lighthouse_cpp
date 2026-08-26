#include "ApplicationPathUtils.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

#include "LogManager.h"

namespace source::utils {

std::optional<QString> get_app_base_path() {
    try {
        const QString appDir = QCoreApplication::applicationDirPath();
        if (!appDir.isEmpty()) {
            return appDir;
        }

        const QFileInfo thisFile(QString::fromUtf8(__FILE__));
        return thisFile.absoluteDir().absolutePath() + QStringLiteral("/..");
    } catch (...) {
        LogManager::get_logger().error("Erro ao obter caminho base do app");
        return std::nullopt;
    }
}

std::optional<QString> get_text_file_path(const QString& filename, const std::optional<QString>& folder) {
    try {
        const auto basePathOpt = get_app_base_path();
        if (!basePathOpt.has_value()) {
            return std::nullopt;
        }

        const QString basePath = QDir(basePathOpt.value()).absolutePath();
        const QString baseParent = QFileInfo(basePath).dir().absolutePath();
        QStringList possiblePaths;

        if (folder.has_value()) {
            const QString folderValue = folder.value();
            possiblePaths << QDir(basePath).filePath("assets/" + folderValue + "/" + filename)
                          << QDir(basePath).filePath("source/assets/" + folderValue + "/" + filename)
                          << QDir(basePath).filePath(folderValue + "/" + filename)
                          << QDir(basePath).filePath("_internal/assets/" + folderValue + "/" + filename)
                          << QDir(basePath).filePath("_internal/" + folderValue + "/" + filename)
                          << QDir(baseParent).filePath("assets/" + folderValue + "/" + filename)
                          << QDir(baseParent).filePath("source/assets/" + folderValue + "/" + filename)
                          << QDir(baseParent).filePath(folderValue + "/" + filename)
                          << QDir(basePath).filePath("main.dist/assets/" + folderValue + "/" + filename)
                          << QDir(basePath).filePath("dist/main.dist/assets/" + folderValue + "/" + filename);
        } else {
            possiblePaths << QDir(basePath).filePath("assets/" + filename)
                          << QDir(basePath).filePath("source/assets/" + filename)
                          << QDir(basePath).filePath(filename)
                          << QDir(basePath).filePath("_internal/assets/" + filename)
                          << QDir(basePath).filePath("_internal/" + filename)
                          << QDir(baseParent).filePath("assets/" + filename)
                          << QDir(baseParent).filePath("source/assets/" + filename)
                          << QDir(baseParent).filePath(filename)
                          << QDir(basePath).filePath("main.dist/assets/" + filename)
                          << QDir(basePath).filePath("dist/main.dist/assets/" + filename);
        }

        for (const auto& path : possiblePaths) {
            const QString absPath = QFileInfo(path).absoluteFilePath();
            if (QFileInfo::exists(absPath)) {
                return absPath;
            }
        }

        if (!possiblePaths.isEmpty()) {
            return QFileInfo(possiblePaths.first()).absoluteFilePath();
        }

        return std::nullopt;
    } catch (...) {
        LogManager::get_logger().error(QString("Erro ao obter caminho do arquivo de texto '%1'").arg(filename));
        return std::nullopt;
    }
}

std::optional<QString> load_text_file(const QString& filename, const std::optional<QString>& folder,
                                      const QString& encoding) {
    try {
        const auto filePathOpt = get_text_file_path(filename, folder);
        if (!filePathOpt.has_value()) {
            LogManager::get_logger().error(QString("Arquivo de texto '%1' não encontrado.").arg(filename));
            return std::nullopt;
        }

        QFile file(filePathOpt.value());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LogManager::get_logger().error(QString("Erro ao abrir arquivo de texto '%1'.").arg(filename));
            return std::nullopt;
        }

        QTextStream stream(&file);
        if (!encoding.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QStringConverter::Encoding enc = QStringConverter::Utf8;
            if (encoding.compare("utf-8", Qt::CaseInsensitive) == 0) {
                enc = QStringConverter::Utf8;
            }
            stream.setEncoding(enc);
#else
            QTextCodec* codec = QTextCodec::codecForName(encoding.toUtf8());
            if (codec != nullptr) {
                stream.setCodec(codec);
            }
#endif
        }

        return stream.readAll();
    } catch (...) {
        LogManager::get_logger().error(QString("Erro ao carregar arquivo de texto '%1'").arg(filename));
        return std::nullopt;
    }
}

}  // namespace source::utils
