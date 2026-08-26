#include "geui_28_traduzir_botoes_detalhes.h"

#include <QAbstractButton>
#include <QMetaObject>
#include <QMessageBox>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void traduzir_botoes_detalhes(source::ui::GerenciadorEstatisticasUI* self, QMessageBox* msg) {
    try {
        if (!self || !msg) return;
        for (QAbstractButton* b : msg->buttons()) {
            if (!b) continue;
            const QString txt = b->text();
            if (txt.contains("Show Details", Qt::CaseInsensitive)) {
                b->setText(get_text(self->loc_obj(), "show_details", "Mostrar detalhes"));
            } else if (txt.contains("Hide Details", Qt::CaseInsensitive)) {
                b->setText(get_text(self->loc_obj(), "hide_details", "Ocultar detalhes"));
            }
            const int w = b->fontMetrics().horizontalAdvance(b->text()) + 20;
            b->setMinimumWidth(w);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir botões de detalhes", true);
    }
}

}
