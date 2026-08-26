#include "common_Manual.h"

#include <QChar>

#include "source/utils/CaminhoPersistenteUtils.h"

namespace source {
namespace ui {
namespace manual {

namespace {

QString normalize_shortcut_language(const QString& lang) {
    if (lang.trimmed().isEmpty()) {
        return QStringLiteral("pt_BR");
    }

    const QString v = QString(lang).trimmed().replace("-", "_").toLower();
    if (v == "pt_br" || v == "pt") return QStringLiteral("pt_BR");
    if (v == "en_us" || v == "en") return QStringLiteral("en_US");
    if (v == "es_es" || v == "es") return QStringLiteral("es_ES");
    if (v == "fr_fr" || v == "fr") return QStringLiteral("fr_FR");
    if (v == "it_it" || v == "it") return QStringLiteral("it_IT");
    if (v == "de_de" || v == "de") return QStringLiteral("de_DE");
    return QStringLiteral("pt_BR");
}

std::vector<ShortcutEntry> shortcut_entries_pt_br() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Abre a janela Ajuda com a lista de atalhos disponíveis.")},
        {QStringLiteral("Ctrl+M"), QStringLiteral("Abre o Manual de Utilização.")},
        {QStringLiteral("Ctrl+I"), QStringLiteral("Abre a janela Sobre.")},
        {QStringLiteral("Ctrl+Shift+O"), QStringLiteral("Abre o diálogo para selecionar o diretório monitorado.")},
        {QStringLiteral("Ctrl+R"), QStringLiteral("Inicia ou interrompe a análise do diretório.")},
        {QStringLiteral("Ctrl+P"), QStringLiteral("Pausa ou retoma o monitoramento ou escaneamento.")},
        {QStringLiteral("Ctrl+Shift+S"), QStringLiteral("Abre o diálogo Salvar como.")},
        {QStringLiteral("Ctrl+S"), QStringLiteral("Salva os dados atuais.")},
        {QStringLiteral("Ctrl+G"), QStringLiteral("Abre a janela de estatísticas.")},
        {QStringLiteral("Ctrl+L"), QStringLiteral("Limpa os dados carregados.")},
        {QStringLiteral("Ctrl+Q"), QStringLiteral("Encerra a aplicação.")},
        {QStringLiteral("Ctrl+A"), QStringLiteral("Seleciona todas as colunas no menu de configuração de colunas.")},
        {QStringLiteral("Ctrl+Shift+A"), QStringLiteral("Seleciona todas as colunas coloridas no menu de coloração de colunas.")},
        {QStringLiteral("Ctrl+O"), QStringLiteral("Ativa ou desativa a ordenação automática das linhas.")}
    };
}

std::vector<ShortcutEntry> shortcut_entries_en_us() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Opens the Help window with the list of available shortcuts.")},
        {QStringLiteral("Ctrl+M"), QStringLiteral("Opens the User Manual.")},
        {QStringLiteral("Ctrl+I"), QStringLiteral("Opens the About window.")},
        {QStringLiteral("Ctrl+Shift+O"), QStringLiteral("Opens the dialog to select the monitored directory.")},
        {QStringLiteral("Ctrl+R"), QStringLiteral("Starts or stops the directory analysis.")},
        {QStringLiteral("Ctrl+P"), QStringLiteral("Pauses or resumes monitoring or scanning.")},
        {QStringLiteral("Ctrl+Shift+S"), QStringLiteral("Opens the Save As dialog.")},
        {QStringLiteral("Ctrl+S"), QStringLiteral("Saves the current data.")},
        {QStringLiteral("Ctrl+G"), QStringLiteral("Opens the statistics window.")},
        {QStringLiteral("Ctrl+L"), QStringLiteral("Clears the loaded data.")},
        {QStringLiteral("Ctrl+Q"), QStringLiteral("Closes the application.")},
        {QStringLiteral("Ctrl+A"), QStringLiteral("Selects all columns in the column configuration menu.")},
        {QStringLiteral("Ctrl+Shift+A"), QStringLiteral("Selects all colored columns in the column coloring menu.")},
        {QStringLiteral("Ctrl+O"), QStringLiteral("Enables or disables automatic row sorting.")}
    };
}

