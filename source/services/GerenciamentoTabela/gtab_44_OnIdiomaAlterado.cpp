#include "gtab_44_OnIdiomaAlterado.h"

#include <QMetaObject>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void on_idioma_alterado(source::services::GerenciadorTabela* self, const QString& idioma) {
    try {
        if (!self) return;

        QString idioma_atual;
        if (self->loc) {
            idioma_atual = self->loc->property("idioma_atual").toString();
        }

        if (self->retraducao_realizada_para_idioma && idioma_atual == self->idioma_ultima_retraducao) {
            return;
        }

        self->idioma_alvo_retraducao = idioma;
        self->retraducao_realizada_para_idioma = false;
        self->retraduzir_dados_existentes();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em on_idioma_alterado", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

