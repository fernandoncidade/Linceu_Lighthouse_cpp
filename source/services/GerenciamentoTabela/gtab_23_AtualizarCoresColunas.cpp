#include "gtab_23_AtualizarCoresColunas.h"

#include <QApplication>
#include <QHeaderView>
#include <QPainter>
#include <QPalette>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QTableWidget>

#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {
namespace {

QString normalizar_operacao(QString valor) {
    valor = valor.normalized(QString::NormalizationForm_D).toCaseFolded().trimmed();
    QString out;
    out.reserve(valor.size());
    for (const QChar ch : valor) {
        if (ch.category() == QChar::Mark_NonSpacing ||
            ch.category() == QChar::Mark_SpacingCombining ||
            ch.category() == QChar::Mark_Enclosing) {
            continue;
        }
        out.append(ch);
    }
    return out.simplified();
}

QColor cor_hover_suave(const QColor& base, const QPalette& palette, bool tema_escuro) {
    QColor c = base.isValid() ? base : palette.color(QPalette::Base);
    if (!c.isValid() || c == QColor()) {
        c = palette.color(QPalette::Base);
    }
    if (!c.isValid()) {
        c = QColor(240, 240, 240);
    }
    if (c.alpha() == 0) {
        c.setAlpha(255);
    }

    QColor hover = tema_escuro ? c.lighter(128) : c.lighter(112);
    if (!tema_escuro && hover.lightness() >= 248) {
        // Em temas claros, evita "sumir" no branco usando leve matiz do highlight.
        const QColor h = palette.color(QPalette::Highlight).lighter(195);
        hover.setRed((hover.red() * 8 + h.red() * 2) / 10);
        hover.setGreen((hover.green() * 8 + h.green() * 2) / 10);
        hover.setBlue((hover.blue() * 8 + h.blue() * 2) / 10);
    }
    return hover;
}

class DelegateCorEstadoVisual final : public QStyledItemDelegate {
public:
    DelegateCorEstadoVisual(source::services::GerenciadorTabela* self, QTableWidget* tabela)
        : QStyledItemDelegate(tabela), self_(self), tabela_(tabela) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (self_ == nullptr || tabela_ == nullptr) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);

        const bool hovered = (option.state & QStyle::State_MouseOver) && !(option.state & QStyle::State_Selected);
        const bool tema_escuro =
            self_ != nullptr && self_->detectar_tema_windows() == QStringLiteral("escuro");
        const QColor texto_padrao =
            self_ != nullptr ? self_->calcular_cor_texto_ideal(QColor(), false)
                             : QApplication::palette().color(QPalette::Text);
        QColor cor_fundo_aplicada;
        bool pintar_fundo = false;
        opt.backgroundBrush = QBrush(Qt::NoBrush);
        opt.palette.setBrush(QPalette::Text, QBrush(texto_padrao));
        opt.palette.setBrush(QPalette::WindowText, QBrush(texto_padrao));
        opt.palette.setBrush(QPalette::HighlightedText, QBrush(texto_padrao));

        const QVariantMap colunas_por_indice = tabela_->property("_gtab_col_key_by_index").toMap();
        const QString coluna_key = colunas_por_indice.value(QString::number(index.column())).toString();
        if (!coluna_key.isEmpty() && self_->cores_visiveis) {
            const bool eh_coluna_tipo = (coluna_key == QStringLiteral("tipo_operacao"));
            const bool coluna_ativa = eh_coluna_tipo || self_->colunas_para_colorir.contains(coluna_key);
            if (coluna_ativa) {
                QString tipo_operacao = index.data(Qt::DisplayRole).toString();
                if (!eh_coluna_tipo) {
                    const int idx_tipo = tabela_->property("_gtab_tipo_col_index").toInt();
                    if (idx_tipo >= 0) {
                        tipo_operacao = index.sibling(index.row(), idx_tipo).data(Qt::DisplayRole).toString();
                    }
                }

                if (self_->cache_cores.isEmpty()) {
                    self_->_obter_cores_operacao();
                }

                QColor cor_fundo = self_->cache_cores.value(tipo_operacao).value<QColor>();
                if (!cor_fundo.isValid()) {
                    cor_fundo = self_->cache_cores.value(tipo_operacao.toCaseFolded()).value<QColor>();
                }
                if (!cor_fundo.isValid()) {
                    cor_fundo = self_->cache_cores.value(normalizar_operacao(tipo_operacao)).value<QColor>();
                }
                if (!cor_fundo.isValid()) {
                    cor_fundo = self_->cache_cores.value(QStringLiteral("op_scanned")).value<QColor>();
                }
                if (!cor_fundo.isValid()) {
                    cor_fundo = QColor(128, 128, 128);
                }

                const bool eh_personalizada =
                    eh_coluna_tipo ? self_->eh_coluna_personalizada_colorida(coluna_key) : true;
                const QColor cor_texto = self_->calcular_cor_texto_ideal(cor_fundo, eh_personalizada);
                cor_fundo_aplicada = cor_fundo;
                pintar_fundo = cor_fundo_aplicada.isValid();
                opt.palette.setBrush(QPalette::Text, QBrush(cor_texto));
                opt.palette.setBrush(QPalette::WindowText, QBrush(cor_texto));
                opt.palette.setBrush(QPalette::HighlightedText, QBrush(cor_texto));
            }
        }

        if (hovered) {
            const QColor base_hover = pintar_fundo
                                          ? cor_fundo_aplicada
                                          : ((tabela_->alternatingRowColors() && (index.row() % 2 != 0))
                                                 ? option.palette.color(QPalette::AlternateBase)
                                                 : option.palette.color(QPalette::Base));
            const QColor hover = cor_hover_suave(base_hover, option.palette, tema_escuro);
            painter->save();
            painter->fillRect(option.rect, hover);
            painter->restore();
        } else if (pintar_fundo) {
            painter->save();
            painter->fillRect(option.rect, cor_fundo_aplicada);
            painter->restore();
        }

