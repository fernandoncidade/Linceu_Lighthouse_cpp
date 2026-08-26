#include "find_VersionEditor.h"

#include <algorithm>

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHash>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringList>
#include <QSpinBox>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace {

struct FileCheckStatus {
    bool exists;
    QStringList found;
    QStringList missing;
};

struct UpdateResult {
    QString key;
    bool ok;
    QString msg;
};

struct LangInput {
    QString version;
    int day;
    int month_index;
    int year;
};

QString project_root_dir() {
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

const QHash<QString, QString> FILES_REL = {
    {"privacy_pt", "source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt"},
    {"privacy_en", "source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt"},
    {"privacy_es", "source/assets/PRIVACY_POLICY/Privacy_Policy_es_ES.txt"},
    {"privacy_fr", "source/assets/PRIVACY_POLICY/Privacy_Policy_fr_FR.txt"},
    {"privacy_it", "source/assets/PRIVACY_POLICY/Privacy_Policy_it_IT.txt"},
    {"privacy_de", "source/assets/PRIVACY_POLICY/Privacy_Policy_de_DE.txt"},
    {"eula_pt", "source/assets/EULA/EULA_pt_BR - Linceu Lighthouse.txt"},
    {"eula_en", "source/assets/EULA/EULA_en_US - Linceu Lighthouse.txt"},
    {"eula_es", "source/assets/EULA/EULA_es_ES - Linceu Lighthouse.txt"},
    {"eula_fr", "source/assets/EULA/EULA_fr_FR - Linceu Lighthouse.txt"},
    {"eula_it", "source/assets/EULA/EULA_it_IT - Linceu Lighthouse.txt"},
    {"eula_de", "source/assets/EULA/EULA_de_DE - Linceu Lighthouse.txt"},
    {"clc_pt", "source/assets/CLC/CLC_pt_BR - Linceu Lighthouse.txt"},
    {"clc_en", "source/assets/CLC/CLC_en_US - Linceu Lighthouse.txt"},
    {"clc_es", "source/assets/CLC/CLC_es_ES - Linceu Lighthouse.txt"},
    {"clc_fr", "source/assets/CLC/CLC_fr_FR - Linceu Lighthouse.txt"},
    {"clc_it", "source/assets/CLC/CLC_it_IT - Linceu Lighthouse.txt"},
    {"clc_de", "source/assets/CLC/CLC_de_DE - Linceu Lighthouse.txt"},
    {"about_pt", "source/assets/ABOUT/ABOUT_pt_BR.txt"},
    {"about_en", "source/assets/ABOUT/ABOUT_en_US.txt"},
    {"about_es", "source/assets/ABOUT/ABOUT_es_ES.txt"},
    {"about_fr", "source/assets/ABOUT/ABOUT_fr_FR.txt"},
    {"about_it", "source/assets/ABOUT/ABOUT_it_IT.txt"},
    {"about_de", "source/assets/ABOUT/ABOUT_de_DE.txt"},
    {"readme", "README.md"},
    {"observacao", "OBSERVACAO.md"},
    {"sobre_dialog_cpp", "source/ui/GerenciamentoMenusUI/gmui_03_SobreDialog.cpp"},
    {"exibir_sobre_cpp", "source/ui/GerenciamentoMenusUI/gmui_22_exibir_sobre.cpp"},
    {"installer_default", "repo/Linceu_Lighthouse.iss"},
    {"installer_mingw", "repo/Linceu_Lighthouse_mingw.iss"},
    {"installer_msvc", "repo/Linceu_Lighthouse_msvc.iss"}
};

const QStringList PT_MONTHS = {"Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};
const QStringList EN_MONTHS = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const QStringList ES_MONTHS = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
const QStringList FR_MONTHS = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};
const QStringList IT_MONTHS = {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"};
const QStringList DE_MONTHS = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};

QString find_window_icon_path() {
    const QString base_dir = project_root_dir();
    const QStringList candidates = {
        QDir(base_dir).filePath("source/assets/icones/linceu_sem_fundo.ico")
    };

    for (const QString& candidate : candidates) {
        const QString absolute_path = QFileInfo(candidate).absoluteFilePath();
        if (QFileInfo::exists(absolute_path)) {
            return absolute_path;
        }
    }

    return {};
}

LangInput current_default_input() {
    const QDate today = QDate::currentDate();
    return {
        QString("%1.%2.%3.0").arg(today.year()).arg(today.month()).arg(today.day()),
        today.day(),
        today.month() - 1,
        today.year()
    };
}

QString readme_version(const QString& version) {
    QString normalized = version.trimmed();
    if (!normalized.startsWith('v', Qt::CaseInsensitive)) {
        normalized.prepend('v');
    }
    return normalized;
}

QString plain_version(const QString& version) {
    QString normalized = version.trimmed();
    if (normalized.startsWith('v', Qt::CaseInsensitive)) {
        normalized.remove(0, 1);
    }
    return normalized;
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
        {"readme",
         {R"(^(> \*\*Observação:\*\*.*)$)",
          R"(^(> \*\*Note:\*\*.*)$)",
          R"(^(Versão:\s*v[0-9]+(?:\.[0-9]+){3}<br>)$)",
          R"(^(\*\*Versão:\*\*\s*v[0-9]+(?:\.[0-9]+){3}<br>)$)",
          R"(^(Version:\s*v[0-9]+(?:\.[0-9]+){3}<br>)$)",
          R"(^(\*\*Version:\*\*\s*v[0-9]+(?:\.[0-9]+){3}<br>)$)"}},
        {"observacao", {R"(^(-\s*`v[0-9]+(?:\.[0-9]+){3}`)\s*$)"}},
        {"sobre_dialog_cpp",
         {R"(^(\s*\+ QStringLiteral\("<p><b>"\)\s*\+\s*get_text\(loc_,\s*"version",\s*"Version"\)\s*\+\s*QStringLiteral\(":</b> [0-9]+(?:\.[0-9]+){3}</p>"\).*)$)"}},
        {"exibir_sobre_cpp",
         {R"(^(\s*\+ QStringLiteral\("<p><b>"\)\s*\+\s*get_text\(loc,\s*"version",\s*"Version"\)\s*\+\s*QStringLiteral\(":</b> [0-9]+(?:\.[0-9]+){3}</p>"\).*)$)"}},
        {"installer_default", {R"(^#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){3}"\s*$)"}},
        {"installer_mingw",
         {R"(^#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){3}"\s*$)",
          R"(^OutputBaseFilename=.*_v[0-9]+(?:\.[0-9]+){3}\s*$)"}},
        {"installer_msvc",
         {R"(^#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){3}"\s*$)",
          R"(^OutputBaseFilename=.*_v[0-9]+(?:\.[0-9]+){3}\s*$)"}}
    };
}

