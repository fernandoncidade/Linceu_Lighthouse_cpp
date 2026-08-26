#include "gtab_07_AjustarAlturaCabecalho.h"

#include <QFontMetrics>
#include <QHeaderView>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void ajustar_altura_cabecalho(source::services::GerenciadorTabela* self, QObject* tabela) {
    try {
        Q_UNUSED(self)
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        if (!tw) return;

        auto* header = tw->horizontalHeader();
        const QFontMetrics fm(header->font());

        int max_linhas = 1;
        for (int i = 0; i < tw->columnCount(); ++i) {
            if (tw->isColumnHidden(i)) continue;
            if (auto* item = tw->horizontalHeaderItem(i)) {
                const int linhas = item->text().count('\n') + 1;
                max_linhas = std::max(max_linhas, linhas);
            }
        }

        const int altura_linha = fm.height();
        const int altura_necessaria = max_linhas * altura_linha + 10;
        header->setMinimumHeight(altura_necessaria);
        header->setMaximumHeight(altura_necessaria);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em ajustar_altura_cabecalho", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

