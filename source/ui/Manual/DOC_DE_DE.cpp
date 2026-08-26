#include "DOC_DE_DE.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_de_de() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("neue-funktionen"),
            QStringLiteral("Neue Funktionen"),
            {
                QStringLiteral("Es wurden Verbesserungen bei der Zuverlässigkeit der Echtzeit-Überwachung, bei Pause/Fortsetzen/Stoppen und bei der Tabellenaktualisierung unter hoher Ereignislast umgesetzt."),
                QStringLiteral("Die folgenden Anweisungen zeigen, wie diese Verbesserungen im täglichen Betrieb sicher genutzt werden."),
            },
            {
                QStringLiteral("Präzisere Erkennung von Hinzufügen, Ändern, Löschen, Umbenennen und Verschieben von Dateien."),
                QStringLiteral("Stabilere visuelle Aktualisierung bei großen Listen mit geringerem Risiko temporärer Inkonsistenzen während Lastspitzen."),
                QStringLiteral("Konsistenterer Sprachwechsel in Menüs, Tabelle, Handbuch, Hilfe und Statistiken ohne Neustart."),
            },
            {
                {
                    QStringLiteral("Empfohlener Ablauf für kontinuierliche Überwachung"),
                    {
                        QStringLiteral("1) Zielverzeichnisse hinzufügen und einen vollständigen initialen Scan ausführen."),
                        QStringLiteral("2) Prüfen, ob Tabelle und Statusanzeigen korrekt aktualisiert wurden."),
                        QStringLiteral("3) Kontinuierliche Überwachung aktivieren, um neue Ereignisse in Echtzeit zu erfassen."),
                        QStringLiteral("4) Filter verwenden, um Rauschen zu reduzieren und Audits zu beschleunigen."),
                    },
                    {}
                },
                {
                    QStringLiteral("Sicherer Einsatz von Pause, Fortsetzen und Stoppen"),
                    {
                        QStringLiteral("Bei langen Vorgängen Überwachung pausieren und Zwischenprüfung durchführen."),
                        QStringLiteral("Erst nach Validierung der sichtbaren Daten fortsetzen."),
                        QStringLiteral("Vor dem Beenden Überwachung/Scan explizit stoppen und letzte Statusaktualisierung abwarten."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("dateien-oeffnen-mime-ordnerdaten"),
            QStringLiteral("Dateien öffnen, MIME und Ordnerdaten"),
            {
                QStringLiteral("Die Tabelle hilft nun dabei, gemeldete Elemente auch dann zu öffnen, wenn es keine Textdateien sind. Bei Öffnen oder Doppelklick auf eine Dateizeile versucht die Anwendung, die Windows-Standard-App zu verwenden, und kann weitere kompatible Optionen anbieten."),
                QStringLiteral("Die MIME-Spalte zeigt den tatsächlichen Inhaltstyp in lesbarer Form in der aktuellen Sprache und behält die ursprüngliche Kennung in Klammern für Audits bei. Erstellungsdatum, Änderungsdatum und Zugriffsdatum werden während des Scans auch für Ordner, Unterordner und untergeordnete Verzeichnisse gefüllt."),
            },
            {
                QStringLiteral("Nutzen Sie Öffnen oder Doppelklick für Dokumente, Bilder, Videos, Tabellen, ausführbare Dateien, Archive und andere vom System erkannte Dateitypen."),
                QStringLiteral("Wenn Windows mehrere Optionen meldet, wählen Sie die gewünschte Anwendung im Auswahlfenster."),
                QStringLiteral("Aktivieren Sie die MIME-Spalte, um erkannte Inhaltsfamilie und Untertyp zu prüfen."),
                QStringLiteral("Aktivieren Sie die drei Datumsspalten, um zu prüfen, wann Ordner und Dateien erstellt, geändert oder aufgerufen wurden."),
            },
            {
                {
                    QStringLiteral("Beliebige Dateitypen öffnen"),
                    {
                        QStringLiteral("1) Wählen Sie eine Zeile, die eine vorhandene Datei darstellt."),
                        QStringLiteral("2) Doppelklicken Sie darauf oder verwenden Sie die Aktion Öffnen."),
                        QStringLiteral("3) Wenn ein Auswahlfenster erscheint, wählen Sie die gewünschte Anwendung und bestätigen Sie."),
                        QStringLiteral("4) Wenn das Element ein Ordner ist, nutzen Sie die Ordner-öffnen-Aktion, um ihn im Datei-Explorer anzuzeigen."),
                    },
                    {}
                },
                {
                    QStringLiteral("MIME-Spalte verstehen"),
                    {
                        QStringLiteral("Der erste Teil zeigt die Inhaltsfamilie, etwa Text, Bild, Video, Audio oder Anwendung."),
                        QStringLiteral("Der zweite Teil zeigt den Untertyp in einfacher Sprache, etwa PDF, PNG, ZIP oder Einfacher Text."),
                        QStringLiteral("Der Text in Klammern behält die ursprüngliche Kennung bei, zum Beispiel text/plain oder application/pdf."),
                    },
                    {}
                },
                {
                    QStringLiteral("Daten von Ordnern und Unterordnern prüfen"),
                    {
                        QStringLiteral("1) Öffnen Sie das Spaltenmenü und aktivieren Sie Erstellungsdatum, Änderungsdatum und Zugriffsdatum."),
                        QStringLiteral("2) Führen Sie einen neuen Scan für das gewählte Verzeichnis aus."),
                        QStringLiteral("3) Sortieren oder filtern Sie die Tabelle, um untergeordnete Ordner und tiefere Verzeichnisse zu prüfen."),
                        QStringLiteral("4) Nutzen Sie diese Daten, um aktuelle Änderungen mit aufgezeichneten Ereignissen zu vergleichen."),
                    },
                    {}
                },
                {
                    QStringLiteral("Wenn eine Datei nicht geöffnet wird"),
                    {
                        QStringLiteral("Prüfen Sie, ob der Pfad noch existiert und ob Sie Leserechte besitzen."),
                        QStringLiteral("Prüfen Sie, ob Windows eine Anwendung mit diesem Dateityp verknüpft hat."),
                        QStringLiteral("Wählen Sie bei Bedarf manuell eine andere kompatible Anwendung im Auswahlfenster."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("betrieb-bei-hoher-last"),
            QStringLiteral("Betrieb bei hoher Last"),
            {
                QStringLiteral("Wenn viele Dateien in kurzer Folge geändert werden, in kurzen Zyklen arbeiten: erfassen, prüfen, filtern und exportieren."),
            },
            {
                QStringLiteral("Nach Datum/Ereignis sortieren, um aktuelle Änderungen zu priorisieren."),
                QStringLiteral("Vor dem Export Filter nach Typ/Verzeichnis setzen."),
                QStringLiteral("Strategische Pausen nutzen, um klare Prüfpunkte zu schaffen."),
            },
            {
                {
                    QStringLiteral("Sprachwechsel bei hoher Aktivität"),
                    {
                        QStringLiteral("Ein Sprachwechsel ist bei aktiver Überwachung möglich, für kritische Sichtprüfungen wird jedoch eine Pause empfohlen."),
                        QStringLiteral("Nach dem Wechsel Menüs, Tabellenköpfe, Hilfe und Statistiktexte validieren."),
                    },
                    {}
                },
                {
                    QStringLiteral("Schnelle Stabilitätsprüfung"),
                    {
                        QStringLiteral("Prüfen, ob nach dem Fortsetzen weiterhin neue Ereignisse erscheinen."),
                        QStringLiteral("Sicherstellen, dass Status- und Fortschrittsanzeigen korrekt zwischen Zuständen wechseln."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("komplementaere-nutzungsressourcen"),
            QStringLiteral("Komplementäre Nutzungsressourcen"),
            {
                QStringLiteral("Diese Version ergänzt den Alltagseinsatz um praktische Ressourcen: konfigurierbares Sicherheits-Popup, präzisere Ereignisklassifikation und Statistikaufruf mit Konsistenzprüfung."),
            },
            {
                QStringLiteral("Sicherheits-Popup-Kontrollen: Mindestschwelle, maximale Zeilen und Intervall zwischen Warnungen."),
                QStringLiteral("Klarere Trennung zwischen Umbenannt- und Verschoben-Ereignissen für Audits."),
                QStringLiteral("Konsistenterer Statistikablauf, wenn noch Schreibvorgänge ausstehen."),
            },
            {
                {
                    QStringLiteral("Sicherheits-Popup konfigurieren"),
                    {
                        QStringLiteral("Öffnen Sie das Sicherheitsmenü und rufen Sie die Popup-/Benachrichtigungseinstellungen auf."),
                        QStringLiteral("Legen Sie die Mindestschwelle für die Auslösung einer Warnung fest."),
                        QStringLiteral("Definieren Sie die maximale Anzahl der pro Popup angezeigten Zeilen."),
                        QStringLiteral("Stellen Sie das Intervall zwischen Warnungen ein und speichern Sie die Einstellungen."),
                    },
                    {}
                },
                {
                    QStringLiteral("Umbenannt vs Verschoben interpretieren"),
                    {
                        QStringLiteral("Umbenannt: gleiches Verzeichnis, nur der Name ändert sich."),
                        QStringLiteral("Verschoben: Verzeichniswechsel, mit oder ohne Namensänderung."),
                        QStringLiteral("Für Audits nach Ereignis filtern und Quell-/Zielpfade im Detailbereich prüfen."),
                    },
                    {}
                },
                {
                    QStringLiteral("Statistiken mit stabilen Daten öffnen"),
                    {
                        QStringLiteral("Beenden Sie schwere Vorgänge oder pausieren Sie die Überwachung vor dem Öffnen der Statistik."),
                        QStringLiteral("Nach dem Öffnen prüfen, ob keine ausstehenden Verarbeitungszustände vorhanden sind."),
                        QStringLiteral("Bei Abweichungen die Ansicht schließen und nach Stabilisierung erneut öffnen."),
                    },
                    {}
                },
                {
                    QStringLiteral("Erweiterter Grafikmodus (optional)"),
                    {
                        QStringLiteral("Bei Grafik-Kompatibilitätsproblemen LINCEU_OPENGL_MODE vor dem Start setzen."),
                        QStringLiteral("Akzeptierte Werte: auto, desktop, angle und software."),
                        QStringLiteral("software nur zur Diagnose nutzen, da die Leistung sinken kann."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("operativer-ablauf-aktualisierte-menues"),
            QStringLiteral("Operativer Ablauf der aktualisierten Menüs"),
            {
                QStringLiteral("Die Menüs Datei, Einstellungen und Optionen decken nun den vollständigen Arbeitszyklus ab: Verzeichnisse wählen, Analyse starten oder pausieren, Tabelle vorbereiten, Statistiken konfigurieren, Daten exportieren und Sicherheitsressourcen prüfen."),
            },
            {
                QStringLiteral("Nutzen Sie Datei > Verzeichnis auswählen oder Strg+Shift+O vor dem ersten Scan."),
                QStringLiteral("Nutzen Sie Datei > Start/Stopp oder Strg+R zur Steuerung der Analyse und Datei > Pause oder Strg+P für einen Prüfpunkt ohne Löschen des aktuellen Zustands."),
                QStringLiteral("Nutzen Sie Einstellungen > Filter, Spalten und Farbige Spalten vor Export oder Statistik, damit nur relevante Daten hervorgehoben werden."),
                QStringLiteral("Nutzen Sie Optionen > Handbuch, wenn Sie diese Anweisungen ohne Neustart erneut öffnen möchten."),
            },
            {
                {
                    QStringLiteral("Empfohlene Sequenz"),
                    {
                        QStringLiteral("1. Wählen Sie ein Verzeichnis und führen Sie den ersten Scan aus."),
                        QStringLiteral("2. Passen Sie Filter, Spalten und Farben an, sodass nur nützliche Daten sichtbar bleiben."),
                        QStringLiteral("3. Öffnen Sie Einstellungen > Statistiken > Diagramme zeichnen und wählen Sie die benötigten Diagramme."),
                        QStringLiteral("4. Öffnen Sie Datei > Statistiken oder drücken Sie Strg+G, um die visuelle Analyse zu erzeugen."),
                        QStringLiteral("5. Exportieren Sie erst, wenn Tabelle und Diagramme den gewünschten Umfang zeigen."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("statistik-menue-diagramme-verwenden"),
            QStringLiteral("Einstellungen > Statistiken > Diagramme zeichnen verwenden"),
            {
                QStringLiteral("Dieses Untermenü steuert, welche Diagramme im Statistikfenster erzeugt werden. Aktivierte Einträge werden in statistics_charts_config.json im persistenten Datenordner gespeichert und beim nächsten Öffnen wiederverwendet."),
            },
            {
                QStringLiteral("Aktivieren Sie nur die benötigten Diagramme, um Verarbeitungszeit zu reduzieren und das Fenster übersichtlich zu halten."),
                QStringLiteral("Deaktivieren Sie schwere oder unnötige Diagramme, wenn viele Ereignisse analysiert werden."),
                QStringLiteral("Öffnen Sie nach der Änderung Datei > Statistiken oder drücken Sie Strg+G, um das Fenster mit der gewählten Auswahl zu erzeugen."),
            },
            {
                {
                    QStringLiteral("Diagrammgruppen auswählen"),
                    {
                        QStringLiteral("Nutzen Sie Kreis, Balken, Pareto und Verteilungen, um Mengen nach Typ, Größe oder Operation zu vergleichen."),
                        QStringLiteral("Nutzen Sie Zeitachse, Operationen pro Tag, Woche/Stunde und Heatmap zur Untersuchung zeitlicher Konzentration."),
                        QStringLiteral("Nutzen Sie Sankey, Ereignis pro Verzeichnis und Typ pro Verzeichnis, um Flüsse zwischen Operation, Dateityp und überwachtem Ordner zu erkennen."),
                        QStringLiteral("Nutzen Sie Verzeichnisbaum, um aktive Ordnerebenen zu finden."),
                        QStringLiteral("Nutzen Sie Scatter, Radar und Dotplot zum Vergleich von Größen, Häufigkeit und Operationsverteilung."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("statistikfenster-verwenden"),
            QStringLiteral("Statistikfenster verwenden"),
            {
                QStringLiteral("Das Statistikfenster liest die Überwachungsdatenbank und zeigt die ausgewählten Diagramme. Öffnen Sie es erst, wenn die Tabelle Daten enthält; warten Sie bei sehr großen Scans vor dem Aktualisieren auf einen stabilen Status."),
            },
            {
                QStringLiteral("Nutzen Sie das Auswahlfeld Diagramme auswählen, um Diagramme in der aktuellen Sitzung ein- oder auszublenden."),
                QStringLiteral("Nutzen Sie Alle auswählen, um alle verfügbaren Diagramme zu aktivieren."),
                QStringLiteral("Nutzen Sie Ausgewählte speichern, um nur markierte Diagramme als PNG zu exportieren."),
                QStringLiteral("Nutzen Sie Alle speichern, um alle erzeugten Diagramme in einen Ordner mit Zeitstempel zu exportieren."),
                QStringLiteral("Nutzen Sie Aktualisieren nach neuen Ereignissen, relevanten Filteränderungen oder Änderungen im Diagrammauswahlmenü."),
                QStringLiteral("Blenden Sie das Auswahlfeld aus, wenn Sie mehr Platz zum Vergleichen von Registerkarten und visuellen Details benötigen."),
            },
            {}
        },
        {
            QStringLiteral("sicherheitsmenue-verwenden"),
            QStringLiteral("Einstellungen > Sicherheit verwenden"),
            {
                QStringLiteral("Das Sicherheitsuntermenü bündelt Audit, Echtzeitsitzungen, Quarantäne, Wiederherstellung und Evidenzpakete. Verwenden Sie es nach der Verzeichniskonfiguration und möglichst nach einem ersten Scan."),
            },
            {},
            {
                {
                    QStringLiteral("Profile und Multi-Verzeichnis"),
                    {
                        QStringLiteral("Öffnen Sie Einstellungen > Sicherheit > Profile und Multi-Verzeichnis, um das aktive Profil zu verwalten."),
                        QStringLiteral("Nutzen Sie Verzeichnis hinzufügen, um einen weiteren Ordner aufzunehmen, und Ausgewähltes entfernen, um den markierten Ordner zu entfernen."),
                        QStringLiteral("Bearbeiten Sie Profilname und klicken Sie Profil speichern, um die überwachte Ordnergruppe zu benennen."),
                        QStringLiteral("Klicken Sie Aktualisieren, um Hauptverzeichnis, Verzeichnisanzahl und aktives Profil neu zu laden."),
                    },
                    {}
                },
                {
                    QStringLiteral("Alarmzentrale und Echtzeitsitzungen"),
                    {
                        QStringLiteral("Öffnen Sie Alarmzentrale, um Zeit, Risikostufe, Zusammenfassung, ausgelöste Regeln, Pfad, Name, Sitzung und Evidenz zu prüfen."),
                        QStringLiteral("Priorisieren Sie kritische oder hohe Alarme vor gewöhnlichen Ereignissen in der Haupttabelle."),
                        QStringLiteral("Öffnen Sie Echtzeitsitzungen, um aktuelle Sitzungen, Ereignisanzahl und beteiligte Verzeichnisse zu prüfen."),
                        QStringLiteral("Nutzen Sie Aktualisieren in jedem Fenster bei aktiver Überwachung, um neue Datensätze ohne Schließen des Dialogs zu laden."),
                    },
                    {}
                },
                {
                    QStringLiteral("Quarantäne, Wiederherstellung und Evidenz"),
                    {
                        QStringLiteral("Öffnen Sie Quarantäne und Wiederherstellung, um die Register Quarantäne und Überwachte Versionen zu prüfen."),
                        QStringLiteral("Wählen Sie in Quarantäne eine Zeile und nutzen Sie Ausgewählten Ordner öffnen, um die isolierte Kopie zu finden; stellen Sie erst nach Zielprüfung wieder her."),
                        QStringLiteral("Wählen Sie in Überwachte Versionen eine Zeile und nutzen Sie Überwachte Version wiederherstellen, um eine registrierte Version zurückzuholen."),
                        QStringLiteral("Öffnen Sie Evidenzpaket, wählen Sie das Ziel, klicken Sie Jetzt exportieren und prüfen Sie den erzeugten Ordner."),
                        QStringLiteral("Nutzen Sie Sicherheitsdatenbank leeren nur nach Evidenzexport oder wenn der alte Auditverlauf nicht mehr benötigt wird."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("benachrichtigungen-exportoptionen-verwenden"),
            QStringLiteral("Benachrichtigungen und Exportoptionen verwenden"),
            {
                QStringLiteral("Sicherheitsbenachrichtigungen und Exportoptionen helfen, Informationsmenge, Alarmhäufigkeit und Format gespeicherter Daten zu steuern."),
            },
            {
                QStringLiteral("Öffnen Sie Einstellungen > Sicherheit > Benachrichtigungen, um Fuß-/Statuszusammenfassungen zu aktivieren oder Popup-Typen zu konfigurieren."),
                QStringLiteral("In Alarm-, Sitzungs- oder Quarantäne-Popups steuern Sie Dieses Popup aktivieren, Auslösen ab, Maximale Zeilen im Überblick und Cooldown zwischen Popups."),
                QStringLiteral("Öffnen Sie Einstellungen > Exportoptionen, um festzulegen, ob die Datei aktive Spalten, aktive Filter oder nur ausgewählte Zeilen berücksichtigt."),
                QStringLiteral("Nutzen Sie Exportoptionen zurücksetzen, um vor CSV, TXT, JSON, XML, DB, XLSX oder Evidenzpaketen zum Standardverhalten zurückzukehren."),
            },
            {}
        },
        {
            QStringLiteral("uebersicht"),
            QStringLiteral("Übersicht"),
            {
            QStringLiteral("Linceu Lighthouse hilft Ihnen, Dateien und Ordner an mehreren Speicherorten Ihres Computers zu prüfen, zu überwachen und zu analysieren. Es zentralisiert Elementinformationen und bietet Werkzeuge zum Filtern, Erstellen von Statistiken und Extrahieren von Metadaten."),
        },
            {},
            {}
        },
        {
            QStringLiteral("hauptbildschirm"),
            QStringLiteral("Hauptbildschirm — Elemente"),
            {},
            {
            QStringLiteral("𝐖𝐞𝐫𝐤𝐳𝐞𝐮𝐠𝐥𝐞𝐢𝐬𝐭𝐞: Schnellzugriff auf: Verzeichnis hinzufügen, Scan starten, Stopp, Aktualisieren und Einstellungen."),
            QStringLiteral("𝐕𝐞𝐫𝐳𝐞𝐢𝐜𝐡𝐧𝐢𝐬𝐰𝐚𝐡𝐥: Bereich zum Hinzufügen, Entfernen oder Wechseln überwachten Ordnern."),
            QStringLiteral("𝐄𝐥𝐞𝐦𝐞𝐧𝐭𝐭𝐚𝐛𝐞𝐥𝐥𝐞: Zentrales Listing mit Spalten (Name, Typ, Größe, Datum, Ereignis) mit Sortier-/Filterunterstützung."),
            QStringLiteral("𝐃𝐞𝐭𝐚𝐢𝐥𝐛𝐞𝐫𝐞𝐢𝐜𝐡: Zeigt Metadaten und Schnellaktionen für das ausgewählte Element an."),
            QStringLiteral("𝐌𝐞𝐥𝐝𝐮𝐧𝐠𝐞𝐧 & 𝐅𝐨𝐫𝐭𝐬𝐜𝐡𝐫𝐢𝐭𝐭: Zeigt Benachrichtigungen, Betriebsprotokolle und Fortschrittsbalken an."),
            QStringLiteral("𝐒𝐭𝐚𝐭𝐢𝐬𝐭𝐢𝐤𝐞𝐧: Bereich für Diagramme und Berichte, die aus gesammelten Daten erzeugt werden."),
        },
            {}
        },
        {
            QStringLiteral("erste-schritte"),
            QStringLiteral("Erste Schritte"),
            {},
            {
            QStringLiteral("Starten Sie die Anwendung über die Verknüpfung oder die ausführbare Datei."),
            QStringLiteral("Klicken Sie auf 𝐕𝐞𝐫𝐳𝐞𝐢𝐜𝐡𝐧𝐢𝐬 𝐡𝐢𝐧𝐳𝐮𝐟ü𝐠𝐞𝐧 und wählen Sie den zu überwachenden Ordner aus."),
            QStringLiteral("Führen Sie einen ersten Scan mit 𝐒𝐭𝐚𝐫𝐭/𝐒𝐜𝐚𝐧 durch."),
            QStringLiteral("Warten Sie; die Elemente werden mit den verfügbaren Metadaten angezeigt."),
            QStringLiteral("Aktivieren Sie 𝐊𝐨𝐧𝐭𝐢𝐧𝐮𝐢𝐞𝐫𝐥𝐢𝐜𝐡𝐞 Ü𝐛𝐞𝐫𝐰𝐚𝐜𝐡𝐮𝐧𝐠 für automatische Aktualisierungen."),
        },
            {}
        },
        {
            QStringLiteral("betriebsmodi"),
            QStringLiteral("Betriebsmodi"),
            {},
            {
            QStringLiteral("𝐄𝐢𝐧𝐦𝐚𝐥𝐢𝐠𝐞𝐫 𝐒𝐜𝐚𝐧: Sofortige Überprüfung des/der ausgewählten Ordners/Ordner."),
            QStringLiteral("𝐊𝐨𝐧𝐭𝐢𝐧𝐮𝐢𝐞𝐫𝐥𝐢𝐜𝐡𝐞 Ü𝐛𝐞𝐫𝐰𝐚𝐜𝐡𝐮𝐧𝐠: Beobachtet Änderungen in Echtzeit und aktualisiert die Tabelle automatisch."),
            QStringLiteral("𝐒𝐭𝐚𝐩𝐞𝐥𝐯𝐞𝐫𝐚𝐫𝐛𝐞𝐢𝐭𝐮𝐧𝐠: Extrahiert Metadaten oder führt Aktionen für mehrere Dateien gleichzeitig aus."),
        },
            {}
        },
        {
            QStringLiteral("tabelle-verstehen"),
            QStringLiteral("Verstehen der Elementtabelle"),
            {
            QStringLiteral("Jede Spalte zeigt eine Eigenschaft des Elements. Verwenden Sie Sortierung, Filter und Auswahl, um Elemente effizient zu finden und zu untersuchen."),
        },
            {
            QStringLiteral("𝐍𝐚𝐦𝐞: Dateiname oder Ordner; Doppelklick öffnet das Element."),
            QStringLiteral("𝐓𝐲𝐩: Erkannten Typ (Bild, Video, Dokument, Ordner)."),
            QStringLiteral("𝐆𝐫öß𝐞: In lesbaren Einheiten angezeigt (KB/MB/GB)."),
            QStringLiteral("𝐃𝐚𝐭𝐮𝐦: Letzte Änderung oder aufgezeichnetes Ereignisdatum."),
            QStringLiteral("𝐄𝐫𝐞𝐢𝐠𝐧𝐢𝐬: Angezeigte Aktion (erstellt, geändert, verschoben, gelöscht)."),
        },
            {}
        },
        {
            QStringLiteral("detailbereich"),
            QStringLiteral("Detailbereich & Metadaten"),
            {
            QStringLiteral("Wenn ein Element ausgewählt ist, zeigt der Seitenbereich relevante Metadaten und Schnellaktionen (Ordner öffnen, Pfad kopieren, Metadaten aktualisieren)."),
        },
            {
            QStringLiteral("𝐕𝐨𝐥𝐥𝐬𝐭ä𝐧𝐝𝐢𝐠𝐞𝐫 𝐏𝐟𝐚𝐝: Speicherort der Datei auf der Festplatte."),
            QStringLiteral("𝐀𝐮𝐭𝐨𝐫 / 𝐄𝐫𝐬𝐭𝐞𝐥𝐥𝐞𝐫: Falls in den Metadaten vorhanden."),
            QStringLiteral("𝐀𝐛𝐦𝐞𝐬𝐬𝐮𝐧𝐠𝐞𝐧: Für Bilder (Breite × Höhe)."),
            QStringLiteral("𝐃𝐚𝐮𝐞𝐫: Für Audio/Video."),
            QStringLiteral("𝐁𝐢𝐭𝐫𝐚𝐭𝐞: Für Medien, falls verfügbar."),
            QStringLiteral("𝐁𝐞𝐫𝐞𝐜𝐡𝐭𝐢𝐠𝐮𝐧𝐠𝐞𝐧: Zeigt Schutz- oder Schreibbeschränkungen an."),
        },
            {}
        },
        {
            QStringLiteral("datei-aktionen"),
            QStringLiteral("Aktionen für Dateien"),
            {},
            {
            QStringLiteral("Ö𝐟𝐟𝐧𝐞𝐧: Öffnen mit der systemweiten Standardanwendung (Doppelklick oder Öffnen-Button)."),
            QStringLiteral("𝐊𝐨𝐩𝐢𝐞𝐫𝐞𝐧 / 𝐕𝐞𝐫𝐬𝐜𝐡𝐢𝐞𝐛𝐞𝐧: Verwenden Sie Kopieren/Einfügen oder Drag & Drop, um Dateien zwischen Ordnern zu verschieben."),
            QStringLiteral("𝐔𝐦𝐛𝐞𝐧𝐞𝐧𝐧𝐞𝐧 / 𝐋ö𝐬𝐜𝐡𝐞𝐧: Über das Kontextmenü; permanente Aktionen erfordern eine Bestätigung."),
            QStringLiteral("𝐖𝐢𝐞𝐝𝐞𝐫𝐡𝐞𝐫𝐬𝐭𝐞𝐥𝐥𝐞𝐧: Falls verfügbar, zuletzt gelöschte Elemente wiederherstellen."),
            QStringLiteral("𝐄𝐢𝐠𝐞𝐧𝐬𝐜𝐡𝐚𝐟𝐭𝐞𝐧: Vollständige Metadaten und Ereignishistorie der Datei anzeigen."),
        },
            {}
        },
        {
            QStringLiteral("suche-filter"),
            QStringLiteral("Suche und Filter"),
            {},
            {
            QStringLiteral("𝐒𝐮𝐜𝐡𝐞: Verwenden Sie das Suchfeld, um Elemente nach Name oder Begriff zu finden."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐞𝐫: Wenden Sie Filter nach Typ, Größe, Datum oder Ereignis an, um Ergebnisse einzugrenzen."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐞𝐫 𝐤𝐨𝐦𝐛𝐢𝐧𝐢𝐞𝐫𝐞𝐧: Verwenden Sie mehrere Filter zusammen für präzise Ergebnisse."),
        },
            {}
        },
        {
            QStringLiteral("speichern-wiederherstellen"),
            QStringLiteral("Speichern und Wiederherstellen der Ansicht"),
            {},
            {
            QStringLiteral("𝐒𝐭𝐚𝐭𝐮𝐬 𝐬𝐩𝐞𝐢𝐜𝐡𝐞𝐫𝐧: Speichern Sie die aktuelle Spaltenanordnung und Filter als Profil."),
            QStringLiteral("𝐖𝐢𝐞𝐝𝐞𝐫𝐡𝐞𝐫𝐬𝐭𝐞𝐥𝐥𝐞𝐧: Laden Sie ein gespeichertes Profil, um Layout und Filter wiederherzustellen."),
        },
            {}
        },
        {
            QStringLiteral("stapelverarbeitung"),
            QStringLiteral("Stapelverarbeitung"),
            {
            QStringLiteral("Wählen Sie mehrere Elemente aus, um Metadaten zu extrahieren oder Batch-Aktionen auszuführen. Fortschritt und Steuerungen sind verfügbar, um anzuhalten oder abzubrechen."),
        },
            {
            QStringLiteral("𝐄𝐱𝐭𝐫𝐚𝐤𝐭𝐢𝐨𝐧 𝐬𝐭𝐚𝐫𝐭𝐞𝐧: Wählen Sie Elemente und führen Sie die Batch-Metadatenextraktion aus."),
            QStringLiteral("𝐅𝐨𝐫𝐭𝐬𝐜𝐡𝐫𝐢𝐭𝐭 ü𝐛𝐞𝐫𝐰𝐚𝐜𝐡𝐞𝐧: Verfolgen Sie über Fortschrittsbalken und Meldungsbereich."),
            QStringLiteral("𝐏𝐚𝐮𝐬𝐞/𝐀𝐛𝐛𝐫𝐞𝐜𝐡𝐞𝐧: Verwenden Sie die Steuerelemente, um die Verarbeitung zu unterbrechen."),
        },
            {}
        },
        {
            QStringLiteral("berichte-statistiken"),
            QStringLiteral("Berichte & Statistiken"),
            {
            QStringLiteral("Der Statistik-Bereich erstellt Diagramme zu Dateitypen, Festplattennutzung und Ereignistrends. Exportieren Sie Bilder oder CSV für die externe Analyse."),
        },
            {
            QStringLiteral("𝐃𝐢𝐚𝐠𝐫𝐚𝐦𝐦𝐞 𝐞𝐫𝐬𝐭𝐞𝐥𝐥𝐞𝐧: Wählen Sie Metriken, um Verteilungen und Trends zu visualisieren."),
            QStringLiteral("𝐄𝐱𝐩𝐨𝐫𝐭𝐢𝐞𝐫𝐞𝐧: Speichern Sie Diagramme als Bilder oder exportieren Sie Daten als CSV."),
        },
            {}
        },
        {
            QStringLiteral("ereignis-ueberwachung"),
            QStringLiteral("Ereignisüberwachung"),
            {},
            {
            QStringLiteral("𝐄𝐫𝐞𝐢𝐠𝐧𝐢𝐬𝐭𝐲𝐩𝐞𝐧: Erstellung, Änderung, Entfernung und Verschiebung."),
            QStringLiteral("𝐄𝐜𝐡𝐭𝐳𝐞𝐢𝐭𝐚𝐧𝐬𝐢𝐜𝐡𝐭: Kürzliche Ereignisse werden zur schnellen Überprüfung hervorgehoben."),
            QStringLiteral("𝐁𝐞𝐧𝐚𝐜𝐡𝐫𝐢𝐜𝐡𝐭𝐢𝐠𝐮𝐧𝐠𝐞𝐧: Konfigurieren Sie visuelle oder akustische Alarme in den Einstellungen."),
        },
            {}
        },
        {
            QStringLiteral("einstellungen-de"),
            QStringLiteral("Einstellungen"),
            {
            QStringLiteral("Passen Sie Sprache, Scan-Verhalten, Benachrichtigungen und Updates an. Überprüfen Sie diese Optionen nach der Installation."),
        },
            {
            QStringLiteral("𝐒𝐩𝐫𝐚𝐜𝐡𝐞: Wählen Sie die UI-Sprache."),
            QStringLiteral("𝐒𝐜𝐚𝐧-𝐕𝐞𝐫𝐡𝐚𝐥𝐭𝐞𝐧: Konfigurieren Sie Zeitplan, Tiefe und Ausnahmen."),
            QStringLiteral("𝐁𝐞𝐧𝐚𝐜𝐡𝐫𝐢𝐜𝐡𝐭𝐢𝐠𝐮𝐧𝐠𝐞𝐧: Aktivieren oder deaktivieren Sie Warnungen."),
            QStringLiteral("𝐔𝐩𝐝𝐚𝐭𝐞𝐬: Wählen Sie automatische oder manuelle Prüfungen."),
        },
            {}
        },
        {
            QStringLiteral("haeufige-probleme-de"),
            QStringLiteral("Häufige Probleme und Lösungen"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧 𝐡ä𝐧𝐠𝐭 𝐨𝐝𝐞𝐫 𝐅𝐞𝐡𝐥𝐞𝐫: Warten Sie kurz; wenn es weiterhin besteht, starten Sie den Scan oder die Anwendung neu."),
            QStringLiteral("𝐄𝐥𝐞𝐦𝐞𝐧𝐭𝐞 𝐟𝐞𝐡𝐥𝐞𝐧: Überprüfen Sie, ob der Ordner hinzugefügt wurde und die App Leserechte hat."),
            QStringLiteral("𝐊𝐚𝐧𝐧 𝐧𝐢𝐜𝐡𝐭 ö𝐟𝐟𝐧𝐞𝐧: Stellen Sie sicher, dass eine zugeordnete Anwendung für den Dateityp vorhanden ist."),
            QStringLiteral("𝐌𝐞𝐭𝐚𝐝𝐚𝐭𝐞𝐧 𝐟𝐞𝐡𝐥𝐞𝐧: Nicht alle Dateien enthalten Metadaten; versuchen Sie, zu aktualisieren oder per Batch zu verarbeiten."),
        },
            {}
        },
        {
            QStringLiteral("kuerzel-de"),
            QStringLiteral("Tastenkürzel"),
            {},
            shortcut_lines(QStringLiteral("de_DE")),
            {}
        },
        {
            QStringLiteral("best-practices-de"),
            QStringLiteral("Gute Praxis"),
            {},
            {
            QStringLiteral("Führen Sie einen ersten Scan in einem kleinen Ordner durch, bevor Sie große Vorgänge starten."),
            QStringLiteral("Sichern Sie Zustände und Einstellungen vor Batch-Vorgängen."),
            QStringLiteral("Verwenden Sie Filter, um sich auf relevante Elemente zu konzentrieren."),
        },
            {}
        },
        {
            QStringLiteral("logs-diagnostik-de"),
            QStringLiteral("Logs und Diagnose"),
            {},
            {
            QStringLiteral("Die Anwendungsprotokolle können bei der Diagnose helfen; prüfen Sie die von der Anwendung erzeugte Logdatei hier: ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq-de"),
            QStringLiteral("Häufig gestellte Fragen (FAQ)"),
            {},
            {},
            {
        {
            QStringLiteral("Wo werden meine Daten gespeichert?"),
            {
            QStringLiteral("Aufgaben und Konfigurationsdateien werden gespeichert in: ") + dataDir,
            QStringLiteral("Prüfen Sie dieses Verzeichnis, um Persistenzdateien und Logs zu finden."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("support-de"),
            QStringLiteral("Wie erhalte ich Hilfe und Support"),
            {},
            {
            QStringLiteral("Prüfen Sie den Abschnitt 'Über' in der Anwendung für offizielle Informationen und Versionshinweise."),
            QStringLiteral("Für komplexe Probleme erzeugen Sie Logs und senden Sie diese an den Support. Logdateien befinden sich unter: ") + dataDir,
            QStringLiteral("Senden Sie die Logdateien und eine detaillierte Problembeschreibung an die Support-E-Mail: linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