        // Mantém a cor da fonte estável no hover; o fundo já é tratado acima.
        opt.state &= ~QStyle::State_MouseOver;
        opt.backgroundBrush = QBrush(Qt::NoBrush);
        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    source::services::GerenciadorTabela* self_;
    QTableWidget* tabela_;
};

class DelegateCorEstadoVisualView final : public QStyledItemDelegate {
public:
    DelegateCorEstadoVisualView(source::services::GerenciadorTabela* self,
                                QTableView* view,
                                source::services::GerenciamentoTabela::EventTableModel* model)
        : QStyledItemDelegate(view), self_(self), view_(view), model_(model) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (self_ == nullptr || view_ == nullptr || model_ == nullptr) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, index);
        const bool hovered = (option.state & QStyle::State_MouseOver) && !(option.state & QStyle::State_Selected);
        const bool tema_escuro =
            self_ != nullptr && self_->detectar_tema_windows() == QStringLiteral("escuro");

        QColor cor_fundo_aplicada;
        const QVariant cor_fundo_var = index.data(Qt::BackgroundRole);
        if (cor_fundo_var.canConvert<QBrush>()) {
            cor_fundo_aplicada = cor_fundo_var.value<QBrush>().color();
        } else if (cor_fundo_var.canConvert<QColor>()) {
            cor_fundo_aplicada = cor_fundo_var.value<QColor>();
        }

        const QVariant cor_texto_var = index.data(Qt::ForegroundRole);
        QColor cor_texto_aplicada;
        if (cor_texto_var.canConvert<QBrush>()) {
            cor_texto_aplicada = cor_texto_var.value<QBrush>().color();
        } else if (cor_texto_var.canConvert<QColor>()) {
            cor_texto_aplicada = cor_texto_var.value<QColor>();
        }

        if (cor_texto_aplicada.isValid()) {
            opt.palette.setBrush(QPalette::Text, QBrush(cor_texto_aplicada));
            opt.palette.setBrush(QPalette::WindowText, QBrush(cor_texto_aplicada));
            opt.palette.setBrush(QPalette::HighlightedText, QBrush(cor_texto_aplicada));
        }

        if (hovered) {
            const QColor base_hover = cor_fundo_aplicada.isValid()
                                          ? cor_fundo_aplicada
                                          : ((view_->alternatingRowColors() && (index.row() % 2 != 0))
                                                 ? option.palette.color(QPalette::AlternateBase)
                                                 : option.palette.color(QPalette::Base));
            const QColor hover = cor_hover_suave(base_hover, option.palette, tema_escuro);
            painter->save();
            painter->fillRect(option.rect, hover);
            painter->restore();
        } else if (cor_fundo_aplicada.isValid()) {
            painter->save();
            painter->fillRect(option.rect, cor_fundo_aplicada);
            painter->restore();
        }

        // Evita que o estilo nativo altere texto no hover.
        opt.state &= ~QStyle::State_MouseOver;
        opt.backgroundBrush = QBrush(Qt::NoBrush);
        QStyledItemDelegate::paint(painter, opt, index);
    }

private:
    source::services::GerenciadorTabela* self_;
    QTableView* view_;
    source::services::GerenciamentoTabela::EventTableModel* model_;
};

void garantir_delegate_estado_visual(source::services::GerenciadorTabela* self, QTableWidget* tabela) {
    if (self == nullptr || tabela == nullptr) {
        return;
    }

    auto* delegate_atual = dynamic_cast<DelegateCorEstadoVisual*>(tabela->itemDelegate());
    if (delegate_atual != nullptr) {
        return;
    }

    tabela->setItemDelegate(new DelegateCorEstadoVisual(self, tabela));
}

void garantir_delegate_estado_visual_view(source::services::GerenciadorTabela* self,
                                          QTableView* tabela_view,
                                          source::services::GerenciamentoTabela::EventTableModel* model) {
    if (self == nullptr || tabela_view == nullptr || model == nullptr) {
        return;
    }

    auto* delegate_atual = dynamic_cast<DelegateCorEstadoVisualView*>(tabela_view->itemDelegate());
    if (delegate_atual != nullptr) {
        return;
    }

    tabela_view->setItemDelegate(new DelegateCorEstadoVisualView(self, tabela_view, model));
}

}  // namespace

void atualizar_cores_colunas(source::services::GerenciadorTabela* self,
                             bool aplicar_em_massa,
                             const QSet<QString>& colunas_afetadas) {
    try {
        Q_UNUSED(aplicar_em_massa)
        if (!self || !self->interface) return;
        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        self->_invalidar_cache_cores();

        if (auto* tabela = qobject_cast<QTableWidget*>(tabela_obj)) {
            garantir_delegate_estado_visual(self, tabela);
            tabela->viewport()->update();
            self->atualizacao_pendente = true;
            return;
        }

        auto* tabela_view = qobject_cast<QTableView*>(tabela_obj);
        if (tabela_view == nullptr || tabela_view->model() == nullptr) {
            return;
        }

        if (auto* model = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(tabela_view->model())) {
            garantir_delegate_estado_visual_view(self, tabela_view, model);
            model->notify_coloring_changed(colunas_afetadas);
            tabela_view->viewport()->update();
            if (auto* header = tabela_view->horizontalHeader()) {
                header->viewport()->update();
            }
        } else {
            tabela_view->viewport()->update();
        }
        self->atualizacao_pendente = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_cores_colunas", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela
