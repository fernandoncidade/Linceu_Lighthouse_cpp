#include "gtab_05_AplicarQuebraLinhaCabecalho.h"

#include <QFontMetrics>
#include <QHeaderView>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void aplicar_quebra_linha_cabecalho(source::services::GerenciadorTabela* self, QObject* tabela, int coluna_index) {
    try {
        if (!self) return;
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        if (!tw) return;

        const QString idx = QString::number(coluna_index);
        if (!self->texto_original_cabecalhos.contains(idx)) return;

        const QString texto_original = self->texto_original_cabecalhos.value(idx);
        const int largura_coluna = tw->columnWidth(coluna_index);
        const QFontMetrics fm(tw->horizontalHeader()->font());

        const int padding = 10;
        const int espaco_disponivel = std::max(1, largura_coluna - padding);
        const int largura_texto = fm.horizontalAdvance(texto_original);

        auto* item = tw->horizontalHeaderItem(coluna_index);
        if (!item) return;

        if (largura_texto <= espaco_disponivel) {
            item->setText(texto_original);
            item->setTextAlignment(Qt::AlignCenter);
            return;
        }

        const int char_width = std::max(1, fm.averageCharWidth());
        const int max_chars = std::max(1, espaco_disponivel / char_width);

        QStringList palavras = texto_original.split(' ', Qt::SkipEmptyParts);
        QStringList linhas;
        QString atual;
        for (const QString& p : palavras) {
            const QString candidato = atual.isEmpty() ? p : (atual + QStringLiteral(" ") + p);
            if (candidato.size() <= max_chars || atual.isEmpty()) {
                atual = candidato;
            } else {
                linhas << atual;
                atual = p;
            }
        }
        if (!atual.isEmpty()) linhas << atual;
        if (linhas.isEmpty()) linhas << texto_original;

        item->setText(linhas.join(QStringLiteral("\n")));
        item->setTextAlignment(Qt::AlignCenter);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em aplicar_quebra_linha_cabecalho", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

