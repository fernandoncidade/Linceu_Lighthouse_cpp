#include "geui_26_calcular_largura_ideal.h"

#include <QFont>
#include <QFontMetrics>
#include <QMetaObject>
#include <QVariantMap>

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

int calcular_largura_ideal(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos) {
    try {
        QFontMetrics fm{QFont{}};
        int maxWidth = 0;

        for (const QVariant& v : graficos) {
            maxWidth = qMax(maxWidth, fm.horizontalAdvance(v.toMap().value("titulo").toString()));
        }

        QObject* loc = self ? self->loc_obj() : nullptr;
        const QStringList textos = {
            get_text(loc, "save_selected", "Salvar selecionados"),
            get_text(loc, "save_all", "Salvar todos"),
            get_text(loc, "refresh", "Atualizar"),
            get_text(loc, "select_all", "Selecionar todos"),
            get_text(loc, "select_graphs", "Selecionar gráficos")
        };

        for (const QString& t : textos) maxWidth = qMax(maxWidth, fm.horizontalAdvance(t));

        const int minPanel = maxWidth + 70;
        return qBound(200, minPanel, 400);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao calcular largura ideal", true);
        return 260;
    }
}

}
