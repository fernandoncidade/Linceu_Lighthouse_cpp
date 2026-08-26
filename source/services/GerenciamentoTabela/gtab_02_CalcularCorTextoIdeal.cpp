#include "gtab_02_CalcularCorTextoIdeal.h"

#include <QtMath>

#include <QAbstractScrollArea>
#include <QApplication>
#include <QPalette>
#include <QWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {
namespace {

QPalette obter_palette_referencia(source::services::GerenciadorTabela* self) {
    if (self && self->interface) {
        QObject* tabela_obj = self->interface->property("tabela_dados").value<QObject*>();
        if (auto* area = qobject_cast<QAbstractScrollArea*>(tabela_obj)) {
            if (QWidget* viewport = area->viewport()) {
                return viewport->palette();
            }
            return area->palette();
        }
        if (auto* widget = qobject_cast<QWidget*>(tabela_obj)) {
            return widget->palette();
        }
    }
    return QApplication::palette();
}

QColor obter_cor_texto_padrao_real(source::services::GerenciadorTabela* self) {
    const QPalette palette = obter_palette_referencia(self);
    QColor cor = palette.color(QPalette::Active, QPalette::Text);
    if (!cor.isValid()) {
        cor = palette.color(QPalette::Text);
    }
    if (cor.isValid()) {
        return cor;
    }

    const QString tema = self ? self->detectar_tema_windows() : QStringLiteral("claro");
    return tema == QStringLiteral("escuro") ? QColor(255, 255, 255) : QColor(0, 0, 0);
}

}  // namespace

QColor calcular_cor_texto_ideal(source::services::GerenciadorTabela* self, const QColor& cor_fundo,
                                bool eh_coluna_personalizada) {
    try {
        if (!cor_fundo.isValid() || cor_fundo == QColor() || cor_fundo.alpha() == 0) {
            return obter_cor_texto_padrao_real(self);
        }

        const double r_linear = cor_fundo.redF();
        const double g_linear = cor_fundo.greenF();
        const double b_linear = cor_fundo.blueF();

        const auto to_srgb = [](double c) { return c <= 0.03928 ? c / 12.92 : qPow((c + 0.055) / 1.055, 2.4); };
        const double r_srgb = to_srgb(r_linear);
        const double g_srgb = to_srgb(g_linear);
        const double b_srgb = to_srgb(b_linear);

        const double luminosidade = 0.2126 * r_srgb + 0.7152 * g_srgb + 0.0722 * b_srgb;

        if (cor_fundo.red() == 255 && cor_fundo.green() == 0 && cor_fundo.blue() == 255) {
            return QColor(0, 0, 0);
        }

        if (eh_coluna_personalizada) {
            return luminosidade <= 0.5 ? QColor(255, 255, 255) : QColor(0, 0, 0);
        }

        if (self && self->_eh_cor_padrao_qualquer_tema(cor_fundo)) {
            return obter_cor_texto_padrao_real(self);
        }

        const QString tema = self ? self->detectar_tema_windows() : QStringLiteral("claro");
        if (tema == QStringLiteral("claro")) {
            return luminosidade >= 0.4 ? QColor(0, 0, 0) : QColor(255, 255, 255);
        }
        return luminosidade <= 0.6 ? QColor(255, 255, 255) : QColor(0, 0, 0);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em calcular_cor_texto_ideal", true);
    }
    return obter_cor_texto_padrao_real(self);
}

bool eh_cor_fundo_padrao(source::services::GerenciadorTabela* self, const QColor& cor) {
    try {
        const QPalette p = obter_palette_referencia(self);
        const QColor base = p.color(QPalette::Base);
        const QColor alt = p.color(QPalette::AlternateBase);
        const QColor win = p.color(QPalette::Window);

        const auto prox = [&](const QColor& a, const QColor& b) {
            return qAbs(a.red() - b.red()) < 10 && qAbs(a.green() - b.green()) < 10 &&
                   qAbs(a.blue() - b.blue()) < 10;
        };

        return prox(cor, base) || prox(cor, alt) || prox(cor, win);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em eh_cor_fundo_padrao", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoTabela
