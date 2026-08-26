#include "glqt_09_carregar_preferencia_idioma.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "glqt_08_salvar_preferencia_idioma.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString carregar_preferencia_idioma(source::ui::LocalizadorQt* self) {
    try {
        const QString config_path = QDir(source::utils::obter_caminho_persistente()).filePath("language_config.json");
        if (QFile::exists(config_path)) {
            QFile file(config_path);
            if (file.open(QIODevice::ReadOnly)) {
                const auto doc = QJsonDocument::fromJson(file.readAll());
                file.close();
                const QString idioma = doc.object().value("idioma").toString(self->system_locale_);
                if (self->idiomas_suportados_.contains(idioma)) {
                    return idioma;
                }
                return "pt_BR";
            }
        }

        const QString idioma = self->idiomas_suportados_.contains(self->system_locale_) ? self->system_locale_ : QString("pt_BR");
        salvar_preferencia_idioma(self, idioma);
        return idioma;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao carregar preferência de idioma", true);
        return "pt_BR";
    }
}

}
