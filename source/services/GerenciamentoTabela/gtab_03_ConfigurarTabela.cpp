#include "gtab_03_ConfigurarTabela.h"

#include <algorithm>

#include <QAbstractItemView>
#include <QApplication>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMetaObject>
#include <QPushButton>
#include <QTableWidget>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void configurar_tabela(source::services::GerenciadorTabela* self, QObject* tabela_dados) {
    try {
        if (!self) return;
        auto* tabela = qobject_cast<QTableWidget*>(tabela_dados);
        if (!tabela && self->interface) {
            tabela = qobject_cast<QTableWidget*>(self->interface->property("tabela_dados").value<QObject*>());
        }
        if (!tabela) return;
        if (self->interface &&
            (self->interface->property("_traducao_em_execucao").toBool() ||
             self->interface->property("_trocando_idioma").toBool()) &&
            tabela->rowCount() > 0 && tabela->columnCount() > 0) {
            // Evita reset estrutural da tabela enquanto a retradução está em andamento.
            return;
        }

        tabela->setAlternatingRowColors(true);
        tabela->setSortingEnabled(self->ordenacao_habilitada);
        tabela->setSelectionBehavior(QAbstractItemView::SelectItems);
        tabela->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tabela->setCornerButtonEnabled(true);

        auto selecionar_todas = [tabela]() {
            tabela->blockSignals(true);
            tabela->selectAll();
            tabela->blockSignals(false);
        };
        auto selecionar_coluna = [tabela](int logical_index) {
            if (!tabela || logical_index < 0 || logical_index >= tabela->columnCount()) {
                return;
            }

            auto* sm = tabela->selectionModel();
            if (!sm) {
                tabela->blockSignals(true);
                tabela->selectColumn(logical_index);
                tabela->blockSignals(false);
                return;
            }

            const Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
            const bool ctrl_pressed = mods.testFlag(Qt::ControlModifier);
            const bool shift_pressed = mods.testFlag(Qt::ShiftModifier);
            if (tabela->isSortingEnabled() && !ctrl_pressed && !shift_pressed) {
                return;
            }

            const int row_count = tabela->rowCount();
            if (row_count <= 0) {
                return;
            }

            const QModelIndex top_left = tabela->model()->index(0, logical_index);
            const QModelIndex bottom_right = tabela->model()->index(row_count - 1, logical_index);
            QItemSelection column_selection(top_left, bottom_right);

            auto* header = tabela->horizontalHeader();
            const int anchor_column = (header != nullptr)
                                          ? header->property("_header_anchor_column").toInt()
                                          : -1;

            tabela->blockSignals(true);
            if (shift_pressed) {
                const int effective_anchor = (anchor_column >= 0 && anchor_column < tabela->columnCount())
                                                 ? anchor_column
                                                 : logical_index;
                const int start_col = std::min(effective_anchor, logical_index);
                const int end_col = std::max(effective_anchor, logical_index);

                const QModelIndex range_top_left = tabela->model()->index(0, start_col);
                const QModelIndex range_bottom_right = tabela->model()->index(row_count - 1, end_col);
                const QItemSelection range_selection(range_top_left, range_bottom_right);

                const QItemSelectionModel::SelectionFlags flags =
                    (ctrl_pressed ? QItemSelectionModel::Select : QItemSelectionModel::ClearAndSelect) |
                    QItemSelectionModel::Columns;
                sm->select(range_selection, flags);
            } else if (ctrl_pressed) {
                // Ctrl+Click toggles only this column while preserving existing selection.
                if (sm->isColumnSelected(logical_index, QModelIndex())) {
                    sm->select(column_selection,
                               QItemSelectionModel::Deselect | QItemSelectionModel::Columns);
                } else {
                    sm->select(column_selection,
                               QItemSelectionModel::Select | QItemSelectionModel::Columns);
                }
            } else {
                sm->select(column_selection,
                           QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Columns);
            }

            if (!shift_pressed && header != nullptr) {
                header->setProperty("_header_anchor_column", logical_index);
            } else if (shift_pressed && header != nullptr && anchor_column < 0) {
                header->setProperty("_header_anchor_column", logical_index);
            }

            tabela->blockSignals(false);
        };

        if (auto* corner_button = tabela->findChild<QPushButton*>()) {
            if (!corner_button->property("_gtab_corner_connected").toBool()) {
                QObject::connect(corner_button, &QPushButton::clicked, tabela, selecionar_todas);
                corner_button->setProperty("_gtab_corner_connected", true);
            }
        }

        tabela->setStyleSheet(QStringLiteral(
            "QTableWidget::item:hover { background-color: palette(alternate-base); }"
            "QTableWidget { selection-background-color: transparent; selection-color: palette(text); }"
            "QTableWidget::item:selected { background-color: transparent; color: palette(text); border: 1px solid palette(highlight);"
            "border-radius: 3px; padding: 1px; }"
            "QTableCornerButton::section { background-color: palette(button); border: 1px solid palette(mid); }"
            "QTableCornerButton::section:pressed { background-color: palette(highlight); }"));

        tabela->setPalette(QApplication::palette());

        QHeaderView* header = tabela->horizontalHeader();
        header->setSectionsMovable(true);
        header->setHighlightSections(true);
        header->setDefaultAlignment(Qt::AlignCenter);
        header->setTextElideMode(Qt::ElideNone);
        header->setStretchLastSection(false);
        QFont f = header->font();
        f.setBold(true);
        header->setFont(f);

        if (!header->property("_gtab_header_connected").toBool()) {
            QObject::connect(header, &QHeaderView::sectionClicked, tabela, selecionar_coluna);
            QObject::connect(header, &QHeaderView::sectionResized, self,
                             [self](int logicalIndex, int oldSize, int newSize) {
                                 self->redimensionar_cabecalho(logicalIndex, oldSize, newSize);
                             });
            header->setProperty("_gtab_header_connected", true);
        }

        auto* gc = self->interface ? qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>()) : nullptr;
        if (!gc) return;

        QList<QPair<QString, QVariantMap>> colunas;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            colunas.append({it.key(), it.value()});
        }
        std::sort(colunas.begin(), colunas.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        tabela->setColumnCount(colunas.size());
        QStringList headers;
        self->texto_original_cabecalhos.clear();
        QVariantList colunas_variants;
        QVariantMap colunas_por_indice;
        int idx_tipo_operacao = -1;
        for (int i = 0; i < colunas.size(); ++i) {
            const QString key = colunas[i].first;
            const QVariantMap col = colunas[i].second;
            const QString texto = col.value("nome", key).toString();
            self->texto_original_cabecalhos.insert(QString::number(i), texto);
            headers << texto;
            tabela->setColumnHidden(i, !col.value("visivel", true).toBool());
            colunas_por_indice.insert(QString::number(i), key);
            if (key == QStringLiteral("tipo_operacao")) {
                idx_tipo_operacao = i;
            }

            QVariantMap packed;
            packed.insert("key", key);
            packed.insert("coluna", col);
            colunas_variants.append(packed);
        }
        tabela->setProperty("_gtab_col_key_by_index", colunas_por_indice);
        tabela->setProperty("_gtab_tipo_col_index", idx_tipo_operacao);

        tabela->setHorizontalHeaderLabels(headers);
        self->ajustar_larguras_colunas(tabela, colunas_variants);
        self->aplicar_quebra_linha_todos_cabecalhos(tabela);
        self->_invalidar_cache_cores();
        self->atualizar_cores_colunas(false);
        self->atualizar_dados_tabela(tabela);
        tabela->viewport()->update();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em configurar_tabela", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
