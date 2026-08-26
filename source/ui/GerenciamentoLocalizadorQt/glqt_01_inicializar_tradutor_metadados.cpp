#include "glqt_01_inicializar_tradutor_metadados.h"

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

void inicializar_tradutor_metadados(source::ui::LocalizadorQt* self) {
    try {
        delete self->tradutor_metadados_;
        self->tradutor_metadados_ = new source::ui::TradutorMetadadosQt(self);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao importar TradutorMetadadosQt");
        self->tradutor_metadados_ = nullptr;
    }
}

}
