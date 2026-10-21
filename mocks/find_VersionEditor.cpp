#include "find_VersionEditor.h"

#include <algorithm>
#include <cctype>
#include <iostream>

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHash>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSet>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStringList>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace mocks {

namespace {

const QStringList PT_MONTHS = {"Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
const QStringList EN_MONTHS = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const QStringList ES_MONTHS = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
const QStringList FR_MONTHS = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};
const QStringList IT_MONTHS = {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"};
const QStringList DE_MONTHS = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};

QString find_release_doc_rel_path(const QString& base_dir) {
    const QString docs_dir = QDir(base_dir).filePath(QStringLiteral("docs"));
    QDir dir(docs_dir);
    const QStringList files = dir.entryList({QStringLiteral("release-v*.md")}, QDir::Files, QDir::Name);
    if (!files.isEmpty()) {
        return QStringLiteral("docs/%1").arg(files.first());
    }
    return QStringLiteral("docs/release-v2026.9.28.0.md");
}

QString determine_file_group(const QString& rel_path) {
    const QString normalized = QDir::fromNativeSeparators(rel_path);
    if (!normalized.contains('/')) {
        return QStringLiteral("RAIZ");
    }

    if (normalized.startsWith(QStringLiteral("source/assets/"))) {
        const QString rest = normalized.mid(QStringLiteral("source/assets/").size());
        const int slash_idx = rest.indexOf('/');
        if (slash_idx > 0) {
            return rest.left(slash_idx);
        }
        return QStringLiteral("assets");
    }

    if (normalized.startsWith(QStringLiteral("source/cli/"))) {
        return QStringLiteral("CLI");
    }

    if (normalized.startsWith(QStringLiteral("source/ui/GerenciamentoMenusUI/"))) {
        return QStringLiteral("GerenciamentoMenusUI");
    }

    if (normalized.startsWith(QStringLiteral("source/ui/"))) {
        const QString rest = normalized.mid(QStringLiteral("source/ui/").size());
        const int slash_idx = rest.indexOf('/');
        if (slash_idx > 0) {
            return rest.left(slash_idx);
        }
        return QStringLiteral("UI");
    }

    if (normalized.startsWith(QStringLiteral("docs/"))) {
        return QStringLiteral("DOCS");
    }

    if (normalized.startsWith(QStringLiteral("repo/"))) {
        return QStringLiteral("REPO");
    }

    const int first_slash = normalized.indexOf('/');
    if (first_slash > 0) {
        return normalized.left(first_slash).toUpper();
    }

    return QStringLiteral("RAIZ");
}

QString find_window_icon_path() {
    const QString base_dir = project_root_dir();
    const QStringList candidates = {
        QDir(base_dir).filePath("source/assets/images/icons/linceu_sem_fundo.ico"),
        QDir(base_dir).filePath("source/assets/icones/linceu_sem_fundo.ico"),
        QDir(base_dir).filePath("source/assets/icons/icon_256x256.png")
    };

    for (const QString& candidate : candidates) {
        const QString absolute_path = QFileInfo(candidate).absoluteFilePath();
        if (QFileInfo::exists(absolute_path)) {
            return absolute_path;
        }
    }

    return {};
}

}  // namespace

QString readme_version(const QString& version) {
    QString normalized = version.trimmed();
    if (!normalized.startsWith('v', Qt::CaseInsensitive)) {
        normalized.prepend('v');
    }
    return normalized;
}

namespace {

QString plain_version(const QString& version) {
    QString normalized = version.trimmed();
    if (normalized.startsWith('v', Qt::CaseInsensitive)) {
        normalized.remove(0, 1);
    }
    return normalized;
}

int parse_month_name_to_index(const QString& month_name) {
    QString norm = month_name.trimmed().toLower();

    bool is_num = false;
    int num = norm.toInt(&is_num);
    if (is_num && num >= 1 && num <= 12) {
        return num - 1;
    }

    norm.replace(QStringLiteral("á"), QStringLiteral("a"))
        .replace(QStringLiteral("ã"), QStringLiteral("a"))
        .replace(QStringLiteral("à"), QStringLiteral("a"))
        .replace(QStringLiteral("ä"), QStringLiteral("a"))
        .replace(QStringLiteral("é"), QStringLiteral("e"))
        .replace(QStringLiteral("ê"), QStringLiteral("e"))
        .replace(QStringLiteral("è"), QStringLiteral("e"))
        .replace(QStringLiteral("ë"), QStringLiteral("e"))
        .replace(QStringLiteral("í"), QStringLiteral("i"))
        .replace(QStringLiteral("ì"), QStringLiteral("i"))
        .replace(QStringLiteral("ï"), QStringLiteral("i"))
        .replace(QStringLiteral("ó"), QStringLiteral("o"))
        .replace(QStringLiteral("õ"), QStringLiteral("o"))
        .replace(QStringLiteral("ô"), QStringLiteral("o"))
        .replace(QStringLiteral("ö"), QStringLiteral("o"))
        .replace(QStringLiteral("ú"), QStringLiteral("u"))
        .replace(QStringLiteral("ù"), QStringLiteral("u"))
        .replace(QStringLiteral("ü"), QStringLiteral("u"))
        .replace(QStringLiteral("ç"), QStringLiteral("c"))
        .replace(QStringLiteral("ñ"), QStringLiteral("n"))
        .replace(QStringLiteral("ß"), QStringLiteral("ss"));

    // 0: Janeiro (pt), January (en), Enero (es), Janvier (fr), Gennaio (it), Januar (de)
    if (norm.startsWith("jan") || norm.startsWith("ene") || norm.startsWith("gen")) return 0;

    // 1: Fevereiro (pt), February (en), Febrero (es), Février (fr), Febbraio (it), Februar (de)
    if (norm.startsWith("fev") || norm.startsWith("feb")) return 1;

    // 2: Março (pt), March (en), Marzo (es/it), Mars (fr), März (de)
    if (norm.startsWith("mar")) return 2;

    // 3: Abril (pt/es), April (en/de), Avril (fr), Aprile (it)
    if (norm.startsWith("abr") || norm.startsWith("apr") || norm.startsWith("avr") || norm.startsWith("av")) return 3;

    // 4: Maio (pt), May (en), Mayo (es), Mai (fr/de), Maggio (it)
    if (norm.startsWith("mai") || norm.startsWith("may") || norm.startsWith("mag")) return 4;

    // 5: Junho (pt), June (en), Junio (es), Juin (fr), Giugno (it), Juni (de)
    if (norm.startsWith("jun") || norm.startsWith("giu") || norm.startsWith("juin")) return 5;

    // 6: Julho (pt), July (en), Julio (es), Juillet (fr), Luglio (it), Juli (de)
    if (norm.startsWith("jul") || norm.startsWith("lug") || norm.startsWith("juil")) return 6;

    // 7: Agosto (pt/es/it), August (en/de), Août (fr)
    if (norm.startsWith("ago") || norm.startsWith("aug") || norm.startsWith("aou") || norm.startsWith("aot")) return 7;

    // 8: Setembro (pt), September (en/de), Septiembre (es), Septembre (fr), Settembre (it)
    if (norm.startsWith("set") || norm.startsWith("sep")) return 8;

    // 9: Outubro (pt), October (en), Octubre (es), Octobre (fr), Ottobre (it), Oktober (de)
    if (norm.startsWith("out") || norm.startsWith("oct") || norm.startsWith("okt") || norm.startsWith("ott")) return 9;

    // 10: Novembro (pt), November (en/de), Noviembre (es), Novembre (fr/it)
    if (norm.startsWith("nov")) return 10;

    // 11: Dezembro (pt), December (en), Diciembre (es), Décembre (fr), Dicembre (it), Dezember (de)
    if (norm.startsWith("dez") || norm.startsWith("dec") || norm.startsWith("dic")) return 11;

    return -1;
}

bool parse_date_string(const QString& raw_text, int& day, int& month_index, int& year) {
    QString text = raw_text.trimmed();
    if (text.startsWith('`') && text.endsWith('`') && text.size() >= 2) {
        text = text.mid(1, text.size() - 2).trimmed();
    }

    // 1. Textual: e.g. "28 de setembro de 2026", "28 September 2026", "28. September 2026", "28 de septiembre de 2026"
    static const QRegularExpression rx_textual_day_first(
        R"((\d{1,2})\.?\s+(?:de\s+|del\s+|d'|du\s+)?([A-Za-zÀ-ÿ]+)(?:\s+(?:de|del|d'|du))?\s+(\d{4}))",
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch m = rx_textual_day_first.match(text);
    if (m.hasMatch()) {
        int d = m.captured(1).toInt();
        int y = m.captured(3).toInt();
        int m_idx = parse_month_name_to_index(m.captured(2));
        if (m_idx >= 0 && d >= 1 && d <= 31 && y >= 1900 && y <= 3000) {
            day = d;
            month_index = m_idx;
            year = y;
            return true;
        }
    }

    // 2. Textual month first: e.g. "September 28, 2026", "September 28th, 2026"
    static const QRegularExpression rx_textual_month_first(
        R"(([A-Za-zÀ-ÿ]+)\s+(\d{1,2})(?:st|nd|rd|th)?,\s*(\d{4}))",
        QRegularExpression::CaseInsensitiveOption);
    m = rx_textual_month_first.match(text);
    if (m.hasMatch()) {
        int m_idx = parse_month_name_to_index(m.captured(1));
        int d = m.captured(2).toInt();
        int y = m.captured(3).toInt();
        if (m_idx >= 0 && d >= 1 && d <= 31 && y >= 1900 && y <= 3000) {
            day = d;
            month_index = m_idx;
            year = y;
            return true;
        }
    }

    // 3. Numeric ISO: e.g. "2026-09-28", "2026/09/28"
    static const QRegularExpression rx_numeric_iso(
        R"(\b(\d{4})[-/](\d{1,2})[-/](\d{1,2})\b)");
    m = rx_numeric_iso.match(text);
    if (m.hasMatch()) {
        int y = m.captured(1).toInt();
        int mo = m.captured(2).toInt();
        int d = m.captured(3).toInt();
        if (mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && y >= 1900 && y <= 3000) {
            day = d;
            month_index = mo - 1;
            year = y;
            return true;
        }
    }

    // 4. Numeric DMY: e.g. "28/09/2026", "28.09.2026"
    static const QRegularExpression rx_numeric_dmy(
        R"((\d{1,2})[-/.](\d{1,2})[-/.](\d{4}))");
    m = rx_numeric_dmy.match(text);
    if (m.hasMatch()) {
        int d = m.captured(1).toInt();
        int mo = m.captured(2).toInt();
        int y = m.captured(3).toInt();
        if (mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && y >= 1900 && y <= 3000) {
            day = d;
            month_index = mo - 1;
            year = y;
            return true;
        }
    }

    return false;
}

bool parse_version_string(const QString& raw_text, QString& version_out) {
    static const QRegularExpression rx_ver(R"(\b(v?[0-9]+(?:\.[0-9]+){1,3})\b)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch m = rx_ver.match(raw_text);
    if (m.hasMatch()) {
        version_out = m.captured(1);
        return true;
    }
    return false;
}

QString read_file_robust(const QString& path, bool* ok_out = nullptr) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (ok_out) *ok_out = false;
        return {};
    }

    const QByteArray bytes = file.readAll();
    QString text = QString::fromUtf8(bytes);
    if (text.contains(QChar::ReplacementCharacter)) {
        text = QString::fromLatin1(bytes);
    }

    if (text.startsWith(QChar(0xFEFF))) text.remove(0, 1);
    if (ok_out) *ok_out = true;
    return text;
}

bool write_file_utf8(const QString& path, const QString& text, QString* err = nullptr) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (err) *err = file.errorString();
        return false;
    }
    const QByteArray out = text.toUtf8();
    if (file.write(out) != out.size()) {
        if (err) *err = file.errorString();
        return false;
    }
    return true;
}

