#include "gtab_47_EventTableModel.h"

#include <algorithm>
#include <memory>

#include <QApplication>
#include <QBrush>
#include <QCollator>
#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QLocale>
#include <QPalette>
#include <QPoint>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QTableView>
#include <QTimer>
#include <QTranslator>
#include <QVector>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {
namespace {

constexpr int kBatchInsertChunkSize = 512;

const QSet<QString>& numeric_sort_keys() {
    static const QSet<QString> keys = {
        QStringLiteral("size_b"),
        QStringLiteral("size_kb"),
        QStringLiteral("size_mb"),
        QStringLiteral("size_gb"),
        QStringLiteral("size_tb"),
        QStringLiteral("taxa_bits"),
        QStringLiteral("paginas"),
        QStringLiteral("linhas"),
        QStringLiteral("palavras"),
        QStringLiteral("paginas_estimadas"),
        QStringLiteral("linhas_codigo"),
        QStringLiteral("total_linhas"),
        QStringLiteral("slides_estimados"),
        QStringLiteral("arquivos"),
        QStringLiteral("unzipped_b"),
        QStringLiteral("unzipped_kb"),
        QStringLiteral("unzipped_mb"),
        QStringLiteral("unzipped_gb"),
        QStringLiteral("unzipped_tb"),
        QStringLiteral("slides"),
        QStringLiteral("binary_file_b"),
        QStringLiteral("binary_file_kb"),
        QStringLiteral("binary_file_mb"),
        QStringLiteral("binary_file_gb"),
        QStringLiteral("binary_file_tb"),
        QStringLiteral("planilhas"),
        QStringLiteral("colunas"),
        QStringLiteral("registros"),
        QStringLiteral("tabelas"),
        QStringLiteral("risk_score"),
        QStringLiteral("evidence_id")
    };
    return keys;
}

const QSet<QString>& datetime_sort_keys() {
    static const QSet<QString> keys = {
        QStringLiteral("timestamp"),
        QStringLiteral("data_criacao"),
        QStringLiteral("data_modificacao"),
        QStringLiteral("data_acesso")
    };
    return keys;
}

bool try_parse_number(const QVariant& value, double* out) {
    if (out == nullptr || !value.isValid() || value.isNull()) {
        return false;
    }

    bool ok = false;
    const double parsed = value.toDouble(&ok);
    if (ok) {
        *out = parsed;
        return true;
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty()) {
        return false;
    }

    const double reparsed = text.toDouble(&ok);
    if (!ok) {
        return false;
    }

    *out = reparsed;
    return true;
}

QDateTime parse_sort_datetime(const QVariant& value) {
    if (!value.isValid() || value.isNull()) {
        return {};
    }

    if (value.canConvert<QDateTime>()) {
        const QDateTime dt = value.toDateTime();
        if (dt.isValid()) {
            return dt;
        }
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty()) {
        return {};
    }

    for (const Qt::DateFormat format : {Qt::ISODateWithMs, Qt::ISODate}) {
        const QDateTime dt = QDateTime::fromString(text, format);
        if (dt.isValid()) {
            return dt;
        }
    }

    const QStringList formatos_customizados = {
        QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"),
        QStringLiteral("yyyy-MM-dd HH:mm:ss"),
        QStringLiteral("yyyy-MM-ddTHH:mm:ss.zzz"),
        QStringLiteral("yyyy-MM-ddTHH:mm:ss")
    };

    for (const QString& format : formatos_customizados) {
        const QDateTime dt = QDateTime::fromString(text, format);
        if (dt.isValid()) {
            return dt;
        }
    }

    if (text.size() >= 23 && text.at(10) == QLatin1Char(' ')) {
        const QDateTime dt = QDateTime::fromString(text.left(23), QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
        if (dt.isValid()) {
            return dt;
        }
    }

    return {};
}

const QStringList& chaves_operacao_base() {
    static const QStringList keys = {
        QStringLiteral("op_renamed"),
        QStringLiteral("op_added"),
        QStringLiteral("op_deleted"),
        QStringLiteral("op_modified"),
        QStringLiteral("op_moved"),
        QStringLiteral("op_scanned")
    };
    return keys;
}

QString normalizar_tipo(const QString& valor) {
    QString normalizado = valor.normalized(QString::NormalizationForm_D).toCaseFolded().trimmed();
    QString out;
    out.reserve(normalizado.size());
    for (const QChar ch : normalizado) {
        if (ch.category() == QChar::Mark_NonSpacing ||
            ch.category() == QChar::Mark_SpacingCombining ||
            ch.category() == QChar::Mark_Enclosing) {
            continue;
        }
        out.append(ch);
    }
    return out.simplified();
}

QString humanizar_token(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) {
        return value;
    }
    value.replace(QLatin1Char('_'), QLatin1Char(' '));
    return value.simplified();
}

QString normalizar_chave_mime(QString value) {
    value = value.trimmed().toLower();
    QString out;
    out.reserve(value.size());
    bool last_underscore = false;
    for (const QChar ch : value) {
        if (ch.isLetterOrNumber()) {
            out.append(ch);
            last_underscore = false;
        } else if (!last_underscore) {
            out.append(QLatin1Char('_'));
            last_underscore = true;
        }
    }
    while (out.startsWith(QLatin1Char('_'))) out.remove(0, 1);
    while (out.endsWith(QLatin1Char('_'))) out.chop(1);
    return out;
}

QString humanizar_subtipo_mime(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) {
        return value;
    }

    QString normalized = value.toLower();
    if (normalized.startsWith(QStringLiteral("x-"))) {
        normalized = normalized.mid(2);
    }
    if (normalized.startsWith(QStringLiteral("vnd."))) {
        normalized = normalized.mid(4);
    }

