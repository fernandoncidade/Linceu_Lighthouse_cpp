#include "LogManager.h"

#include <QDateTime>
#include <QDir>
#include <QTextStream>

namespace source::utils {
namespace {

QMutex g_logMutex;
bool g_configured = false;
QString g_logFilePath;
Logger g_logger;

QString default_log_dir() {
    QString base = qEnvironmentVariable("LOCALAPPDATA");
    if (base.isEmpty()) {
        base = QDir::homePath() + QStringLiteral("/AppData/Local");
    }
    return QDir(base).filePath("LinceuLighthouse/logs");
}

QString fallback_log_dir() {
    return QDir(QDir::homePath()).filePath("AppData/Local/File-Folder-Manager/logs");
}

QString now_file_stamp() {
    return QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
}

QString now_log_stamp() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

void append_line(const QString& line) {
    QMutexLocker locker(&g_logMutex);
    QFile file(g_logFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << line << Qt::endl;
    }

    QTextStream serr(stderr);
    serr << line << Qt::endl;
}

}  // namespace

void Logger::write(const QString& level, const QString& message) const {
    if (!g_configured) {
        LogManager::configure_logging();
    }
    const QString line = QString("%1 [%2] FileManager: %3").arg(now_log_stamp(), level, message);
    append_line(line);
}

void Logger::debug(const QString& message) const {
    write("DEBUG", message);
}

void Logger::info(const QString& message) const {
    write("INFO", message);
}

void Logger::warning(const QString& message) const {
    write("WARNING", message);
}

void Logger::error(const QString& message, bool) const {
    write("ERROR", message);
}

void Logger::critical(const QString& message, bool) const {
    write("CRITICAL", message);
}

Logger& LogManager::get_logger() {
    if (!g_configured) {
        configure_logging();
    }
    return g_logger;
}

QString LogManager::get_log_file() {
    if (!g_configured) {
        configure_logging();
    }
    return g_logFilePath;
}

QString LogManager::ensure_unicode(const QString& message) {
    return message;
}

void LogManager::configure_logging() {
    if (g_configured) {
        return;
    }

    QString dirPath = default_log_dir();
    if (!QDir().mkpath(dirPath)) {
        dirPath = fallback_log_dir();
        QDir().mkpath(dirPath);
    }

    g_logFilePath = QDir(dirPath).filePath(QString("file_monitor_%1.log").arg(now_file_stamp()));
    g_configured = true;
}

void LogManager::debug(const QString& message) {
    get_logger().debug(ensure_unicode(message));
}

void LogManager::info(const QString& message) {
    get_logger().info(ensure_unicode(message));
}

void LogManager::warning(const QString& message) {
    get_logger().warning(ensure_unicode(message));
}

void LogManager::error(const QString& message, bool exc_info) {
    get_logger().error(ensure_unicode(message), exc_info);
}

void LogManager::critical(const QString& message, bool exc_info) {
    get_logger().critical(ensure_unicode(message), exc_info);
}

}  // namespace source::utils
