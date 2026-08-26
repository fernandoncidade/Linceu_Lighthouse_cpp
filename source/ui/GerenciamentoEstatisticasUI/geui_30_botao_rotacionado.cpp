#include "geui_30_botao_rotacionado.h"

#include <QFontMetrics>
#include <QPainter>

#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

BotaoRotacionado::BotaoRotacionado(const QString& texto, QWidget* parent)
    : QPushButton(texto, parent) {
    try {
        calcular_tamanho();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar BotaoRotacionado", true);
    }
}

void BotaoRotacionado::setText(const QString& texto) {
    try {
        QPushButton::setText(texto);
        calcular_tamanho();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao definir texto do BotaoRotacionado", true);
    }
}

void BotaoRotacionado::paintEvent(QPaintEvent*) {
    try {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.save();

        painter.fillRect(rect(), isDown() ? palette().button().color().darker(110) : palette().button().color());
        painter.setPen(palette().dark().color());
        painter.drawRect(0, 0, width() - 1, height() - 1);

        painter.translate(0, height());
        painter.rotate(270);

        QFontMetrics fm(font());
        const int texto_largura = fm.horizontalAdvance(text());
        const qreal x = (height() - texto_largura) / 2.0;
        const qreal y = (width() + fm.ascent() - fm.descent()) / 2.0;

        painter.setPen(isEnabled() ? palette().buttonText().color() : palette().color(QPalette::Disabled, QPalette::ButtonText));
        painter.drawText(QPointF(x, y), text());

        painter.restore();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao pintar BotaoRotacionado", true);
    }
}

void BotaoRotacionado::calcular_tamanho() {
    try {
        QFontMetrics fm(font());
        const int largura_botao = fm.height() + 5;
        const int altura_botao = fm.horizontalAdvance(text()) + 20;
        setFixedSize(largura_botao, altura_botao);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao calcular tamanho do BotaoRotacionado", true);
    }
}

}
