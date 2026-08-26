#include "geui_09_criar_mapeamento_funcoes.h"

#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

QVariantHash criar_mapeamento_funcoes(source::ui::GerenciadorEstatisticasUI*, const QVariantList& graficos_atualizados) {
    try {
        QVariantHash mapeamento;
        for (const QVariant& v : graficos_atualizados) {
            const QVariantMap g = v.toMap();
            mapeamento.insert(g.value("func").toString(), g.value("titulo"));
        }
        return mapeamento;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar mapeamento de funções", true);
        return {};
    }
}

}
