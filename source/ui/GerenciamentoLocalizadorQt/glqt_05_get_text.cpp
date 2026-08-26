#include "glqt_05_get_text.h"

#include <QCoreApplication>
#include <QSet>

#include "glqt_06_get_fallback_text.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString get_text(source::ui::LocalizadorQt* self, const QString& key, const QStringList& args) {
    try {
        const QStringList candidates = {key, key.toLower(), key.toUpper(),
                                        key.left(1).toUpper() + key.mid(1).toLower()};
        QSet<QString> seen;
        QString result;

        for (const auto& cand : candidates) {
            if (seen.contains(cand)) {
                continue;
            }
            seen.insert(cand);

            const QString texto = QCoreApplication::translate("LinceuLighthouse", cand.toUtf8().constData());
            if (!texto.isEmpty() && (texto != cand || cand == key)) {
                result = texto;
                break;
            }

            const QString fallback = get_fallback_text(self, cand);
            if (fallback != cand) {
                result = fallback;
                break;
            }
        }

        if (result.isEmpty()) {
            source::utils::LogManager::get_logger().warning(
                QString("Tradução não encontrada para chave: '%1'").arg(key));
            result = key;
        }

        for (int i = 0; i < args.size(); ++i) {
            result.replace(QString("%%1").arg(i + 1), args.at(i));
        }

        return result;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao obter tradução para '%1'").arg(key), true);
        return key;
    }
}

}
