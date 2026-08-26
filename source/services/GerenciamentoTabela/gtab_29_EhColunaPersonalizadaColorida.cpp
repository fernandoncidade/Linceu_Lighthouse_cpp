#include "gtab_29_EhColunaPersonalizadaColorida.h"

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

bool eh_coluna_personalizada_colorida(source::services::GerenciadorTabela* self, const QString& key_coluna) {
    try {
        if (!self || key_coluna.isEmpty()) return false;
        if (key_coluna == QStringLiteral("tipo_operacao")) return true;
        if (self->colunas_para_colorir.contains(key_coluna)) return true;

        if (self->interface) {
            auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
            if (gc) {
                const QVariantMap col = gc->COLUNAS_DISPONIVEIS.value(key_coluna);
                if (!col.isEmpty() && col.value("colorida", false).toBool()) return true;
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em eh_coluna_personalizada_colorida", true);
    }
    return false;
}
}  // namespace source::services::GerenciamentoTabela

