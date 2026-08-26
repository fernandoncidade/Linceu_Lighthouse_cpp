#include "gtab_33_AtualizarEstiloTema.h"

#include <algorithm>
#include <QApplication>
#include <QHeaderView>
#include <QPalette>
#include <QPointer>
#include <QPushButton>
#include <QScrollBar>
#include <QStyle>
#include <QTableView>
#include <QTableWidget>
#include <QTimer>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {
namespace {

QString normalizar_tema(const QString& novo_tema, source::services::GerenciadorTabela* self) {
    QString tema = novo_tema.trimmed().toLower();
    if (tema != QStringLiteral("escuro") && tema != QStringLiteral("claro")) {
        tema = self ? self->detectar_tema_windows() : QStringLiteral("claro");
    }
    if (tema != QStringLiteral("escuro") && tema != QStringLiteral("claro")) {
        tema = QStringLiteral("claro");
    }
    return tema;
}

QPalette criar_palette_tabela(const QString& tema) {
    QPalette pal;

    auto set_group = [](QPalette& p,
                        QPalette::ColorGroup g,
                        const QColor& window,
                        const QColor& base,
                        const QColor& alternate_base,
                        const QColor& text,
                        const QColor& button,
                        const QColor& button_text,
                        const QColor& mid,
                        const QColor& dark,
                        const QColor& light,
                        const QColor& shadow,
                        const QColor& highlight,
                        const QColor& highlighted_text) {
        p.setColor(g, QPalette::Window, window);
        p.setColor(g, QPalette::Base, base);
        p.setColor(g, QPalette::AlternateBase, alternate_base);
        p.setColor(g, QPalette::Text, text);
        p.setColor(g, QPalette::WindowText, text);
        p.setColor(g, QPalette::Button, button);
        p.setColor(g, QPalette::ButtonText, button_text);
        p.setColor(g, QPalette::Mid, mid);
        p.setColor(g, QPalette::Dark, dark);
        p.setColor(g, QPalette::Light, light);
        p.setColor(g, QPalette::Shadow, shadow);
        p.setColor(g, QPalette::Highlight, highlight);
        p.setColor(g, QPalette::HighlightedText, highlighted_text);
        p.setColor(g, QPalette::ToolTipBase, base);
        p.setColor(g, QPalette::ToolTipText, text);
    };

    if (tema == QStringLiteral("escuro")) {
        const QColor window(30, 30, 32);
        const QColor base(34, 34, 36);
        const QColor alt(44, 44, 46);
        const QColor text(235, 235, 235);
        const QColor button(48, 48, 52);
        const QColor button_text(235, 235, 235);
        const QColor mid(96, 96, 102);
        const QColor dark(22, 22, 24);
        const QColor light(78, 78, 84);
        const QColor shadow(12, 12, 14);
        const QColor highlight(82, 133, 255);
        const QColor highlighted_text(255, 255, 255);
        set_group(pal, QPalette::Active, window, base, alt, text, button, button_text, mid, dark, light,
                  shadow, highlight, highlighted_text);
        set_group(pal, QPalette::Inactive, window, base, alt, text, button, button_text, mid, dark, light,
                  shadow, highlight, highlighted_text);
        set_group(pal, QPalette::Disabled, window, base, alt, QColor(145, 145, 150), button,
                  QColor(145, 145, 150), mid, dark, light, shadow, QColor(92, 92, 96), QColor(210, 210, 210));
    } else {
        const QColor window(250, 250, 250);
        const QColor base(255, 255, 255);
        const QColor alt(247, 247, 247);
        const QColor text(20, 20, 20);
        const QColor button(243, 243, 243);
        const QColor button_text(20, 20, 20);
        const QColor mid(191, 195, 201);
        const QColor dark(159, 165, 173);
        const QColor light(255, 255, 255);
        const QColor shadow(115, 121, 129);
        const QColor highlight(40, 120, 220);
        const QColor highlighted_text(255, 255, 255);
        set_group(pal, QPalette::Active, window, base, alt, text, button, button_text, mid, dark, light,
                  shadow, highlight, highlighted_text);
        set_group(pal, QPalette::Inactive, window, base, alt, text, button, button_text, mid, dark, light,
                  shadow, highlight, highlighted_text);
        set_group(pal, QPalette::Disabled, window, base, alt, QColor(120, 120, 120), button,
                  QColor(120, 120, 120), mid, dark, light, shadow, QColor(145, 170, 210), QColor(245, 245, 245));
    }
    return pal;
}

void repolish_widget(QWidget* w) {
    if (w == nullptr) return;
    if (QStyle* st = w->style()) {
        st->unpolish(w);
        st->polish(w);
    }
    w->update();
}

void aplicar_palette_widget(QWidget* w, const QPalette& pal) {
    if (w == nullptr) return;
    w->setPalette(pal);
    repolish_widget(w);
}

void aplicar_palette_tabela_view(QTableView* tabela_view, const QString& tema) {
    if (tabela_view == nullptr) return;
    const QPalette pal = criar_palette_tabela(tema);
    aplicar_palette_widget(tabela_view, pal);
    aplicar_palette_widget(tabela_view->viewport(), pal);
    if (auto* hh = tabela_view->horizontalHeader()) {
        aplicar_palette_widget(hh, pal);
        aplicar_palette_widget(hh->viewport(), pal);
    }
    if (auto* vh = tabela_view->verticalHeader()) {
        aplicar_palette_widget(vh, pal);
        aplicar_palette_widget(vh->viewport(), pal);
    }
    if (auto* sbh = tabela_view->horizontalScrollBar()) {
        aplicar_palette_widget(sbh, pal);
    }
    if (auto* sbv = tabela_view->verticalScrollBar()) {
        aplicar_palette_widget(sbv, pal);
    }
    if (auto* corner = tabela_view->findChild<QPushButton*>()) {
        aplicar_palette_widget(corner, pal);
    }

    const QString ss = tabela_view->styleSheet();
    if (!ss.isEmpty()) {
        tabela_view->setStyleSheet(QString());
        tabela_view->setStyleSheet(ss);
    }
}

void aplicar_palette_tabela_widget(QTableWidget* tabela, const QString& tema) {
    if (tabela == nullptr) return;
    const QPalette pal = criar_palette_tabela(tema);
    aplicar_palette_widget(tabela, pal);
    aplicar_palette_widget(tabela->viewport(), pal);
    if (auto* hh = tabela->horizontalHeader()) {
        aplicar_palette_widget(hh, pal);
        aplicar_palette_widget(hh->viewport(), pal);
    }
    if (auto* vh = tabela->verticalHeader()) {
        aplicar_palette_widget(vh, pal);
        aplicar_palette_widget(vh->viewport(), pal);
    }
    if (auto* sbh = tabela->horizontalScrollBar()) {
        aplicar_palette_widget(sbh, pal);
    }
    if (auto* sbv = tabela->verticalScrollBar()) {
        aplicar_palette_widget(sbv, pal);
    }
    if (auto* corner = tabela->findChild<QPushButton*>()) {
        aplicar_palette_widget(corner, pal);
    }

    const QString ss = tabela->styleSheet();
    if (!ss.isEmpty()) {
        tabela->setStyleSheet(QString());
        tabela->setStyleSheet(ss);
    }
}

}  // namespace