const QHash<QString, QStringList> PATTERNS = build_patterns();

QHash<QString, QString> get_files_for_base(const QString& base_dir) {
    QHash<QString, QString> files;
    const QDir base(base_dir);

    for (auto it = FILES_REL.constBegin(); it != FILES_REL.constEnd(); ++it) {
        QString candidate = QDir::cleanPath(base.filePath(it.value()));
        if (QFileInfo::exists(candidate)) {
            files.insert(it.key(), QFileInfo(candidate).absoluteFilePath());
            continue;
        }

        QString alt_rel = it.value();
        if (alt_rel.startsWith("source/")) alt_rel = alt_rel.mid(QString("source/").size());
        const QString alt = QDir::cleanPath(base.filePath(alt_rel));
        files.insert(it.key(), QFileInfo(QFileInfo::exists(alt) ? alt : candidate).absoluteFilePath());
    }

    return files;
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

QString replace_regex_capture(const QString& text, const QString& pattern, int capture_index, const QString& replacement);

QString replace_line_value_after_prefix(const QString& text, const QString& prefix, const QString& new_value) {
    return replace_regex_capture(
        text,
        QString(R"((^%1)(v?[0-9]+(?:\.[0-9]+){3})((?:<br>)?\r?)$)").arg(QRegularExpression::escape(prefix)),
        2,
        new_value);
}

QString replace_line_segment(const QString& text, const QString& prefix, const QString& suffix, const QString& new_segment) {
    struct SegmentReplacement {
        qsizetype start;
        qsizetype length;
        QString replacement;
    };

    QRegularExpression re(
        QString(R"((^%1).*?(%2)(\r?)$)").arg(QRegularExpression::escape(prefix), QRegularExpression::escape(suffix)),
        QRegularExpression::MultilineOption);
    QString out = text;
    QRegularExpressionMatchIterator it = re.globalMatch(text);
    QList<SegmentReplacement> replacements;

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        replacements.append({match.capturedStart(0), match.capturedLength(0), prefix + new_segment + suffix + match.captured(3)});
    }

    for (int i = replacements.size() - 1; i >= 0; --i) {
        out.replace(replacements[i].start, replacements[i].length, replacements[i].replacement);
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

QString update_about_cpp_version(const QString& text, const QString& version) {
    return replace_regex_capture(
        text,
        R"((\+ QStringLiteral\("<p><b>"\)\s*\+\s*get_text\((?:loc_|loc),\s*"version",\s*"Version"\)\s*\+\s*QStringLiteral\(":</b> )([0-9]+(?:\.[0-9]+){3})(</p>"\)))",
        2,
        plain_version(version));
}

QString update_installer_version(const QString& text, const QString& version) {
    const QString normalized = plain_version(version);
    QString updated = replace_line_by_regex(
        text,
        R"(^#define\s+MyAppVersion\s+"[0-9]+(?:\.[0-9]+){3}"\s*$)",
        QString("#define MyAppVersion \"%1\"").arg(normalized));

    updated = replace_regex_capture(
        updated,
        R"((^OutputBaseFilename=.*_v)([0-9]+(?:\.[0-9]+){3})(\r?)$)",
        2,
        normalized);

    return updated;
}

QString update_readme_section(const QString& text, const QString& section_id, const QString& note_prefix, const QString& note_suffix,
                              const QString& plain_prefix, const QString& bold_prefix, const QString& version) {
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
    section = replace_line_segment(section, note_prefix, note_suffix, version);
    section = replace_line_value_after_prefix(section, plain_prefix, version);
    section = replace_line_value_after_prefix(section, bold_prefix, version);

    QString updated = text;
    updated.replace(start_index, end_index - start_index, section);
    return updated;
}

QString update_readme_versions(const QString& text, const LangInput& pt, const LangInput& en, const LangInput& es, const LangInput& fr,
                               const LangInput& it, const LangInput& de) {
    QString updated = text;

    updated = update_readme_section(
        updated, "ptbr", "> **Observação:** Este repositório refere-se à versão **",
        "** do Projeto Linceu Lighthouse. Apoie o projeto e adquira a versão paga através do link: [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Versão: ", "**Versão:** ", readme_version(pt.version));
    updated = update_readme_section(
        updated, "enus", "> **Note:** This repository refers to the **",
        "** version of the Linceu Lighthouse Project. Support the project and purchase the paid version through the link: [Install via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Version: ", "**Version:** ", readme_version(en.version));
    updated = update_readme_section(
        updated, "eses", "> **Nota:** Este repositorio se refere à versão **",
        "** do Projeto Linceu Lighthouse. Apoya el proyecto y adquiere la versión de pago a través del enlace: [Instalar vía Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Versión: ", "**Versión:** ", readme_version(es.version));
    updated = update_readme_section(
        updated, "frfr", "> **Note :** Ce référentiel fait référence à la version **",
        "** du projet Linceu Lighthouse. Soutenez le projet et achetez la version payante via le lien : [Installer via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Version : ", "**Version:** ", readme_version(fr.version));
    updated = update_readme_section(
        updated, "itit", "> **Nota:** Questo repository si riferisce alla versione **",
        "** del Progetto Linceu Lighthouse. Supporta il progetto e acquista la versione a pagamento tramite il link: [Installa via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Versione: ", "**Versione:** ", readme_version(it.version));
    updated = update_readme_section(
        updated, "dede", "> **Hinweis:** Dieses Repository bezieht sich auf die Version **",
        "** des Linceu Lighthouse Projekts. Unterstützen Sie das Projekt und erwerben Sie die kostenpflichtige Version über den Link: [Über Microsoft Store installieren](https://apps.microsoft.com/detail/9NN8Z5Z700TM)",
        "Version: ", "**Version:** ", readme_version(de.version));

    return updated;
}

QHash<QString, FileCheckStatus> check_expected_lines(const QString& base_dir) {
    QHash<QString, FileCheckStatus> status;
    const auto files = get_files_for_base(base_dir);

    for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
        bool ok = false;
        const QString txt = read_file_robust(it.value(), &ok);
        if (!ok) {
            status.insert(it.key(), {false, {}, {}});
            continue;
        }

        QStringList found;
        QStringList missing;
        const QStringList patterns = PATTERNS.value(it.key());
        for (const QString& pat : patterns) {
            QRegularExpression re(pat, QRegularExpression::MultilineOption);
            const QRegularExpressionMatch m = re.match(txt);
            if (m.hasMatch()) found.append(m.captured(1).trimmed());
            else missing.append(pat);
        }

        if (patterns.isEmpty()) {
            const QStringList lines = txt.split(QRegularExpression("\\r?\\n"));
            const int max_lines = std::min(3, static_cast<int>(lines.size()));
            for (int i = 0; i < max_lines; ++i) found.append(lines[i]);
        }

        status.insert(it.key(), {true, found, missing});
    }

    return status;
}

QList<UpdateResult> apply_updates(const LangInput& pt, const LangInput& en, const LangInput& es, const LangInput& fr,
                                  const LangInput& it, const LangInput& de, const QString& base_dir) {
    QList<UpdateResult> results;
    const auto files = get_files_for_base(base_dir);

    const QString pt_date = QString("%1 de %2 de %3").arg(pt.day).arg(PT_MONTHS.value(pt.month_index).toLower()).arg(pt.year);
    const QString en_date = QString("%1 %2, %3").arg(EN_MONTHS.value(en.month_index)).arg(en.day).arg(en.year);

    const auto make_date = [](const LangInput& x, const QStringList& months, bool use_de, bool dot_after_day) {
        const QString month = months.value(x.month_index);
        if (use_de) return QString("%1 de %2 de %3").arg(x.day).arg(month).arg(x.year);
        if (dot_after_day) return QString("%1. %2 %3").arg(x.day).arg(month).arg(x.year);
        return QString("%1 %2 %3").arg(x.day).arg(month).arg(x.year);
    };

    for (auto it_file = files.constBegin(); it_file != files.constEnd(); ++it_file) {
        bool ok_read = false;
        const QString original = read_file_robust(it_file.value(), &ok_read);
        if (!ok_read) {
            results.append({it_file.key(), false, "arquivo inexistente"});
            continue;
        }

        QString new_txt = original;

        try {
            const QString& key = it_file.key();
            if (key == "privacy_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão:?.*$)", QString("Versão: %1").arg(pt.version));
                new_txt = replace_line_by_regex(new_txt, R"(^Última atualização:?.*$)", QString("Última atualização: %1").arg(pt_date));
            } else if (key == "privacy_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(en.version));
                new_txt = replace_line_by_regex(new_txt, R"(^Last updated:?.*$)", QString("Last updated: %1").arg(en_date));
            } else if (key == "privacy_es" || key == "privacy_fr" || key == "privacy_it" || key == "privacy_de") {
                QString ver_label, upd_label, localized_date;
                LangInput x;
                if (key == "privacy_es") {
                    ver_label = "Versión";
                    upd_label = "Última actualización";
                    x = es;
                    localized_date = make_date(x, ES_MONTHS, false, false);
                } else if (key == "privacy_fr") {
                    ver_label = "Version";
                    upd_label = "Dernière mise à jour";
                    x = fr;
                    localized_date = make_date(x, FR_MONTHS, false, false);
                } else if (key == "privacy_it") {
                    ver_label = "Versione";
                    upd_label = "Ultimo aggiornamento";
                    x = it;
                    localized_date = make_date(x, IT_MONTHS, false, false);
                } else {
                    ver_label = "Version";
                    upd_label = "Letzte Aktualisierung";
                    x = de;
                    localized_date = make_date(x, DE_MONTHS, true, false);
                }
                new_txt = replace_line_by_regex(new_txt, QString(R"(^%1:?.*$)").arg(QRegularExpression::escape(ver_label)), QString("%1: %2").arg(ver_label, x.version));
                new_txt = replace_line_by_regex(new_txt, QString(R"(^%1\s*:?.*$)").arg(QRegularExpression::escape(upd_label)), QString("%1: %2").arg(upd_label, localized_date));
            } else if (key == "eula_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão\s*.*$)", QString("Versão %1, %2").arg(pt.version, pt_date));
            } else if (key == "eula_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(en.version, en_date));
            } else if (key == "eula_es" || key == "eula_fr" || key == "eula_it" || key == "eula_de") {
                QString label, date;
                LangInput x;
                if (key == "eula_es") {
                    label = "Versión";
                    x = es;
                    date = make_date(x, ES_MONTHS, false, false);
                } else if (key == "eula_fr") {
                    label = "Version";
                    x = fr;
                    date = make_date(x, FR_MONTHS, false, false);
                } else if (key == "eula_it") {
                    label = "Versione";
                    x = it;
                    date = make_date(x, IT_MONTHS, false, false);
                } else {
                    label = "Version";
                    x = de;
                    date = make_date(x, DE_MONTHS, true, false);
                }
                new_txt = replace_line_by_regex(new_txt, QString(R"(^%1\s*.*$)").arg(QRegularExpression::escape(label)), QString("%1 %2, %3").arg(label, x.version, date));
            } else if (key == "clc_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão\s*.*$)", QString("Versão %1, %2").arg(pt.version, pt_date));
            } else if (key == "clc_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version\s*.*$)", QString("Version %1, %2").arg(en.version, en_date));
            } else if (key == "clc_es" || key == "clc_fr" || key == "clc_it" || key == "clc_de") {
                QString label, date;
                LangInput x;
                if (key == "clc_es") {
                    label = "Versión";
                    x = es;
                    date = make_date(x, ES_MONTHS, false, false);
                } else if (key == "clc_fr") {
                    label = "Version";
                    x = fr;
                    date = make_date(x, FR_MONTHS, false, false);
                } else if (key == "clc_it") {
                    label = "Versione";
                    x = it;
                    date = make_date(x, IT_MONTHS, false, false);
                } else {
                    label = "Version";
                    x = de;
                    date = make_date(x, DE_MONTHS, true, false);
                }
                new_txt = replace_line_by_regex(new_txt, QString(R"(^%1\s*.*$)").arg(QRegularExpression::escape(label)), QString("%1 %2, %3").arg(label, x.version, date));
            } else if (key == "about_pt") {
                new_txt = replace_line_by_regex(new_txt, R"(^Versão:?.*$)", QString("Versão: %1").arg(pt.version));
            } else if (key == "about_en") {
                new_txt = replace_line_by_regex(new_txt, R"(^Version:?.*$)", QString("Version: %1").arg(en.version));
            } else if (key == "about_es" || key == "about_fr" || key == "about_it" || key == "about_de") {
                QString label;
                QString version;
                if (key == "about_es") {
                    label = "Versión";
                    version = es.version;
                } else if (key == "about_fr") {
                    label = "Version";
                    version = fr.version;
                } else if (key == "about_it") {
                    label = "Versione";
                    version = it.version;
                } else {
                    label = "Version";
                    version = de.version;
                }
                new_txt = replace_line_by_regex(new_txt, QString(R"(^%1:?.*$)").arg(QRegularExpression::escape(label)), QString("%1: %2").arg(label, version));
            } else if (key == "readme") {
                new_txt = update_readme_versions(new_txt, pt, en, es, fr, it, de);
            } else if (key == "observacao") {
                new_txt = replace_line_by_regex(new_txt, R"(^-\s*`v[0-9]+(?:\.[0-9]+){3}`\s*$)", QString("- `%1`").arg(readme_version(pt.version)));
            } else if (key == "sobre_dialog_cpp" || key == "exibir_sobre_cpp") {
                new_txt = update_about_cpp_version(new_txt, pt.version);
            } else if (key == "installer_default" || key == "installer_mingw" || key == "installer_msvc") {
                new_txt = update_installer_version(new_txt, pt.version);
            }

            if (new_txt != original) {
                QString err;
                if (write_file_utf8(it_file.value(), new_txt, &err)) results.append({it_file.key(), true, "atualizado"});
                else results.append({it_file.key(), false, QString("erro escrita: %1").arg(err)});
            } else {
                results.append({it_file.key(), false, "padrões não encontrados / sem alteração"});
            }
        } catch (...) {
            results.append({it_file.key(), false, "erro durante atualização"});
        }
    }

    return results;
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
    day_spin->setRange(1, 31);
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

    return group;
}

}  // namespace

