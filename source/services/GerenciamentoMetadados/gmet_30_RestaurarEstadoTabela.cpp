#include "gmet_30_RestaurarEstadoTabela.h"

#include <QColor>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void restaurar_estado_tabela(source::services::GerenciadorColunas* gc, QObject* tabela, const QList<QVariantMap>& eventos) {
    try {
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(gc ? gc->loc : nullptr);
        if (!tw || !gc || !loc) return;

        tw->clearContents();
        tw->setRowCount(eventos.size());
        for (int row = 0; row < eventos.size(); ++row) {
            const QVariantMap& evento = eventos[row];
            int col = 0;
            for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd() && col < tw->columnCount(); ++it) {
                const QString key = it.key();
                const QString nome_coluna = it.value().value("nome").toString();
                const QString valor = evento.value(nome_coluna).toString();
                const QString valor_exibicao =
                    (key == QStringLiteral("tipo_operacao")) ? loc->traduzir_tipo_operacao(valor) : valor;
                auto* item = new QTableWidgetItem(valor_exibicao);
                if (key == QStringLiteral("tipo_operacao")) {
                    if (valor_exibicao == loc->get_text("op_renamed")) item->setBackground(QColor(0, 255, 0));
                    else if (valor_exibicao == loc->get_text("op_added")) item->setBackground(QColor(0, 0, 255));
                    else if (valor_exibicao == loc->get_text("op_deleted")) item->setBackground(QColor(255, 0, 0));
                    else if (valor_exibicao == loc->get_text("op_modified")) item->setBackground(QColor(255, 98, 0));
                    else if (valor_exibicao == loc->get_text("op_moved")) item->setBackground(QColor(255, 0, 255));
                    else if (valor_exibicao == loc->get_text("op_scanned")) item->setBackground(QColor(128, 128, 128));
                }
                tw->setItem(row, col, item);
                ++col;
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao restaurar estado da tabela", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
