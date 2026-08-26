#include "gmet_29_SalvarEstadoTabela.h"

#include <QTableWidget>

#include "source/services/ob_09_GerenciadorColunas.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap salvar_estado_tabela(source::services::GerenciadorColunas* gc, QObject* tabela) {
    Q_UNUSED(gc)
    QVariantMap out;
    auto* tw = qobject_cast<QTableWidget*>(tabela);
    if (!tw) return out;

    QVariantList eventos;
    for (int row = 0; row < tw->rowCount(); ++row) {
        QVariantMap evento;
        for (int col = 0; col < tw->columnCount(); ++col) {
            auto* header = tw->horizontalHeaderItem(col);
            auto* item = tw->item(row, col);
            if (header && item) evento[header->text()] = item->text();
        }
        eventos.append(evento);
    }
    out["eventos"] = eventos;
    return out;
}

}  // namespace source::services::GerenciamentoMetadados