QString replace_line_by_regex(const QString& text, const QString& pattern, const QString& replacement_full_line) {
    QRegularExpression re(pattern, QRegularExpression::MultilineOption);
    QString out = text;
    QRegularExpressionMatchIterator it = re.globalMatch(text);
    QList<QPair<int, int>> spans;
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        spans.append({m.capturedStart(0), m.capturedLength(0)});
    }
    for (int i = spans.size() - 1; i >= 0; --i) {
        out.replace(spans[i].first, spans[i].second, replacement_full_line);
    }
    return out;
}

QString replace_regex_capture(const QString& text, const QString& pattern, int capture_index, const QString& replacement) {
    struct CaptureReplacement {
        qsizetype start;
        qsizetype length;
        QString replacement;
    };

    QRegularExpression re(pattern, QRegularExpression::MultilineOption);
    QString out = text;
    QRegularExpressionMatchIterator it = re.globalMatch(text);
    QList<CaptureReplacement> replacements;

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        if (match.capturedStart(capture_index) >= 0) {
            replacements.append({match.capturedStart(capture_index), match.capturedLength(capture_index), replacement});
        }
    }

    for (int i = replacements.size() - 1; i >= 0; --i) {
        out.replace(replacements[i].start, replacements[i].length, replacements[i].replacement);
    }

    return out;
}

QString resolve_file_path(const QString& base_dir, const QString& rel_path) {
    const QDir base(base_dir);
    const QString candidate = QDir::cleanPath(base.filePath(rel_path));
    if (QFileInfo::exists(candidate)) {
        return QFileInfo(candidate).absoluteFilePath();
    }

    QString alt_rel = rel_path;
    if (alt_rel.startsWith("source/")) {
        alt_rel = alt_rel.mid(QString("source/").size());
    }
    const QString alt = QDir::cleanPath(base.filePath(alt_rel));
    return QFileInfo(QFileInfo::exists(alt) ? alt : candidate).absoluteFilePath();
}

QHash<QString, QStringList> build_patterns() {
    return {
        {"privacy_pt", {R"(^(Versão:?\s*.*)$)", R"(^(Última atualização:?\s*.*)$)"}},
        {"privacy_en", {R"(^(Version:?\s*.*)$)", R"(^(Last updated:?\s*.*)$)"}},
        {"privacy_es", {R"(^(Versión:?\s*.*)$)", R"(^(Última actualización:?\s*.*)$)"}},
        {"privacy_fr", {R"(^(Version:?\s*.*)$)", R"(^(Dernière mise à jour\s*:?\s*.*)$)"}},
        {"privacy_it", {R"(^(Versione:?\s*.*)$)", R"(^(Ultimo aggiornamento:?\s*.*)$)"}},
        {"privacy_de", {R"(^(Version:?\s*.*)$)", R"(^(Letzte Aktualisierung:?\s*.*)$)"}},

        {"eula_pt", {R"(^(Versão.*)$)"}},
        {"eula_en", {R"(^(Version.*)$)"}},
        {"eula_es", {R"(^(Versión.*)$)"}},
        {"eula_fr", {R"(^(Version.*)$)"}},
        {"eula_it", {R"(^(Versione.*)$)"}},
        {"eula_de", {R"(^(Version.*)$)"}},

        {"clc_pt", {R"(^(Versão.*)$)"}},
        {"clc_en", {R"(^(Version.*)$)"}},
        {"clc_es", {R"(^(Versión.*)$)"}},
        {"clc_fr", {R"(^(Version.*)$)"}},
        {"clc_it", {R"(^(Versione.*)$)"}},
        {"clc_de", {R"(^(Version.*)$)"}},

        {"about_pt", {R"(^(Versão:?\s*.*)$)"}},
        {"about_en", {R"(^(Version:?\s*.*)$)"}},
        {"about_es", {R"(^(Versión:?\s*.*)$)"}},
        {"about_fr", {R"(^(Version:?\s*.*)$)"}},
        {"about_it", {R"(^(Versione:?\s*.*)$)"}},
        {"about_de", {R"(^(Version:?\s*.*)$)"}},

        {"doc_architecture",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"doc_cli_reference",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"doc_compliance",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"doc_licensing",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"doc_release",
         {R"(^(# Notas de Lançamento / Release Notes — v[0-9]+(?:\.[0-9]+){1,3}.*)$)",
          R"(^(\*\*Tag de Vers(?:ã|a)o:\*\*\s*`v?[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data de Publica(?:çã|ca)o:\*\*\s*.*)$)",
          R"(^(\*\*Version Tag:\*\*\s*`v?[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Publication Date:\*\*\s*.*)$)"}},

        {"doc_security_model",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"doc_suggested_issues", {}},

        {"doc_user_guide",
         {R"(^(\*\*Vers(?:ã|a)o:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Data:\*\*\s*`[^`\r\n]+`.*)$)",
          R"(^(\*\*Version:\*\*\s*`[0-9]+(?:\.[0-9]+){1,3}`.*)$)",
          R"(^(\*\*Date:\*\*\s*`[^`\r\n]+`.*)$)"}},

        {"readme",
         {// 1. Nota Português (BR)
          R"(((?:>\s*)?\*\*Observação:\*\*.*?\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 2. Note English (US)
          R"(((?:>\s*)?\*\*Note:\*\*.*?\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 3. Nota Español (ES)
          R"(((?:>\s*)?\*\*Nota:\*\*.*?repositorio.*?versi(?:ó|o)n\s*\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 4. Remarque Français (FR)
          R"(((?:>\s*)?\*\*Remarque\s*:\*\*.*?\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 5. Nota Italiano (IT)
          R"(((?:>\s*)?\*\*Nota:\*\*.*?(?:repository|Questo repository).*?versione\s*\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 6. Hinweis Deutsch (DE)
          R"(((?:>\s*)?\*\*Hinweis:\*\*.*?\*\*v?[0-9]+(?:\.[0-9]+){1,3}\*\*))",
          // 7. Versão nos blocos de metadados
          R"(((?:Versão|Version|Versión|Versione)\s*:\s*v?[0-9]+(?:\.[0-9]+){1,3}(?:<br>)?))",
          // 8. Versione specifica in italiano
          R"(((?:Versione)\s*:\s*v?[0-9]+(?:\.[0-9]+){1,3}(?:<br>)?))",
          // 9. Versione in grassetto in italiano
          R"((\*\*Versione:\*\*\s*v?[0-9]+(?:\.[0-9]+){1,3}(?:<br>)?))"}},

        {"security_md",
         {R"((\|\s*`v?[0-9]+\.[0-9]+\.x`\s*\|\s*:white_check_mark:\s*\|))",
          R"((\|\s*`< v?[0-9]+\.[0-9]+`\s*\|\s*:x:\s*\|))"}},

        {"code_of_conduct_md", {}},
        {"contributing_md", {}},

        {"observacao",
         {R"(^(-\s*`?v?[0-9]+(?:\.[0-9]+){1,3}`?)\s*$)"}},

        {"manual",
         {R"(^((?:Versão documentada|Documented version|Versión documentada|Version documentée|Versione documentata|Dokumentierte Version)\s*:\s*`?.*`?.*)$)",
          R"(^((?:Data|Date|Fecha|Datum)\s*:\s*`?.*`?.*)$)"}},

        {"sobre_dialog_cpp",
         {R"(^(\s*"<b>%1:</b>\s*[0-9]+(?:\.[0-9]+){1,3}<br>"\s*)$)"}},

        {"exibir_sobre_cpp",
         {R"(^(\s*"<b>%1:</b>\s*[0-9]+(?:\.[0-9]+){1,3}<br>"\s*)$)"}},

        {"installer_default", {R"(^(#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){1,3}".*)$)"}},
        {"installer_mingw",
         {R"(^(#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){1,3}".*)$)",
          R"(^(OutputBaseFilename=.*_v[0-9]+(?:\.[0-9]+){1,3}.*)$)"}},
        {"installer_msvc",
         {R"(^(#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){1,3}".*)$)",
          R"(^(OutputBaseFilename=.*_v[0-9]+(?:\.[0-9]+){1,3}.*)$)"}}
    };
}

const QHash<QString, QStringList> PATTERNS = build_patterns();

QString update_about_cpp_version(const QString& text, const QString& version) {
    const QString normalized = plain_version(version);
    QString updated = replace_regex_capture(
        text,
        R"((\s*"<b>%1:</b>\s*)([0-9]+(?:\.[0-9]+){1,3})(<br>"\s*))",
        2,
        normalized);

    updated = replace_regex_capture(
        updated,
        R"((\+ QStringLiteral\("<p><b>"\)\s*\+\s*get_text\((?:loc_|loc),\s*"version",\s*"Version"\)\s*\+\s*QStringLiteral\(":</b> )([0-9]+(?:\.[0-9]+){1,3})(</p>"\)))",
        2,
        normalized);

    return updated;
}

QString update_installer_version(const QString& text, const QString& version) {
    const QString normalized = plain_version(version);
    QString updated = replace_regex_capture(
        text,
        R"((^#define\s+MyAppVersion\s+")([0-9]+(?:\.[0-9]+){1,3})("))",
        2,
        normalized);

    updated = replace_regex_capture(
        updated,
        R"((^OutputBaseFilename=.*_v)([0-9]+(?:\.[0-9]+){1,3})(\r?$))",
        2,
        normalized);

    return updated;
}

}  // namespace

