#include "gtmqt_09_traduzir_dimensoes.h"

#include <QRegularExpression>
#include <QStringList>
#include <QVector>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_dimensoes(source::ui::TradutorMetadadosQt* self, const QString& valor) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        const QString valor_lower = valor.toLower();
        const QStringList binary_patterns = {
            QStringLiteral("binário:"),
            QStringLiteral("binary:"),
            QStringLiteral("binario:"),
            QStringLiteral("fichier binaire:"),
            QStringLiteral("binario:"),
            QStringLiteral("binär:"),
        };

        for (const QString& pattern : binary_patterns) {
            if (valor_lower.contains(pattern)) {
                const int idx = valor.indexOf(QLatin1Char(':'));
                if (idx >= 0) {
                    const QString direita = valor.mid(idx + 1).trimmed();
                    return self->loc_->get_text(QStringLiteral("binary_file")) + QStringLiteral(": ") +
                           direita;
                }
            }
        }

        const QHash<QString, QString> unidades_mapeamento = {
            {QStringLiteral("páginas"), QStringLiteral("pages")},
            {QStringLiteral("páginas estimadas"), QStringLiteral("pages_estimated")},
            {QStringLiteral("págs."), QStringLiteral("pages")},
            {QStringLiteral("págs. est."), QStringLiteral("pages_estimated")},
            {QStringLiteral("linhas"), QStringLiteral("lines")},
            {QStringLiteral("linhas de código"), QStringLiteral("lines_code")},
            {QStringLiteral("total de linhas"), QStringLiteral("total_lines")},
            {QStringLiteral("palavras"), QStringLiteral("words")},
            {QStringLiteral("colunas"), QStringLiteral("columns")},
            {QStringLiteral("planilhas"), QStringLiteral("spreadsheets")},
            {QStringLiteral("slides"), QStringLiteral("slides")},
            {QStringLiteral("slides estimados"), QStringLiteral("slides_estimated")},
            {QStringLiteral("descompactado"), QStringLiteral("unzipped")},
            {QStringLiteral("descompactados"), QStringLiteral("unzipped")},
            {QStringLiteral("e outros"), QStringLiteral("and_others")},
            {QStringLiteral("arquivos"), QStringLiteral("files")},
            {QStringLiteral("tabelas"), QStringLiteral("tables")},
            {QStringLiteral("parágrafos"), QStringLiteral("paragraphs")},
            {QStringLiteral("registros"), QStringLiteral("records")},
            {QStringLiteral("registros estimados"), QStringLiteral("records_estimated")},
            {QStringLiteral("bytes por registro"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("bytes"), QStringLiteral("bytes")},
            {QStringLiteral("binário"), QStringLiteral("binary_file")},
            {QStringLiteral("minutos"), QStringLiteral("minutes")},
            {QStringLiteral("horas"), QStringLiteral("hours")},
            {QStringLiteral("dias"), QStringLiteral("days")},
            {QStringLiteral("tamanho"), QStringLiteral("size")},

            {QStringLiteral("pages"), QStringLiteral("pages")},
            {QStringLiteral("estimated pages"), QStringLiteral("pages_estimated")},
            {QStringLiteral("est. pages"), QStringLiteral("pages_estimated")},
            {QStringLiteral("lines"), QStringLiteral("lines")},
            {QStringLiteral("lines of code"), QStringLiteral("lines_code")},
            {QStringLiteral("total lines"), QStringLiteral("total_lines")},
            {QStringLiteral("words"), QStringLiteral("words")},
            {QStringLiteral("columns"), QStringLiteral("columns")},
            {QStringLiteral("spreadsheets"), QStringLiteral("spreadsheets")},
            {QStringLiteral("slides"), QStringLiteral("slides")},
            {QStringLiteral("estimated slides"), QStringLiteral("slides_estimated")},
            {QStringLiteral("unzipped"), QStringLiteral("unzipped")},
            {QStringLiteral("and others"), QStringLiteral("and_others")},
            {QStringLiteral("files"), QStringLiteral("files")},
            {QStringLiteral("tables"), QStringLiteral("tables")},
            {QStringLiteral("paragraphs"), QStringLiteral("paragraphs")},
            {QStringLiteral("records"), QStringLiteral("records")},
            {QStringLiteral("estimated records"), QStringLiteral("records_estimated")},
            {QStringLiteral("bytes per record"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("bytes"), QStringLiteral("bytes")},
            {QStringLiteral("binary"), QStringLiteral("binary_file")},
            {QStringLiteral("minutes"), QStringLiteral("minutes")},
            {QStringLiteral("hours"), QStringLiteral("hours")},
            {QStringLiteral("days"), QStringLiteral("days")},
            {QStringLiteral("size"), QStringLiteral("size")},

            {QStringLiteral("páginas"), QStringLiteral("pages")},
            {QStringLiteral("páginas estimadas"), QStringLiteral("pages_estimated")},
            {QStringLiteral("líneas"), QStringLiteral("lines")},
            {QStringLiteral("líneas de código"), QStringLiteral("lines_code")},
            {QStringLiteral("total de líneas"), QStringLiteral("total_lines")},
            {QStringLiteral("palabras"), QStringLiteral("words")},
            {QStringLiteral("columnas"), QStringLiteral("columns")},
            {QStringLiteral("hojas de cálculo"), QStringLiteral("spreadsheets")},
            {QStringLiteral("diapositivas"), QStringLiteral("slides")},
            {QStringLiteral("diapositivas estimadas"), QStringLiteral("slides_estimated")},
            {QStringLiteral("descomprimido"), QStringLiteral("unzipped")},
            {QStringLiteral("descomprimidos"), QStringLiteral("unzipped")},
            {QStringLiteral("y otros"), QStringLiteral("and_others")},
            {QStringLiteral("archivos"), QStringLiteral("files")},
            {QStringLiteral("tablas"), QStringLiteral("tables")},
            {QStringLiteral("párrafos"), QStringLiteral("paragraphs")},
            {QStringLiteral("registros"), QStringLiteral("records")},
            {QStringLiteral("registros estimados"), QStringLiteral("records_estimated")},
            {QStringLiteral("bytes por registro"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("bytes"), QStringLiteral("bytes")},
            {QStringLiteral("binario"), QStringLiteral("binary_file")},
            {QStringLiteral("minutos"), QStringLiteral("minutes")},
            {QStringLiteral("horas"), QStringLiteral("hours")},
            {QStringLiteral("días"), QStringLiteral("days")},
            {QStringLiteral("tamaño"), QStringLiteral("size")},

            {QStringLiteral("pages"), QStringLiteral("pages")},
            {QStringLiteral("pages estimées"), QStringLiteral("pages_estimated")},
            {QStringLiteral("lignes"), QStringLiteral("lines")},
            {QStringLiteral("lignes de code"), QStringLiteral("lines_code")},
            {QStringLiteral("total des lignes"), QStringLiteral("total_lines")},
            {QStringLiteral("mots"), QStringLiteral("words")},
            {QStringLiteral("colonnes"), QStringLiteral("columns")},
            {QStringLiteral("feuilles de calcul"), QStringLiteral("spreadsheets")},
            {QStringLiteral("diapositives"), QStringLiteral("slides")},
            {QStringLiteral("diapositives estimées"), QStringLiteral("slides_estimated")},
            {QStringLiteral("décompressé"), QStringLiteral("unzipped")},
            {QStringLiteral("décompressés"), QStringLiteral("unzipped")},
            {QStringLiteral("et autres"), QStringLiteral("and_others")},
            {QStringLiteral("fichiers"), QStringLiteral("files")},
            {QStringLiteral("tables"), QStringLiteral("tables")},
            {QStringLiteral("paragraphes"), QStringLiteral("paragraphs")},
            {QStringLiteral("enregistrements"), QStringLiteral("records")},
            {QStringLiteral("enregistrements estimés"), QStringLiteral("records_estimated")},
            {QStringLiteral("octets par enregistrement"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("octets"), QStringLiteral("bytes")},
            {QStringLiteral("fichier binaire"), QStringLiteral("binary_file")},
            {QStringLiteral("minutes"), QStringLiteral("minutes")},
            {QStringLiteral("heures"), QStringLiteral("hours")},
            {QStringLiteral("jours"), QStringLiteral("days")},
            {QStringLiteral("taille"), QStringLiteral("size")},

            {QStringLiteral("pagine"), QStringLiteral("pages")},
            {QStringLiteral("pagine stimate"), QStringLiteral("pages_estimated")},
            {QStringLiteral("righe"), QStringLiteral("lines")},
            {QStringLiteral("righe di codice"), QStringLiteral("lines_code")},
            {QStringLiteral("totale righe"), QStringLiteral("total_lines")},
            {QStringLiteral("parole"), QStringLiteral("words")},
            {QStringLiteral("colonne"), QStringLiteral("columns")},
            {QStringLiteral("fogli di calcolo"), QStringLiteral("spreadsheets")},
            {QStringLiteral("diapositive"), QStringLiteral("slides")},
            {QStringLiteral("diapositive stimate"), QStringLiteral("slides_estimated")},
            {QStringLiteral("decompresso"), QStringLiteral("unzipped")},
            {QStringLiteral("decompressi"), QStringLiteral("unzipped")},
            {QStringLiteral("e altri"), QStringLiteral("and_others")},
            {QStringLiteral("file"), QStringLiteral("files")},
            {QStringLiteral("tabelle"), QStringLiteral("tables")},
            {QStringLiteral("paragrafi"), QStringLiteral("paragraphs")},
            {QStringLiteral("record"), QStringLiteral("records")},
            {QStringLiteral("record stimati"), QStringLiteral("records_estimated")},
            {QStringLiteral("byte per record"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("byte"), QStringLiteral("bytes")},
            {QStringLiteral("binario"), QStringLiteral("binary_file")},
            {QStringLiteral("minuti"), QStringLiteral("minutes")},
            {QStringLiteral("ore"), QStringLiteral("hours")},
            {QStringLiteral("giorni"), QStringLiteral("days")},
            {QStringLiteral("dimensioni"), QStringLiteral("size")},

            {QStringLiteral("seiten"), QStringLiteral("pages")},
            {QStringLiteral("geschätzte seiten"), QStringLiteral("pages_estimated")},
            {QStringLiteral("zeilen"), QStringLiteral("lines")},
            {QStringLiteral("codezeilen"), QStringLiteral("lines_code")},
            {QStringLiteral("gesamtzeilen"), QStringLiteral("total_lines")},
            {QStringLiteral("wörter"), QStringLiteral("words")},
            {QStringLiteral("spalten"), QStringLiteral("columns")},
            {QStringLiteral("tabellenkalkulationen"), QStringLiteral("spreadsheets")},
            {QStringLiteral("folien"), QStringLiteral("slides")},
            {QStringLiteral("geschätzte folien"), QStringLiteral("slides_estimated")},
            {QStringLiteral("entpackt"), QStringLiteral("unzipped")},
            {QStringLiteral("und andere"), QStringLiteral("and_others")},
            {QStringLiteral("dateien"), QStringLiteral("files")},
            {QStringLiteral("tabellen"), QStringLiteral("tables")},
            {QStringLiteral("absätze"), QStringLiteral("paragraphs")},
            {QStringLiteral("datensätze"), QStringLiteral("records")},
            {QStringLiteral("geschätzte datensätze"), QStringLiteral("records_estimated")},
            {QStringLiteral("bytes pro datensatz"), QStringLiteral("bytes_per_record")},
            {QStringLiteral("bytes"), QStringLiteral("bytes")},
            {QStringLiteral("binär"), QStringLiteral("binary_file")},
            {QStringLiteral("minuten"), QStringLiteral("minutes")},
            {QStringLiteral("stunden"), QStringLiteral("hours")},
            {QStringLiteral("tage"), QStringLiteral("days")},
            {QStringLiteral("größe"), QStringLiteral("size")},
        };

        const QRegularExpression arquivo_compactado_pattern(
            QStringLiteral(
                R"((\d+)\s+(files|arquivos|archivos|fichiers|file|dateien),\s+(.*?)\s+(unzipped|descompactados|descomprimidos|décompressés|decompressi|entpackt))"),
            QRegularExpression::CaseInsensitiveOption);
        const QRegularExpressionMatch compactado_match = arquivo_compactado_pattern.match(valor);
        if (compactado_match.hasMatch()) {
            const QString num_arquivos = compactado_match.captured(1);
            const QString tamanho = compactado_match.captured(3);
            return num_arquivos + QStringLiteral(" ") + self->loc_->get_text(QStringLiteral("files")) +
                   QStringLiteral(", ") + tamanho + QStringLiteral(" ") +
                   self->loc_->get_text(QStringLiteral("unzipped"));
        }

        const QRegularExpression pattern(
            QStringLiteral(R"((\d+|~\d+)\s+([\p{L}\s.]+?)(?=,|\s+\d|\s*$))"));
        QRegularExpressionMatchIterator it = pattern.globalMatch(valor);
        QVector<QRegularExpressionMatch> matches;
        while (it.hasNext()) {
            matches.append(it.next());
        }

        if (!matches.isEmpty()) {
            QString texto_traduzido = valor;
            for (int i = matches.size() - 1; i >= 0; --i) {
                const QRegularExpressionMatch& match = matches.at(i);
                const QString numero = match.captured(1);
                const QString unidade_trim = match.captured(2).trimmed();
                QString unidade_traduzida = unidade_trim;

                for (auto map_it = unidades_mapeamento.cbegin();
                     map_it != unidades_mapeamento.cend(); ++map_it) {
                    if (map_it.key().compare(unidade_trim, Qt::CaseInsensitive) == 0) {
                        unidade_traduzida = self->loc_->get_text(map_it.value());
                        break;
                    }
                }

                const QString padrao_original = numero + QStringLiteral(" ") + unidade_trim;
                const QString substituicao = numero + QStringLiteral(" ") + unidade_traduzida;
                texto_traduzido.replace(padrao_original, substituicao);
            }

            return texto_traduzido;
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir dimensões", true);
        return valor;
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
