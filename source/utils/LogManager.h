#pragma once

#include <QFile>
#include <QMutex>
#include <QString>

namespace source::utils {

class Logger {
public:
    void debug(const QString& message) const;
    void info(const QString& message) const;
    void warning(const QString& message) const;
    void error(const QString& message, bool exc_info = false) const;
    void critical(const QString& message, bool exc_info = true) const;

private:
    void write(const QString& level, const QString& message) const;
};

class LogManager {
public:
    static Logger& get_logger();
    static QString get_log_file();
    static QString ensure_unicode(const QString& message);

    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message, bool exc_info = false);
    static void critical(const QString& message, bool exc_info = true);

    static void configure_logging();
};

}  // namespace source::utils