QString update_readme_section(const QString& text, const QString& section_id, const QString& version) {
    const QString start_marker = QString("<h2 id=\"%1\">").arg(section_id);
    const int start_index = text.indexOf(start_marker);
    if (start_index < 0) {
        return text;
    }

    int end_index = text.indexOf("<h2 id=\"", start_index + start_marker.size());
    if (end_index < 0) {
        end_index = text.size();
    }

    QString section = text.mid(start_index, end_index - start_index);
    const QString target_version = readme_version(version);

    // 1. Bloco específico para Nota em Italiano (> **Nota:** Questo repository fa riferimento alla versione **v...** ...)
    section = replace_regex_capture(
        section,
        R"((^\s*(?:>\s*)?\*\*Nota:\*\*\s*Questo repository fa riferimento alla versione\s*\*\*)(v?[0-9]+(?:\.[0-9]+){1,3})(\*\*.*$))",
        2,
        target_version);

    // 1.1 Bloco geral de Observação/Nota/Remarque/Hinweis (> **...:** ... **v...** ...)
    section = replace_regex_capture(
        section,
        R"((^\s*(?:>\s*)?\*\*(?:Observação|Note|Nota|Remarque|Hinweis)\s*:\*\*.*?\*\*)(v?[0-9]+(?:\.[0-9]+){1,3})(\*\*.*$))",
        2,
        target_version);

    // 2. Linha simples de versão (Versão: v...<br> / Version : v...<br> / Versione: v...<br> / etc.)
    section = replace_regex_capture(
        section,
        R"((^\s*(?:Versão|Version|Versión|Versione)\s*:\s*)(v?[0-9]+(?:\.[0-9]+){1,3})((?:<br>)?\r?$))",
        2,
        target_version);

    // 3. Linha em negrito de versão (**Versão:** v...<br> / **Version :** v...<br> / **Versione:** v...<br> / etc.)
    section = replace_regex_capture(
        section,
        R"((^\s*\*\*(?:Versão|Version|Versión|Versione)\s*:\*\*\s*)(v?[0-9]+(?:\.[0-9]+){1,3})((?:<br>)?\r?$))",
        2,
        target_version);

    QString updated = text;
    updated.replace(start_index, end_index - start_index, section);
    return updated;
}

QString update_readme_versions(const QString& text, const LangInput& pt, const LangInput& en, const LangInput& es, const LangInput& fr,
                               const LangInput& it, const LangInput& de) {
    QString updated = text;

    updated = update_readme_section(updated, "ptbr", pt.version);
    updated = update_readme_section(updated, "enus", en.version);
    updated = update_readme_section(updated, "eses", es.version);
    updated = update_readme_section(updated, "frfr", fr.version);
    updated = update_readme_section(updated, "itit", it.version);
    updated = update_readme_section(updated, "dede", de.version);

    return updated;
}

namespace {

QString update_manual_section(const QString& text, const QString& section_id, const QString& version, const QString& date_str) {
    const QString start_marker = QString("<h2 id=\"%1\">").arg(section_id);
    const int start_index = text.indexOf(start_marker);
    if (start_index < 0) {
        return text;
    }

    int end_index = text.indexOf("<h2 id=\"", start_index + start_marker.size());
    if (end_index < 0) {
        end_index = text.size();
    }

    QString section = text.mid(start_index, end_index - start_index);
    const QString normalized_ver = plain_version(version);

    // 1. Linha de versão documentada em qualquer um dos 6 idiomas
    section = replace_regex_capture(
        section,
        R"((^(?:Versão documentada|Documented version|Versión documentada|Version documentée|Versione documentata|Dokumentierte Version)\s*:\s*`?)(v?[0-9]+(?:\.[0-9]+){1,3})(`?.*$))",
        2,
        normalized_ver);

    // 2. Linha de data em qualquer um dos 6 idiomas
    section = replace_regex_capture(
        section,
        R"((^(?:Data|Date|Fecha|Datum)\s*:\s*`?)([^`\r\n]+)(`?.*$))",
        2,
        date_str);

    QString updated = text;
    updated.replace(start_index, end_index - start_index, section);
    return updated;
}

QString update_manual_versions(const QString& text, const LangInput& pt, const LangInput& en, const LangInput& es,
                               const LangInput& fr, const LangInput& it, const LangInput& de) {
    QString updated = text;

    const QString pt_date = QString("%1 de %2 de %3").arg(pt.day).arg(PT_MONTHS.value(pt.month_index).toLower()).arg(pt.year);
    const QString en_date = QString("%1 %2, %3").arg(EN_MONTHS.value(en.month_index)).arg(en.day).arg(en.year);
    const QString es_date = QString("%1 de %2 de %3").arg(es.day).arg(ES_MONTHS.value(es.month_index).toLower()).arg(es.year);
    const QString fr_date = QString("%1 %2 %3").arg(fr.day).arg(FR_MONTHS.value(fr.month_index).toLower()).arg(fr.year);
    const QString it_date = QString("%1 %2 %3").arg(it.day).arg(IT_MONTHS.value(it.month_index).toLower()).arg(it.year);
    const QString de_date = QString("%1. %2 %3").arg(de.day).arg(DE_MONTHS.value(de.month_index)).arg(de.year);

    updated = update_manual_section(updated, "ptbr", pt.version, pt_date);
    updated = update_manual_section(updated, "enus", en.version, en_date);
    updated = update_manual_section(updated, "eses", es.version, es_date);
    updated = update_manual_section(updated, "frfr", fr.version, fr_date);
    updated = update_manual_section(updated, "itit", it.version, it_date);
    updated = update_manual_section(updated, "dede", de.version, de_date);

    return updated;
}

QString update_standard_doc_versions(const QString& text, const LangInput& pt, const LangInput& en) {
    const QString normalized_pt_ver = plain_version(pt.version);
    const QString normalized_en_ver = plain_version(en.version);
    const QString pt_date = QString("%1 de %2 de %3").arg(pt.day).arg(PT_MONTHS.value(pt.month_index).toLower()).arg(pt.year);
    const QString en_date = QString("%1 %2, %3").arg(EN_MONTHS.value(en.month_index)).arg(en.day).arg(en.year);

    QString updated = text;

    // **Versão:** `2026.9.28.0`  /  **Versao:** `2026.9.28.0`
    updated = replace_regex_capture(updated, R"((\*\*Vers(?:ã|a)o:\*\*\s*`)([^`]+)(`))", 2, normalized_pt_ver);

    // **Data:** `28 de setembro de 2026`
    updated = replace_regex_capture(updated, R"((\*\*Data:\*\*\s*`)([^`]+)(`))", 2, pt_date);

    // **Version:** `2026.9.28.0`
    updated = replace_regex_capture(updated, R"((\*\*Version:\*\*\s*`)([^`]+)(`))", 2, normalized_en_ver);

    // **Date:** `September 28, 2026`
    updated = replace_regex_capture(updated, R"((\*\*Date:\*\*\s*`)([^`]+)(`))", 2, en_date);

    return updated;
}

QString update_release_doc_versions(const QString& text, const LangInput& pt, const LangInput& en) {
    const QString normalized_pt_ver = plain_version(pt.version);
    const QString normalized_en_ver = plain_version(en.version);
    const QString pt_date = QString("%1 de %2 de %3").arg(pt.day).arg(PT_MONTHS.value(pt.month_index).toLower()).arg(pt.year);
    const QString en_date = QString("%1 %2, %3").arg(EN_MONTHS.value(en.month_index)).arg(en.day).arg(en.year);

    QString updated = text;

    // # Notas de Lançamento / Release Notes — v2026.9.28.0
    updated = replace_regex_capture(updated, R"((# Notas de Lançamento / Release Notes — v)([\d\.]+))", 2, normalized_pt_ver);

    // # Linceu Lighthouse — Release v2026.9.28.0
    updated = replace_regex_capture(updated, R"((# Linceu Lighthouse — Release v)([\d\.]+))", 2, normalized_pt_ver);

    // **Linceu Lighthouse v2026.9.28.0**
    updated = replace_regex_capture(updated, R"((\*\*Linceu Lighthouse v)([\d\.]+)(\*\*))", 2, normalized_pt_ver);

    // **Tag de Versão:** `v2026.9.28.0`
    updated = replace_regex_capture(updated, R"((\*\*Tag de Vers(?:ã|a)o:\*\*\s*`v?)([^`]+)(`))", 2, normalized_pt_ver);

    // **Data de Publicação:** 28 de setembro de 2026
    updated = replace_regex_capture(updated, R"((\*\*Data de Publica(?:çã|ca)o:\*\*\s*)([^\r\n]+))", 2, pt_date);

    // **Version Tag:** `v2026.9.28.0`
    updated = replace_regex_capture(updated, R"((\*\*Version Tag:\*\*\s*`v?)([^`]+)(`))", 2, normalized_en_ver);

    // **Publication Date:** September 28, 2026
    updated = replace_regex_capture(updated, R"((\*\*Publication Date:\*\*\s*)([^\r\n]+))", 2, en_date);

    // Artefatos: Linceu_Lighthouse-v2026.9.28.0-x64-Setup.exe / Portable.zip
    updated = replace_regex_capture(updated, R"((Linceu_Lighthouse-v)([\d\.]+)(-x64-(?:Setup\.exe|Portable\.zip)))", 2, normalized_pt_ver);

    // Seção de Destaques
    updated = replace_regex_capture(updated, R"((## 2\. Destaques da Vers(?:ã|a)o v)([\d\.]+))", 2, normalized_pt_ver);
    updated = replace_regex_capture(updated, R"((## 2\. Highlights in Release v)([\d\.]+))", 2, normalized_en_ver);

    return updated;
}

QString update_suggested_issues_versions(const QString& text, const LangInput& pt) {
    const QString normalized_ver = plain_version(pt.version);
    QString updated = text;

    // (ex: `v2026.9.28.0`) / (e.g., `v2026.9.28.0`)
    updated = replace_regex_capture(updated, R"(((?:\(ex:|\(e\.g\.,)\s*`v?)([\d\.]+)(`\)))", 2, normalized_ver);

    return updated;
}

QString update_security_md_versions(const QString& text, const LangInput& pt) {
    QString updated = text;
    int day = 0, month_idx = 0, year = 0;
    if (!parse_version_to_date(pt.version, day, month_idx, year)) {
        year = pt.year;
        month_idx = pt.month_index;
    }
    const QString ym = QStringLiteral("%1.%2").arg(year).arg(month_idx + 1);

    // | `v2026.9.x` | :white_check_mark: |
    updated = replace_regex_capture(updated, R"((\|\s*`v?)(\d+\.\d+)(\.x`\s*\|\s*:white_check_mark:\s*\|))", 2, ym);

    // | `< v2026.9` | :x: |
    updated = replace_regex_capture(updated, R"((\|\s*`< v?)(\d+\.\d+)(`\s*\|\s*:x:\s*\|))", 2, ym);

    return updated;
}

QString update_observacao_versions(const QString& text, const LangInput& pt, const LangInput& en,
                                   const LangInput& es, const LangInput& fr,
                                   const LangInput& it, const LangInput& de) {
    QString updated = text;

    // 1. Linha de versão consolidada nos 6 idiomas (- `v2026.9.28.0`)
    updated = replace_line_by_regex(updated, R"(^-\s*`?v?[0-9]+(?:\.[0-9]+){1,3}`?\s*$)", QString("- `%1`").arg(readme_version(pt.version)));

    // 2. Headings de atualização funcional nos 6 idiomas
    updated = replace_regex_capture(updated, R"((## Atualiza(?:çã|ca)o funcional observada na vers(?:ã|a)o\s+)([\d\.]+))", 2, plain_version(pt.version));
    updated = replace_regex_capture(updated, R"((## Functional [Uu]pdate [Oo]bserved in [Vv]ersion\s+)([\d\.]+))", 2, plain_version(en.version));
    updated = replace_regex_capture(updated, R"((## Actualizaci(?:ó|o)n funcional observada en la versi(?:ó|o)n\s+)([\d\.]+))", 2, plain_version(es.version));
    updated = replace_regex_capture(updated, R"((## Mise à jour fonctionnelle observée dans la version\s+)([\d\.]+))", 2, plain_version(fr.version));
    updated = replace_regex_capture(updated, R"((## Aggiornamento funzionale osservato nella versione\s+)([\d\.]+))", 2, plain_version(it.version));
    updated = replace_regex_capture(updated, R"((## Funktionelles Update beobachtet in Version\s+)([\d\.]+))", 2, plain_version(de.version));

    return updated;
}

QGroupBox* create_group(const QString& title, const QString& version_label, const QString& day_label, const QString& month_label, const QString& year_label,
                        const QStringList& months, QLineEdit*& version_edit, QSpinBox*& day_spin, QComboBox*& month_combo, QSpinBox*& year_spin) {
    const LangInput defaults = current_default_input();
    auto* group = new QGroupBox(title);
    auto* layout = new QHBoxLayout(group);

    layout->addWidget(new QLabel(version_label));
    version_edit = new QLineEdit(defaults.version);
    layout->addWidget(version_edit);

    layout->addWidget(new QLabel(day_label));
    day_spin = new QSpinBox();
    const int initial_max_days = QDate(defaults.year, defaults.month_index + 1, 1).daysInMonth();
    day_spin->setRange(1, initial_max_days);
    day_spin->setValue(defaults.day);
    layout->addWidget(day_spin);

    layout->addWidget(new QLabel(month_label));
    month_combo = new QComboBox();
    month_combo->addItems(months);
    month_combo->setCurrentIndex(defaults.month_index);
    layout->addWidget(month_combo);

    layout->addWidget(new QLabel(year_label));
    year_spin = new QSpinBox();
    year_spin->setRange(1900, 3000);
    year_spin->setValue(defaults.year);
    layout->addWidget(year_spin);

    auto update_days = [day_spin, month_combo, year_spin]() {
        const int y = year_spin->value();
        const int m = month_combo->currentIndex() + 1;
        const int max_d = QDate(y, m, 1).daysInMonth();
        day_spin->setMaximum(max_d);
    };

    QObject::connect(month_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), group, [update_days](int) {
        update_days();
    });
    QObject::connect(year_spin, QOverload<int>::of(&QSpinBox::valueChanged), group, [update_days](int) {
        update_days();
    });