void atualizar_estilo_tema(source::services::GerenciadorTabela* self, const QString& novo_tema) {
    try {
        if (!self || !self->interface) return;
        const QString tema = normalizar_tema(novo_tema, self);

        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
            aplicar_palette_tabela_view(tabela_view, tema);
            self->atualizar_cores_colunas(true);
            self->atualizacao_pendente = true;
            return;
        }

        auto* tabela = qobject_cast<QTableWidget*>(tabela_obj);
        if (!tabela) return;

        aplicar_palette_tabela_widget(tabela, tema);
        tabela->setProperty("_gtab_theme_row_cursor", 0);
        self->_atualizar_cores_celulas_incolores(novo_tema);
        self->atualizar_cores_colunas(true);
        self->ajustar_cor_selecao();
        tabela->viewport()->update();
        self->atualizacao_pendente = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em atualizar_estilo_tema", true);
    }
    
}
void atualizar_cores_celulas_incolores(source::services::GerenciadorTabela* self, const QString& novo_tema) {
    Q_UNUSED(novo_tema)
    if (!self || !self->interface) return;
    auto* tabela = qobject_cast<QTableWidget*>(self->interface->property("tabela_dados").value<QObject*>());
    if (!tabela) return;

    constexpr int kRowsPerBatch = 200;
    int row_inicio = tabela->property("_gtab_theme_row_cursor").toInt();
    if (row_inicio < 0 || row_inicio >= tabela->rowCount()) {
        row_inicio = 0;
    }
    const int row_fim = std::min(tabela->rowCount(), row_inicio + kRowsPerBatch);

    const QColor texto_padrao = self->calcular_cor_texto_ideal(QColor(), false);
    for (int r = row_inicio; r < row_fim; ++r) {
        for (int c = 0; c < tabela->columnCount(); ++c) {
            if (auto* item = tabela->item(r, c)) {
                const QColor bg = item->background().color();
                if (!bg.isValid() || bg == QColor() || eh_cor_padrao_qualquer_tema(self, bg)) {
                    item->setForeground(texto_padrao);
                }
            }
        }
    }

    if (row_fim < tabela->rowCount()) {
        tabela->setProperty("_gtab_theme_row_cursor", row_fim);
        QPointer<source::services::GerenciadorTabela> self_guard(self);
        QPointer<QTableWidget> tabela_guard(tabela);
        QTimer::singleShot(0, tabela, [self_guard, tabela_guard, novo_tema]() {
            if (!self_guard || !tabela_guard) return;
            self_guard->_atualizar_cores_celulas_incolores(novo_tema);
        });
        return;
    }

    tabela->setProperty("_gtab_theme_row_cursor", 0);
}

bool eh_cor_padrao_qualquer_tema(source::services::GerenciadorTabela* self, const QColor& cor) {
    if (!self) return false;
    if (!cor.isValid() || cor == QColor() || cor.alpha() == 0) return true;

    const QPalette p = QApplication::palette();
    const QColor base = p.color(QPalette::Base);
    const QColor alt = p.color(QPalette::AlternateBase);
    const QColor win = p.color(QPalette::Window);
    const auto prox = [&](const QColor& a, const QColor& b) {
        return qAbs(a.red() - b.red()) < 20 && qAbs(a.green() - b.green()) < 20 && qAbs(a.blue() - b.blue()) < 20;
    };
    return prox(cor, base) || prox(cor, alt) || prox(cor, win);
}

void forcar_reconstrucao_todas_celulas(source::services::GerenciadorTabela* self, const QString& novo_tema) {
    atualizar_cores_celulas_incolores(self, novo_tema);
    if (self) self->atualizar_cores_colunas(true);
}
}  // namespace source::services::GerenciamentoTabela