    normalized.replace(QLatin1Char('+'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('-'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('.'), QLatin1Char(' '));
    normalized.replace(QLatin1Char('_'), QLatin1Char(' '));

    static const QSet<QString> acronimos = {
        QStringLiteral("7z"), QStringLiteral("avi"), QStringLiteral("bmp"), QStringLiteral("csv"),
        QStringLiteral("epub"), QStringLiteral("gif"), QStringLiteral("gzip"), QStringLiteral("html"),
        QStringLiteral("jpeg"), QStringLiteral("jpg"), QStringLiteral("json"), QStringLiteral("mp3"),
        QStringLiteral("mp4"), QStringLiteral("mpeg"), QStringLiteral("ogg"), QStringLiteral("pdf"),
        QStringLiteral("png"), QStringLiteral("rar"), QStringLiteral("rtf"), QStringLiteral("svg"),
        QStringLiteral("tiff"), QStringLiteral("wav"), QStringLiteral("webm"), QStringLiteral("webp"),
        QStringLiteral("xhtml"), QStringLiteral("xml"), QStringLiteral("zip")
    };

    QStringList words;
    const QStringList tokens = normalized.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
    words.reserve(tokens.size());
    for (const QString& token : tokens) {
        if (acronimos.contains(token)) {
            words.append(token.toUpper());
        } else {
            words.append(token.left(1).toUpper() + token.mid(1));
        }
    }
    return words.join(QLatin1Char(' '));
}

bool is_security_field(const QString& field_key) {
    static const QStringList fields = {
        QStringLiteral("baseline_status"),
        QStringLiteral("alerta_resumo"),
        QStringLiteral("session_label"),
        QStringLiteral("diff_resumo"),
        QStringLiteral("risk_level"),
        QStringLiteral("quarantine_status"),
        QStringLiteral("actor_process"),
        QStringLiteral("risk_reasons"),
        QStringLiteral("integrity_flags"),
    };
    return fields.contains(field_key);
}

bool is_metadata_field(const QString& field_key) {
    static const QStringList fields = {
        QStringLiteral("tipo"),
        QStringLiteral("atributos"),
        QStringLiteral("autor"),
        QStringLiteral("protegido"),
        QStringLiteral("dimensoes"),
        QStringLiteral("mime")
    };
    return fields.contains(field_key);
}

QString translations_dir_from_interface(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return QString();
    }

    if (QObject* loc = interface_obj->property("loc").value<QObject*>()) {
        const QString translations_dir = loc->property("translations_dir").toString();
        if (!translations_dir.isEmpty()) {
            return translations_dir;
        }
    }

    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString parent_dir = QDir(app_dir + QStringLiteral("/..")).absolutePath();
    const QString grandparent_dir = QDir(parent_dir + QStringLiteral("/..")).absolutePath();
    const QStringList candidates = {
        QDir(app_dir).filePath(QStringLiteral("source/locale")),
        QDir(app_dir).filePath(QStringLiteral("locale")),
        QDir(parent_dir).filePath(QStringLiteral("source/locale")),
        QDir(parent_dir).filePath(QStringLiteral("locale")),
        QDir(grandparent_dir).filePath(QStringLiteral("source/locale")),
        QDir(grandparent_dir).filePath(QStringLiteral("locale"))
    };

    for (const QString& candidate : candidates) {
        if (QDir(candidate).exists()) {
            return QDir(candidate).absolutePath();
        }
    }

    return QString();
}

QList<QPair<QString, QVariantMap>> construir_colunas_ordenadas(QObject* interface_obj) {
    QList<QPair<QString, QVariantMap>> ordenadas;
    auto* gc = qobject_cast<source::services::GerenciadorColunas*>(
        interface_obj != nullptr ? interface_obj->property("gerenciador_colunas").value<QObject*>()
                                 : nullptr);
    if (gc == nullptr) {
        return ordenadas;
    }

    std::vector<QPair<QString, QVariantMap>> buffer;
    buffer.reserve(static_cast<size_t>(gc->COLUNAS_DISPONIVEIS.size()));
    for (auto it = gc->COLUNAS_DISPONIVEIS.begin(); it != gc->COLUNAS_DISPONIVEIS.end(); ++it) {
        buffer.emplace_back(it.key(), it.value());
    }

    std::sort(buffer.begin(), buffer.end(), [](const auto& a, const auto& b) {
        return a.second.value("ordem", 0).toInt() < b.second.value("ordem", 0).toInt();
    });

    for (const auto& item : buffer) {
        ordenadas.append(item);
    }

    return ordenadas;
}

}  // namespace

class FixedLanguageLocalizer {
public:
    FixedLanguageLocalizer(const QString& idioma_exibicao, const QString& translations_dir)
        : idioma_exibicao_(idioma_exibicao.isEmpty() ? QStringLiteral("pt_BR") : idioma_exibicao),
          translations_dir_(translations_dir) {
        if (!translations_dir_.isEmpty()) {
            const QString qm_path =
                QDir(translations_dir_).filePath(QStringLiteral("linceu_%1.qm").arg(idioma_exibicao_));
            translator_loaded_ = QFileInfo::exists(qm_path) && translator_.load(qm_path);
        }
    }

    QString get_text(const QString& key, const QString& fallback = QString()) const {
        const QString cache_key = key + QLatin1Char('\x1f') + fallback;
        const auto it_cache = cache_texto_.constFind(cache_key);
        if (it_cache != cache_texto_.constEnd()) {
            return it_cache.value();
        }

        QStringList candidates;
        candidates << key;
        if (!key.isEmpty()) {
            candidates << key.toLower() << key.toUpper();
            candidates << key.left(1).toUpper() + key.mid(1).toLower();
        }

        QString resultado;
        QSet<QString> vistos;
        for (const QString& candidate : candidates) {
            if (candidate.isEmpty() || vistos.contains(candidate)) {
                continue;
            }
            vistos.insert(candidate);

            if (translator_loaded_) {
                const QString traduzido =
                    translator_.translate("LinceuLighthouse", candidate.toUtf8().constData());
                if (!traduzido.isEmpty() && (traduzido != candidate || candidate == key)) {
                    resultado = traduzido;
                    break;
                }
            }
        }

        if (resultado.isEmpty()) {
            resultado = fallback.isEmpty() ? key : fallback;
        }

        cache_texto_.insert(cache_key, resultado);
        return resultado;
    }

    QString traduzir_valor(const QString& field_key, const QString& raw_value) const {
        if (raw_value.trimmed().isEmpty()) {
            return raw_value;
        }
        if (field_key == QStringLiteral("tipo_operacao")) {
            return traduzir_tipo_operacao(raw_value);
        }
        if (is_security_field(field_key)) {
            return traduzir_valor_seguranca(field_key, raw_value);
        }
        if (is_metadata_field(field_key)) {
            return traduzir_metadados(field_key, raw_value);
        }
        if (field_key == QStringLiteral("dir_anterior") || field_key == QStringLiteral("dir_atual")) {
            QString caminho = QDir::toNativeSeparators(raw_value);
            if (caminho == QStringLiteral(".")) {
                caminho.clear();
            }
            return caminho;
        }
        return raw_value;
    }

private:
    QString traduzir_chave(const QString& key, const QString& fallback = QString()) const {
        return get_text(key, fallback.isEmpty() ? humanizar_token(key) : fallback);
    }

    QString traduzir_lista_por_ponto_virgula(const QString& raw_value) const {
        const QStringList parts = raw_value.split(QLatin1Char(';'), Qt::SkipEmptyParts);
        QStringList translated;
        translated.reserve(parts.size());

        for (const QString& part : parts) {
            const QString token = part.trimmed();
            if (!token.isEmpty()) {
                translated.append(traduzir_chave(token));
            }
        }

        return translated.join(QStringLiteral("; "));
    }