    auto update_ver = [version_edit, day_spin, month_combo, year_spin]() {
        const int y = year_spin->value();
        const int m = month_combo->currentIndex() + 1;
        const int d = day_spin->value();
        const QString new_ver = format_version_from_date(y, m, d, version_edit->text());
        if (version_edit->text() != new_ver) {
            version_edit->setText(new_ver);
        }
    };

    QObject::connect(day_spin, QOverload<int>::of(&QSpinBox::valueChanged), group, [update_ver](int) {
        update_ver();
    });
    QObject::connect(month_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), group, [update_ver](int) {
        update_ver();
    });
    QObject::connect(year_spin, QOverload<int>::of(&QSpinBox::valueChanged), group, [update_ver](int) {
        update_ver();
    });

    QObject::connect(version_edit, &QLineEdit::editingFinished, group, [version_edit, day_spin, month_combo, year_spin]() {
        int d = 0, m_idx = 0, y = 0;
        if (parse_version_to_date(version_edit->text(), d, m_idx, y)) {
            const QSignalBlocker b_day(day_spin);
            const QSignalBlocker b_month(month_combo);
            const QSignalBlocker b_year(year_spin);
            day_spin->setMaximum(QDate(y, m_idx + 1, 1).daysInMonth());
            day_spin->setValue(d);
            month_combo->setCurrentIndex(m_idx);
            year_spin->setValue(y);
        }
    });

    return group;
}

}  // namespace

