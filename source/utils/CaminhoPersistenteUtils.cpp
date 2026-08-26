#include "CaminhoPersistenteUtils.h"

#include <QDir>
#include <QStandardPaths>

#include "LogManager.h"

namespace source::utils {

QString obter_caminho_persistente() {
    QString basePath = qEnvironmentVariable("LOCALAPPDATA");
    if (basePath.isEmpty()) {
        basePath = QDir::homePath();
    }

    const QString configDir = QDir(basePath).filePath("LinceuLighthouse");
    QDir dir(configDir);
    if (!dir.exists()) {
        if (!QDir().mkpath(configDir)) {
            LogManager::get_logger().error("Erro ao criar diretório de configuração");
        }
    }

    return configDir;
}

}  // namespace source::utils
