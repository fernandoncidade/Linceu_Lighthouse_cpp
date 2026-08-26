#include "gtmqt_01_verificar_idioma_cache.h"

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {
void verificar_idioma_cache(source::ui::TradutorMetadadosQt* self) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return;
        }

        const QString idioma_atual = self->loc_->property("idioma_atual").toString();
        if (self->idioma_cache_ != idioma_atual) {
            self->cache_traducoes_tipo_operacao_.clear();
            self->cache_traducoes_tipo_.clear();
            self->cache_traducoes_atributos_.clear();
            self->cache_traducoes_protegido_.clear();
            self->idioma_cache_ = idioma_atual;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar idioma do cache", true);
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