QString project_root_dir() {
#ifdef LINCEU_PROJECT_ROOT_PATH
    const QString configured_root = QDir::cleanPath(QString::fromUtf8(LINCEU_PROJECT_ROOT_PATH));
    const QFileInfo configured_cmake(configured_root + QStringLiteral("/CMakeLists.txt"));
    const QFileInfo configured_source(configured_root + QStringLiteral("/source"));
    if (configured_cmake.exists() && configured_cmake.isFile() && configured_source.exists() && configured_source.isDir()) {
        return configured_root;
    }
#endif

    QDir dir(QCoreApplication::applicationDirPath());

    for (int depth = 0; depth < 8; ++depth) {
        const QFileInfo cmake_file(dir.filePath("CMakeLists.txt"));
        const QFileInfo source_dir(dir.filePath("source"));

        if (cmake_file.exists() && cmake_file.isFile() && source_dir.exists() && source_dir.isDir()) {
            return QDir::cleanPath(dir.absolutePath());
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QDir::cleanPath(QCoreApplication::applicationDirPath());
}

QString format_version_from_date(int year, int month, int day, const QString& existing_version) {
    QString revision = QStringLiteral("0");
    if (!existing_version.isEmpty()) {
        const QString plain = plain_version(existing_version.trimmed());
        const QStringList parts = plain.split('.');
        if (parts.size() >= 4) {
            bool ok = false;
            const int rev = parts.at(3).toInt(&ok);
            if (ok && rev >= 0) {
                revision = QString::number(rev);
            }
        }
    }
    return QStringLiteral("%1.%2.%3.%4").arg(year).arg(month).arg(day).arg(revision);
}

LangInput::LangInput() {
    const QDate today = QDate::currentDate();
    day = today.day();
    month_index = today.month() - 1;
    year = today.year();
    version = format_version_from_date(today.year(), today.month(), today.day());
}

LangInput::LangInput(const QString& ver, int d, int m, int y)
    : version(ver), day(d), month_index(m), year(y) {}

QString current_composed_version(const QString& revision) {
    const QDate today = QDate::currentDate();
    return format_version_from_date(today.year(), today.month(), today.day(), revision);
}

LangInput current_default_input() {
    return LangInput();
}

bool parse_version_to_date(const QString& version_str, int& day, int& month_index, int& year, int* revision) {
    static const QRegularExpression rx_ver_date(R"(^v?(\d{4})\.(\d{1,2})\.(\d{1,2})(?:\.(\d+))?$)", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch m = rx_ver_date.match(version_str.trimmed());
    if (m.hasMatch()) {
        const int y = m.captured(1).toInt();
        const int mo = m.captured(2).toInt();
        const int d = m.captured(3).toInt();
        if (mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && y >= 1900 && y <= 3000) {
            const QDate qd(y, mo, d);
            if (qd.isValid()) {
                year = y;
                month_index = mo - 1;
                day = d;
                if (revision && !m.captured(4).isEmpty()) {
                    *revision = m.captured(4).toInt();
                }
                return true;
            }
        }
    }
    return false;
}

bool is_release_notes_file(const QString& rel_path) {
    const QString norm = QDir::fromNativeSeparators(rel_path).trimmed();
    return norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_de_DE.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_en_US.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_es_ES.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_fr_FR.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_it_IT.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("source/assets/RELEASE/RELEASE NOTES_pt_BR.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_de_DE.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_en_US.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_es_ES.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_fr_FR.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_it_IT.txt"), Qt::CaseInsensitive)
        || norm.endsWith(QStringLiteral("RELEASE NOTES_pt_BR.txt"), Qt::CaseInsensitive);
}

bool is_updatable_file(const QString& rel_path) {
    if (is_release_notes_file(rel_path)) {
        return false;
    }
    const QString norm = QDir::fromNativeSeparators(rel_path).trimmed();
    if (norm.endsWith(QStringLiteral("CODE_OF_CONDUCT.md"), Qt::CaseInsensitive) ||
        norm.endsWith(QStringLiteral("CONTRIBUTING.md"), Qt::CaseInsensitive)) {
        return false;
    }
    return true;
}

QVector<FileEntry> get_all_file_entries(const QString& base_dir) {
    const QString rel_doc_path = find_release_doc_rel_path(base_dir);

    return {
        // PRIVACY_POLICY
        {"privacy_pt", "source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt", "PRIVACY_POLICY", "pt"},
        {"privacy_en", "source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt", "PRIVACY_POLICY", "en"},
        {"privacy_es", "source/assets/PRIVACY_POLICY/Privacy_Policy_es_ES.txt", "PRIVACY_POLICY", "es"},
        {"privacy_fr", "source/assets/PRIVACY_POLICY/Privacy_Policy_fr_FR.txt", "PRIVACY_POLICY", "fr"},
        {"privacy_it", "source/assets/PRIVACY_POLICY/Privacy_Policy_it_IT.txt", "PRIVACY_POLICY", "it"},
        {"privacy_de", "source/assets/PRIVACY_POLICY/Privacy_Policy_de_DE.txt", "PRIVACY_POLICY", "de"},

        // EULA
        {"eula_pt", "source/assets/EULA/EULA_pt_BR - Linceu Lighthouse.txt", "EULA", "pt"},
        {"eula_en", "source/assets/EULA/EULA_en_US - Linceu Lighthouse.txt", "EULA", "en"},
        {"eula_es", "source/assets/EULA/EULA_es_ES - Linceu Lighthouse.txt", "EULA", "es"},
        {"eula_fr", "source/assets/EULA/EULA_fr_FR - Linceu Lighthouse.txt", "EULA", "fr"},
        {"eula_it", "source/assets/EULA/EULA_it_IT - Linceu Lighthouse.txt", "EULA", "it"},
        {"eula_de", "source/assets/EULA/EULA_de_DE - Linceu Lighthouse.txt", "EULA", "de"},

        // CLC
        {"clc_pt", "source/assets/CLC/CLC_pt_BR - Linceu Lighthouse.txt", "CLC", "pt"},
        {"clc_en", "source/assets/CLC/CLC_en_US - Linceu Lighthouse.txt", "CLC", "en"},
        {"clc_es", "source/assets/CLC/CLC_es_ES - Linceu Lighthouse.txt", "CLC", "es"},
        {"clc_fr", "source/assets/CLC/CLC_fr_FR - Linceu Lighthouse.txt", "CLC", "fr"},
        {"clc_it", "source/assets/CLC/CLC_it_IT - Linceu Lighthouse.txt", "CLC", "it"},
        {"clc_de", "source/assets/CLC/CLC_de_DE - Linceu Lighthouse.txt", "CLC", "de"},

        // ABOUT
        {"about_pt", "source/assets/ABOUT/ABOUT_pt_BR.txt", "ABOUT", "pt"},
        {"about_en", "source/assets/ABOUT/ABOUT_en_US.txt", "ABOUT", "en"},
        {"about_es", "source/assets/ABOUT/ABOUT_es_ES.txt", "ABOUT", "es"},
        {"about_fr", "source/assets/ABOUT/ABOUT_fr_FR.txt", "ABOUT", "fr"},
        {"about_it", "source/assets/ABOUT/ABOUT_it_IT.txt", "ABOUT", "it"},
        {"about_de", "source/assets/ABOUT/ABOUT_de_DE.txt", "ABOUT", "de"},

        // DOCS (Especificações modulares em docs/)
        {"doc_architecture", "docs/architecture.md", "DOCS", "all"},
        {"doc_cli_reference", "docs/cli-reference.md", "DOCS", "all"},
        {"doc_compliance", "docs/compliance.md", "DOCS", "all"},
        {"doc_licensing", "docs/licensing.md", "DOCS", "all"},
        {"doc_release", rel_doc_path, "DOCS", "all"},
        {"doc_security_model", "docs/security-model.md", "DOCS", "all"},
        {"doc_suggested_issues", "docs/suggested-issues.md", "DOCS", "all"},
        {"doc_user_guide", "docs/user-guide.md", "DOCS", "all"},

        // RAIZ (Documentação e metadados de raiz)
        {"readme", "README.md", "RAIZ", "all"},
        {"security_md", "SECURITY.md", "RAIZ", "all"},
        {"code_of_conduct_md", "CODE_OF_CONDUCT.md", "RAIZ", "all", false},
        {"contributing_md", "CONTRIBUTING.md", "RAIZ", "all", false},
        {"manual", "MANUAL.md", "RAIZ", "all"},
        {"observacao", "OBSERVACAO.md", "RAIZ", "all"},

        // UI (GerenciamentoMenusUI)
        {"sobre_dialog_cpp", "source/ui/GerenciamentoMenusUI/gmui_03_SobreDialog.cpp", "GerenciamentoMenusUI", "pt"},
        {"exibir_sobre_cpp", "source/ui/GerenciamentoMenusUI/gmui_22_exibir_sobre.cpp", "GerenciamentoMenusUI", "pt"},


        // REPO (Instaladores Inno Setup e manifesto MSIX)
        {"installer_default", "repo/Linceu_Lighthouse.iss", "REPO", "pt"},
        {"installer_mingw", "repo/Linceu_Lighthouse_mingw.iss", "REPO", "pt"},
        {"installer_msvc", "repo/Linceu_Lighthouse_msvc.iss", "REPO", "pt"}
    };
}

QList<FileCheckStatus> check_all_files(const QString& base_dir) {
    QList<FileCheckStatus> statuses;
    const QVector<FileEntry> entries = get_all_file_entries(base_dir);

    for (const FileEntry& entry : entries) {
        if (is_release_notes_file(entry.rel_path)) {
            continue;
        }

        FileCheckStatus st;
        st.key = entry.key;
        st.rel_path = entry.rel_path;
        st.group = determine_file_group(entry.rel_path);
        st.lang = entry.lang;
        st.is_updatable = entry.is_updatable;
        st.absolute_path = resolve_file_path(base_dir, entry.rel_path);

        bool ok = false;
        const QString txt = read_file_robust(st.absolute_path, &ok);
        st.exists = ok;

        if (ok) {
            const QString clean_txt = QString(txt).remove('\r');

            // 1. Verifica padrões configurados
            const QStringList patterns = PATTERNS.value(entry.key);
            for (const QString& pat : patterns) {
                QRegularExpression re(pat, QRegularExpression::MultilineOption);
                const QRegularExpressionMatch match = re.match(clean_txt);
                if (match.hasMatch()) {
                    QString captured = (match.lastCapturedIndex() >= 1) ? match.captured(1).trimmed() : match.captured(0).trimmed();
                    if (captured.isEmpty()) {
                        captured = match.captured(0).trimmed();
                    }
                    st.found_patterns.append(captured);
                } else {
                    st.missing_patterns.append(pat);
                }
            }

            // 2. Detecta versão
            QString found_ver;
            for (const QString& f : st.found_patterns) {
                if (parse_version_string(f, found_ver)) {
                    st.detected_version = found_ver;
                    break;
                }
            }

            if (st.detected_version.isEmpty()) {
                const QStringList lines = clean_txt.split('\n');
                for (const QString& line : lines) {
                    const QString trimmed = line.trimmed();
                    if (trimmed.startsWith('#') && !trimmed.startsWith("#define", Qt::CaseInsensitive)) {
                        continue;
                    }
                    if (line.contains("MyAppVersion", Qt::CaseInsensitive) ||
                        line.contains("OutputBaseFilename", Qt::CaseInsensitive) ||
                        line.contains("Versão", Qt::CaseInsensitive) ||
                        line.contains("Version", Qt::CaseInsensitive) ||
                        line.contains("Versión", Qt::CaseInsensitive) ||
                        line.contains("Versione", Qt::CaseInsensitive)) {
                        if (parse_version_string(line, found_ver)) {
                            st.detected_version = found_ver;
                            break;
                        }
                    }
                }
            }

            // 3. Detecta data
            int d = 0, m = 0, y = 0;
            // 3.1 Primeiro tenta nos padrões já encontrados e validados pelo PATTERNS
            for (const QString& f : st.found_patterns) {
                if (parse_date_string(f, d, m, y)) {
                    st.detected_date = QString("%1/%2/%3").arg(d).arg(m + 1).arg(y);
                    break;
                }
            }

            // 3.2 Se ainda não detectou, busca em linhas que contenham palavras-chave de data ou versão
            if (st.detected_date.isEmpty()) {
                const QStringList lines = clean_txt.split('\n');
                for (const QString& line : lines) {
                    if (line.trimmed().startsWith('#')) continue;
                    if (line.contains("atualização", Qt::CaseInsensitive) ||
                        line.contains("atualizado", Qt::CaseInsensitive) ||
                        line.contains("actualización", Qt::CaseInsensitive) ||
                        line.contains("actualizado", Qt::CaseInsensitive) ||
                        line.contains("updated", Qt::CaseInsensitive) ||
                        line.contains("update", Qt::CaseInsensitive) ||
                        line.contains("mise à jour", Qt::CaseInsensitive) ||
                        line.contains("mis à jour", Qt::CaseInsensitive) ||
                        line.contains("aggiornamento", Qt::CaseInsensitive) ||
                        line.contains("aggiornato", Qt::CaseInsensitive) ||
                        line.contains("Aktualisierung", Qt::CaseInsensitive) ||
                        line.contains("aktualisiert", Qt::CaseInsensitive) ||
                        line.contains("Data", Qt::CaseInsensitive) ||
                        line.contains("Date", Qt::CaseInsensitive) ||
                        line.contains("Fecha", Qt::CaseInsensitive) ||
                        line.contains("Datum", Qt::CaseInsensitive) ||
                        line.contains("Versão", Qt::CaseInsensitive) ||
                        line.contains("Version", Qt::CaseInsensitive) ||
                        line.contains("Versión", Qt::CaseInsensitive) ||
                        line.contains("Versione", Qt::CaseInsensitive)) {
                        if (parse_date_string(line, d, m, y)) {
                            st.detected_date = QString("%1/%2/%3").arg(d).arg(m + 1).arg(y);
                            break;
                        }
                    }
                }
            }

            // 3.3 Fallback geral por linha
            if (st.detected_date.isEmpty()) {
                const QStringList lines = clean_txt.split('\n');
                for (const QString& line : lines) {
                    const QString trimmed = line.trimmed();
                    if (trimmed.isEmpty() || trimmed.startsWith('#')) continue;
                    if (parse_date_string(line, d, m, y)) {
                        st.detected_date = QString("%1/%2/%3").arg(d).arg(m + 1).arg(y);
                        break;
                    }
                }
            }

            // 3.4 Fallback para arquivos sem linha explícita de data (como instaladores .iss),
            // deduz a data a partir da versão caso ela siga o formato de data (YYYY.M.D.rev):
            if (st.detected_date.isEmpty() && !st.detected_version.isEmpty()) {
                int vd = 0, vm = 0, vy = 0;
                if (parse_version_to_date(st.detected_version, vd, vm, vy)) {
                    st.detected_date = QString("%1/%2/%3").arg(vd).arg(vm + 1).arg(vy);
                }
            }
        }

        statuses.append(st);
    }

    return statuses;
}

QList<UpdateResult> apply_updates(const LangInput& pt, const LangInput& en, const LangInput& es, const LangInput& fr,
                                  const LangInput& it, const LangInput& de, const QString& base_dir) {
    QList<UpdateResult> results;
    const QVector<FileEntry> entries = get_all_file_entries(base_dir);

    const QString pt_date = QString("%1 de %2 de %3").arg(pt.day).arg(PT_MONTHS.value(pt.month_index).toLower()).arg(pt.year);
    const QString en_date = QString("%1 %2, %3").arg(EN_MONTHS.value(en.month_index)).arg(en.day).arg(en.year);
    const QString es_date = QString("%1 de %2 de %3").arg(es.day).arg(ES_MONTHS.value(es.month_index).toLower()).arg(es.year);
    const QString fr_date = QString("%1 %2 %3").arg(fr.day).arg(FR_MONTHS.value(fr.month_index).toLower()).arg(fr.year);
    const QString it_date = QString("%1 %2 %3").arg(it.day).arg(IT_MONTHS.value(it.month_index).toLower()).arg(it.year);
    const QString de_date = QString("%1. %2 %3").arg(de.day).arg(DE_MONTHS.value(de.month_index)).arg(de.year);

    for (const FileEntry& entry : entries) {
        // Os arquivos de notas de lançamento (RELEASE NOTES_de_DE, en_US, es_ES, fr_FR, it_IT, pt_BR)
        // e demais arquivos configurados como não-atualizáveis NUNCA realizam atualizações e são ignorados na gravação.
        if (!entry.is_updatable || is_release_notes_file(entry.rel_path)) {
            continue;
        }

        QString abs_path = resolve_file_path(base_dir, entry.rel_path);
        const QString group = determine_file_group(entry.rel_path);

        bool read_ok = false;
        const QString original = read_file_robust(abs_path, &read_ok);
        if (!read_ok) {
            results.append({entry.key, entry.rel_path, group, false, "Arquivo não encontrado ou erro de leitura"});
            continue;
        }

        QString new_txt = original;

        try {
            const QString& key = entry.key;
            if (key == "privacy_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão:?.*$)", QString("Versão: %1").arg(plain_version(pt.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Última atualização:?.*$)", QString("Última atualização: %1").arg(pt_date));
            } else if (key == "privacy_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(en.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Last updated:?.*$)", QString("Last updated: %1").arg(en_date));
            } else if (key == "privacy_es") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versión:?.*$)", QString("Versión: %1").arg(plain_version(es.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Última actualización:?.*$)", QString("Última actualización: %1").arg(es_date));
            } else if (key == "privacy_fr") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(fr.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Dernière mise à jour\s*:?.*$)", QString("Dernière mise à jour: %1").arg(fr_date));
            } else if (key == "privacy_it") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versione:?.*$)", QString("Versione: %1").arg(plain_version(it.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Ultimo aggiornamento:?.*$)", QString("Ultimo aggiornamento: %1").arg(it_date));
            } else if (key == "privacy_de") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(de.version)));
                new_txt = replace_line_by_regex(new_txt, R"(^Letzte Aktualisierung:?.*$)", QString("Letzte Aktualisierung: %1").arg(de_date));
            } else if (key == "eula_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão\s*.*$)", QString("Versão %1, %2").arg(plain_version(pt.version), pt_date));
            } else if (key == "eula_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(en.version), en_date));
            } else if (key == "eula_es") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versión\s*.*$)", QString("Versión %1, %2").arg(plain_version(es.version), es_date));
            } else if (key == "eula_fr") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(fr.version), fr_date));
            } else if (key == "eula_it") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versione\s*.*$)", QString("Versione %1, %2").arg(plain_version(it.version), it_date));
            } else if (key == "eula_de") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(de.version), de_date));
            } else if (key == "clc_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão\s*.*$)", QString("Versão %1, %2").arg(plain_version(pt.version), pt_date));
            } else if (key == "clc_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(en.version), en_date));
            } else if (key == "clc_es") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versión\s*.*$)", QString("Versión %1, %2").arg(plain_version(es.version), es_date));
            } else if (key == "clc_fr") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(fr.version), fr_date));
            } else if (key == "clc_it") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versione\s*.*$)", QString("Versione %1, %2").arg(plain_version(it.version), it_date));
            } else if (key == "clc_de") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(plain_version(de.version), de_date));
            } else if (key == "about_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão:?.*$)", QString("Versão: %1").arg(plain_version(pt.version)));
            } else if (key == "about_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(en.version)));
            } else if (key == "about_es") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versión:?.*$)", QString("Versión: %1").arg(plain_version(es.version)));
            } else if (key == "about_fr") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(fr.version)));
            } else if (key == "about_it") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versione:?.*$)", QString("Versione: %1").arg(plain_version(it.version)));
            } else if (key == "about_de") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(plain_version(de.version)));
            } else if (key == "doc_architecture" || key == "doc_cli_reference" || key == "doc_compliance"
                       || key == "doc_licensing" || key == "doc_security_model" || key == "doc_user_guide") {
                new_txt = update_standard_doc_versions(new_txt, pt, en);
            } else if (key == "doc_release") {
                new_txt = update_release_doc_versions(new_txt, pt, en);
                const QString old_rel_path = entry.rel_path;
                const QString new_rel_path = QStringLiteral("docs/release-v%1.md").arg(plain_version(pt.version));
                if (old_rel_path != new_rel_path && QFileInfo::exists(abs_path)) {
                    const QString new_abs_path = resolve_file_path(base_dir, new_rel_path);
                    if (new_abs_path != abs_path) {
                        QFile::remove(new_abs_path);
                        if (QFile::rename(abs_path, new_abs_path)) {
                            abs_path = new_abs_path;
                        }
                    }
                }
            } else if (key == "doc_suggested_issues") {
                new_txt = update_suggested_issues_versions(new_txt, pt);
            } else if (key == "readme") {
                new_txt = update_readme_versions(new_txt, pt, en, es, fr, it, de);
                new_txt = replace_regex_capture(new_txt, R"(((?:docs/)?release-v)([\d\.]+)(\.md))", 2, plain_version(pt.version));
            } else if (key == "security_md") {
                new_txt = update_security_md_versions(new_txt, pt);
            } else if (key == "code_of_conduct_md" || key == "contributing_md") {
                // Arquivos de diretrizes e conduta sem marcadores de versão/data mantidos íntegros
            } else if (key == "observacao") {
                new_txt = update_observacao_versions(new_txt, pt, en, es, fr, it, de);
            } else if (key == "manual") {
                new_txt = update_manual_versions(new_txt, pt, en, es, fr, it, de);
            } else if (key == "sobre_dialog_cpp" || key == "exibir_sobre_cpp") {
                new_txt = update_about_cpp_version(new_txt, pt.version);
            } else if (key == "installer_default" || key == "installer_mingw" || key == "installer_msvc") {
                new_txt = update_installer_version(new_txt, pt.version);
            }

            if (new_txt != original) {
                QString err;
                if (write_file_utf8(abs_path, new_txt, &err)) {
                    results.append({entry.key, entry.rel_path, group, true, "Atualizado com sucesso"});
                } else {
                    results.append({entry.key, entry.rel_path, group, false, QString("Erro ao gravar: %1").arg(err)});
                }
            } else {
                results.append({entry.key, entry.rel_path, group, true, "Sem alterações necessárias (já atualizado)"});
            }
        } catch (...) {
            results.append({entry.key, entry.rel_path, group, false, "Exceção inesperada durante atualização"});
        }
    }

    return results;
}

