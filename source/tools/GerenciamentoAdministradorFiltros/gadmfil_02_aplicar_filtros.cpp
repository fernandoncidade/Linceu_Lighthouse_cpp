#include "gadmfil_02_aplicar_filtros.h"

#include <algorithm>
#include <functional>

#include <QAbstractItemModel>
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QHash>
#include <QLineEdit>
#include <QTableView>
#include <QTableWidget>

#include "source/gui/ic_01_InterfaceMonitor.h"
#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_00_operacao_utils.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
namespace filtros_detail = source::tools::GerenciamentoAdministradorFiltros::detail;

QString item_text(QTableWidget* tabela, int row, int col) {
    if (!tabela || col < 0) return QString();
    auto* item = tabela->item(row, col);
    return item ? item->text() : QString();
}

QDateTime parse_data_evento(const QString& texto) {
    const QString s = texto.trimmed();
    if (s.isEmpty()) return {};

    QDateTime dt = QDateTime::fromString(s, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (!dt.isValid()) dt = QDateTime::fromString(s, QStringLiteral("dd/MM/yyyy HH:mm:ss"));
    if (!dt.isValid()) {
        const QString first = s.split(' ').value(0);
        dt = QDateTime::fromString(first, QStringLiteral("yyyy-MM-dd"));
    }
    return dt;
}

bool checkbox_checked(source::tools::Filtros* parent, const QString& objectName) {
    if (!parent) return false;
    auto* cb = parent->findChild<QCheckBox*>(objectName);
    return cb && cb->isChecked();
}

QString model_text(QAbstractItemModel* model, int row, int col) {
    if (model == nullptr || row < 0 || col < 0 || row >= model->rowCount() ||
        col >= model->columnCount()) {
        return {};
    }
    return model->index(row, col).data(Qt::DisplayRole).toString();
}

void popular_indices_por_cabecalho(QAbstractItemModel* model, QHash<QString, int>& indices) {
    if (model == nullptr) {
        return;
    }

    for (int col = 0; col < model->columnCount(); ++col) {
        const QString h = model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString().toLower();
        if (h.contains(QStringLiteral("operação")) || h.contains(QStringLiteral("operacao")) ||
            h.contains(QStringLiteral("event")) || h.contains(QStringLiteral("operation"))) {
            indices[QStringLiteral("tipo_operacao")] = col;
        } else if (h.contains(QStringLiteral("nome")) || h.contains(QStringLiteral("name"))) {
            indices[QStringLiteral("nome")] = col;
        } else if (h.contains(QStringLiteral("anterior")) || h.contains(QStringLiteral("previous"))) {
            indices[QStringLiteral("dir_anterior")] = col;
        } else if (h.contains(QStringLiteral("atual")) || h.contains(QStringLiteral("current"))) {
            indices[QStringLiteral("dir_atual")] = col;
        } else if (h.contains(QStringLiteral("modif"))) {
            indices[QStringLiteral("data_modificacao")] = col;
        } else if (h.contains(QStringLiteral("cria")) || h.contains(QStringLiteral("creat"))) {
            indices[QStringLiteral("data_criacao")] = col;
        }
    }
}

void popular_indices_por_cabecalho(QTableWidget* tabela, QHash<QString, int>& indices) {
    if (tabela == nullptr) {
        return;
    }

    for (int col = 0; col < tabela->columnCount(); ++col) {
        const QString h =
            tabela->horizontalHeaderItem(col) ? tabela->horizontalHeaderItem(col)->text().toLower()
                                              : QString();
        if (h.contains(QStringLiteral("operação")) || h.contains(QStringLiteral("operacao")) ||
            h.contains(QStringLiteral("event")) || h.contains(QStringLiteral("operation"))) {
            indices[QStringLiteral("tipo_operacao")] = col;
        } else if (h.contains(QStringLiteral("nome")) || h.contains(QStringLiteral("name"))) {
            indices[QStringLiteral("nome")] = col;
        } else if (h.contains(QStringLiteral("anterior")) || h.contains(QStringLiteral("previous"))) {
            indices[QStringLiteral("dir_anterior")] = col;
        } else if (h.contains(QStringLiteral("atual")) || h.contains(QStringLiteral("current"))) {
            indices[QStringLiteral("dir_atual")] = col;
        } else if (h.contains(QStringLiteral("modif"))) {
            indices[QStringLiteral("data_modificacao")] = col;
        } else if (h.contains(QStringLiteral("cria")) || h.contains(QStringLiteral("creat"))) {
            indices[QStringLiteral("data_criacao")] = col;
        }
    }
}

void garantir_indices_padrao(QHash<QString, int>& indices) {
    if (!indices.contains(QStringLiteral("tipo_operacao"))) indices[QStringLiteral("tipo_operacao")] = 0;
    if (!indices.contains(QStringLiteral("nome"))) indices[QStringLiteral("nome")] = 1;
    if (!indices.contains(QStringLiteral("dir_anterior"))) indices[QStringLiteral("dir_anterior")] = 2;
    if (!indices.contains(QStringLiteral("dir_atual"))) indices[QStringLiteral("dir_atual")] = 3;
}

QHash<QString, int> indices_tabela_widget(QTableWidget* tabela,
                                          source::services::GerenciadorColunas* gerenciador_colunas) {
    QHash<QString, int> indices;
    if (gerenciador_colunas != nullptr && !gerenciador_colunas->COLUNAS_DISPONIVEIS.isEmpty()) {
        QList<QPair<QString, QVariantMap>> colunas_visiveis;
        colunas_visiveis.reserve(gerenciador_colunas->COLUNAS_DISPONIVEIS.size());
        for (auto it = gerenciador_colunas->COLUNAS_DISPONIVEIS.constBegin();
             it != gerenciador_colunas->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            if (it.value().value("visivel").toBool()) {
                colunas_visiveis.append({it.key(), it.value()});
            }
        }

        std::sort(colunas_visiveis.begin(), colunas_visiveis.end(),
                  [](const QPair<QString, QVariantMap>& a, const QPair<QString, QVariantMap>& b) {
                      return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
                  });

        int idx = 0;
        for (const auto& col : colunas_visiveis) {
            indices[col.first] = idx;
            idx += 1;
        }
    } else {
        popular_indices_por_cabecalho(tabela, indices);
    }

    garantir_indices_padrao(indices);
    return indices;
}

QHash<QString, int> indices_tabela_view(QTableView* view) {
    QHash<QString, int> indices;
    auto* model = view != nullptr ? view->model() : nullptr;
    if (auto* event_model =
            qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(model)) {
        const QStringList keys = {
            QStringLiteral("tipo_operacao"),
            QStringLiteral("nome"),
            QStringLiteral("dir_anterior"),
            QStringLiteral("dir_atual"),
            QStringLiteral("data_modificacao"),
            QStringLiteral("data_criacao"),
        };
        for (const QString& key : keys) {
            const int index = event_model->column_index_for_key(key);
            if (index >= 0) {
                indices[key] = index;
            }
        }
    } else {
        popular_indices_por_cabecalho(model, indices);
    }

    garantir_indices_padrao(indices);
    return indices;
}

void aplicar_filtros_linhas(source::tools::AdministradorFiltros* self,
                            source::tools::Filtros* parent,
                            int row_count,
                            const QHash<QString, int>& indices,
                            const std::function<QString(int, int)>& text_at,
                            const std::function<void(int, bool)>& set_hidden) {
    if (self == nullptr || parent == nullptr || row_count <= 0 || !text_at || !set_hidden) {
        return;
    }

    auto* campo_busca = parent->findChild<QLineEdit*>(QStringLiteral("campo_busca"));
    auto* campo_ext = parent->findChild<QLineEdit*>(QStringLiteral("campo_extensao"));
    auto* data_inicial = parent->findChild<QDateTimeEdit*>(QStringLiteral("data_inicial"));
    auto* data_final = parent->findChild<QDateTimeEdit*>(QStringLiteral("data_final"));

    const QString texto_busca = campo_busca ? campo_busca->text().toLower() : QString();

    QStringList extensoes;
    if (campo_ext) {
        for (const QString& raw : campo_ext->text().split(',')) {
            const QString ext = raw.trimmed().toLower();
            if (!ext.isEmpty()) extensoes.append(ext);
        }
    }

    const QDateTime dt_ini = data_inicial ? data_inicial->dateTime() : QDateTime::fromSecsSinceEpoch(0);
    const QDateTime dt_fim = data_final ? data_final->dateTime() : QDateTime::currentDateTime();

    auto text_by_key = [&](int row, const QString& key, int fallback_col = -1) {
        return text_at(row, indices.value(key, fallback_col));
    };

    auto* loc = parent->localizador();
    for (int row = 0; row < row_count; ++row) {
        const QString tipo_op = text_by_key(row, QStringLiteral("tipo_operacao"), 0);
        const QString chave = filtros_detail::resolver_chave_operacao(loc, tipo_op);
        if (!chave.isEmpty() && self->contadores_originais_ref().contains(chave)) {
            self->contadores_originais_ref()[chave] += 1;
        }
    }

    for (int row = 0; row < row_count; ++row) {
        bool mostrar = true;
        const QString tipo_op = text_by_key(row, QStringLiteral("tipo_operacao"), 0);
        const QString chave = filtros_detail::resolver_chave_operacao(loc, tipo_op);

        if (!tipo_op.isEmpty() && !filtros_detail::operacao_permitida(parent, tipo_op)) mostrar = false;

        if (mostrar && !texto_busca.isEmpty()) {
            bool encontrado = false;
            for (const QString& campo : {QStringLiteral("nome"), QStringLiteral("dir_anterior"), QStringLiteral("dir_atual")}) {
                const QString txt = text_by_key(row, campo).toLower();
                if (!txt.isEmpty() && txt.contains(texto_busca)) {
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) mostrar = false;
        }

        if (mostrar && !extensoes.isEmpty()) {
            const QString nome = text_by_key(row, QStringLiteral("nome"), 1);
            const QString ext = nome.contains('.') ? nome.section('.', -1).toLower() : QString();
            if (!extensoes.contains(ext)) mostrar = false;
        }

        if (mostrar) {
            const bool ignorar_data =
                (chave == "op_moved" && checkbox_checked(parent, "ignorar_mover")) ||
                (chave == "op_renamed" && checkbox_checked(parent, "ignorar_renomeados")) ||
                (chave == "op_added" && checkbox_checked(parent, "ignorar_adicionados")) ||
                (chave == "op_deleted" && checkbox_checked(parent, "ignorar_excluidos")) ||
                (chave == "op_modified" && checkbox_checked(parent, "ignorar_data_modificados")) ||
                (chave == "op_scanned" && checkbox_checked(parent, "ignorar_escaneados"));

            if (!ignorar_data) {
                QString data_texto = text_by_key(row, QStringLiteral("data_modificacao"));
                if (data_texto.trimmed().isEmpty()) {
                    data_texto = text_by_key(row, QStringLiteral("data_criacao"));
                }

                if (!data_texto.trimmed().isEmpty()) {
                    const QDateTime data_evento = parse_data_evento(data_texto);
                    if (data_evento.isValid() && (data_evento < dt_ini || data_evento > dt_fim)) {
                        mostrar = false;
                    }
                }
            }
        }

        if (mostrar && !chave.isEmpty() && self->contadores_ref().contains(chave)) {
            self->contadores_ref()[chave] += 1;
        }

        set_hidden(row, !mostrar);
    }
}
}

namespace source::tools::GerenciamentoAdministradorFiltros {

void aplicar_filtros(source::tools::AdministradorFiltros* self) {
    try {
        if (!self || !self->parent_obj()) return;
        auto* parent = self->parent_obj();

        self->salvar_estado_checkboxes();

        for (auto it = self->contadores_ref().begin(); it != self->contadores_ref().end(); ++it) it.value() = 0;
        for (auto it = self->contadores_originais_ref().begin(); it != self->contadores_originais_ref().end(); ++it) it.value() = 0;

        source::services::GerenciadorColunas* gerenciador_colunas = nullptr;
        for (QWidget* widget : QApplication::topLevelWidgets()) {
            if (auto* monitor = qobject_cast<source::gui::InterfaceMonitor*>(widget);
                monitor != nullptr && monitor->gerenciador_colunas != nullptr) {
                gerenciador_colunas = monitor->gerenciador_colunas;
                break;
            }
        }

        if (auto* tabela = qobject_cast<QTableWidget*>(parent->tabela_dados())) {
            const QHash<QString, int> indices = indices_tabela_widget(tabela, gerenciador_colunas);
            aplicar_filtros_linhas(
                self,
                parent,
                tabela->rowCount(),
                indices,
                [tabela](int row, int col) { return item_text(tabela, row, col); },
                [tabela](int row, bool hidden) { tabela->setRowHidden(row, hidden); });
        } else if (auto* view = qobject_cast<QTableView*>(parent->tabela_dados())) {
            QAbstractItemModel* model = view->model();
            if (model == nullptr) {
                return;
            }
            const QHash<QString, int> indices = indices_tabela_view(view);
            aplicar_filtros_linhas(
                self,
                parent,
                model->rowCount(),
                indices,
                [model](int row, int col) { return model_text(model, row, col); },
                [view](int row, bool hidden) { view->setRowHidden(row, hidden); });
            if (view->viewport() != nullptr) {
                view->viewport()->update();
            }
        } else {
            return;
        }

        self->sincronizar_menu_principal_com_filtros();
        emit parent->filtroAplicado();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao aplicar filtros", true);
        if (self && self->parent_obj()) {
            if (auto* tabela = qobject_cast<QTableWidget*>(self->parent_obj()->tabela_dados())) {
                for (int row = 0; row < tabela->rowCount(); ++row) tabela->setRowHidden(row, false);
            } else if (auto* view = qobject_cast<QTableView*>(self->parent_obj()->tabela_dados())) {
                const int total = view->model() != nullptr ? view->model()->rowCount() : 0;
                for (int row = 0; row < total; ++row) view->setRowHidden(row, false);
            }
        }
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