    QString traduzir_valor_seguranca(const QString& field_key, const QString& raw_value) const {
        const QString cache_key =
            field_key + QLatin1Char('\x1f') + raw_value + QLatin1Char('\x1f') + idioma_exibicao_;
        const auto it_cache = cache_seguranca_.constFind(cache_key);
        if (it_cache != cache_seguranca_.constEnd()) {
            return it_cache.value();
        }

        QString resultado = raw_value;
        if (field_key == QStringLiteral("baseline_status")) {
            static const QHash<QString, QString> map = {
                {QStringLiteral("unavailable"), QStringLiteral("security_baseline_unavailable")},
                {QStringLiteral("directory"), QStringLiteral("security_baseline_directory")},
                {QStringLiteral("new"), QStringLiteral("security_baseline_new")},
                {QStringLiteral("unchanged"), QStringLiteral("security_baseline_unchanged")},
                {QStringLiteral("changed"), QStringLiteral("security_baseline_changed")},
                {QStringLiteral("missing"), QStringLiteral("security_baseline_missing")},
            };
            const QString translation_key = map.value(raw_value.trimmed().toLower());
            if (!translation_key.isEmpty()) {
                resultado = traduzir_chave(translation_key, raw_value);
            }
        } else if (field_key == QStringLiteral("alerta_resumo") ||
                   field_key == QStringLiteral("risk_reasons") ||
                   field_key == QStringLiteral("integrity_flags")) {
            resultado = traduzir_lista_por_ponto_virgula(raw_value);
        } else if (field_key == QStringLiteral("session_label")) {
            resultado = traduzir_chave(raw_value, humanizar_token(raw_value));
        } else if (field_key == QStringLiteral("diff_resumo")) {
            const QString trimmed = raw_value.trimmed();
            if (trimmed == QStringLiteral("sem_alteracao_textual")) {
                resultado = traduzir_chave(QStringLiteral("sem_alteracao_textual"));
            } else if (trimmed.startsWith(QStringLiteral("security_diff_lines_changed|"))) {
                const QStringList parts = trimmed.split(QLatin1Char('|'));
                if (parts.size() >= 3) {
                    const QString pattern = traduzir_chave(
                        QStringLiteral("security_diff_lines_changed"),
                        QStringLiteral("%1 lines added / %2 lines removed"));
                    resultado = pattern.arg(parts.at(1), parts.at(2));
                }
            } else {
                static const QRegularExpression legacy_pattern(
                    QStringLiteral("^\\+(\\d+)\\s*/\\s*-(\\d+)\\s+linhas$"),
                    QRegularExpression::CaseInsensitiveOption);
                const QRegularExpressionMatch match = legacy_pattern.match(trimmed);
                if (match.hasMatch()) {
                    const QString pattern = traduzir_chave(
                        QStringLiteral("security_diff_lines_changed"),
                        QStringLiteral("%1 lines added / %2 lines removed"));
                    resultado = pattern.arg(match.captured(1), match.captured(2));
                }
            }
        } else if (field_key == QStringLiteral("risk_level")) {
            static const QHash<QString, QString> map = {
                {QStringLiteral("none"), QStringLiteral("security_risk_none")},
                {QStringLiteral("low"), QStringLiteral("security_risk_low")},
                {QStringLiteral("medium"), QStringLiteral("security_risk_medium")},
                {QStringLiteral("high"), QStringLiteral("security_risk_high")},
                {QStringLiteral("critical"), QStringLiteral("security_risk_critical")},
            };
            const QString translation_key = map.value(raw_value.trimmed().toLower());
            if (!translation_key.isEmpty()) {
                resultado = traduzir_chave(translation_key, raw_value);
            }
        } else if (field_key == QStringLiteral("quarantine_status")) {
            static const QHash<QString, QString> map = {
                {QStringLiteral("not_applicable"), QStringLiteral("security_quarantine_not_applicable")},
                {QStringLiteral("queued"), QStringLiteral("security_quarantine_queued")},
                {QStringLiteral("captured"), QStringLiteral("security_quarantine_captured")},
                {QStringLiteral("copy_failed"), QStringLiteral("security_quarantine_copy_failed")},
            };
            const QString normalized = raw_value.trimmed().toLower();
            const QString translation_key = map.value(normalized);
            resultado = translation_key.isEmpty() ? traduzir_chave(normalized, raw_value)
                                                  : traduzir_chave(translation_key, raw_value);
        } else if (field_key == QStringLiteral("actor_process")) {
            const QString normalized = raw_value.trimmed().toLower();
            if (normalized.isEmpty() ||
                normalized == QStringLiteral("unknown_process") ||
                normalized == QStringLiteral("desconhecido")) {
                resultado = traduzir_chave(QStringLiteral("unknown_process"));
            }
        }

        cache_seguranca_.insert(cache_key, resultado);
        return resultado;
    }

    QString traduzir_tipo_operacao(const QString& valor) const {
        const QString cache_key = valor + QLatin1Char('\x1f') + idioma_exibicao_;
        const auto it_cache = cache_operacao_.constFind(cache_key);
        if (it_cache != cache_operacao_.constEnd()) {
            return it_cache.value();
        }

        static const QVector<QPair<QString, QString>> mapeamento_operacoes = {
            {QStringLiteral("moved"), QStringLiteral("op_moved")},
            {QStringLiteral("renamed"), QStringLiteral("op_renamed")},
            {QStringLiteral("added"), QStringLiteral("op_added")},
            {QStringLiteral("deleted"), QStringLiteral("op_deleted")},
            {QStringLiteral("modified"), QStringLiteral("op_modified")},
            {QStringLiteral("scanned"), QStringLiteral("op_scanned")},
            {QStringLiteral("movido"), QStringLiteral("op_moved")},
            {QStringLiteral("renomeado"), QStringLiteral("op_renamed")},
            {QStringLiteral("adicionado"), QStringLiteral("op_added")},
            {QStringLiteral("excluido"), QStringLiteral("op_deleted")},
            {QStringLiteral("excluído"), QStringLiteral("op_deleted")},
            {QStringLiteral("modificado"), QStringLiteral("op_modified")},
            {QStringLiteral("escaneado"), QStringLiteral("op_scanned")},
            {QStringLiteral("renombrado"), QStringLiteral("op_renamed")},
            {QStringLiteral("añadido"), QStringLiteral("op_added")},
            {QStringLiteral("eliminado"), QStringLiteral("op_deleted")},
            {QStringLiteral("déplacé"), QStringLiteral("op_moved")},
            {QStringLiteral("renommé"), QStringLiteral("op_renamed")},
            {QStringLiteral("ajouté"), QStringLiteral("op_added")},
            {QStringLiteral("supprimé"), QStringLiteral("op_deleted")},
            {QStringLiteral("modifié"), QStringLiteral("op_modified")},
            {QStringLiteral("numérisé"), QStringLiteral("op_scanned")},
            {QStringLiteral("spostato"), QStringLiteral("op_moved")},
            {QStringLiteral("rinominato"), QStringLiteral("op_renamed")},
            {QStringLiteral("aggiunto"), QStringLiteral("op_added")},
            {QStringLiteral("eliminato"), QStringLiteral("op_deleted")},
            {QStringLiteral("scansionato"), QStringLiteral("op_scanned")},
            {QStringLiteral("verschoben"), QStringLiteral("op_moved")},
            {QStringLiteral("umbenannt"), QStringLiteral("op_renamed")},
            {QStringLiteral("hinzugefügt"), QStringLiteral("op_added")},
            {QStringLiteral("gelöscht"), QStringLiteral("op_deleted")},
            {QStringLiteral("geändert"), QStringLiteral("op_modified")},
            {QStringLiteral("gescannt"), QStringLiteral("op_scanned")},
        };

        const QString valor_normalizado = normalizar_tipo(valor);
        QString resultado = valor;
        if (valor_normalizado.startsWith(QStringLiteral("op_"))) {
            resultado = get_text(valor_normalizado, valor);
        } else {
            for (const auto& item : mapeamento_operacoes) {
                if (normalizar_tipo(item.first) == valor_normalizado) {
                    resultado = get_text(item.second, valor);
                    break;
                }
            }
        }

        cache_operacao_.insert(cache_key, resultado);
        return resultado;
    }

