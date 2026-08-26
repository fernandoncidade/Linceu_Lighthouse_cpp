#include "gtab_19_AplicarCoresLinhaEspecifica.h"

#include <QAbstractItemModel>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void aplicar_cores_linha_especifica(source::services::GerenciadorTabela* self, QObject* tabela, int row, const QString& tipo_operacao_valor) {
    try {
        Q_UNUSED(tipo_operacao_valor);
        if (!self || !self->interface) return;
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        if (!tw || row < 0 || row >= tw->rowCount()) return;

        if (QAbstractItemModel* model = tw->model()) {
            const QModelIndex idx_ini = model->index(row, 0);
            const QModelIndex idx_fim = model->index(row, tw->columnCount() - 1);
            tw->viewport()->update(tw->visualRect(idx_ini).united(tw->visualRect(idx_fim)));
        } else {
            tw->viewport()->update();
        }

        self->atualizacao_pendente = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em aplicar_cores_linha_especifica", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
