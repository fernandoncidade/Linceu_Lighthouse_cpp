#include "glqt_11_traduzir_tipo_operacao.h"

#include "glqt_13_traduzir_tipo_operacao_fallback.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString traduzir_tipo_operacao(source::ui::LocalizadorQt* self, const QString& valor, const QString& idioma_origem) {
    try {
        if (self->tradutor_metadados_ != nullptr) {
            return self->tradutor_metadados_->traduzir_tipo_operacao(valor, idioma_origem);
        }
        return traduzir_tipo_operacao_fallback(self, valor);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir tipo de operação", true);
        return valor;
    }
}

}