    QString traduzir_tipo_arquivo(const QString& valor) const {
        static const QVector<QPair<QString, QString>> tipo_mapeamento = {
            {QStringLiteral("pasta"), QStringLiteral("file_folder")},
            {QStringLiteral("folder"), QStringLiteral("file_folder")},
            {QStringLiteral("carpeta"), QStringLiteral("file_folder")},
            {QStringLiteral("dossier"), QStringLiteral("file_folder")},
            {QStringLiteral("cartella"), QStringLiteral("file_folder")},
            {QStringLiteral("ordner"), QStringLiteral("file_folder")},
            {QStringLiteral("vídeo"), QStringLiteral("file_video")},
            {QStringLiteral("video"), QStringLiteral("file_video")},
            {QStringLiteral("vidéo"), QStringLiteral("file_video")},
            {QStringLiteral("imagem"), QStringLiteral("file_image")},
            {QStringLiteral("image"), QStringLiteral("file_image")},
            {QStringLiteral("imagen"), QStringLiteral("file_image")},
            {QStringLiteral("immagine"), QStringLiteral("file_image")},
            {QStringLiteral("bild"), QStringLiteral("file_image")},
            {QStringLiteral("áudio"), QStringLiteral("file_audio")},
            {QStringLiteral("audio"), QStringLiteral("file_audio")},
            {QStringLiteral("código fonte"), QStringLiteral("file_source_code")},
            {QStringLiteral("source code"), QStringLiteral("file_source_code")},
            {QStringLiteral("code source"), QStringLiteral("file_source_code")},
            {QStringLiteral("codice sorgente"), QStringLiteral("file_source_code")},
            {QStringLiteral("quellcode"), QStringLiteral("file_source_code")},
            {QStringLiteral("documento"), QStringLiteral("file_document")},
            {QStringLiteral("document"), QStringLiteral("file_document")},
            {QStringLiteral("planilha"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("spreadsheet"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("hoja de cálculo"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("tableur"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("foglio di calcolo"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("tabellenkalkulation"), QStringLiteral("file_spreadsheet")},
            {QStringLiteral("apresentação"), QStringLiteral("file_presentation")},
            {QStringLiteral("presentation"), QStringLiteral("file_presentation")},
            {QStringLiteral("presentación"), QStringLiteral("file_presentation")},
            {QStringLiteral("banco de dados"), QStringLiteral("file_database")},
            {QStringLiteral("database"), QStringLiteral("file_database")},
            {QStringLiteral("base de datos"), QStringLiteral("file_database")},
            {QStringLiteral("base de données"), QStringLiteral("file_database")},
            {QStringLiteral("executável"), QStringLiteral("file_executable")},
            {QStringLiteral("executable"), QStringLiteral("file_executable")},
            {QStringLiteral("ejecutable"), QStringLiteral("file_executable")},
            {QStringLiteral("exécutable"), QStringLiteral("file_executable")},
            {QStringLiteral("eseguibile"), QStringLiteral("file_executable")},
            {QStringLiteral("ausführbar"), QStringLiteral("file_executable")},
            {QStringLiteral("temporário"), QStringLiteral("file_temp")},
            {QStringLiteral("temporary"), QStringLiteral("file_temp")},
            {QStringLiteral("temporal"), QStringLiteral("file_temp")},
            {QStringLiteral("temporaire"), QStringLiteral("file_temp")},
            {QStringLiteral("temporaneo"), QStringLiteral("file_temp")},
            {QStringLiteral("vorübergehend"), QStringLiteral("file_temp")},
            {QStringLiteral("compactado"), QStringLiteral("file_compressed")},
            {QStringLiteral("compressed"), QStringLiteral("file_compressed")},
            {QStringLiteral("comprimido"), QStringLiteral("file_compressed")},
            {QStringLiteral("compressé"), QStringLiteral("file_compressed")},
            {QStringLiteral("compresso"), QStringLiteral("file_compressed")},
            {QStringLiteral("komprimiert"), QStringLiteral("file_compressed")},
            {QStringLiteral("backup"), QStringLiteral("file_backup")},
            {QStringLiteral("registro"), QStringLiteral("file_log")},
            {QStringLiteral("registry"), QStringLiteral("file_log")},
            {QStringLiteral("registre"), QStringLiteral("file_log")},
            {QStringLiteral("protokoll"), QStringLiteral("file_log")},
            {QStringLiteral("configuração"), QStringLiteral("file_config")},
            {QStringLiteral("config"), QStringLiteral("file_config")},
            {QStringLiteral("configuración"), QStringLiteral("file_config")},
            {QStringLiteral("configuration"), QStringLiteral("file_config")},
            {QStringLiteral("konfiguration"), QStringLiteral("file_config")},
            {QStringLiteral("desconhecido"), QStringLiteral("unknown")},
            {QStringLiteral("unknown"), QStringLiteral("unknown")},
            {QStringLiteral("desconocido"), QStringLiteral("unknown")},
            {QStringLiteral("inconnu"), QStringLiteral("unknown")},
            {QStringLiteral("sconosciuto"), QStringLiteral("unknown")},
            {QStringLiteral("unbekannt"), QStringLiteral("unknown")},
        };

        const QString valor_lower = valor.toLower();
        for (const auto& item : tipo_mapeamento) {
            if (valor_lower.contains(item.first)) {
                return get_text(item.second, valor);
            }
        }
        return valor;
    }

    QString traduzir_atributos(const QString& valor) const {
        static const QVector<QPair<QString, QString>> atributos_mapeamento = {
            {QStringLiteral("somente leitura"), QStringLiteral("readonly")},
            {QStringLiteral("read only"), QStringLiteral("readonly")},
            {QStringLiteral("readonly"), QStringLiteral("readonly")},
            {QStringLiteral("solo lectura"), QStringLiteral("readonly")},
            {QStringLiteral("lecture seule"), QStringLiteral("readonly")},
            {QStringLiteral("sola lettura"), QStringLiteral("readonly")},
            {QStringLiteral("schreibgeschützt"), QStringLiteral("readonly")},
            {QStringLiteral("oculto"), QStringLiteral("hidden")},
            {QStringLiteral("hidden"), QStringLiteral("hidden")},
            {QStringLiteral("caché"), QStringLiteral("hidden")},
            {QStringLiteral("nascosto"), QStringLiteral("hidden")},
            {QStringLiteral("versteckt"), QStringLiteral("hidden")},
            {QStringLiteral("sistema"), QStringLiteral("system")},
            {QStringLiteral("system"), QStringLiteral("system")},
            {QStringLiteral("système"), QStringLiteral("system")},
            {QStringLiteral("arquivo"), QStringLiteral("archive")},
            {QStringLiteral("archive"), QStringLiteral("archive")},
            {QStringLiteral("archivo"), QStringLiteral("archive")},
            {QStringLiteral("fichier"), QStringLiteral("archive")},
            {QStringLiteral("archivio"), QStringLiteral("archive")},
            {QStringLiteral("datei"), QStringLiteral("archive")},
            {QStringLiteral("criptografado"), QStringLiteral("encrypted")},
            {QStringLiteral("encrypted"), QStringLiteral("encrypted")},
            {QStringLiteral("cifrado"), QStringLiteral("encrypted")},
            {QStringLiteral("chiffré"), QStringLiteral("encrypted")},
            {QStringLiteral("cifrato"), QStringLiteral("encrypted")},
            {QStringLiteral("verschlüsselt"), QStringLiteral("encrypted")},
            {QStringLiteral("compactado"), QStringLiteral("compressed")},
            {QStringLiteral("compressed"), QStringLiteral("compressed")},
            {QStringLiteral("comprimido"), QStringLiteral("compressed")},
            {QStringLiteral("compressé"), QStringLiteral("compressed")},
            {QStringLiteral("compresso"), QStringLiteral("compressed")},
            {QStringLiteral("komprimiert"), QStringLiteral("compressed")},
            {QStringLiteral("pasta"), QStringLiteral("folder")},
            {QStringLiteral("folder"), QStringLiteral("folder")},
            {QStringLiteral("carpeta"), QStringLiteral("folder")},
            {QStringLiteral("dossier"), QStringLiteral("folder")},
            {QStringLiteral("cartella"), QStringLiteral("folder")},
            {QStringLiteral("ordner"), QStringLiteral("folder")},
        };

        const QStringList partes = valor.split(QStringLiteral(","), Qt::SkipEmptyParts);
        QStringList traduzidas;
        for (const QString& parte_original : partes) {
            const QString parte = parte_original.trimmed();
            if (parte.isEmpty()) {
                continue;
            }

            QString resultado = parte;
            const QString parte_lower = parte.toLower().trimmed();
            for (const auto& item : atributos_mapeamento) {
                if (item.first == parte_lower ||
                    item.first.contains(parte_lower) ||
                    parte_lower.contains(item.first)) {
                    resultado = get_text(item.second, parte);
                    break;
                }
            }
            traduzidas.append(resultado);
        }

        return traduzidas.isEmpty() ? valor : traduzidas.join(QStringLiteral(", "));
    }

    QString traduzir_autor(const QString& valor) const {
        static const QHash<QString, QString> autor_mapeamento = {
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
            {QStringLiteral("autor desconocido - archivo comprimido"), QStringLiteral("compressed_file")},
            {QStringLiteral("auteur inconnu"), QStringLiteral("unknown_author")},
            {QStringLiteral("auteur inconnu - fichier compressé"), QStringLiteral("compressed_file")},
            {QStringLiteral("autore sconosciuto"), QStringLiteral("unknown_author")},
            {QStringLiteral("autore sconosciuto - file compresso"), QStringLiteral("compressed_file")},
            {QStringLiteral("unbekannter autor"), QStringLiteral("unknown_author")},
            {QStringLiteral("unbekannter autor - komprimierte datei"), QStringLiteral("compressed_file")},
        };

        const QString translation_key = autor_mapeamento.value(valor.toLower());
        return translation_key.isEmpty() ? valor : get_text(translation_key, valor);
    }

    QString traduzir_protegido(const QString& valor) const {
        const QStringList valores_sim = {
            QStringLiteral("sim"), QStringLiteral("yes"), QStringLiteral("sí"),
            QStringLiteral("oui"), QStringLiteral("sì"), QStringLiteral("ja"),
        };
        const QStringList valores_nao = {
            QStringLiteral("não"), QStringLiteral("no"),
            QStringLiteral("non"), QStringLiteral("nein"),
        };

        const QString valor_lower = valor.toLower();
        for (const QString& candidato : valores_sim) {
            if (valor_lower.startsWith(candidato)) {
                QString resto = valor.mid(candidato.size()).trimmed();
                if (!resto.isEmpty()) {
                    if (resto.startsWith(QStringLiteral(","))) {
                        resto = resto.mid(1).trimmed();
                    } else if (resto.startsWith(QStringLiteral("("))) {
                        return get_text(QStringLiteral("yes"), QStringLiteral("Yes")) + resto;
                    }
                    if (!resto.isEmpty()) {
                        return get_text(QStringLiteral("yes"), QStringLiteral("Yes")) +
                               QStringLiteral(", ") + traduzir_atributos(resto);
                    }
                }
                return get_text(QStringLiteral("yes"), QStringLiteral("Yes"));
            }
        }

        for (const QString& candidato : valores_nao) {
            if (valor_lower.contains(candidato)) {
                return get_text(QStringLiteral("no"), QStringLiteral("No"));
            }
        }

        return valor;
    }

    QString traduzir_dimensoes(const QString& valor) const {
        const QString valor_lower = valor.toLower();
        const QStringList binary_patterns = {
            QStringLiteral("binário:"), QStringLiteral("binary:"), QStringLiteral("binario:"),
            QStringLiteral("fichier binaire:"), QStringLiteral("binär:")
        };

        for (const QString& pattern : binary_patterns) {
            if (valor_lower.contains(pattern)) {
                const int idx = valor.indexOf(QLatin1Char(':'));
                if (idx >= 0) {
                    return get_text(QStringLiteral("binary_file"), QStringLiteral("Binary file")) +
                           QStringLiteral(": ") + valor.mid(idx + 1).trimmed();
                }
            }
        }

        static const QHash<QString, QString> unidades = {
            {QStringLiteral("páginas"), QStringLiteral("pages")},
            {QStringLiteral("páginas estimadas"), QStringLiteral("pages_estimated")},
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
            {QStringLiteral("arquivos"), QStringLiteral("files")},
            {QStringLiteral("tabelas"), QStringLiteral("tables")},
            {QStringLiteral("registros"), QStringLiteral("records")},
            {QStringLiteral("bytes"), QStringLiteral("bytes")},
            {QStringLiteral("binário"), QStringLiteral("binary_file")},
            {QStringLiteral("minutes"), QStringLiteral("minutes")},
            {QStringLiteral("horas"), QStringLiteral("hours")},
            {QStringLiteral("dias"), QStringLiteral("days")},
            {QStringLiteral("tamanho"), QStringLiteral("size")},
            {QStringLiteral("pages"), QStringLiteral("pages")},
            {QStringLiteral("estimated pages"), QStringLiteral("pages_estimated")},
            {QStringLiteral("lines"), QStringLiteral("lines")},
            {QStringLiteral("lines of code"), QStringLiteral("lines_code")},
            {QStringLiteral("total lines"), QStringLiteral("total_lines")},
            {QStringLiteral("words"), QStringLiteral("words")},
            {QStringLiteral("columns"), QStringLiteral("columns")},
            {QStringLiteral("spreadsheets"), QStringLiteral("spreadsheets")},
            {QStringLiteral("files"), QStringLiteral("files")},
            {QStringLiteral("tables"), QStringLiteral("tables")},
            {QStringLiteral("records"), QStringLiteral("records")},
            {QStringLiteral("binary"), QStringLiteral("binary_file")},
            {QStringLiteral("hours"), QStringLiteral("hours")},
            {QStringLiteral("days"), QStringLiteral("days")},
            {QStringLiteral("size"), QStringLiteral("size")},
        };

        QString texto_traduzido = valor;
        for (auto it = unidades.begin(); it != unidades.end(); ++it) {
            texto_traduzido.replace(it.key(), get_text(it.value(), it.key()), Qt::CaseInsensitive);
        }
        return texto_traduzido;
    }

    QString traduzir_mime(const QString& valor) const {
        const QString raw = valor.trimmed();
        if (raw.isEmpty()) {
            return raw;
        }

        const QStringList parts = raw.split(QLatin1Char('/'), Qt::SkipEmptyParts);
        const QString primary = parts.value(0).trimmed().toLower();
        const QString subtype = parts.size() > 1 ? parts.mid(1).join(QLatin1Char('/')).trimmed()
                                                 : QString();

        const QString primary_key = QStringLiteral("mime_primary_") + normalizar_chave_mime(primary);
        const QString primary_fallback =
            primary.isEmpty()
                ? get_text(QStringLiteral("mime_primary_other"), QStringLiteral("Other"))
                : humanizar_subtipo_mime(primary);
        const QString primary_text = get_text(primary_key, primary_fallback);

        if (subtype.isEmpty()) {
            return QStringLiteral("%1 (%2)").arg(primary_text, raw);
        }

        const QString subtype_key = QStringLiteral("mime_subtype_") + normalizar_chave_mime(subtype);
        const QString subtype_text = get_text(subtype_key, humanizar_subtipo_mime(subtype));
        return QStringLiteral("%1: %2 (%3)").arg(primary_text, subtype_text, raw);
    }

    QString traduzir_metadados(const QString& field_key, const QString& valor) const {
        const QString cache_key =
            field_key + QLatin1Char('\x1f') + valor + QLatin1Char('\x1f') + idioma_exibicao_;
        const auto it_cache = cache_metadados_.constFind(cache_key);
        if (it_cache != cache_metadados_.constEnd()) {
            return it_cache.value();
        }

        QString resultado = valor;
        const QString field_lower = field_key.toLower();
        if (field_lower == QStringLiteral("tipo") || field_lower == QStringLiteral("type")) {
            resultado = traduzir_tipo_arquivo(valor);
        } else if (field_lower == QStringLiteral("atributos") || field_lower == QStringLiteral("attributes")) {
            resultado = traduzir_atributos(valor);
        } else if (field_lower == QStringLiteral("autor") || field_lower == QStringLiteral("author")) {
            resultado = traduzir_autor(valor);
        } else if (field_lower == QStringLiteral("protegido") || field_lower == QStringLiteral("protected")) {
            resultado = traduzir_protegido(valor);
        } else if (field_lower == QStringLiteral("dimensoes") || field_lower == QStringLiteral("dimensions")) {
            resultado = traduzir_dimensoes(valor);
        } else if (field_lower == QStringLiteral("mime")) {
            resultado = traduzir_mime(valor);
        }

        cache_metadados_.insert(cache_key, resultado);
        return resultado;
    }

    QString idioma_exibicao_;
    QString translations_dir_;
    mutable QTranslator translator_;
    bool translator_loaded_ = false;
    mutable QHash<QString, QString> cache_texto_;
    mutable QHash<QString, QString> cache_operacao_;
    mutable QHash<QString, QString> cache_metadados_;
    mutable QHash<QString, QString> cache_seguranca_;
};

EventTableModel::EventTableModel(QObject* interface_obj,
                                 const QString& idioma_exibicao,
                                 QObject* parent)
    : QAbstractTableModel(parent),
      interface_(interface_obj),
      idioma_exibicao_(idioma_exibicao.isEmpty()
                           ? QStringLiteral("pt_BR")
                           : idioma_exibicao),
      colunas_disponiveis_(construir_colunas_ordenadas(interface_obj)),
      flush_prepend_agendado_(false),
      tabela_view_(nullptr),
      localizer_(new FixedLanguageLocalizer(idioma_exibicao_, translations_dir_from_interface(interface_obj))),
      cache_rotulo_inicializado_(false),
      cache_brushes_inicializado_(false),
      coluna_ordenacao_ativa_(-1),
      ordem_ordenacao_ativa_(Qt::AscendingOrder) {
    cores_padrao_operacao_.insert(QStringLiteral("op_renamed"), QColor(0, 255, 0));
    cores_padrao_operacao_.insert(QStringLiteral("op_added"), QColor(0, 0, 255));
    cores_padrao_operacao_.insert(QStringLiteral("op_deleted"), QColor(255, 0, 0));
    cores_padrao_operacao_.insert(QStringLiteral("op_modified"), QColor(255, 98, 0));
    cores_padrao_operacao_.insert(QStringLiteral("op_moved"), QColor(255, 0, 255));
    cores_padrao_operacao_.insert(QStringLiteral("op_scanned"), QColor(128, 128, 128));
    invalidate_color_cache();
}

EventTableModel::~EventTableModel() = default;

int EventTableModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(data_.size());
}

int EventTableModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : colunas_disponiveis_.size();
}

QVariant EventTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }
    if (orientation == Qt::Vertical) {
        return section + 1;
    }
    if (section < 0 || section >= colunas_disponiveis_.size()) {
        return {};
    }

    const QVariantMap coluna = colunas_disponiveis_.at(section).second;
    const QString translation_key =
        coluna.value(QStringLiteral("translation_key"), colunas_disponiveis_.at(section).first).toString();
    const QString fallback =
        coluna.value(QStringLiteral("nome"), colunas_disponiveis_.at(section).first).toString();
    return localizer_ ? localizer_->get_text(translation_key, fallback) : fallback;
}

QVariant EventTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return {};
    }
    if (index.row() < 0 || index.row() >= static_cast<int>(data_.size())) {
        return {};
    }
    if (index.column() < 0 || index.column() >= colunas_disponiveis_.size()) {
        return {};
    }

    const QString key = colunas_disponiveis_[index.column()].first;
    const QVariantMap& row_data = data_[static_cast<size_t>(index.row())];
    if (role == EventDataRole) {
        return row_data;
    }

    if (role == Qt::DisplayRole) {
        return valor_exibicao_traduzido(key, row_data);
    }

    if (role != Qt::BackgroundRole && role != Qt::ForegroundRole) {
        return {};
    }

    auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
        interface_ != nullptr ? interface_->property("gerenciador_tabela").value<QObject*>() : nullptr);
    if (gerenciador_tabela == nullptr) {
        return {};
    }

    reconstruir_cache_brushes();

    const bool eh_coluna_tipo_operacao = (key == QStringLiteral("tipo_operacao"));
    if (!eh_coluna_tipo_operacao && !gerenciador_tabela->colunas_para_colorir.contains(key)) {
        if (role == Qt::ForegroundRole) {
            return cache_brush_texto_padrao_;
        }
        return {};
    }

    if (!gerenciador_tabela->cores_visiveis) {
        if (role == Qt::ForegroundRole) {
            return cache_brush_texto_padrao_;
        }
        return {};
    }

    QString op_key_resolvida = resolver_chave_operacao(row_data.value(QStringLiteral("tipo_operacao")).toString());
    if (op_key_resolvida.isEmpty()) {
        op_key_resolvida = QStringLiteral("op_scanned");
    }

    if (role == Qt::BackgroundRole) {
        return cache_bg_por_operacao_.value(op_key_resolvida, cache_bg_por_operacao_.value(QStringLiteral("op_scanned")));
    }
    if (eh_coluna_tipo_operacao) {
        return cache_fg_tipo_por_operacao_.value(op_key_resolvida, cache_brush_texto_padrao_);
    }
    return cache_fg_coluna_por_operacao_.value(op_key_resolvida, cache_brush_texto_padrao_);
}

