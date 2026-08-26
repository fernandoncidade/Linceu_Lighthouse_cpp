#include "gtab_13_AtualizarVisibilidadeColunas.h"

#include <algorithm>

#include <QTableView>
#include <QTableWidget>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_visibilidade_colunas(source::services::GerenciadorTabela* self, bool atualizar_em_massa) {
    try {
        Q_UNUSED(atualizar_em_massa)
        if (!self || !self->interface) return;

        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        auto* tabela = qobject_cast<QTableWidget*>(tabela_obj);
        auto* tabela_view = qobject_cast<QTableView*>(tabela_obj);
        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        if ((!tabela && !tabela_view) || !gc) return;

        QList<QPair<QString, QVariantMap>> colunas;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            colunas.append({it.key(), it.value()});
        }
        std::sort(colunas.begin(), colunas.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        if (tabela != nullptr) {
            if (tabela->columnCount() != colunas.size()) {
                self->configurar_tabela(tabela);
            }

            tabela->setUpdatesEnabled(false);
            tabela->blockSignals(true);
            for (int i = 0; i < colunas.size(); ++i) {
                tabela->setColumnHidden(i, !colunas[i].second.value("visivel", true).toBool());
            }
            tabela->blockSignals(false);
            tabela->setUpdatesEnabled(true);

            self->aplicar_quebra_linha_todos_cabecalhos(tabela);
            self->ajustar_altura_cabecalho(tabela);
            self->_invalidar_cache_cores();
            tabela->viewport()->update();
            return;
        }

        if (tabela_view != nullptr) {
            if (tabela_view->model() == nullptr) return;

            const int total_colunas_cfg = static_cast<int>(colunas.size());
            const int max_colunas = std::min(total_colunas_cfg, tabela_view->model()->columnCount());
            tabela_view->setUpdatesEnabled(false);
            tabela_view->blockSignals(true);
            for (int i = 0; i < max_colunas; ++i) {
                tabela_view->setColumnHidden(i, !colunas[i].second.value("visivel", true).toBool());
            }
            tabela_view->blockSignals(false);
            tabela_view->setUpdatesEnabled(true);
            tabela_view->viewport()->update();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_visibilidade_colunas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

