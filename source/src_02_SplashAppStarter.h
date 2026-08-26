#pragma once

#include <optional>
#include <QString>

class QApplication;
class QWidget;
class QProgressBar;
class QTextEdit;
class QIcon;

namespace source {

class SplashAppStarter {
public:
    SplashAppStarter();
    int start(int argc, char* argv[]);

private:
    std::optional<QIcon> load_qicon(const QString& path) const;
    void create_app(int argc, char* argv[]);
    QWidget* create_splash();
    void report_progress(int percent, const QString& message = QString());
    void loader();

private:
    QApplication* app_;
    QWidget* splash_;
    QProgressBar* progress_;
    QTextEdit* log_widget_;
    QWidget* window_;
};

}
