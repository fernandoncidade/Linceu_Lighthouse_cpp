#include "gtmqt_06_traduzir_atributos.h"

#include <QStringList>
#include <QVector>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_atributos(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        const QVector<QPair<QString, QString>> atributos_mapeamento = {
            {QStringLiteral("somente leitura"), QStringLiteral("readonly")},
            {QStringLiteral("oculto"), QStringLiteral("hidden")},
            {QStringLiteral("sistema"), QStringLiteral("system")},
            {QStringLiteral("arquivo"), QStringLiteral("archive")},
            {QStringLiteral("criptografado"), QStringLiteral("encrypted")},
            {QStringLiteral("compactado"), QStringLiteral("compressed")},
            {QStringLiteral("pasta"), QStringLiteral("folder")},

            {QStringLiteral("read only"), QStringLiteral("readonly")},
            {QStringLiteral("readonly"), QStringLiteral("readonly")},
            {QStringLiteral("hidden"), QStringLiteral("hidden")},
            {QStringLiteral("system"), QStringLiteral("system")},
            {QStringLiteral("archive"), QStringLiteral("archive")},
            {QStringLiteral("encrypted"), QStringLiteral("encrypted")},
            {QStringLiteral("compressed"), QStringLiteral("compressed")},
            {QStringLiteral("folder"), QStringLiteral("folder")},

            {QStringLiteral("solo lectura"), QStringLiteral("readonly")},
            {QStringLiteral("oculto"), QStringLiteral("hidden")},
            {QStringLiteral("sistema"), QStringLiteral("system")},
            {QStringLiteral("archivo"), QStringLiteral("archive")},
            {QStringLiteral("cifrado"), QStringLiteral("encrypted")},
            {QStringLiteral("comprimido"), QStringLiteral("compressed")},
            {QStringLiteral("carpeta"), QStringLiteral("folder")},

            {QStringLiteral("lecture seule"), QStringLiteral("readonly")},
            {QStringLiteral("caché"), QStringLiteral("hidden")},
            {QStringLiteral("système"), QStringLiteral("system")},
            {QStringLiteral("fichier"), QStringLiteral("archive")},
            {QStringLiteral("chiffré"), QStringLiteral("encrypted")},
            {QStringLiteral("compressé"), QStringLiteral("compressed")},
            {QStringLiteral("dossier"), QStringLiteral("folder")},

            {QStringLiteral("sola lettura"), QStringLiteral("readonly")},
            {QStringLiteral("nascosto"), QStringLiteral("hidden")},
            {QStringLiteral("sistema"), QStringLiteral("system")},
            {QStringLiteral("archivio"), QStringLiteral("archive")},
            {QStringLiteral("cifrato"), QStringLiteral("encrypted")},
            {QStringLiteral("compresso"), QStringLiteral("compressed")},
            {QStringLiteral("cartella"), QStringLiteral("folder")},

            {QStringLiteral("schreibgeschützt"), QStringLiteral("readonly")},
            {QStringLiteral("versteckt"), QStringLiteral("hidden")},
            {QStringLiteral("system"), QStringLiteral("system")},
            {QStringLiteral("datei"), QStringLiteral("archive")},
            {QStringLiteral("verschlüsselt"), QStringLiteral("encrypted")},
            {QStringLiteral("komprimiert"), QStringLiteral("compressed")},
            {QStringLiteral("ordner"), QStringLiteral("folder")},
        };

        const QStringList partes = valor.split(QStringLiteral(","));
        QStringList partes_traduzidas;
        partes_traduzidas.reserve(partes.size());

        for (const QString& parte_original : partes) {
            const QString parte = parte_original.trimmed();
            if (parte.isEmpty()) {
                continue;
            }

            QString parte_traduzida = parte;
            const QString parte_lower = parte.toLower().trimmed();

            bool encontrou = false;
            for (const auto& item : atributos_mapeamento) {
                if (item.first == parte_lower) {
                    parte_traduzida = self->loc_->get_text(item.second);
                    encontrou = true;
                    break;
                }
            }

            if (!encontrou) {
                for (const auto& item : atributos_mapeamento) {
                    if (item.first.contains(parte_lower) || parte_lower.contains(item.first)) {
                        parte_traduzida = self->loc_->get_text(item.second);
                        break;
                    }
                }
            }

            partes_traduzidas.append(parte_traduzida);
        }

        return partes_traduzidas.join(QStringLiteral(", "));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Error translating attributes", true);
        return valor;
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