void EventTableModel::agendar_flush_pendente(int delay_ms) {
    if (flush_prepend_agendado_) {
        return;
    }

    if (delay_ms < 0) {
        delay_ms = ordenacao_dinamica_ativa() ? 40 : 16;
    }

    flush_prepend_agendado_ = true;
    QTimer::singleShot(delay_ms, this, [this]() { flush_pending_prepend(); });
}

bool EventTableModel::modelo_ativo() const {
    return interface_ != nullptr &&
           tabela_view_ != nullptr &&
           interface_->property("tabela_dados").value<QObject*>() == tabela_view_;
}

bool EventTableModel::ordenacao_dinamica_ativa() const {
    return interface_ != nullptr &&
           interface_->property("_ordenacao_linhas_habilitada").toBool() &&
           coluna_ordenacao_ativa_ >= 0 &&
           coluna_ordenacao_ativa_ < colunas_disponiveis_.size() &&
           modelo_ativo();
}

int EventTableModel::comparar_registros(const QVariantMap& lhs,
                                        const QVariantMap& rhs,
                                        const QString& key,
                                        const QCollator& collator) const {
    const QVariant lhs_raw = lhs.value(key);
    const QVariant rhs_raw = rhs.value(key);

    const QString lhs_display = valor_exibicao_traduzido(key, lhs).trimmed();
    const QString rhs_display = valor_exibicao_traduzido(key, rhs).trimmed();
    const bool lhs_empty = lhs_display.isEmpty();
    const bool rhs_empty = rhs_display.isEmpty();
    if (lhs_empty != rhs_empty) {
        return lhs_empty ? 1 : -1;
    }

    if (datetime_sort_keys().contains(key)) {
        const QDateTime lhs_dt = parse_sort_datetime(lhs_raw);
        const QDateTime rhs_dt = parse_sort_datetime(rhs_raw);
        if (lhs_dt.isValid() && rhs_dt.isValid()) {
            if (lhs_dt < rhs_dt) return -1;
            if (lhs_dt > rhs_dt) return 1;
            return 0;
        }

        const int cmp_data = collator.compare(lhs_raw.toString().trimmed(), rhs_raw.toString().trimmed());
        if (cmp_data != 0) {
            return cmp_data;
        }
    }

    double lhs_number = 0.0;
    double rhs_number = 0.0;
    const bool eh_coluna_numerica = numeric_sort_keys().contains(key);
    const bool lhs_is_number = try_parse_number(lhs_raw, &lhs_number);
    const bool rhs_is_number = try_parse_number(rhs_raw, &rhs_number);
    if ((eh_coluna_numerica && lhs_is_number && rhs_is_number) ||
        (!eh_coluna_numerica && lhs_is_number && rhs_is_number &&
         !lhs_display.isEmpty() && !rhs_display.isEmpty())) {
        if (lhs_number < rhs_number) return -1;
        if (lhs_number > rhs_number) return 1;
        return 0;
    }

    const int display_cmp = collator.compare(lhs_display, rhs_display);
    if (display_cmp != 0) {
        return display_cmp;
    }

    const int raw_cmp = collator.compare(lhs_raw.toString().trimmed(), rhs_raw.toString().trimmed());
    if (raw_cmp != 0) {
        return raw_cmp;
    }

    return 0;
}

