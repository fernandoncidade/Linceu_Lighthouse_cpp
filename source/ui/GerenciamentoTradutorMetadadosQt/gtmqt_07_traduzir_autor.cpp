#include "gtmqt_07_traduzir_autor.h"

#include <QHash>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_autor(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        const QHash<QString, QString> autor_mapeamento = {
            {QStringLiteral("autor desconhecido"), QStringLiteral("unknown_author")},
            {QStringLiteral("autor desconhecido - excel"), QStringLiteral("xls")},
            {QStringLiteral("autor desconhecido - access"), QStringLiteral("access")},
            {QStringLiteral("autor desconhecido - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("autor desconhecido - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("autor desconhecido - visio"), QStringLiteral("visio")},
            {QStringLiteral("autor desconhecido - project"), QStringLiteral("project")},
            {QStringLiteral("autor desconhecido - arquivo comprimido"), QStringLiteral("compressed_file")},

            {QStringLiteral("unknown author"), QStringLiteral("unknown_author")},
            {QStringLiteral("unknown author - excel"), QStringLiteral("xls")},
            {QStringLiteral("unknown author - access"), QStringLiteral("access")},
            {QStringLiteral("unknown author - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("unknown author - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("unknown author - visio"), QStringLiteral("visio")},
            {QStringLiteral("unknown author - project"), QStringLiteral("project")},
            {QStringLiteral("unknown author - compressed file"), QStringLiteral("compressed_file")},

            {QStringLiteral("autor desconocido"), QStringLiteral("unknown_author")},
            {QStringLiteral("autor desconocido - excel"), QStringLiteral("xls")},
            {QStringLiteral("autor desconocido - access"), QStringLiteral("access")},
            {QStringLiteral("autor desconocido - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("autor desconocido - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("autor desconocido - visio"), QStringLiteral("visio")},
            {QStringLiteral("autor desconocido - project"), QStringLiteral("project")},
            {QStringLiteral("autor desconocido - archivo comprimido"), QStringLiteral("compressed_file")},

            {QStringLiteral("auteur inconnu"), QStringLiteral("unknown_author")},
            {QStringLiteral("auteur inconnu - excel"), QStringLiteral("xls")},
            {QStringLiteral("auteur inconnu - access"), QStringLiteral("access")},
            {QStringLiteral("auteur inconnu - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("auteur inconnu - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("auteur inconnu - visio"), QStringLiteral("visio")},
            {QStringLiteral("auteur inconnu - project"), QStringLiteral("project")},
            {QStringLiteral("auteur inconnu - fichier compressé"), QStringLiteral("compressed_file")},

            {QStringLiteral("autore sconosciuto"), QStringLiteral("unknown_author")},
            {QStringLiteral("autore sconosciuto - excel"), QStringLiteral("xls")},
            {QStringLiteral("autore sconosciuto - access"), QStringLiteral("access")},
            {QStringLiteral("autore sconosciuto - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("autore sconosciuto - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("autore sconosciuto - visio"), QStringLiteral("visio")},
            {QStringLiteral("autore sconosciuto - project"), QStringLiteral("project")},
            {QStringLiteral("autore sconosciuto - file compresso"), QStringLiteral("compressed_file")},

            {QStringLiteral("unbekannter autor"), QStringLiteral("unknown_author")},
            {QStringLiteral("unbekannter autor - excel"), QStringLiteral("xls")},
            {QStringLiteral("unbekannter autor - access"), QStringLiteral("access")},
            {QStringLiteral("unbekannter autor - outlook"), QStringLiteral("outllok")},
            {QStringLiteral("unbekannter autor - publisher"), QStringLiteral("publisher")},
            {QStringLiteral("unbekannter autor - visio"), QStringLiteral("visio")},
            {QStringLiteral("unbekannter autor - project"), QStringLiteral("project")},
            {QStringLiteral("unbekannter autor - komprimierte datei"), QStringLiteral("compressed_file")},
        };

        const QString valor_lower = valor.toLower();
        for (auto it = autor_mapeamento.cbegin(); it != autor_mapeamento.cend(); ++it) {
            if (it.key() == valor_lower) {
                return self->loc_->get_text(it.value());
            }
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Error translating author", true);
        return valor;
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