VersionEditor::VersionEditor(QWidget* parent)
    : QWidget(parent),
      selected_base_dir_(project_root_dir()),
      tab_widget_(nullptr),
      general_status_(nullptr) {
    setWindowTitle("Editor de Versões e Datas Multi-idioma - Linceu Lighthouse");
    resize(1020, 740);

    const QString icon_path = find_window_icon_path();
    if (!icon_path.isEmpty()) {
        setWindowIcon(QIcon(icon_path));
    }

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // 1. Diretório base
    auto* folder_layout = new QHBoxLayout();
    folder_layout->addWidget(new QLabel("Pasta dos arquivos (Raiz do Projeto):"));
    folder_path_ = new QLineEdit(selected_base_dir_);
    folder_path_->setReadOnly(true);
    folder_layout->addWidget(folder_path_, 1);
    auto* select_folder_btn = new QPushButton("Selecionar...");
    folder_layout->addWidget(select_folder_btn);
    root->addLayout(folder_layout);

    // 2. Grupos de idiomas (Grid 3x2)
    auto* lang_grid = new QGridLayout();
    lang_grid->setHorizontalSpacing(8);
    lang_grid->setVerticalSpacing(6);

    auto* grp_pt = create_group("🇧🇷 Português (pt-BR)", "Versão:", "Dia:", "Mês:", "Ano:", PT_MONTHS, pt_version_, pt_day_, pt_month_, pt_year_);
    auto* grp_en = create_group("🇺🇸 English (en-US)", "Version:", "Day:", "Month:", "Year:", EN_MONTHS, en_version_, en_day_, en_month_, en_year_);
    auto* grp_es = create_group("🇪🇸 Español (es-ES)", "Versión:", "Día:", "Mes:", "Año:", ES_MONTHS, es_version_, es_day_, es_month_, es_year_);
    auto* grp_fr = create_group("🇫🇷 Français (fr-FR)", "Version:", "Jour:", "Mois:", "Année:", FR_MONTHS, fr_version_, fr_day_, fr_month_, fr_year_);
    auto* grp_it = create_group("🇮🇹 Italiano (it-IT)", "Versione:", "Giorno:", "Mese:", "Anno:", IT_MONTHS, it_version_, it_day_, it_month_, it_year_);
    auto* grp_de = create_group("🇩🇪 Deutsch (de-DE)", "Version:", "Tag:", "Monat:", "Jahr:", DE_MONTHS, de_version_, de_day_, de_month_, de_year_);

    lang_grid->addWidget(grp_pt, 0, 0);
    lang_grid->addWidget(grp_en, 0, 1);
    lang_grid->addWidget(grp_es, 1, 0);
    lang_grid->addWidget(grp_fr, 1, 1);
    lang_grid->addWidget(grp_it, 2, 0);
    lang_grid->addWidget(grp_de, 2, 1);
    root->addLayout(lang_grid);

    // 3. Linha de ações rápidas e botões
    auto* btn_layout = new QHBoxLayout();
    auto* load_btn = new QPushButton("🔍 Verificar Status dos Arquivos");
    auto* reload_btn = new QPushButton("🔄 Recarregar Dados");
    auto* save_btn = new QPushButton("💾 Salvar Alterações em Todos");
    auto* sync_ver_btn = new QPushButton("🔗 Sincronizar Versão PT");

    btn_layout->addWidget(load_btn);
    btn_layout->addWidget(reload_btn);
    btn_layout->addWidget(save_btn);
    btn_layout->addWidget(sync_ver_btn);
    btn_layout->addStretch(1);
    root->addLayout(btn_layout);

    // 4. Abas organizadas por Grupo de Arquivos
    tab_widget_ = new QTabWidget(this);

    general_status_ = new QTextEdit(this);
    general_status_->setReadOnly(true);
    general_status_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    tab_widget_->addTab(general_status_, QStringLiteral("📊 Geral"));

    const QStringList group_order = {
        QStringLiteral("PRIVACY_POLICY"),
        QStringLiteral("EULA"),
        QStringLiteral("CLC"),
        QStringLiteral("ABOUT"),
        QStringLiteral("DOCS"),
        QStringLiteral("RAIZ"),
        QStringLiteral("GerenciamentoMenusUI"),
        QStringLiteral("REPO")
    };

    const QHash<QString, QString> group_icons = {
        {"PRIVACY_POLICY", "🔒 "},
        {"EULA", "📜 "},
        {"CLC", "💼 "},
        {"ABOUT", "ℹ️ "},
        {"DOCS", "📚 "},
        {"RAIZ", "📁 "},
        {"GerenciamentoMenusUI", "🖥️ "},
        {"REPO", "📦 "}
    };

    for (const QString& grp : group_order) {
        auto* te = new QTextEdit(this);
        te->setReadOnly(true);
        te->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        group_status_map_.insert(grp, te);
        const QString icon = group_icons.value(grp, QStringLiteral("📄 "));
        tab_widget_->addTab(te, icon + grp);
    }

    root->addWidget(tab_widget_, 1);

    // Sinais e Slots
    QObject::connect(select_folder_btn, &QPushButton::clicked, this, &VersionEditor::on_select_folder);
    QObject::connect(load_btn, &QPushButton::clicked, this, &VersionEditor::on_check);
    QObject::connect(reload_btn, &QPushButton::clicked, this, &VersionEditor::on_reload_data);
    QObject::connect(save_btn, &QPushButton::clicked, this, &VersionEditor::on_save);
    QObject::connect(sync_ver_btn, &QPushButton::clicked, this, &VersionEditor::on_sync_pt_version);

    // Inicializa com a data atual e verifica arquivos
    set_today_all();
    on_check();
}