void EventTableModel::sort(int column, Qt::SortOrder order) {
    if (column < 0 || column >= colunas_disponiveis_.size()) {
        coluna_ordenacao_ativa_ = -1;
        ordem_ordenacao_ativa_ = Qt::AscendingOrder;
        if (interface_ != nullptr) {
            interface_->setProperty("_ordenacao_linhas_coluna", -1);
            interface_->setProperty("_ordenacao_linhas_ordem", static_cast<int>(Qt::AscendingOrder));
        }
        return;
    }

    while (!pending_prepend_.empty()) {
        data_.push_back(std::move(pending_prepend_.front()));
        pending_prepend_.pop_front();
    }
    flush_prepend_agendado_ = false;

    const QString key = colunas_disponiveis_.at(column).first;
    QCollator collator{QLocale(idioma_exibicao_)};
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);
    collator.setIgnorePunctuation(false);

    emit layoutAboutToBeChanged();
    std::stable_sort(data_.begin(), data_.end(),
                     [this, &collator, &key, order](const QVariantMap& lhs, const QVariantMap& rhs) {
                         const int cmp = comparar_registros(lhs, rhs, key, collator);
                         if (cmp == 0) {
                             return false;
                         }
                         return order == Qt::AscendingOrder ? (cmp < 0) : (cmp > 0);
                     });
    emit layoutChanged();

    coluna_ordenacao_ativa_ = column;
    ordem_ordenacao_ativa_ = order;
    if (interface_ != nullptr) {
        interface_->setProperty("_ordenacao_linhas_coluna", column);
        interface_->setProperty("_ordenacao_linhas_ordem", static_cast<int>(order));
    }
}

