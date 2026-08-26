#include "gtab_04_AjustarLargurasColunas.h"

#include <QFontMetrics>
#include <QHeaderView>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void ajustar_larguras_colunas(source::services::GerenciadorTabela* self, QObject* tabela_dados, const QVariantList& colunas_visiveis) {
    try {
        if (!self) return;
        auto* tabela = qobject_cast<QTableWidget*>(tabela_dados);
        if (!tabela) return;

        auto* header = tabela->horizontalHeader();
        header->setMinimumSectionSize(10);
        const QFontMetrics fm(header->font());

        constexpr int PADDING = 40;
        constexpr int MIN_WIDTH = 10;

        for (int i = 0; i < colunas_visiveis.size(); ++i) {
            const QVariantMap packed = colunas_visiveis[i].toMap();
            const QVariantMap coluna = packed.value("coluna").toMap();
            const QString texto_ref = self->texto_original_cabecalhos.value(QString::number(i), coluna.value("nome").toString());
            const QString texto = texto_ref;
            const QString texto_sem_quebra = QString(texto).replace('\n', ' ');

            const int texto_largura = fm.horizontalAdvance(texto_sem_quebra);
            const int largura_ideal = std::max(MIN_WIDTH, texto_largura + PADDING);

            tabela->setColumnWidth(i, largura_ideal);
            header->setSectionResizeMode(i, QHeaderView::Interactive);

            if (auto* item = tabela->horizontalHeaderItem(i)) {
                const QString limpo = texto_sem_quebra;
                item->setText(limpo);
                item->setTextAlignment(Qt::AlignCenter);
                item->setToolTip(limpo);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em ajustar_larguras_colunas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

