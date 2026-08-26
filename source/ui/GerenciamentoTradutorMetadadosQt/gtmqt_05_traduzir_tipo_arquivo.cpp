#include "gtmqt_05_traduzir_tipo_arquivo.h"

#include <QVector>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_tipo_arquivo(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        const QVector<QPair<QString, QString>> tipo_mapeamento = {
            {QStringLiteral("semext"), QStringLiteral("no_ext")},
            {QStringLiteral("noext"), QStringLiteral("no_ext")},
            {QStringLiteral("without extension"), QStringLiteral("no_ext")},
            {QStringLiteral("sin extension"), QStringLiteral("no_ext")},
            {QStringLiteral("sans extension"), QStringLiteral("no_ext")},
            {QStringLiteral("senza estensione"), QStringLiteral("no_ext")},
            {QStringLiteral("ohne erweiterung"), QStringLiteral("no_ext")},
            {QStringLiteral("pasta"), QStringLiteral("file_folder")},
            {QStringLiteral("vídeo"), QStringLiteral("file_video")},
            {QStringLiteral("imagem"), QStringLiteral("file_image")},
            {QStringLiteral("áudio"), QStringLiteral("file_audio")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("código fonte"), QStringLiteral("file_source_code")},
            {QStringLiteral("documento"), QStringLiteral("file_document")},
            {QStringLiteral("planilha"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("apresentação"), QStringLiteral("file_presentation")},
            {QStringLiteral("banco de dados"), QStringLiteral("file_database")},
            {QStringLiteral("executável"), QStringLiteral("file_executable")},
            {QStringLiteral("temporário"), QStringLiteral("file_temp")},
            {QStringLiteral("compactado"), QStringLiteral("file_compressed")},
            {QStringLiteral("backup"), QStringLiteral("file_backup")},
            {QStringLiteral("registro"), QStringLiteral("file_log")},
            {QStringLiteral("configuração"), QStringLiteral("file_config")},
            {QStringLiteral("desconhecido"), QStringLiteral("unknown")},

            {QStringLiteral("folder"), QStringLiteral("file_folder")},
            {QStringLiteral("video"), QStringLiteral("file_video")},
            {QStringLiteral("image"), QStringLiteral("file_image")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("source code"), QStringLiteral("file_source_code")},
            {QStringLiteral("document"), QStringLiteral("file_document")},
            {QStringLiteral("spreadsheet"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("presentation"), QStringLiteral("file_presentation")},
            {QStringLiteral("database"), QStringLiteral("file_database")},
            {QStringLiteral("executable"), QStringLiteral("file_executable")},
            {QStringLiteral("temporary"), QStringLiteral("file_temp")},
            {QStringLiteral("compressed"), QStringLiteral("file_compressed")},
            {QStringLiteral("backup"), QStringLiteral("file_backup")},
            {QStringLiteral("registry"), QStringLiteral("file_log")},
            {QStringLiteral("config"), QStringLiteral("file_config")},
            {QStringLiteral("unknown"), QStringLiteral("unknown")},

            {QStringLiteral("carpeta"), QStringLiteral("file_folder")},
            {QStringLiteral("video"), QStringLiteral("file_video")},
            {QStringLiteral("imagen"), QStringLiteral("file_image")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("código fuente"), QStringLiteral("file_source_code")},
            {QStringLiteral("documento"), QStringLiteral("file_document")},
            {QStringLiteral("hoja de cálculo"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("presentación"), QStringLiteral("file_presentation")},
            {QStringLiteral("base de datos"), QStringLiteral("file_database")},
            {QStringLiteral("ejecutable"), QStringLiteral("file_executable")},
            {QStringLiteral("temporal"), QStringLiteral("file_temp")},
            {QStringLiteral("comprimido"), QStringLiteral("file_compressed")},
            {QStringLiteral("respaldo"), QStringLiteral("file_backup")},
            {QStringLiteral("registro"), QStringLiteral("file_log")},
            {QStringLiteral("configuración"), QStringLiteral("file_config")},
            {QStringLiteral("desconocido"), QStringLiteral("unknown")},

            {QStringLiteral("dossier"), QStringLiteral("file_folder")},
            {QStringLiteral("vidéo"), QStringLiteral("file_video")},
            {QStringLiteral("image"), QStringLiteral("file_image")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("code source"), QStringLiteral("file_source_code")},
            {QStringLiteral("document"), QStringLiteral("file_document")},
            {QStringLiteral("tableur"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("présentation"), QStringLiteral("file_presentation")},
            {QStringLiteral("base de données"), QStringLiteral("file_database")},
            {QStringLiteral("exécutable"), QStringLiteral("file_executable")},
            {QStringLiteral("temporaire"), QStringLiteral("file_temp")},
            {QStringLiteral("compressé"), QStringLiteral("file_compressed")},
            {QStringLiteral("sauvegarde"), QStringLiteral("file_backup")},
            {QStringLiteral("registre"), QStringLiteral("file_log")},
            {QStringLiteral("configuration"), QStringLiteral("file_config")},
            {QStringLiteral("inconnu"), QStringLiteral("unknown")},

            {QStringLiteral("cartella"), QStringLiteral("file_folder")},
            {QStringLiteral("video"), QStringLiteral("file_video")},
            {QStringLiteral("immagine"), QStringLiteral("file_image")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("codice sorgente"), QStringLiteral("file_source_code")},
            {QStringLiteral("documento"), QStringLiteral("file_document")},
            {QStringLiteral("foglio di calcolo"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("presentazione"), QStringLiteral("file_presentation")},
            {QStringLiteral("database"), QStringLiteral("file_database")},
            {QStringLiteral("eseguibile"), QStringLiteral("file_executable")},
            {QStringLiteral("temporaneo"), QStringLiteral("file_temp")},
            {QStringLiteral("compresso"), QStringLiteral("file_compressed")},
            {QStringLiteral("backup"), QStringLiteral("file_backup")},
            {QStringLiteral("registro"), QStringLiteral("file_log")},
            {QStringLiteral("configurazione"), QStringLiteral("file_config")},
            {QStringLiteral("sconosciuto"), QStringLiteral("unknown")},

            {QStringLiteral("ordner"), QStringLiteral("file_folder")},
            {QStringLiteral("video"), QStringLiteral("file_video")},
            {QStringLiteral("bild"), QStringLiteral("file_image")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("quellcode"), QStringLiteral("file_source_code")},
            {QStringLiteral("dokument"), QStringLiteral("file_document")},
            {QStringLiteral("tabellenkalkulation"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("präsentation"), QStringLiteral("file_presentation")},
            {QStringLiteral("datenbank"), QStringLiteral("file_database")},
            {QStringLiteral("ausführbar"), QStringLiteral("file_executable")},
            {QStringLiteral("vorübergehend"), QStringLiteral("file_temp")},
            {QStringLiteral("komprimiert"), QStringLiteral("file_compressed")},
            {QStringLiteral("sicherung"), QStringLiteral("file_backup")},
            {QStringLiteral("protokoll"), QStringLiteral("file_log")},
            {QStringLiteral("konfiguration"), QStringLiteral("file_config")},
            {QStringLiteral("unbekannt"), QStringLiteral("unknown")},
        };

        const QString valor_lower = valor.toLower();
        for (const auto& item : tipo_mapeamento) {
            if (valor_lower.contains(item.first)) {
                return self->loc_->get_text(item.second);
            }
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Error translating file type", true);
        return valor;
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