namespace mocks {

VersionEditor::VersionEditor(QWidget* parent) : QWidget(parent), selected_base_dir_(project_root_dir()) {
    setWindowTitle("Editor de Versões / Datas - Lúmen (PT/EN)");
    resize(820, 520);

    const QString icon_path = find_window_icon_path();
    if (!icon_path.isEmpty()) {
        setWindowIcon(QIcon(icon_path));
    }

    auto* root = new QVBoxLayout(this);

    auto* folder_layout = new QHBoxLayout();
    folder_layout->addWidget(new QLabel("Pasta dos arquivos:"));
    folder_path_ = new QLineEdit(selected_base_dir_);
    folder_path_->setReadOnly(true);
    folder_layout->addWidget(folder_path_);
    auto* select_folder_btn = new QPushButton("Selecionar...");
    folder_layout->addWidget(select_folder_btn);
    root->addLayout(folder_layout);

    root->addWidget(create_group("Português (pt-BR)", "Versão:", "Dia:", "Mês:", "Ano:", PT_MONTHS, pt_version_, pt_day_, pt_month_, pt_year_));
    root->addWidget(create_group("English (en-US)", "Version:", "Day:", "Month:", "Year:", EN_MONTHS, en_version_, en_day_, en_month_, en_year_));
    root->addWidget(create_group("Español (es-ES)", "Versión:", "Día:", "Mes:", "Año:", ES_MONTHS, es_version_, es_day_, es_month_, es_year_));
    root->addWidget(create_group("Français (fr-FR)", "Version:", "Jour:", "Mois:", "Année:", FR_MONTHS, fr_version_, fr_day_, fr_month_, fr_year_));
    root->addWidget(create_group("Italiano (it-IT)", "Versione:", "Giorno:", "Mese:", "Anno:", IT_MONTHS, it_version_, it_day_, it_month_, it_year_));
    root->addWidget(create_group("Deutsch (de-DE)", "Version:", "Tag:", "Monat:", "Jahr:", DE_MONTHS, de_version_, de_day_, de_month_, de_year_));

    auto* btn_layout = new QHBoxLayout();
    auto* check_btn = new QPushButton("🔍 Verificar arquivos");
    auto* save_btn = new QPushButton("💾 Salvar alterações");
    auto* refresh_btn = new QPushButton("🔄 Recarregar verificação");
    btn_layout->addWidget(check_btn);
    btn_layout->addWidget(save_btn);
    btn_layout->addWidget(refresh_btn);
    root->addLayout(btn_layout);

    status_ = new QTextEdit();
    status_->setReadOnly(true);
    root->addWidget(status_);

    QObject::connect(select_folder_btn, &QPushButton::clicked, this, &VersionEditor::on_select_folder);
    QObject::connect(check_btn, &QPushButton::clicked, this, &VersionEditor::on_check);
    QObject::connect(save_btn, &QPushButton::clicked, this, &VersionEditor::on_save);
    QObject::connect(refresh_btn, &QPushButton::clicked, this, &VersionEditor::on_check);

    on_check();
}

void VersionEditor::on_select_folder() {
    const QString dir_path = QFileDialog::getExistingDirectory(this, "Selecione a pasta onde os arquivos estão", selected_base_dir_);
    if (!dir_path.isEmpty()) {
        selected_base_dir_ = dir_path;
        folder_path_->setText(selected_base_dir_);
        on_check();
    }
}

void VersionEditor::on_check() {
    const auto status = check_expected_lines(selected_base_dir_);
    QStringList lines;
    lines << QString("Base: %1").arg(selected_base_dir_);

    const auto files = get_files_for_base(selected_base_dir_);
    for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
        const FileCheckStatus info = status.value(it.key(), {false, {}, {}});
        lines << QString("%1: file %2").arg(it.key(), info.exists ? "exists" : "missing");
        if (info.exists) {
            if (!info.found.isEmpty()) {
                lines << "  found:";
                for (const QString& f : info.found) lines << QString("    - %1").arg(f);
            }
            if (!info.missing.isEmpty()) {
                lines << "  missing:";
                for (const QString& m : info.missing) lines << QString("    - %1").arg(m);
            }
        }
        lines << "";
    }

