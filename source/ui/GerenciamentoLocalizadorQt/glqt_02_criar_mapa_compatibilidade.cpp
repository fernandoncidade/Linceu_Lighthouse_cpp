#include "glqt_02_criar_mapa_compatibilidade.h"

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QVariantMap criar_mapa_compatibilidade(source::ui::LocalizadorQt* self) {
    try {
        QVariantMap mapa;
        for (auto it = self->idiomas_suportados_.begin(); it != self->idiomas_suportados_.end(); ++it) {
            QVariantMap compatibilidade;
            compatibilidade.insert("select_all_columns", "select_all_columns");
            compatibilidade.insert("deselect_all_columns", "deselect_all_columns");
            mapa.insert(it.key(), compatibilidade);
        }
        return mapa;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar mapa de compatibilidade", true);
        return {};
    }
}

}
