#include "IconUtils.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include "LogManager.h"

namespace source::utils {

QString get_app_base_path_icon() {
    try {
        const QString appDir = QCoreApplication::applicationDirPath();
        if (!appDir.isEmpty()) {
            return appDir;
        }

        const QFileInfo thisFile(QString::fromUtf8(__FILE__));
        return thisFile.absoluteDir().absolutePath() + QStringLiteral("/..");
    } catch (...) {
        LogManager::get_logger().error("Erro ao obter caminho base do app");
        return QString();
    }
}

QString get_icon_path(const QString& icon_name) {
    try {
        const QString basePathRaw = get_app_base_path_icon();
        if (basePathRaw.isEmpty()) {
            return QString();
        }

        const QString basePath = QDir(basePathRaw).absolutePath();
        QStringList possiblePaths;
        possiblePaths << QDir(basePath).filePath("assets/images/icons/" + icon_name)
                      << QDir(basePath).filePath("assets/images/imgs/" + icon_name)
                      << QDir(basePath).filePath("images/icons/" + icon_name)
                      << QDir(basePath).filePath("images/imgs/" + icon_name)
                      << QDir(basePath).filePath("_internal/images/icons/" + icon_name)
                      << QDir(basePath).filePath("_internal/images/imgs/" + icon_name)
                      << QDir(QFileInfo(QString::fromUtf8(__FILE__)).absoluteDir().filePath("../assets/images/icons")).filePath(icon_name)
                      << QDir(QFileInfo(QString::fromUtf8(__FILE__)).absoluteDir().filePath("../assets/images/imgs")).filePath(icon_name)
                      << QDir(QFileInfo(QString::fromUtf8(__FILE__)).absoluteDir().filePath("../images/icons")).filePath(icon_name)
                      << QDir(QFileInfo(QString::fromUtf8(__FILE__)).absoluteDir().filePath("../images/imgs")).filePath(icon_name);

        for (const QString& iconPath : possiblePaths) {
            const QString absPath = QFileInfo(iconPath).absoluteFilePath();
            if (QFileInfo::exists(absPath)) {
                return absPath;
            }
        }

        if (!possiblePaths.isEmpty()) {
            return QFileInfo(possiblePaths.first()).absoluteFilePath();
        }

        return QString();
    } catch (...) {
        LogManager::get_logger().error(QString("Erro ao obter caminho do ícone '%1'").arg(icon_name));
        return QString();
    }
}

}  // namespace source::utils