std::vector<ShortcutEntry> shortcut_entries_es_es() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Abre la ventana Ayuda con la lista de atajos disponibles.")},
        {QStringLiteral("Ctrl+M"), QStringLiteral("Abre el Manual de Uso.")},
        {QStringLiteral("Ctrl+I"), QStringLiteral("Abre la ventana Acerca de.")},
        {QStringLiteral("Ctrl+Shift+O"), QStringLiteral("Abre el diálogo para seleccionar el directorio monitorizado.")},
        {QStringLiteral("Ctrl+R"), QStringLiteral("Inicia o detiene el análisis del directorio.")},
        {QStringLiteral("Ctrl+P"), QStringLiteral("Pausa o reanuda la monitorización o el escaneo.")},
        {QStringLiteral("Ctrl+Shift+S"), QStringLiteral("Abre el diálogo Guardar como.")},
        {QStringLiteral("Ctrl+S"), QStringLiteral("Guarda los datos actuales.")},
        {QStringLiteral("Ctrl+G"), QStringLiteral("Abre la ventana de estadísticas.")},
        {QStringLiteral("Ctrl+L"), QStringLiteral("Limpia los datos cargados.")},
        {QStringLiteral("Ctrl+Q"), QStringLiteral("Cierra la aplicación.")},
        {QStringLiteral("Ctrl+A"), QStringLiteral("Selecciona todas las columnas en el menú de configuración de columnas.")},
        {QStringLiteral("Ctrl+Shift+A"), QStringLiteral("Selecciona todas las columnas coloreadas en el menú de coloración de columnas.")},
        {QStringLiteral("Ctrl+O"), QStringLiteral("Activa o desactiva la ordenación automática de las filas.")}
    };
}

std::vector<ShortcutEntry> shortcut_entries_fr_fr() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Ouvre la fenêtre d'aide avec la liste des raccourcis disponibles.")},
        {QStringLiteral("Ctrl+M"), QStringLiteral("Ouvre le manuel d'utilisation.")},
        {QStringLiteral("Ctrl+I"), QStringLiteral("Ouvre la fenêtre À propos.")},
        {QStringLiteral("Ctrl+Shift+O"), QStringLiteral("Ouvre la boîte de dialogue pour sélectionner le répertoire surveillé.")},
        {QStringLiteral("Ctrl+R"), QStringLiteral("Démarre ou arrête l'analyse du répertoire.")},
        {QStringLiteral("Ctrl+P"), QStringLiteral("Met en pause ou reprend la surveillance ou l'analyse.")},
        {QStringLiteral("Ctrl+Shift+S"), QStringLiteral("Ouvre la boîte de dialogue Enregistrer sous.")},
        {QStringLiteral("Ctrl+S"), QStringLiteral("Enregistre les données actuelles.")},
        {QStringLiteral("Ctrl+G"), QStringLiteral("Ouvre la fenêtre des statistiques.")},
        {QStringLiteral("Ctrl+L"), QStringLiteral("Efface les données chargées.")},
        {QStringLiteral("Ctrl+Q"), QStringLiteral("Ferme l'application.")},
        {QStringLiteral("Ctrl+A"), QStringLiteral("Sélectionne toutes les colonnes dans le menu de configuration des colonnes.")},
        {QStringLiteral("Ctrl+Shift+A"), QStringLiteral("Sélectionne toutes les colonnes colorées dans le menu de coloration des colonnes.")},
        {QStringLiteral("Ctrl+O"), QStringLiteral("Active ou désactive le tri automatique des lignes.")}
    };
}

std::vector<ShortcutEntry> shortcut_entries_it_it() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Apre la finestra Aiuto con l'elenco delle scorciatoie disponibili.")},
        {QStringLiteral("Ctrl+M"), QStringLiteral("Apre il manuale utente.")},
        {QStringLiteral("Ctrl+I"), QStringLiteral("Apre la finestra Informazioni.")},
        {QStringLiteral("Ctrl+Shift+O"), QStringLiteral("Apre la finestra per selezionare la cartella monitorata.")},
        {QStringLiteral("Ctrl+R"), QStringLiteral("Avvia o interrompe l'analisi della cartella.")},
        {QStringLiteral("Ctrl+P"), QStringLiteral("Mette in pausa o riprende il monitoraggio o la scansione.")},
        {QStringLiteral("Ctrl+Shift+S"), QStringLiteral("Apre la finestra Salva con nome.")},
        {QStringLiteral("Ctrl+S"), QStringLiteral("Salva i dati correnti.")},
        {QStringLiteral("Ctrl+G"), QStringLiteral("Apre la finestra delle statistiche.")},
        {QStringLiteral("Ctrl+L"), QStringLiteral("Cancella i dati caricati.")},
        {QStringLiteral("Ctrl+Q"), QStringLiteral("Chiude l'applicazione.")},
        {QStringLiteral("Ctrl+A"), QStringLiteral("Seleziona tutte le colonne nel menu di configurazione delle colonne.")},
        {QStringLiteral("Ctrl+Shift+A"), QStringLiteral("Seleziona tutte le colonne colorate nel menu di colorazione delle colonne.")},
        {QStringLiteral("Ctrl+O"), QStringLiteral("Attiva o disattiva l'ordinamento automatico delle righe.")}
    };
}