void EventTableModel::prepend_event(const QVariantMap& evento) {
    if (evento.isEmpty()) {
        return;
    }
    pending_prepend_.push_back(evento);
    agendar_flush_pendente(static_cast<int>(pending_prepend_.size()) >= kBatchInsertChunkSize ? 0 : -1);
}

void EventTableModel::append_events(const QVariantList& eventos) {
    if (eventos.isEmpty()) {
        return;
    }
    for (const QVariant& e : eventos) {
        pending_prepend_.push_back(e.toMap());
    }
    agendar_flush_pendente(static_cast<int>(pending_prepend_.size()) >= kBatchInsertChunkSize ? 0 : -1);
}

void EventTableModel::replace_events(const QVariantList& eventos) {
    pending_prepend_.clear();
    flush_prepend_agendado_ = false;
    invalidate_color_cache();
    beginResetModel();
    data_.clear();
    for (const QVariant& evento : eventos) {
        data_.push_back(evento.toMap());
    }
    endResetModel();

    if (ordenacao_dinamica_ativa()) {
        sort(coluna_ordenacao_ativa_, ordem_ordenacao_ativa_);
    }
}

QVariantList EventTableModel::snapshot_events() const {
    const_cast<EventTableModel*>(this)->flush_pending_prepend();

    QVariantList snapshot;
    snapshot.reserve(static_cast<int>(data_.size()));
    for (const QVariantMap& row : data_) {
        snapshot.append(row);
    }
    return snapshot;
}

QString EventTableModel::column_key(int model_column) const {
    if (model_column < 0 || model_column >= colunas_disponiveis_.size()) {
        return {};
    }
    return colunas_disponiveis_[model_column].first;
}

int EventTableModel::column_index_for_key(const QString& key) const {
    return indice_coluna_por_chave(key);
}

void EventTableModel::invalidate_color_cache() {
    cache_tipo_para_operacao_.clear();
    cache_rotulo_para_operacao_.clear();
    cache_bg_por_operacao_.clear();
    cache_fg_tipo_por_operacao_.clear();
    cache_fg_coluna_por_operacao_.clear();
    cache_brush_texto_padrao_ = QBrush();
    cache_rotulo_inicializado_ = false;
    cache_brushes_inicializado_ = false;
}

void EventTableModel::notify_coloring_changed(const QSet<QString>& colunas_afetadas) {
    invalidate_color_cache();

    const int total_linhas = rowCount();
    const int total_colunas = columnCount();
    if (total_linhas <= 0 || total_colunas <= 0) {
        return;
    }

    const QList<int> roles = {Qt::BackgroundRole, Qt::ForegroundRole};
    int primeira_linha = 0;
    int ultima_linha = total_linhas - 1;

    QTableView* tabela_ref = tabela_view_;
    if (tabela_ref == nullptr) {
        tabela_ref = qobject_cast<QTableView*>(
            interface_ != nullptr ? interface_->property("tabela_dados").value<QObject*>() : nullptr);
    }

    if (tabela_ref != nullptr && tabela_ref->viewport() != nullptr) {
        const int fallback_linhas_visiveis = 256;
        const QModelIndex idx_topo = tabela_ref->indexAt(QPoint(1, 1));
        const QModelIndex idx_base =
            tabela_ref->indexAt(QPoint(std::max(1, tabela_ref->viewport()->width() - 2),
                                       std::max(1, tabela_ref->viewport()->height() - 2)));

        if (idx_topo.isValid()) {
            primeira_linha = idx_topo.row();
        } else {
            const int row_topo = tabela_ref->rowAt(0);
            if (row_topo >= 0) {
                primeira_linha = row_topo;
            }
        }

        if (idx_base.isValid()) {
            ultima_linha = idx_base.row();
        } else {
            const int row_base = tabela_ref->rowAt(tabela_ref->viewport()->height() - 1);
            if (row_base >= 0) {
                ultima_linha = row_base;
            } else {
                ultima_linha = std::min(total_linhas - 1, primeira_linha + fallback_linhas_visiveis);
            }
        }

        if (primeira_linha < 0) {
            primeira_linha = 0;
        }
        if (ultima_linha < primeira_linha) {
            ultima_linha = std::min(total_linhas - 1, primeira_linha + fallback_linhas_visiveis);
        } else {
            ultima_linha = std::min(ultima_linha, total_linhas - 1);
        }
    }

    if (colunas_afetadas.isEmpty()) {
        emit dataChanged(index(primeira_linha, 0), index(ultima_linha, total_colunas - 1), roles);
        return;
    }

    QList<int> indices_colunas;
    indices_colunas.reserve(colunas_afetadas.size());
    for (const QString& chave : colunas_afetadas) {
        const int idx = indice_coluna_por_chave(chave);
        if (idx >= 0) {
            indices_colunas.append(idx);
        }
    }

    if (indices_colunas.isEmpty()) {
        emit dataChanged(index(primeira_linha, 0), index(ultima_linha, total_colunas - 1), roles);
        return;
    }

    std::sort(indices_colunas.begin(), indices_colunas.end());
    indices_colunas.erase(std::unique(indices_colunas.begin(), indices_colunas.end()), indices_colunas.end());
    for (const int idx_coluna : indices_colunas) {
        emit dataChanged(index(primeira_linha, idx_coluna), index(ultima_linha, idx_coluna), roles);
    }
}

void EventTableModel::bind_view(QTableView* view) {
    tabela_view_ = view;
}

void EventTableModel::refresh_headers() {
    sincronizar_colunas_do_gerenciador();
    if (columnCount() > 0) {
        emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
    }
}

QString EventTableModel::idioma_exibicao() const {
    return idioma_exibicao_;
}