void VersionEditor::set_date_and_version(QLineEdit* ver_edit, QSpinBox* day_spin, QComboBox* month_combo, QSpinBox* year_spin, const QDate& date) {
    if (!ver_edit || !day_spin || !month_combo || !year_spin || !date.isValid()) return;

    const QSignalBlocker b_day(day_spin);
    const QSignalBlocker b_month(month_combo);
    const QSignalBlocker b_year(year_spin);
    const QSignalBlocker b_ver(ver_edit);

    day_spin->setRange(1, date.daysInMonth());
    day_spin->setValue(date.day());
    month_combo->setCurrentIndex(date.month() - 1);
    year_spin->setValue(date.year());

    const QString new_ver = format_version_from_date(date.year(), date.month(), date.day(), ver_edit->text());
    ver_edit->setText(new_ver);
}

void VersionEditor::update_version_from_date(QLineEdit* ver_edit, int day, int month, int year) {
    if (!ver_edit) return;
    const QString new_ver = format_version_from_date(year, month, day, ver_edit->text());
    if (ver_edit->text() != new_ver) {
        ver_edit->setText(new_ver);
    }
}

void VersionEditor::on_select_folder() {
    const QString dir_path = QFileDialog::getExistingDirectory(this, "Selecione a pasta raiz do projeto", selected_base_dir_);
    if (!dir_path.isEmpty()) {
        selected_base_dir_ = QDir::cleanPath(dir_path);
        folder_path_->setText(selected_base_dir_);
        on_check();
    }
}

void VersionEditor::load_versions_from_disk_files() {
    const auto statuses = check_all_files(selected_base_dir_);

    auto try_populate = [this](const QList<FileCheckStatus>& list, const QString& lang,
                               QLineEdit* ver_edit, QSpinBox* day_spin, QComboBox* month_combo, QSpinBox* year_spin) {
        for (const FileCheckStatus& st : list) {
            if (!st.is_updatable || is_release_notes_file(st.rel_path)) {
                continue;
            }
            if (st.lang == lang && st.exists) {
                int d = 0, m = 0, y = 0;
                bool date_ok = false;
                if (!st.detected_date.isEmpty()) {
                    date_ok = parse_date_string(st.detected_date, d, m, y);
                }
                if (date_ok) {
                    const QDate parsed_date(y, m + 1, d);
                    if (parsed_date.isValid()) {
                        set_date_and_version(ver_edit, day_spin, month_combo, year_spin, parsed_date);
                        if (!st.detected_version.isEmpty()) {
                            const QString f_ver = plain_version(st.detected_version);
                            ver_edit->setText(format_version_from_date(y, m + 1, d, f_ver));
                        }
                    }
                } else if (!st.detected_version.isEmpty()) {
                    ver_edit->setText(plain_version(st.detected_version));
                    int vd = 0, vm_idx = 0, vy = 0;
                    if (parse_version_to_date(st.detected_version, vd, vm_idx, vy)) {
                        set_date_and_version(ver_edit, day_spin, month_combo, year_spin, QDate(vy, vm_idx + 1, vd));
                    }
                }
                if (!st.detected_version.isEmpty() || date_ok) {
                    break;
                }
            }
        }
    };

    try_populate(statuses, "pt", pt_version_, pt_day_, pt_month_, pt_year_);
    try_populate(statuses, "en", en_version_, en_day_, en_month_, en_year_);
    try_populate(statuses, "es", es_version_, es_day_, es_month_, es_year_);
    try_populate(statuses, "fr", fr_version_, fr_day_, fr_month_, fr_year_);
    try_populate(statuses, "it", it_version_, it_day_, it_month_, it_year_);
    try_populate(statuses, "de", de_version_, de_day_, de_month_, de_year_);
}

void VersionEditor::on_sync_pt_version() {
    const QString pt_ver = plain_version(pt_version_->text().trimmed());
    if (pt_ver.isEmpty()) return;

    const int d = pt_day_->value();
    const int m = pt_month_->currentIndex();
    const int y = pt_year_->value();
    const QDate pt_date(y, m + 1, d);

    auto sync_target = [&](QLineEdit* ver_edit, QSpinBox* day_spin, QComboBox* month_combo, QSpinBox* year_spin) {
        if (pt_date.isValid()) {
            set_date_and_version(ver_edit, day_spin, month_combo, year_spin, pt_date);
            ver_edit->setText(pt_ver);
        } else {
            ver_edit->setText(pt_ver);
            day_spin->setValue(d);
            month_combo->setCurrentIndex(m);
            year_spin->setValue(y);
        }
    };

    sync_target(en_version_, en_day_, en_month_, en_year_);
    sync_target(es_version_, es_day_, es_month_, es_year_);
    sync_target(fr_version_, fr_day_, fr_month_, fr_year_);
    sync_target(it_version_, it_day_, it_month_, it_year_);
    sync_target(de_version_, de_day_, de_month_, de_year_);
}

void VersionEditor::set_today_all() {
    const QDate today = QDate::currentDate();

    auto set_all = [&](QLineEdit* ver_edit, QSpinBox* day_spin, QComboBox* month_combo, QSpinBox* year_spin) {
        set_date_and_version(ver_edit, day_spin, month_combo, year_spin, today);
    };

    set_all(pt_version_, pt_day_, pt_month_, pt_year_);
    set_all(en_version_, en_day_, en_month_, en_year_);
    set_all(es_version_, es_day_, es_month_, es_year_);
    set_all(fr_version_, fr_day_, fr_month_, fr_year_);
    set_all(it_version_, it_day_, it_month_, it_year_);
    set_all(de_version_, de_day_, de_month_, de_year_);
}

void VersionEditor::on_reload_data() {
    load_versions_from_disk_files();
    on_check();
}

void VersionEditor::on_check() {
    const auto statuses = check_all_files(selected_base_dir_);

    const QDate today = QDate::currentDate();
    const QString today_ver = format_version_from_date(today.year(), today.month(), today.day());

    // Group statuses
    QMap<QString, QList<FileCheckStatus>> grouped_status;
    int found_count = 0;
    int pending_count = 0;

    for (const FileCheckStatus& st : statuses) {
        grouped_status[st.group].append(st);
        if (st.exists) {
            ++found_count;
            if (!st.missing_patterns.isEmpty()) {
                ++pending_count;
            }
        }
    }

    // 1. General tab
    QStringList general_lines;
    general_lines << "=== Resumo Geral da Verificação ===";
    general_lines << QString("Data Presente do Sistema: %1 (Dia: %2, Mês: %3, Ano: %4)")
                         .arg(today.toString(QStringLiteral("dd/MM/yyyy")))
                         .arg(today.day())
                         .arg(today.month())
                         .arg(today.year());
    general_lines << QString("Versão Atual Composta: %1").arg(today_ver);
    general_lines << QString("Diretório Base: %1").arg(selected_base_dir_);
    general_lines << QString("Total de Arquivos Monitorados: %1").arg(statuses.size());
    general_lines << QString("Arquivos Presentes: %1 / %2").arg(found_count).arg(statuses.size());
    general_lines << QString("Arquivos com Padrões Pendentes: %1").arg(pending_count);
    general_lines << "";
    general_lines << "=== Configuração Atual de Versão e Data nos Idiomas ===";
    general_lines << QString("  • PT: Versão: %1 | Data: %2/%3/%4")
                         .arg(pt_version_->text()).arg(pt_day_->value()).arg(pt_month_->currentIndex() + 1).arg(pt_year_->value());
    general_lines << QString("  • EN: Versão: %1 | Data: %2/%3/%4")
                         .arg(en_version_->text()).arg(en_day_->value()).arg(en_month_->currentIndex() + 1).arg(en_year_->value());
    general_lines << QString("  • ES: Versão: %1 | Data: %2/%3/%4")
                         .arg(es_version_->text()).arg(es_day_->value()).arg(es_month_->currentIndex() + 1).arg(es_year_->value());
    general_lines << QString("  • FR: Versão: %1 | Data: %2/%3/%4")
                         .arg(fr_version_->text()).arg(fr_day_->value()).arg(fr_month_->currentIndex() + 1).arg(fr_year_->value());
    general_lines << QString("  • IT: Versão: %1 | Data: %2/%3/%4")
                         .arg(it_version_->text()).arg(it_day_->value()).arg(it_month_->currentIndex() + 1).arg(it_year_->value());
    general_lines << QString("  • DE: Versão: %1 | Data: %2/%3/%4")
                         .arg(de_version_->text()).arg(de_day_->value()).arg(de_month_->currentIndex() + 1).arg(de_year_->value());
    general_lines << "";
    general_lines << "Status por Arquivo:";

    for (const FileCheckStatus& st : statuses) {
        QString tag = "[OK]";
        if (!st.exists) {
            tag = "[NÃO ENCONTRADO]";
        } else if (!st.missing_patterns.isEmpty()) {
            tag = "[PENDENTE]";
        }
        const QString ro_tag = st.is_updatable ? QString() : QStringLiteral(" [SOMENTE LEITURA]");
        general_lines << QString("%1 %2 (%3)%4 - Versão: %5, Data: %6")
                             .arg(tag, -16)
                             .arg(st.rel_path)
                             .arg(st.group)
                             .arg(ro_tag)
                             .arg(st.detected_version.isEmpty() ? "N/D" : st.detected_version)
                             .arg(st.detected_date.isEmpty() ? "N/D" : st.detected_date);
    }

    general_status_->setPlainText(general_lines.join("\n"));

    // 2. Group tabs
    for (auto it = group_status_map_.begin(); it != group_status_map_.end(); ++it) {
        const QString group_name = it.key();
        QTextEdit* text_edit = it.value();

        const auto group_files = grouped_status.value(group_name);
        QStringList lines;
        lines << QString("=== Grupo: %1 ===").arg(group_name);
        lines << QString("Data Presente do Sistema: %1 (Dia: %2, Mês: %3, Ano: %4)")
                     .arg(today.toString(QStringLiteral("dd/MM/yyyy")))
                     .arg(today.day())
                     .arg(today.month())
                     .arg(today.year());
        lines << QString("Versão Atual Composta: %1").arg(today_ver);
        lines << QString("Total de arquivos no grupo: %1").arg(group_files.size());
        lines << "";

        for (const FileCheckStatus& st : group_files) {
            lines << QString("📄 Arquivo: %1").arg(st.rel_path);
            lines << QString("   Status: %1").arg(st.exists ? "✅ Presente" : "❌ Não encontrado");
            lines << QString("   Caminho absoluto: %1").arg(st.absolute_path);
            if (st.exists) {
                lines << QString("   Versão detectada no arquivo: %1").arg(st.detected_version.isEmpty() ? "(não encontrada)" : st.detected_version);
                lines << QString("   Data detectada no arquivo: %1").arg(st.detected_date.isEmpty() ? "(não encontrada)" : st.detected_date);
                lines << QString("   Atualização automática: %1").arg(st.is_updatable ? "Habilitada" : "Desabilitada (somente leitura)");
                if (!st.found_patterns.isEmpty()) {
                    lines << "   Padrões encontrados no arquivo:";
                    for (const QString& f : st.found_patterns) {
                        lines << QString("     • %1").arg(f);
                    }
                }
                if (!st.missing_patterns.isEmpty()) {
                    lines << "   Padrões pendentes / não correspondidos:";
                    for (const QString& m : st.missing_patterns) {
                        lines << QString("     ⚠️ %1").arg(m);
                    }
                }
            }
            lines << "";
        }

        text_edit->setPlainText(lines.join("\n"));
    }
}

