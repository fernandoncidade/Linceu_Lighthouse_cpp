#include "gtab_45_RetraduzirDadosExistentes.h"

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void retraduzir_dados_existentes(source::services::GerenciadorTabela* self) {
    try {
        if (!self) return;

        const QString idioma_atual = self->loc ? self->loc->property("idioma_atual").toString() : QString();
        if (self->retraducao_realizada_para_idioma && idioma_atual == self->idioma_ultima_retraducao) {
            return;
        }

        if (self->retraducao_em_andamento) {
            self->retraducao_agendada = true;
            return;
        }

        if (self->timer_retraducao.isActive()) {
            return;
        }

        self->retraducao_agendada = true;
        self->timer_retraducao.start(50);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em retraduzir_dados_existentes", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

