#include "glqt_08_salvar_preferencia_idioma.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

void salvar_preferencia_idioma(source::ui::LocalizadorQt*, const QString& idioma) {
    try {
        const QString config_path = QDir(source::utils::obter_caminho_persistente()).filePath("language_config.json");
        QFile file(config_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return;
        }

        QJsonObject obj;
        obj.insert("idioma", idioma);
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
        file.close();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao salvar preferência de idioma", true);
    }
}

}