void VersionEditor::on_save() {
    const LangInput pt{pt_version_->text().trimmed(), pt_day_->value(), pt_month_->currentIndex(), pt_year_->value()};
    const LangInput en{en_version_->text().trimmed(), en_day_->value(), en_month_->currentIndex(), en_year_->value()};
    const LangInput es{es_version_->text().trimmed(), es_day_->value(), es_month_->currentIndex(), es_year_->value()};
    const LangInput fr{fr_version_->text().trimmed(), fr_day_->value(), fr_month_->currentIndex(), fr_year_->value()};
    const LangInput it{it_version_->text().trimmed(), it_day_->value(), it_month_->currentIndex(), it_year_->value()};
    const LangInput de{de_version_->text().trimmed(), de_day_->value(), de_month_->currentIndex(), de_year_->value()};

    const QList<UpdateResult> results = apply_updates(pt, en, es, fr, it, de, selected_base_dir_);

    // Group results
    QMap<QString, QList<UpdateResult>> grouped_results;
    int success_count = 0;

    for (const UpdateResult& r : results) {
        grouped_results[r.group].append(r);
        if (r.ok) ++success_count;
    }

    // Recarrega os dados relacionados a versão dos arquivos .iss e demais arquivos após atualizá-los
    on_reload_data();

    // 1. General tab update report
    const QDate today = QDate::currentDate();
    const QString today_ver = format_version_from_date(today.year(), today.month(), today.day());

    QStringList general_lines;
    general_lines << "=== Relatório de Gravação e Atualização ===";
    general_lines << QString("Data Presente do Sistema: %1 (Dia: %2, Mês: %3, Ano: %4)")
                         .arg(today.toString(QStringLiteral("dd/MM/yyyy")))
                         .arg(today.day())
                         .arg(today.month())
                         .arg(today.year());
    general_lines << QString("Versão Atual Composta: %1").arg(today_ver);
    general_lines << QString("Versão Salva (PT): %1 | Data: %2/%3/%4")
                         .arg(pt.version).arg(pt.day).arg(pt.month_index + 1).arg(pt.year);
    general_lines << QString("Sucessos: %1 / %2 arquivos processados (dados dos arquivos recarregados)")
                         .arg(success_count).arg(results.size());
    general_lines << "";
    for (const UpdateResult& r : results) {
        general_lines << QString("[%1] %2 (%3) -> %4").arg(r.ok ? "OK" : "FALHA", r.rel_path, r.group, r.msg);
    }
    general_lines << "";
    general_lines << general_status_->toPlainText();
    general_status_->setPlainText(general_lines.join("\n"));

    QMessageBox::information(this, "Atualização Concluída",
                             QString("Atualização concluída!\n\n%1 de %2 arquivos processados com sucesso.\nOs dados relacionados à versão dos arquivos .iss e demais arquivos foram recarregados.")
                                 .arg(success_count).arg(results.size()));
}

}  // namespace mocks

#if defined(Q_OS_WIN)
static void init_cli_console() {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD fileType = (hStdOut != NULL && hStdOut != INVALID_HANDLE_VALUE) ? GetFileType(hStdOut) : FILE_TYPE_UNKNOWN;
    if (fileType == FILE_TYPE_UNKNOWN) {
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            FILE* fp = nullptr;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
        }
    }
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
    std::ios::sync_with_stdio();
}
#endif

static void print_cli_msg(const QString& msg) {
    const std::string s = msg.toStdString() + "\n";
    std::cout << s << std::flush;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        const QString arg = QString::fromLocal8Bit(argv[1]).trimmed();
        if (arg == QStringLiteral("--help") || arg == QStringLiteral("-h")) {
#if defined(Q_OS_WIN)
            init_cli_console();
#endif
            print_cli_msg(QStringLiteral("find_VersionEditor - Editor de Versoes e Datas Multi-idioma (Linceu Lighthouse)"));
            print_cli_msg(QStringLiteral("Uso:"));
            print_cli_msg(QStringLiteral("  find_VersionEditor.exe                   (abre a interface grafica)"));
            print_cli_msg(QStringLiteral("  find_VersionEditor.exe --check [dir]     (verifica status e padroes dos arquivos)"));
            print_cli_msg(QStringLiteral("  find_VersionEditor.exe --apply [dir] [v] (aplica versao e data atual a todos os arquivos)"));
            return 0;
        } else if (arg == QStringLiteral("--check") || arg == QStringLiteral("-c")) {
#if defined(Q_OS_WIN)
            init_cli_console();
#endif
            QCoreApplication core_app(argc, argv);
            const QString base_dir = (argc > 2) ? QDir::cleanPath(QString::fromLocal8Bit(argv[2])) : mocks::project_root_dir();
            const QDate today = QDate::currentDate();
            const QString composed_ver = mocks::format_version_from_date(today.year(), today.month(), today.day());
            print_cli_msg(QStringLiteral("======================================================================"));
            print_cli_msg(QStringLiteral("Data Presente: %1 (Dia: %2, Mes: %3, Ano: %4)")
                              .arg(today.toString(QStringLiteral("dd/MM/yyyy")))
                              .arg(today.day())
                              .arg(today.month())
                              .arg(today.year()));
            print_cli_msg(QStringLiteral("Versao Atual Composta: %1").arg(composed_ver));
            print_cli_msg(QStringLiteral("Diretorio Base: %1").arg(base_dir));
            print_cli_msg(QStringLiteral("======================================================================"));

            const auto statuses = mocks::check_all_files(base_dir);
            bool all_ok = true;
            for (const auto& st : statuses) {
                if (!st.exists) {
                    all_ok = false;
                    print_cli_msg(QStringLiteral("[AUSENTE]   %1").arg(st.rel_path));
                } else if (!st.missing_patterns.isEmpty()) {
                    all_ok = false;
                    print_cli_msg(QStringLiteral("[PENDENTE]  %1 (%2 padroes ausentes)").arg(st.rel_path).arg(st.missing_patterns.size()));
                } else {
                    const QString ver_tag = st.detected_version.isEmpty() ? QString() : QStringLiteral(" (v%1)").arg(st.detected_version);
                    const QString date_tag = st.detected_date.isEmpty() ? QString() : QStringLiteral(" [Data: %1]").arg(st.detected_date);
                    const QString ro_tag = st.is_updatable ? QString() : QStringLiteral(" [Somente leitura]");
                    print_cli_msg(QStringLiteral("[OK]        %1%2%3%4").arg(st.rel_path, ver_tag, date_tag, ro_tag));
                }
            }
            print_cli_msg(QStringLiteral("======================================================================"));
            print_cli_msg(all_ok ? QStringLiteral("TODOS OS ARQUIVOS VERIFICADOS COM SUCESSO.") : QStringLiteral("FALHAS NA VERIFICACAO DE ARQUIVOS."));
            return all_ok ? 0 : 1;
        } else if (arg == QStringLiteral("--apply") || arg == QStringLiteral("--save") || arg == QStringLiteral("--update")) {
#if defined(Q_OS_WIN)
            init_cli_console();
#endif
            QCoreApplication core_app(argc, argv);
            const QString base_dir = (argc > 2) ? QDir::cleanPath(QString::fromLocal8Bit(argv[2])) : mocks::project_root_dir();
            const QDate today = QDate::currentDate();
            QString version = (argc > 3) ? QString::fromLocal8Bit(argv[3]).trimmed() : QString();
            if (version.isEmpty()) {
                version = mocks::format_version_from_date(today.year(), today.month(), today.day());
            }

            int day = today.day();
            int month_idx = today.month() - 1;
            int year = today.year();

            int pd = 0, pm = 0, py = 0;
            if (mocks::parse_version_to_date(version, pd, pm, py)) {
                day = pd;
                month_idx = pm;
                year = py;
            }

            print_cli_msg(QStringLiteral("======================================================================"));
            print_cli_msg(QStringLiteral("Data Presente: %1 (Dia: %2, Mes: %3, Ano: %4)")
                              .arg(today.toString(QStringLiteral("dd/MM/yyyy")))
                              .arg(today.day())
                              .arg(today.month())
                              .arg(today.year()));
            print_cli_msg(QStringLiteral("Versao Atual Composta Aplicada: %1").arg(version));
            print_cli_msg(QStringLiteral("Diretorio Base: %1").arg(base_dir));
            print_cli_msg(QStringLiteral("======================================================================"));

            const mocks::LangInput pt{version, day, month_idx, year};
            const mocks::LangInput en{version, day, month_idx, year};
            const mocks::LangInput es{version, day, month_idx, year};
            const mocks::LangInput fr{version, day, month_idx, year};
            const mocks::LangInput it{version, day, month_idx, year};
            const mocks::LangInput de{version, day, month_idx, year};

            const auto results = mocks::apply_updates(pt, en, es, fr, it, de, base_dir);
            bool all_ok = true;
            for (const auto& r : results) {
                if (!r.ok) {
                    all_ok = false;
                    print_cli_msg(QStringLiteral("[FALHA]     %1 - %2").arg(r.rel_path, r.msg));
                } else {
                    print_cli_msg(QStringLiteral("[ATUALIZADO] %1").arg(r.rel_path));
                }
            }
            print_cli_msg(QStringLiteral("======================================================================"));
            print_cli_msg(all_ok ? QStringLiteral("TODOS OS ARQUIVOS FORAM PROCESSADOS COM SUCESSO.") : QStringLiteral("FALHAS NA ATUALIZACAO DE ARQUIVOS."));
            return all_ok ? 0 : 1;
        }
    }

    QApplication app(argc, argv);
    mocks::VersionEditor editor;
    editor.show();
    return app.exec();
}
