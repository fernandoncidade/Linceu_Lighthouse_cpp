#include "gmui_09_criar_icone_cor.h"

#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

QIcon criar_icone_cor(source::ui::GerenciadorMenusUI*, const QString& cor_hex) {
    try {
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(cor_hex));
        painter.drawRect(0, 0, 16, 16);
        painter.end();
        return QIcon(pix);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar ícone de cor", true);
        return QIcon();
    }
}

}