    status_->setPlainText(lines.join("\n"));
}

void VersionEditor::on_save() {
    const LangInput pt{pt_version_->text().trimmed(), pt_day_->value(), pt_month_->currentIndex(), pt_year_->value()};
    const LangInput en{en_version_->text().trimmed(), en_day_->value(), en_month_->currentIndex(), en_year_->value()};
    const LangInput es{es_version_->text().trimmed(), es_day_->value(), es_month_->currentIndex(), es_year_->value()};
    const LangInput fr{fr_version_->text().trimmed(), fr_day_->value(), fr_month_->currentIndex(), fr_year_->value()};
    const LangInput it{it_version_->text().trimmed(), it_day_->value(), it_month_->currentIndex(), it_year_->value()};
    const LangInput de{de_version_->text().trimmed(), de_day_->value(), de_month_->currentIndex(), de_year_->value()};

    const QList<UpdateResult> results = apply_updates(pt, en, es, fr, it, de, selected_base_dir_);
    QStringList lines;
    lines << "Resultados:";
    for (const UpdateResult& r : results) {
        lines << QString("%1: %2 - %3").arg(r.key, r.ok ? "OK" : "FAIL", r.msg);
    }
    status_->setPlainText(lines.join("\n"));
}

}  // namespace mocks

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    mocks::VersionEditor editor;
    editor.show();
    return app.exec();
}
