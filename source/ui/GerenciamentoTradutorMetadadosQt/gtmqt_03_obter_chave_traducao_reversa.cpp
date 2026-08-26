#include "gtmqt_03_obter_chave_traducao_reversa.h"

#include <QStringList>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString obter_chave_traducao_reversa(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return QString();
        }

        const QString alvo = valor.toLower().trimmed();
        const QStringList chaves = {
            QStringLiteral("op_moved"),
            QStringLiteral("op_renamed"),
            QStringLiteral("op_added"),
            QStringLiteral("op_deleted"),
            QStringLiteral("op_modified"),
            QStringLiteral("op_scanned"),
        };

        for (const QString& chave : chaves) {
            const QString traducao_atual = self->loc_->get_text(chave).toLower().trimmed();
            if (traducao_atual == alvo) {
                return chave;
            }
        }
        return QString();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter chave de tradução reversa", true);
        return QString();
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
