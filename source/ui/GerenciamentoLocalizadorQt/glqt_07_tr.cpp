#include "glqt_07_tr.h"

#include "glqt_05_get_text.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString tr(source::ui::LocalizadorQt* self, const QString& text, const QStringList& args) {
    try {
        return get_text(self, text, args);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir texto", true);
        return text;
    }
}

}
