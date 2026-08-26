#include "gmui_19_get_texto_traduzido_para_idioma.h"

#include <QMetaObject>
#include <QTranslator>
#include <QVariant>

#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

QString get_texto_traduzido_para_idioma(source::ui::GerenciadorMenusUI* self, const QString& chave, const QString& idioma) {
    try {
        if (!self || !self->loc_obj()) return chave;
        QObject* loc = self->loc_obj();
        QObject* iface = self->interface_obj();

        const QString dir = loc->property("translations_dir").toString();
        if (dir.isEmpty()) {
            QString out = chave;
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, out), Q_ARG(QString, chave));
            return out;
        }

        QTranslator trans;
        if (trans.load(dir + "/linceu_" + idioma + ".qm")) {
            const QString txt = trans.translate("LinceuLighthouse", chave.toUtf8().constData());
            if (!txt.isEmpty()) return txt;
        }

        QString fallback = chave;
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, fallback), Q_ARG(QString, chave));
        return fallback;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter texto traduzido para idioma", true);
        if (self != nullptr && self->loc_obj() != nullptr) {
            QString fallback = chave;
            QMetaObject::invokeMethod(self->loc_obj(), "get_text", Q_RETURN_ARG(QString, fallback), Q_ARG(QString, chave));
            return fallback;
        }
        return chave;
    }
}

}
