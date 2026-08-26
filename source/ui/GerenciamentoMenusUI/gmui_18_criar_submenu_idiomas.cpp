#include "gmui_18_criar_submenu_idiomas.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMetaObject>
#include <QVariantMap>

#include "gmui_01_MenuPersistente.h"
#include "gmui_20_confirmar_alteracao_idioma.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace {
QString gt(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString t = fallback;
    if (loc) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty()) {
            t = translated;
        }
    }
    return t;
}
}

namespace source::ui::GerenciamentoMenusUI {

void criar_submenu_idiomas(source::ui::GerenciadorMenusUI* self, QMenu* menu_opcoes) {
    try {
        if (!self || !menu_opcoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        auto* submenu = new MenuPersistente(gt(loc, "language", "Idioma"), qobject_cast<QWidget*>(iface));
        menu_opcoes->addMenu(submenu);

        auto* group = new QActionGroup(iface);
        QVariantMap acoes;

        QVariantMap idiomas;
        if (loc) QMetaObject::invokeMethod(loc, "get_idiomas_disponiveis", Q_RETURN_ARG(QVariantMap, idiomas));
        if (idiomas.isEmpty()) {
            idiomas.insert("pt_BR", "Português");
            idiomas.insert("en_US", "English");
            idiomas.insert("es_ES", "Español");
            idiomas.insert("fr_FR", "Français");
            idiomas.insert("it_IT", "Italiano");
            idiomas.insert("de_DE", "Deutsch");
        }

        const QString atual = loc ? loc->property("idioma_atual").toString() : QStringLiteral("pt_BR");

        for (auto it = idiomas.cbegin(); it != idiomas.cend(); ++it) {
            auto* a = new QAction(it.value().toString(), iface);
            a->setCheckable(true);
            a->setChecked(it.key() == atual);
            a->setData(it.key());
            QObject::connect(a, &QAction::triggered, iface, [self, key = it.key()](bool) { confirmar_alteracao_idioma(self, key); });
            group->addAction(a);
            submenu->addAction(a);
            acoes.insert(it.key(), QVariant::fromValue(a));
        }

        iface->setProperty("_acoes_idioma", acoes);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu de idiomas", true);
    }
}

}
