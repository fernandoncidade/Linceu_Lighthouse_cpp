#pragma once

#include <QString>
#include <QVector>

namespace source::utils {

struct AssociatedApp {
    enum class Kind {
        SystemDefault,
        Executable
    };

    Kind kind = Kind::Executable;
    QString displayName;
    QString executablePath;
    QString source;
};

class AssociatedApps {
public:
    static QVector<AssociatedApp> appsForFile(const QString& filePath);
    static bool openWith(const AssociatedApp& app,
                         const QString& filePath,
                         QString* errorMessage = nullptr);
    static bool openWithSystemDefault(const QString& filePath,
                                      QString* errorMessage = nullptr);
};

}  // namespace source::utils
