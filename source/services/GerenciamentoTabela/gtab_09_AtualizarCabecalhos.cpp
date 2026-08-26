#include "gtab_09_AtualizarCabecalhos.h"

#include <algorithm>

#include <QHeaderView>
#include <QTableView>
#include <QTableWidget>

#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void atualizar_cabecalhos(source::services::GerenciadorTabela* self) {
    try {
        if (!self || !self->interface) return;
        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        auto* tabela = qobject_cast<QTableWidget*>(tabela_obj);
        auto* tabela_view = qobject_cast<QTableView*>(tabela_obj);
        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        if ((!tabela && !tabela_view) || !gc) return;

        if (tabela_view != nullptr) {
            if (auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(tabela_view->model())) {
                model->refresh_headers();
                if (auto* header = tabela_view->horizontalHeader()) {
                    header->viewport()->update();
                }
                tabela_view->viewport()->update();
            }
            self->_invalidar_cache_cores();
            return;
        }

        QList<QPair<QString, QVariantMap>> colunas;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            colunas.append({it.key(), it.value()});
        }
        std::sort(colunas.begin(), colunas.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        if (tabela->columnCount() != colunas.size()) {
            if (self->interface &&
                (self->interface->property("_traducao_em_execucao").toBool() ||
                 self->interface->property("_trocando_idioma").toBool()) &&
                tabela->rowCount() > 0) {
                source::utils::LogManager::get_logger().warning(
                    "Reconfiguração estrutural da tabela adiada durante retradução de idioma.");
                return;
            }
            self->configurar_tabela(tabela);
            return;
        }

        QStringList headers;
        self->texto_original_cabecalhos.clear();
        QVariantList packed;
        for (int i = 0; i < colunas.size(); ++i) {
            const QString texto = colunas[i].second.value("nome", colunas[i].first).toString();
            self->texto_original_cabecalhos.insert(QString::number(i), texto);
            headers << texto;

            QVariantMap p;
            p.insert("key", colunas[i].first);
            p.insert("coluna", colunas[i].second);
            packed.append(p);
        }

        tabela->setHorizontalHeaderLabels(headers);
        self->cache_indices_colunas.clear();

        QHeaderView* header = tabela->horizontalHeader();
        for (int i = 0; i < tabela->columnCount(); ++i) {
            if (auto* item = tabela->horizontalHeaderItem(i)) {
                const QString texto = self->texto_original_cabecalhos.value(QString::number(i), item->text()).replace('\n', ' ');
                item->setText(texto);
                item->setToolTip(texto);
            }
        }

        self->setProperty("_suspender_quebra_cabecalho", true);
        self->ajustar_larguras_colunas(tabela, packed);
        self->ajustar_altura_cabecalho(tabela);
        self->setProperty("_suspender_quebra_cabecalho", false);

        tabela->viewport()->update();
        self->_invalidar_cache_cores();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_cabecalhos", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