void EventTableModel::flush_pending_prepend() {
    flush_prepend_agendado_ = false;
    if (pending_prepend_.empty()) {
        return;
    }

    QList<QVariantMap> lote;
    const int tamanho_lote = std::min(static_cast<int>(pending_prepend_.size()), kBatchInsertChunkSize);
    lote.reserve(tamanho_lote);
    while (!pending_prepend_.empty() && lote.size() < tamanho_lote) {
        lote.append(std::move(pending_prepend_.front()));
        pending_prepend_.pop_front();
    }
    const bool possui_mais_lotes = !pending_prepend_.empty();

    if (!ordenacao_dinamica_ativa()) {
        beginInsertRows(QModelIndex(), 0, lote.size() - 1);
        for (auto it = lote.rbegin(); it != lote.rend(); ++it) {
            data_.push_front(std::move(*it));
        }
        endInsertRows();
        if (possui_mais_lotes) {
            agendar_flush_pendente(0);
        }
        return;
    }

    const QString key = colunas_disponiveis_.at(coluna_ordenacao_ativa_).first;
    QCollator collator{QLocale(idioma_exibicao_)};
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    collator.setNumericMode(true);
    collator.setIgnorePunctuation(false);

    std::stable_sort(lote.begin(), lote.end(),
                     [this, &collator, &key](const QVariantMap& lhs, const QVariantMap& rhs) {
                         const int cmp = comparar_registros(lhs, rhs, key, collator);
                         if (cmp == 0) {
                             return false;
                         }
                         return ordem_ordenacao_ativa_ == Qt::AscendingOrder ? (cmp < 0) : (cmp > 0);
                     });

    emit layoutAboutToBeChanged();
    std::deque<QVariantMap> mesclado;
    auto it_existente = data_.cbegin();
    auto it_lote = lote.cbegin();
    while (it_existente != data_.cend() && it_lote != lote.cend()) {
        const int cmp = comparar_registros(*it_existente, *it_lote, key, collator);
        const bool manter_existente =
            ordem_ordenacao_ativa_ == Qt::AscendingOrder ? (cmp <= 0) : (cmp >= 0);
        if (manter_existente) {
            mesclado.push_back(*it_existente);
            ++it_existente;
        } else {
            mesclado.push_back(*it_lote);
            ++it_lote;
        }
    }
    while (it_existente != data_.cend()) {
        mesclado.push_back(*it_existente);
        ++it_existente;
    }
    while (it_lote != lote.cend()) {
        mesclado.push_back(*it_lote);
        ++it_lote;
    }
    data_ = std::move(mesclado);
    emit layoutChanged();

    if (possui_mais_lotes) {
        agendar_flush_pendente(0);
    }
}

QString EventTableModel::resolver_chave_operacao(const QString& tipo_valor) const {
    const QString tipo_normalizado = normalizar_tipo(tipo_valor);
    if (tipo_normalizado.isEmpty()) {
        return {};
    }

    const auto it_cache = cache_tipo_para_operacao_.constFind(tipo_normalizado);
    if (it_cache != cache_tipo_para_operacao_.constEnd()) {
        return it_cache.value();
    }

    reconstruir_cache_rotulos_operacao();
    const QString op_key_resolvida = cache_rotulo_para_operacao_.value(tipo_normalizado);
    cache_tipo_para_operacao_.insert(tipo_normalizado, op_key_resolvida);
    return op_key_resolvida;
}

void EventTableModel::reconstruir_cache_rotulos_operacao() const {
    if (cache_rotulo_inicializado_) {
        return;
    }

    cache_rotulo_para_operacao_.clear();
    for (const QString& chave : chaves_operacao_base()) {
        cache_rotulo_para_operacao_.insert(normalizar_tipo(chave), chave);
        if (localizer_) {
            cache_rotulo_para_operacao_.insert(normalizar_tipo(localizer_->get_text(chave, chave)), chave);
        }
    }

    static const QHash<QString, QStringList> aliases = {
        {QStringLiteral("op_renamed"), {QStringLiteral("renamed"), QStringLiteral("renomeado"), QStringLiteral("renombrado"), QStringLiteral("renommé"), QStringLiteral("rinominato"), QStringLiteral("umbenannt")}},
        {QStringLiteral("op_added"), {QStringLiteral("added"), QStringLiteral("adicionado"), QStringLiteral("añadido"), QStringLiteral("ajouté"), QStringLiteral("aggiunto"), QStringLiteral("hinzugefügt")}},
        {QStringLiteral("op_deleted"), {QStringLiteral("deleted"), QStringLiteral("excluído"), QStringLiteral("eliminado"), QStringLiteral("eliminato"), QStringLiteral("supprimé"), QStringLiteral("gelöscht")}},
        {QStringLiteral("op_modified"), {QStringLiteral("modified"), QStringLiteral("modificado"), QStringLiteral("modifié"), QStringLiteral("modificato"), QStringLiteral("geändert")}},
        {QStringLiteral("op_moved"), {QStringLiteral("moved"), QStringLiteral("movido"), QStringLiteral("déplacé"), QStringLiteral("spostato"), QStringLiteral("verschoben")}},
        {QStringLiteral("op_scanned"), {QStringLiteral("scanned"), QStringLiteral("escaneado"), QStringLiteral("numérisé"), QStringLiteral("scansionato"), QStringLiteral("gescannt"), QStringLiteral("scan")}},
    };

    for (auto it = aliases.begin(); it != aliases.end(); ++it) {
        for (const QString& alias : it.value()) {
            cache_rotulo_para_operacao_.insert(normalizar_tipo(alias), it.key());
        }
    }

    cache_rotulo_inicializado_ = true;
}

void EventTableModel::reconstruir_cache_brushes() const {
    if (cache_brushes_inicializado_) {
        return;
    }

    cache_bg_por_operacao_.clear();
    cache_fg_tipo_por_operacao_.clear();
    cache_fg_coluna_por_operacao_.clear();

    auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
        interface_ != nullptr ? interface_->property("gerenciador_tabela").value<QObject*>() : nullptr);
    QColor texto_padrao = QApplication::palette().color(QPalette::Text);
    if (gerenciador_tabela != nullptr) {
        texto_padrao = gerenciador_tabela->calcular_cor_texto_ideal(QColor(), false);
    }
    cache_brush_texto_padrao_ = QBrush(texto_padrao);

    if (gerenciador_tabela == nullptr) {
        cache_brushes_inicializado_ = true;
        return;
    }

    auto* gerenciador_cores = qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(
        interface_ != nullptr ? interface_->property("gerenciador_cores").value<QObject*>() : nullptr);
    const bool coluna_tipo_personalizada =
        gerenciador_tabela->eh_coluna_personalizada_colorida(QStringLiteral("tipo_operacao"));

    for (const QString& chave_operacao : chaves_operacao_base()) {
        QColor cor_fundo = cores_padrao_operacao_.value(chave_operacao, QColor(255, 255, 255));
        if (gerenciador_cores != nullptr) {
            const QColor cor_custom(gerenciador_cores->obter_cor_hex(chave_operacao));
            if (cor_custom.isValid()) {
                cor_fundo = cor_custom;
            }
        }

        cache_bg_por_operacao_.insert(chave_operacao, QBrush(cor_fundo));
        cache_fg_tipo_por_operacao_.insert(
            chave_operacao,
            QBrush(gerenciador_tabela->calcular_cor_texto_ideal(cor_fundo, coluna_tipo_personalizada)));
        cache_fg_coluna_por_operacao_.insert(
            chave_operacao,
            QBrush(gerenciador_tabela->calcular_cor_texto_ideal(cor_fundo, true)));
    }

    cache_brushes_inicializado_ = true;
}

int EventTableModel::indice_coluna_por_chave(const QString& chave) const {
    for (int i = 0; i < colunas_disponiveis_.size(); ++i) {
        if (colunas_disponiveis_[i].first == chave) {
            return i;
        }
    }
    return -1;
}

void EventTableModel::sincronizar_colunas_do_gerenciador() {
    const QList<QPair<QString, QVariantMap>> novas_colunas = construir_colunas_ordenadas(interface_);
    if (novas_colunas.isEmpty()) {
        return;
    }

    bool estrutura_mudou = novas_colunas.size() != colunas_disponiveis_.size();
    if (!estrutura_mudou) {
        for (int i = 0; i < novas_colunas.size(); ++i) {
            if (novas_colunas.at(i).first != colunas_disponiveis_.at(i).first) {
                estrutura_mudou = true;
                break;
            }
        }
    }

    if (estrutura_mudou) {
        beginResetModel();
        colunas_disponiveis_ = novas_colunas;
        endResetModel();
        return;
    }

    for (int i = 0; i < novas_colunas.size(); ++i) {
        colunas_disponiveis_[i].second = novas_colunas.at(i).second;
    }
}

QString EventTableModel::valor_exibicao_traduzido(const QString& key, const QVariantMap& row_data) const {
    const QString raw_value = row_data.value(key).toString();
    return localizer_ ? localizer_->traduzir_valor(key, raw_value) : raw_value;
}

}  // namespace source::services::GerenciamentoTabela
