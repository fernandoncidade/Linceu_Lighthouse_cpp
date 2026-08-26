#include "glqt_12_traduzir_metadados.h"

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString traduzir_metadados(source::ui::LocalizadorQt* self, const QString& valor, const QString& campo) {
    try {
        if (self->tradutor_metadados_ != nullptr) {
            return self->tradutor_metadados_->traduzir_metadados(valor, campo);
        }
        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir metadados", true);
        return valor;
    }
}

}
