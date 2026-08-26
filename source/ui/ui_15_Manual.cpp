#include "ui_15_Manual.h"

#include "source/ui/Manual/DOC_DE_DE.h"
#include "source/ui/Manual/DOC_EN_US.h"
#include "source/ui/Manual/DOC_ES_ES.h"
#include "source/ui/Manual/DOC_FR_FR.h"
#include "source/ui/Manual/DOC_IT_IT.h"
#include "source/ui/Manual/DOC_PT_BR.h"

namespace source::ui::manual {
namespace {
const QString kManualTopSectionId = QStringLiteral("manual_top");

QString intro_text(const QString& lang) {
    if (lang == "pt_BR") {
        return "Este manual descreve como operar o aplicativo Linceu Lighthouse (modo de uso), cobrindo funcionalidades, atalhos, fluxo de trabalho sugerido, solução de problemas e informações sobre persistência de dados.";
    }
    if (lang == "es_ES") {
        return "Este manual describe cómo operar la aplicación Linceu Lighthouse (guía del usuario), cubriendo funcionalidades, atajos, flujos de trabajo sugeridos, solución de problemas e información sobre la persistencia de datos.";
    }
    if (lang == "fr_FR") {
        return "Ce manuel décrit comment utiliser l'application Linceu Lighthouse (guide de l'utilisateur), couvrant les fonctionnalités, raccourcis, flux de travail suggérés, dépannage et informations sur la persistance des données.";
    }
    if (lang == "it_IT") {
        return "Questo manuale descrive come utilizzare l'applicazione Linceu Lighthouse (guida utente), coprendo funzionalità, scorciatoie, flussi di lavoro suggeriti, risoluzione dei problemi e informazioni sulla persistenza dei dati.";
    }
    if (lang == "de_DE") {
        return "Dieses Handbuch beschreibt, wie die Anwendung Linceu Lighthouse verwendet wird (Benutzerhandbuch) und behandelt Funktionen, Tastenkürzel, empfohlene Arbeitsabläufe, Fehlerbehebung und Informationen zur Datenpersistenz.";
    }
    return "This manual describes how to operate the Linceu Lighthouse application (user guide), covering features, shortcuts, suggested workflows, troubleshooting, and information about data persistence.";
}

QString non_dev_text(const QString& lang) {
    if (lang == "pt_BR") {
        return "Não é um guia de desenvolvimento.";
    }
    if (lang == "es_ES") {
        return "No es una guía de desarrollo.";
    }
    if (lang == "fr_FR") {
        return "Ce n'est pas un guide de développement.";
    }
    if (lang == "it_IT") {
        return "Non è una guida per sviluppatori.";
    }
    if (lang == "de_DE") {
        return "Es ist kein Entwicklerhandbuch.";
    }
    return "It is not a development guide.";
}

QString toc_title(const QString& lang) {
    if (lang == "pt_BR") return "Indice";
    if (lang == "es_ES") return "Índice";
    if (lang == "fr_FR") return "Table des matières";
    if (lang == "it_IT") return "Indice";
    if (lang == "de_DE") return "Inhaltsverzeichnis";
    return "Table of Contents";
}

QString toc_divider(const QString& lang) {
    if (lang == "pt_BR" || lang == "es_ES" || lang == "it_IT") {
        return QString("-").repeated(10);
    }
    return QString("-").repeated(30);
}

std::vector<ManualSection> get_doc_for_lang(const QString& lang) {
    if (lang == "en_US") return doc_en_us();
    if (lang == "es_ES") return doc_es_es();
    if (lang == "fr_FR") return doc_fr_fr();
    if (lang == "it_IT") return doc_it_it();
    if (lang == "de_DE") return doc_de_de();
    return doc_pt_br();
}

}

QString normalize_language(const QString& lang) {
    if (lang.trimmed().isEmpty()) {
        return "pt_BR";
    }

    const QString v = QString(lang).trimmed().replace("-", "_").toLower();
    if (v == "pt_br" || v == "pt") return "pt_BR";
    if (v == "en_us" || v == "en") return "en_US";
    if (v == "es_es" || v == "es") return "es_ES";
    if (v == "fr_fr" || v == "fr") return "fr_FR";
    if (v == "it_it" || v == "it") return "it_IT";
    if (v == "de_de" || v == "de") return "de_DE";
    return "pt_BR";
}

QString get_manual_title(const QString& lang) {
    const QString l = normalize_language(lang);
    if (l == "pt_BR") return "Manual de Utilização — Linceu Lighthouse";
    if (l == "en_US") return "User Manual — Linceu Lighthouse";
    if (l == "es_ES") return "Manual de Uso — Linceu Lighthouse";
    if (l == "fr_FR") return "Manuel d'utilisation — Linceu Lighthouse";
    if (l == "it_IT") return "Manuale utente — Linceu Lighthouse";
    if (l == "de_DE") return "Benutzerhandbuch — Linceu Lighthouse";
    return "User Manual — Linceu Lighthouse";
}

std::vector<ManualSection> get_manual_document(const QString& lang) {
    return get_doc_for_lang(normalize_language(lang));
}

std::tuple<std::vector<ManualBlock>, QStringList> get_manual_blocks(const QString& lang) {
    const QString l = normalize_language(lang);
    const auto sections = get_manual_document(l);

    std::vector<ManualBlock> blocks;
    QStringList order;

    blocks.push_back(ManualBlock{"line", get_manual_title(l), ""});
    blocks.push_back(ManualBlock{"line", QString("=").repeated(get_manual_title(l).size()), ""});
    blocks.push_back(ManualBlock{"blank", "", ""});
    blocks.push_back(ManualBlock{"paragraph", intro_text(l), ""});
    blocks.push_back(ManualBlock{"paragraph", non_dev_text(l), ""});
    blocks.push_back(ManualBlock{"blank", "", ""});
    blocks.push_back(ManualBlock{"toc_title", toc_title(l), ""});
    blocks.push_back(ManualBlock{"toc_item", QStringLiteral("1. %1").arg(get_manual_title(l)), kManualTopSectionId});

    int idx = 2;
    for (const auto& s : sections) {
        blocks.push_back(ManualBlock{"toc_item", QString::number(idx) + ". " + s.title, s.id});
        ++idx;
    }

    blocks.push_back(ManualBlock{"blank", "", ""});
    blocks.push_back(ManualBlock{"divider", QString("-").repeated(60), ""});
    blocks.push_back(ManualBlock{"blank", "", ""});

    for (const auto& s : sections) {
        order.push_back(s.id);
        blocks.push_back(ManualBlock{"section_title", s.title, s.id});
        blocks.push_back(ManualBlock{"blank", "", ""});

        for (const auto& p : s.paragraphs) {
            blocks.push_back(ManualBlock{"paragraph", p, ""});
            blocks.push_back(ManualBlock{"blank", "", ""});
        }

        for (const auto& b : s.bullets) {
            blocks.push_back(ManualBlock{"bullet", b, ""});
        }

        if (!s.bullets.empty()) {
            blocks.push_back(ManualBlock{"blank", "", ""});
        }

        for (const auto& d : s.details) {
            blocks.push_back(ManualBlock{"detail_title", d.summary, ""});
            blocks.push_back(ManualBlock{"blank", "", ""});

            for (const auto& p : d.paragraphs) {
                blocks.push_back(ManualBlock{"paragraph", p, ""});
                blocks.push_back(ManualBlock{"blank", "", ""});
            }

            for (const auto& b : d.bullets) {
                blocks.push_back(ManualBlock{"bullet", b, ""});
            }

            if (!d.bullets.empty()) {
                blocks.push_back(ManualBlock{"blank", "", ""});
            }
        }

        blocks.push_back(ManualBlock{"divider", QString("-").repeated(60), ""});
        blocks.push_back(ManualBlock{"blank", "", ""});
    }

    return {blocks, order};
}

QString get_manual_text(const QString& lang) {
    return std::get<0>(get_manual_text_with_positions(lang));
}

std::tuple<QString, QHash<QString, int>, QStringList> get_manual_text_with_positions(const QString& lang) {
    const QString l = normalize_language(lang);
    const auto sections = get_manual_document(l);

    QStringList lines;
    QHash<QString, int> positions;
    QStringList order;

    auto current_offset = [&lines]() {
        int sum = 0;
        for (const auto& line : lines) {
            sum += line.size() + 1;
        }
        return sum;
    };

    const QString title = get_manual_title(l);
    lines << title;
    lines << QString("=").repeated(title.size());
    lines << "";
    lines << intro_text(l);
    lines << non_dev_text(l);
    lines << "";
    lines << toc_title(l);
    lines << toc_divider(l);
    lines << (QStringLiteral("1. ") + title);

    int idx = 2;
    for (const auto& s : sections) {
        lines << (QString::number(idx) + ". " + s.title);
        ++idx;
    }

    lines << "";
    lines << QString("-").repeated(60);
    lines << "";

    for (const auto& s : sections) {
        positions.insert(s.id, current_offset());
        order << s.id;

        lines << s.title;
        lines << QString("-").repeated(s.title.size());
        lines << "";

        for (const auto& p : s.paragraphs) {
            lines << p;
            lines << "";
        }

        for (const auto& b : s.bullets) {
            lines << ("- " + b);
        }

        if (!s.bullets.empty()) {
            lines << "";
        }

        for (const auto& d : s.details) {
            lines << d.summary;
            lines << QString(".").repeated(d.summary.size());
            lines << "";

            for (const auto& p : d.paragraphs) {
                lines << p;
                lines << "";
            }

            for (const auto& b : d.bullets) {
                lines << ("- " + b);
            }

            if (!d.bullets.empty()) {
                lines << "";
            }
        }

        lines << QString("-").repeated(60);
        lines << "";
    }

    return {lines.join("\n"), positions, order};
}

}  // namespace source::ui::manual