std::vector<ShortcutEntry> shortcut_entries_de_de() {
    return {
        {QStringLiteral("F1"), QStringLiteral("Öffnet das Hilfefenster mit der Liste der verfügbaren Tastenkürzel.")},
        {QStringLiteral("Strg+M"), QStringLiteral("Öffnet das Benutzerhandbuch.")},
        {QStringLiteral("Strg+I"), QStringLiteral("Öffnet das Fenster Über.")},
        {QStringLiteral("Strg+Shift+O"), QStringLiteral("Öffnet den Dialog zum Auswählen des überwachten Verzeichnisses.")},
        {QStringLiteral("Strg+R"), QStringLiteral("Startet oder beendet die Verzeichnisanalyse.")},
        {QStringLiteral("Strg+P"), QStringLiteral("Pausiert oder setzt Überwachung bzw. Scan fort.")},
        {QStringLiteral("Strg+Shift+S"), QStringLiteral("Öffnet den Dialog Speichern unter.")},
        {QStringLiteral("Strg+S"), QStringLiteral("Speichert die aktuellen Daten.")},
        {QStringLiteral("Strg+G"), QStringLiteral("Öffnet das Statistikfenster.")},
        {QStringLiteral("Strg+L"), QStringLiteral("Löscht die geladenen Daten.")},
        {QStringLiteral("Strg+Q"), QStringLiteral("Beendet die Anwendung.")},
        {QStringLiteral("Strg+A"), QStringLiteral("Wählt alle Spalten im Menü zur Spaltenkonfiguration aus.")},
        {QStringLiteral("Strg+Shift+A"), QStringLiteral("Wählt alle farbigen Spalten im Menü zur Spaltenfärbung aus.")},
        {QStringLiteral("Strg+O"), QStringLiteral("Aktiviert oder deaktiviert die automatische Zeilensortierung.")}
    };
}

}  // namespace

QString data_dir() {
    try {
        return source::utils::obter_caminho_persistente();
    } catch (...) {
        return QString();
    }
}

QString to_unicode_bold(const QString& s) {
    QString out;
    out.reserve(s.size());

    for (const QChar ch : s) {
        if (ch >= QChar('A') && ch <= QChar('Z')) {
            out.append(QChar::fromUcs4(ch.unicode() - 'A' + 0x1D400));
        } else if (ch >= QChar('a') && ch <= QChar('z')) {
            out.append(QChar::fromUcs4(ch.unicode() - 'a' + 0x1D41A));
        } else if (ch >= QChar('0') && ch <= QChar('9')) {
            out.append(QChar::fromUcs4(ch.unicode() - '0' + 0x1D7CE));
        } else {
            out.append(ch);
        }
    }

    return out;
}

std::vector<ShortcutEntry> shortcut_entries(const QString& lang) {
    const QString normalized = normalize_shortcut_language(lang);

    if (normalized == QStringLiteral("en_US")) return shortcut_entries_en_us();
    if (normalized == QStringLiteral("es_ES")) return shortcut_entries_es_es();
    if (normalized == QStringLiteral("fr_FR")) return shortcut_entries_fr_fr();
    if (normalized == QStringLiteral("it_IT")) return shortcut_entries_it_it();
    if (normalized == QStringLiteral("de_DE")) return shortcut_entries_de_de();
    return shortcut_entries_pt_br();
}

std::vector<QString> shortcut_lines(const QString& lang) {
    const auto entries = source::ui::manual::shortcut_entries(lang);
    std::vector<QString> lines;
    lines.reserve(entries.size());

    for (const auto& entry : entries) {
        lines.push_back(entry.sequence + QStringLiteral(": ") + entry.description);
    }

    return lines;
}

} // namespace manual
} // namespace ui
} // namespace source
