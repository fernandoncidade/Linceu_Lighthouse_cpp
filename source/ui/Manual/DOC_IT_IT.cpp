#include "DOC_IT_IT.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_it_it() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("nuove-funzionalita"),
            QStringLiteral("Nuove funzionalità"),
            {
                QStringLiteral("La versione 2026.9.28.0 introduce miglioramenti strutturali di affidabilità nel monitoraggio in tempo reale, caricamento asincrono non bloccante per le sessioni in tempo reale, sincronizzazione differenziale delle tabelle senza sfarfallio in quarantena e ripristino sicuro con convalida crittografica SHA-256."),
                QStringLiteral("Le seguenti istruzioni guidano all'uso sicuro, prevedibile ed efficiente di queste funzionalità nelle attività quotidiane."),
            },
            {
                QStringLiteral("Sessioni in tempo reale non bloccanti: caricamento in background con indicatori di avanzamento e annullamento sicuro."),
                QStringLiteral("Quarantena e ripristino intelligente: aggiornamento differenziale delle tabelle che preserva selezione, ordinamento e scorrimento."),
                QStringLiteral("Ripristino sicuro con selezione della cartella di destinazione e verifica crittografica dell'integrità SHA-256."),
                QStringLiteral("Centro avvisi con classificazione per livelli di rischio oggettivi e valutazione deterministica delle regole di sicurezza."),
                QStringLiteral("Gestione multi-directory ottimizzata con profili persistenti per il monitoraggio simultaneo."),
            },
            {
                {
                    QStringLiteral("Flusso consigliato per monitoraggio continuo"),
                    {
                        QStringLiteral("1) Aggiungi le directory target ed esegui una scansione iniziale completa."),
                        QStringLiteral("2) Verifica popolamento della tabella e corretto stato dell'interfaccia."),
                        QStringLiteral("3) Attiva il monitoraggio continuo per acquisire nuovi eventi in tempo reale."),
                        QStringLiteral("4) Usa filtri per ridurre rumore e velocizzare le verifiche."),
                    },
                    {}
                },
                {
                    QStringLiteral("Uso sicuro di pausa, ripresa e arresto"),
                    {
                        QStringLiteral("Durante operazioni lunghe, metti in pausa per una revisione intermedia."),
                        QStringLiteral("Riprendi solo dopo aver validato il dataset visibile."),
                        QStringLiteral("Prima della chiusura, arresta monitoraggio/scansione in modo esplicito e attendi l'ultimo aggiornamento di stato."),
                    },
                    {}
                },
                {
                    QStringLiteral("Come utilizzare le Sessioni in Tempo Reale Asincrone"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Sessioni in tempo reale."),
                        QStringLiteral("Lo storico viene caricato in background tramite un thread di lavoro dedicato, mantenendo l'interfaccia reattiva anche con migliaia di eventi."),
                        QStringLiteral("Esamina le colonne: Aggiornato il, Etichetta di sessione, Totale eventi, Directory monitorate e Identificativo di evidenza."),
                        QStringLiteral("Passa il puntatore sulle directory per visualizzare l'elenco completo dei percorsi in caso di percorsi multipli."),
                        QStringLiteral("Fai clic su Aggiorna in qualsiasi momento per richiedere nuovi blocchi di eventi senza chiudere la finestra di dialogo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Come utilizzare Quarantena e Ripristino Intelligente"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Quarantena e ripristino."),
                        QStringLiteral("Naviga tra le schede Quarantena (file isolati per sicurezza) e Versioni monitorate (istantanee della baseline crittografica)."),
                        QStringLiteral("L'aggiornamento della tabella è differenziale: i nuovi record non alterano la selezione attiva né la posizione di scorrimento."),
                        QStringLiteral("Seleziona un file e usa Apri cartella selezionata per visualizzare la copia isolata in Esplora file di Windows."),
                        QStringLiteral("Fai clic su Ripristina copia selezionata (o Ripristina versione monitorata) per scegliere una cartella sicura di destinazione e recuperare il file con verifica di integrità."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("aprire-file-mime-date-cartelle"),
            QStringLiteral("Apertura file, MIME e date delle cartelle"),
            {
                QStringLiteral("La tabella ora aiuta ad aprire gli elementi segnalati anche quando non sono file di testo. Con Apri o doppio clic su una riga di file, l'applicazione prova a usare l'app predefinita di Windows e può proporre altre opzioni compatibili."),
                QStringLiteral("La colonna MIME mostra il tipo reale di contenuto in forma leggibile nella lingua corrente, mantenendo l'identificatore originale tra parentesi per la verifica. Le colonne Data di creazione, Data di modifica e Data di accesso vengono compilate anche per cartelle, sottocartelle e directory figlie durante la scansione."),
            },
            {
                QStringLiteral("Usa Apri o doppio clic per aprire documenti, immagini, video, fogli di calcolo, eseguibili, archivi compressi e altri tipi riconosciuti dal sistema."),
                QStringLiteral("Quando Windows indica più opzioni, scegli l'applicazione desiderata nella finestra di selezione."),
                QStringLiteral("Attiva la colonna MIME per controllare famiglia e sottotipo del contenuto rilevato."),
                QStringLiteral("Attiva le tre colonne data per verificare quando cartelle e file sono stati creati, modificati o consultati."),
            },
            {
                {
                    QStringLiteral("Aprire qualsiasi tipo di file"),
                    {
                        QStringLiteral("1) Seleziona una riga che rappresenta un file esistente."),
                        QStringLiteral("2) Fai doppio clic oppure usa l'azione Apri."),
                        QStringLiteral("3) Se appare una finestra di scelta, seleziona l'applicazione desiderata e conferma."),
                        QStringLiteral("4) Se l'elemento è una cartella, usa l'azione di apertura cartella per raggiungerla in Esplora file."),
                    },
                    {}
                },
                {
                    QStringLiteral("Interpretare la colonna MIME"),
                    {
                        QStringLiteral("La prima parte mostra la famiglia del contenuto, come Testo, Immagine, Video, Audio o Applicazione."),
                        QStringLiteral("La seconda parte mostra il sottotipo in linguaggio semplice, come PDF, PNG, ZIP o Testo normale."),
                        QStringLiteral("Il testo tra parentesi conserva l'identificatore originale, per esempio text/plain o application/pdf."),
                    },
                    {}
                },
                {
                    QStringLiteral("Controllare le date di cartelle e sottocartelle"),
                    {
                        QStringLiteral("1) Apri il menu delle colonne e abilita Data di creazione, Data di modifica e Data di accesso."),
                        QStringLiteral("2) Esegui una nuova scansione sulla directory scelta."),
                        QStringLiteral("3) Ordina o filtra la tabella per rivedere cartelle figlie e directory più profonde."),
                        QStringLiteral("4) Usa queste date per confrontare modifiche recenti ed eventi registrati."),
                    },
                    {}
                },
                {
                    QStringLiteral("Se un file non si apre"),
                    {
                        QStringLiteral("Verifica che il percorso esista ancora e che tu disponga del permesso di lettura."),
                        QStringLiteral("Controlla che in Windows esista un'applicazione associata a quel tipo di file."),
                        QStringLiteral("Quando serve, scegli manualmente un'altra applicazione compatibile nella finestra di selezione."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("operazione-alto-volume"),
            QStringLiteral("Operatività ad alto volume"),
            {
                QStringLiteral("Quando molti file cambiano in sequenza, lavora per cicli brevi: acquisizione, revisione, filtro ed esportazione."),
            },
            {
                QStringLiteral("Ordina per data/evento per dare priorità alle modifiche recenti."),
                QStringLiteral("Applica filtri per tipo/directory prima di esportare i dati."),
                QStringLiteral("Usa pause strategiche per creare punti di controllo chiari."),
            },
            {
                {
                    QStringLiteral("Cambio lingua durante attività intensa"),
                    {
                        QStringLiteral("Il cambio lingua è possibile con monitoraggio attivo, ma è consigliata una pausa durante validazioni visive critiche."),
                        QStringLiteral("Dopo il cambio, verifica testi di menu, intestazioni tabella, aiuto e statistiche."),
                    },
                    {}
                },
                {
                    QStringLiteral("Validazione rapida della stabilità"),
                    {
                        QStringLiteral("Controlla che nuovi eventi continuino ad apparire dopo la ripresa del monitoraggio."),
                        QStringLiteral("Conferma che etichette di stato e progresso cambino correttamente tra gli stati operativi."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("risorse-complementari-uso"),
            QStringLiteral("Risorse d'uso complementari"),
            {
                QStringLiteral("Questa versione aggiunge anche risorse pratiche per l'uso quotidiano: popup di sicurezza configurabile, classificazione eventi più precisa e apertura statistiche con controllo di coerenza."),
            },
            {
                QStringLiteral("Controlli popup sicurezza: soglia minima, numero massimo di righe e intervallo tra avvisi."),
                QStringLiteral("Separazione più chiara tra eventi Rinominato e Spostato per audit."),
                QStringLiteral("Flusso statistiche più coerente quando sono presenti scritture ancora in coda."),
            },
            {
                {
                    QStringLiteral("Configurare il popup di sicurezza"),
                    {
                        QStringLiteral("Apri il menu Sicurezza e accedi alla configurazione popup/notifiche."),
                        QStringLiteral("Imposta la soglia minima per l'attivazione dell'avviso."),
                        QStringLiteral("Definisci il numero massimo di righe visualizzate per popup."),
                        QStringLiteral("Configura l'intervallo tra avvisi e salva le preferenze."),
                    },
                    {}
                },
                {
                    QStringLiteral("Interpretare Rinominato vs Spostato"),
                    {
                        QStringLiteral("Rinominato: stessa directory, cambia solo il nome dell'elemento."),
                        QStringLiteral("Spostato: cambia directory, con o senza cambio nome."),
                        QStringLiteral("Per audit, filtra per Evento e verifica percorsi origine/destinazione nel pannello dettagli."),
                    },
                    {}
                },
                {
                    QStringLiteral("Aprire statistiche con dati stabili"),
                    {
                        QStringLiteral("Completa operazioni pesanti o metti in pausa il monitoraggio prima di aprire le statistiche."),
                        QStringLiteral("Dopo l'apertura, verifica che non ci siano stati di elaborazione pendenti."),
                        QStringLiteral("Se i valori sembrano incoerenti, chiudi e riapri dopo la stabilizzazione della coda."),
                    },
                    {}
                },
                {
                    QStringLiteral("Modalità grafica avanzata (opzionale)"),
                    {
                        QStringLiteral("In caso di problemi di compatibilità grafica, imposta LINCEU_OPENGL_MODE prima dell'avvio."),
                        QStringLiteral("Valori accettati: auto, desktop, angle e software."),
                        QStringLiteral("Usa software solo per diagnosi, poiché le prestazioni possono diminuire."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("flusso-operativo-menu-aggiornati"),
            QStringLiteral("Flusso operativo dei menu aggiornati"),
            {
                QStringLiteral("I menu File, Impostazioni e Opzioni ora coprono l'intero ciclo di lavoro: scegliere directory, avviare o sospendere analisi, preparare tabella, configurare statistiche, esportare dati e consultare la sicurezza."),
            },
            {
                QStringLiteral("Usa File > Seleziona directory o Ctrl+Shift+O prima della scansione iniziale."),
                QStringLiteral("Usa File > Avvia/Interrompi o Ctrl+R per controllare l'analisi, e File > Pausa o Ctrl+P per creare un punto di revisione senza cancellare lo stato corrente."),
                QStringLiteral("Usa Impostazioni > Filtri, Colonne e Colonne colorate prima di esportare o aprire statistiche, così la tabella evidenzia solo i dati rilevanti."),
                QStringLiteral("Usa Opzioni > Manuale quando devi riaprire queste istruzioni senza riavviare l'applicazione."),
            },
            {
                {
                    QStringLiteral("Sequenza consigliata"),
                    {
                        QStringLiteral("1. Seleziona una directory ed esegui la scansione iniziale."),
                        QStringLiteral("2. Regola filtri, colonne e colori per lasciare visibili solo i dati utili."),
                        QStringLiteral("3. Apri Impostazioni > Statistiche > Traccia grafici e scegli i grafici necessari all'audit."),
                        QStringLiteral("4. Apri File > Statistiche o premi Ctrl+G per generare l'analisi visiva."),
                        QStringLiteral("5. Esporta risultati solo dopo aver confermato che tabella e grafici mostrino l'ambito desiderato."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-menu-statistiche-grafici"),
            QStringLiteral("Usare Impostazioni > Statistiche > Traccia grafici"),
            {
                QStringLiteral("Questo sottomenu controlla quali grafici vengono generati nella finestra Statistiche. Gli elementi selezionati sono salvati in statistics_charts_config.json nella cartella dati persistente dell'applicazione e riutilizzati alle aperture successive."),
            },
            {
                QStringLiteral("Seleziona solo i grafici da generare per ridurre il tempo di elaborazione e mantenere la finestra ordinata."),
                QStringLiteral("Deseleziona grafici pesanti o non necessari quando analizzi molti eventi."),
                QStringLiteral("Dopo la modifica, apri File > Statistiche o premi Ctrl+G per generare la finestra con il set scelto."),
            },
            {
                {
                    QStringLiteral("Scegliere gruppi di grafici"),
                    {
                        QStringLiteral("Usa Torta, Barre, Pareto e Distribuzioni per confrontare volumi per tipo, dimensione o operazione."),
                        QStringLiteral("Usa Timeline, Operazioni per giorno, Settimana/Ora e Heatmap per studiare la concentrazione temporale."),
                        QStringLiteral("Usa Sankey, Evento per directory e Tipo per directory per scoprire flussi tra operazione, tipo file e cartella monitorata."),
                        QStringLiteral("Usa Albero directory per individuare i livelli di cartella con più attività."),
                        QStringLiteral("Usa Scatter, Radar e Dotplot per confrontare dimensioni, frequenza e distribuzione delle operazioni."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-finestra-statistiche"),
            QStringLiteral("Usare la finestra Statistiche"),
            {
                QStringLiteral("La finestra Statistiche legge il database di monitoraggio e mostra i grafici selezionati. Aprila solo quando la tabella contiene dati; durante scansioni pesanti, attendi la stabilizzazione dello stato prima di aggiornare."),
            },
            {
                QStringLiteral("Usa il pannello Seleziona grafici per mostrare o nascondere grafici nella sessione corrente."),
                QStringLiteral("Usa Seleziona tutti per attivare tutti i grafici disponibili."),
                QStringLiteral("Usa Salva selezionati per esportare solo i grafici marcati come PNG."),
                QStringLiteral("Usa Salva tutti per esportare tutti i grafici generati in una cartella con data e ora."),
                QStringLiteral("Usa Aggiorna dopo nuovi eventi, cambi di filtro rilevanti o modifiche nel menu di selezione grafici."),
                QStringLiteral("Nascondi il pannello di selezione quando ti serve più spazio per confrontare schede e dettagli visivi."),
            },
            {}
        },
        {
            QStringLiteral("uso-menu-sicurezza"),
            QStringLiteral("Usare Impostazioni > Sicurezza"),
            {
                QStringLiteral("Il sottomenu Sicurezza riunisce risorse di audit, monitoraggio delle sessioni in tempo reale, quarantena, ripristino, baseline crittografiche e pacchetti di evidenza. Usalo dopo aver configurato le directory e preferibilmente dopo una scansione iniziale."),
            },
            {},
            {
                {
                    QStringLiteral("Profili e multi-directory"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Profili e multi-directory per gestire il profilo attivo."),
                        QStringLiteral("Usa Aggiungi directory per includere un'altra cartella nel profilo e Rimuovi selezionato per togliere quella evidenziata."),
                        QStringLiteral("Modifica Nome profilo e fai clic su Salva profilo per identificare l'insieme monitorato."),
                        QStringLiteral("Fai clic su Aggiorna per ricaricare directory principale, numero di directory e profilo attivo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Centro avvisi in tempo reale"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Centro avvisi per esaminare le occorrenze classificate per livelli di rischio: Basso, Medio, Alto e Critico."),
                        QStringLiteral("Consulta il riepilogo dell'avviso, le regole attivate, il percorso, il nome del file, la sessione associata e l'identificativo di evidenza."),
                        QStringLiteral("Dai priorità all'analisi degli avvisi Critici o Alti prima di esaminare gli eventi ordinari nella tabella principale."),
                        QStringLiteral("Fai clic su Aggiorna per richiedere nuovi eventi rilevati senza interrompere il monitoraggio attivo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Sessioni in tempo reale asincrone"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Sessioni in tempo reale per monitorare i blocchi di attività del file system."),
                        QStringLiteral("Il caricamento avviene in background tramite un thread dedicato, evitando blocchi visivi anche con database di grandi dimensioni."),
                        QStringLiteral("Controlla il timestamp, l'etichetta leggibile della sessione, il totale degli eventi registrati, il riepilogo delle directory e l'identificativo univoco."),
                        QStringLiteral("Passa il puntatore sulle directory per visualizzare l'elenco completo dei percorsi quando la sessione include più cartelle."),
                        QStringLiteral("Usa Aggiorna per caricare nuove sessioni incrementali mentre il monitoraggio è attivo."),
                    },
                    {}
                },
                {
                    QStringLiteral("Quarantena e ripristino intelligente"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Quarantena e ripristino per accedere alle schede Quarantena e Versioni monitorate."),
                        QStringLiteral("Le tabelle utilizzano la sincronizzazione differenziale senza sfarfallio: la selezione attiva, l'ordinamento e la posizione di scorrimento rimangono stabili."),
                        QStringLiteral("Nella scheda Quarantena, seleziona un file isolato e usa Apri cartella selezionata per verificarne la posizione in Esplora file."),
                        QStringLiteral("Fai clic su Ripristina copia selezionata per aprire la finestra di scelta della destinazione: ripristina nel percorso originale o scegli una cartella sicura per l'analisi."),
                        QStringLiteral("Nella scheda Versioni monitorate, seleziona un'istantanea della baseline crittografica e usa Ripristina versione monitorata per recuperarla."),
                        QStringLiteral("Il sistema valida l'integrità crittografica SHA-256 e mostra una notifica immediata di successo dopo il ripristino."),
                    },
                    {}
                },
                {
                    QStringLiteral("Pacchetto di evidenza e audit"),
                    {
                        QStringLiteral("Apri Impostazioni > Sicurezza > Pacchetto di evidenza per generare un insieme di report controllabili."),
                        QStringLiteral("Scegli la cartella di destinazione, fai clic su Esporta ora ed esamina il pacchetto generato con baseline crittografiche e log strutturati."),
                        QStringLiteral("Usa Cancella database sicurezza solo dopo aver esportato le evidenze e confermato che lo storico precedente non è più necessario."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("uso-notifiche-esportazione"),
            QStringLiteral("Usare notifiche e opzioni di esportazione"),
            {
                QStringLiteral("Notifiche di sicurezza e opzioni di esportazione aiutano a controllare volume informativo, frequenza degli avvisi e formato dei dati salvati."),
            },
            {
                QStringLiteral("Apri Impostazioni > Sicurezza > Notifiche per attivare riepiloghi nel piè/stato o configurare tipi di popup."),
                QStringLiteral("Nei popup di avvisi, sessioni o quarantena, regola Attiva questo popup, Attiva al raggiungimento, Massimo righe nel riepilogo e Cooldown tra popup."),
                QStringLiteral("Apri Impostazioni > Opzioni di esportazione per decidere se il file rispetta colonne attive, filtri attivi o solo righe selezionate."),
                QStringLiteral("Usa Reimposta opzioni di esportazione per tornare al comportamento standard prima di generare CSV, TXT, JSON, XML, DB, XLSX o pacchetti di evidenza."),
            },
            {}
        },
        {
            QStringLiteral("panoramica"),
            QStringLiteral("Panoramica"),
            {
            QStringLiteral("Linceu Lighthouse ti aiuta a ispezionare, monitorare e analizzare file e cartelle in più posizioni del tuo computer. Centralizza le informazioni degli elementi e fornisce strumenti per filtrare, generare statistiche ed estrarre metadati."),
        },
            {},
            {}
        },
        {
            QStringLiteral("schermata-principale"),
            QStringLiteral("Schermata Principale — Elementi"),
            {},
            {
            QStringLiteral("𝐁𝐚𝐫𝐫𝐚 𝐬𝐭𝐫𝐮𝐦𝐞𝐧𝐭𝐢: Accesso rapido a: Aggiungi Cartella, Avvia Scansione, Interrompi, Aggiorna e Preferenze."),
            QStringLiteral("𝐒𝐞𝐥𝐞𝐭𝐭𝐨𝐫𝐞 𝐜𝐚𝐫𝐭𝐞𝐥𝐥𝐞: Area per aggiungere, rimuovere o cambiare le cartelle monitorate."),
            QStringLiteral("𝐓𝐚𝐛𝐞𝐥𝐥𝐚 𝐞𝐥𝐞𝐦𝐞𝐧𝐭𝐢: Elenco centrale con colonne (Nome, Tipo, Dimensione, Data, Evento) con supporto a ordinamento/filtri."),
            QStringLiteral("𝐏𝐚𝐧𝐧𝐞𝐥𝐥𝐨 𝐝𝐞𝐭𝐭𝐚𝐠𝐥𝐢: Mostra metadati e azioni rapide per l'elemento selezionato."),
            QStringLiteral("𝐌𝐞𝐬𝐬𝐚𝐠𝐠𝐢 & 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐨: Mostra notifiche, log e barre di progresso."),
            QStringLiteral("𝐒𝐭𝐚𝐭𝐢𝐬𝐭𝐢𝐜𝐡𝐞: Area per grafici e report generati dai dati raccolti."),
        },
            {}
        },
        {
            QStringLiteral("iniziare"),
            QStringLiteral("Come Iniziare"),
            {},
            {
            QStringLiteral("Apri l'applicazione tramite collegamento o eseguibile."),
            QStringLiteral("Clicca 𝐀𝐠𝐠𝐢𝐮𝐧𝐠𝐢 𝐂𝐚𝐫𝐭𝐞𝐥𝐥𝐚 e scegli la cartella da monitorare."),
            QStringLiteral("Esegui una scansione iniziale con 𝐀𝐯𝐯𝐢𝐚/𝐒𝐜𝐚𝐧𝐬𝐢𝐨𝐧𝐞."),
            QStringLiteral("Attendi; gli elementi appariranno con i metadati disponibili."),
            QStringLiteral("Abilita 𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚𝐠𝐠𝐢𝐨 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐨 per rilevare automaticamente le modifiche."),
        },
            {}
        },
        {
            QStringLiteral("modalita-operazione"),
            QStringLiteral("Modalità di Funzionamento"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧𝐬𝐢𝐨𝐧𝐞 𝐬𝐢𝐧𝐠𝐨𝐥𝐚: Controllo immediato delle cartelle selezionate."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚𝐠𝐠𝐢𝐨 𝐜𝐨𝐧𝐭𝐢𝐧𝐮𝐨: Osservazione in tempo reale delle modifiche e aggiornamento automatico della tabella."),
            QStringLiteral("𝐄𝐥𝐚𝐛𝐨𝐫𝐚𝐳𝐢𝐨𝐧𝐞 𝐛𝐚𝐭𝐜𝐡: Estrazione dei metadati o esecuzione di azioni su più file contemporaneamente."),
        },
            {}
        },
        {
            QStringLiteral("capire-tabella"),
            QStringLiteral("Comprendere la Tabella degli Elementi"),
            {
            QStringLiteral("Ogni colonna mostra una proprietà dell'elemento. Usa ordinamento, filtri e selezione per trovare e ispezionare gli elementi rapidamente."),
        },
            {
            QStringLiteral("𝐍𝐨𝐦𝐞: Nome file o cartella; doppio clic per aprire."),
            QStringLiteral("𝐓𝐢𝐩𝐨: Tipo rilevato (immagine, video, documento, cartella)."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬𝐢𝐨𝐧𝐞: Visualizzata in unità leggibili (KB/MB/GB)."),
            QStringLiteral("𝐃𝐚𝐭𝐚: Ultima modifica o data evento registrata."),
            QStringLiteral("𝐄𝐯𝐞𝐧𝐭𝐨: Azione rilevata (creato, modificato, spostato, eliminato)."),
        },
            {}
        },
        {
            QStringLiteral("pannello-dettagli"),
            QStringLiteral("Pannello Dettagli e Metadati"),
            {
            QStringLiteral("La selezione di un elemento mostra i metadati rilevanti e azioni rapide (apri cartella, copia percorso, aggiorna metadati)."),
        },
            {
            QStringLiteral("𝐏𝐞𝐫𝐜𝐨𝐫𝐬𝐨 𝐜𝐨𝐦𝐩𝐥𝐞𝐭𝐨: Posizione del file sul disco."),
            QStringLiteral("𝐀𝐮𝐭𝐨𝐫𝐞 / 𝐂𝐫𝐞𝐚𝐭𝐨𝐫𝐞: Se presente nei metadati."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬𝐢𝐨𝐧𝐢: Per le immagini (larghezza × altezza)."),
            QStringLiteral("𝐃𝐮𝐫𝐚𝐭𝐚: Per audio/video."),
            QStringLiteral("𝐁𝐢𝐭𝐫𝐚𝐭𝐞: Per i media, se disponibile."),
            QStringLiteral("𝐏𝐞𝐫𝐦𝐞𝐬𝐬𝐢: Indica protezioni o restrizioni di scrittura."),
        },
            {}
        },
        {
            QStringLiteral("azioni-file"),
            QStringLiteral("Azioni sui File"),
            {},
            {
            QStringLiteral("𝐀𝐩𝐫𝐢: Apri con l'app predefinita di sistema (doppio clic o pulsante Apri)."),
            QStringLiteral("𝐂𝐨𝐩𝐢𝐚 / 𝐒𝐩𝐨𝐬𝐭𝐚: Usa copia/incolla o trascina e rilascia per spostare file tra cartelle."),
            QStringLiteral("𝐑𝐢𝐧𝐨𝐦𝐢𝐧𝐚 / 𝐄𝐥𝐢𝐦𝐢𝐧𝐚: Usa il menu contestuale; le azioni permanenti richiedono conferma."),
            QStringLiteral("𝐑𝐢𝐩𝐫𝐢𝐬𝐭𝐢𝐧𝐚: Quando disponibile, recupera eliminazioni recenti con la funzione di ripristino."),
            QStringLiteral("𝐏𝐫𝐨𝐩𝐫𝐢𝐞𝐭à: Visualizza metadati completi e cronologia eventi del file."),
        },
            {}
        },
        {
            QStringLiteral("ricerca-filtri"),
            QStringLiteral("Ricerca e Filtri"),
            {},
            {
            QStringLiteral("𝐑𝐢𝐜𝐞𝐫𝐜𝐚: Inserisci termini nella casella di ricerca per trovare elementi per nome o contenuto."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐫𝐢: Applica filtri per tipo, dimensione, data o evento per restringere i risultati."),
            QStringLiteral("𝐂𝐨𝐦𝐛𝐢𝐧𝐚 𝐟𝐢𝐥𝐭𝐫𝐢: Usa più filtri insieme per risultati precisi."),
        },
            {}
        },
        {
            QStringLiteral("salva-ripristina"),
            QStringLiteral("Salvataggio e Ripristino Vista"),
            {},
            {
            QStringLiteral("𝐒𝐚𝐥𝐯𝐚 𝐬𝐭𝐚𝐭𝐨: Salva layout colonne e filtri correnti come profilo."),
            QStringLiteral("𝐑𝐢𝐩𝐫𝐢𝐬𝐭𝐢𝐧𝐚: Carica un profilo salvato per recuperare layout e filtri."),
        },
            {}
        },
        {
            QStringLiteral("elaborazione-batch"),
            QStringLiteral("Elaborazione Batch"),
            {
            QStringLiteral("Seleziona più elementi per estrarre metadati o eseguire azioni batch. Sono disponibili progresso e controlli per mettere in pausa o annullare."),
        },
            {
            QStringLiteral("𝐀𝐯𝐯𝐢𝐚 𝐞𝐬𝐭𝐫𝐚𝐳𝐢𝐨𝐧𝐞: Seleziona elementi ed esegui l'estrazione metadati batch."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐚 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬𝐨: Segui tramite barra di progresso e pannello messaggi."),
            QStringLiteral("𝐏𝐚𝐮𝐬𝐚/𝐀𝐧𝐧𝐮𝐥𝐥𝐚: Usa i controlli forniti per interrompere l'elaborazione."),
        },
            {}
        },
        {
            QStringLiteral("rapporti-statistiche"),
            QStringLiteral("Report e Statistiche"),
            {
            QStringLiteral("La sezione statistiche produce grafici su tipi di file, utilizzo disco e trend eventi. Esporta immagini o CSV per analisi esterne."),
        },
            {
            QStringLiteral("𝐆𝐞𝐧𝐞𝐫𝐚 𝐠𝐫𝐚𝐟𝐢𝐜𝐢: Seleziona metriche per visualizzare distribuzioni e trend."),
            QStringLiteral("𝐄𝐬𝐩𝐨𝐫𝐭𝐚: Salva grafici come immagini o esporta dati in CSV."),
        },
            {}
        },
        {
            QStringLiteral("monitoraggio-eventi"),
            QStringLiteral("Monitoraggio Eventi"),
            {},
            {
            QStringLiteral("𝐓𝐢𝐩𝐢 𝐝𝐢 𝐞𝐯𝐞𝐧𝐭𝐨: Creazione, modifica, rimozione e spostamento."),
            QStringLiteral("𝐕𝐢𝐬𝐭𝐚 𝐭𝐞𝐦𝐩𝐨 𝐫𝐞𝐚𝐥𝐞: Eventi recenti sono evidenziati per ispezione rapida."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐡𝐞: Configura avvisi visivi o sonori nelle Preferenze."),
        },
            {}
        },
        {
            QStringLiteral("preferenze-it"),
            QStringLiteral("Preferenze"),
            {
            QStringLiteral("Regola lingua, comportamento di scansione, notifiche e aggiornamenti. Controlla queste impostazioni dopo l'installazione."),
        },
            {
            QStringLiteral("𝐋𝐢𝐧𝐠𝐮𝐚: Scegli la lingua dell'interfaccia."),
            QStringLiteral("𝐒𝐜𝐚𝐧𝐬𝐢𝐨𝐧𝐞: Configura periodicità, profondità ed esclusioni."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐡𝐞: Abilita o disabilita gli avvisi."),
            QStringLiteral("𝐀𝐠𝐠𝐢𝐨𝐫𝐧𝐚𝐦𝐞𝐧𝐭𝐢: Scegli controlli automatici o manuali."),
        },
            {}
        },
        {
            QStringLiteral("problemi-comuni-it"),
            QStringLiteral("Problemi Comuni e Soluzioni"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧𝐬𝐢𝐨𝐧𝐞 𝐛𝐥𝐨𝐜𝐜𝐚𝐭𝐚 𝐨 𝐞𝐫𝐫𝐨𝐫𝐞: Attendi; se persiste, riavvia la scansione o l'applicazione."),
            QStringLiteral("𝐄𝐥𝐞𝐦𝐞𝐧𝐭𝐢 𝐦𝐚𝐧𝐜𝐚𝐧𝐭𝐢: Verifica che la cartella sia stata aggiunta e che l'app abbia permessi di lettura."),
            QStringLiteral("𝐈𝐦𝐩𝐨𝐬𝐬𝐢𝐛𝐢𝐥𝐞 𝐚𝐩𝐫𝐢𝐫𝐞: Assicurati che esista un'app associata al tipo di file."),
            QStringLiteral("𝐌𝐞𝐭𝐚𝐝𝐚𝐭𝐢 𝐦𝐚𝐧𝐜𝐚𝐧𝐭𝐢: Non tutti i file includono metadati; prova ad aggiornare o elaborare in batch."),
        },
            {}
        },
        {
            QStringLiteral("scorciatoie-it"),
            QStringLiteral("Scorciatoie"),
            {},
            shortcut_lines(QStringLiteral("it_IT")),
            {}
        },
        {
            QStringLiteral("buone-pratiche-it"),
            QStringLiteral("Buone Pratiche"),
            {},
            {
            QStringLiteral("Esegui una scansione iniziale su una cartella piccola prima di operazioni grandi."),
            QStringLiteral("Salva stati e impostazioni prima di attività batch."),
            QStringLiteral("Usa filtri per concentrarti sugli elementi rilevanti."),
        },
            {}
        },
        {
            QStringLiteral("log-diagnostica-it"),
            QStringLiteral("Log e diagnostica"),
            {},
            {
            QStringLiteral("I log dell'applicazione possono aiutare a diagnosticare problemi; controlla il file di log generato dall'app qui: ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq-it"),
            QStringLiteral("Domande Frequenti (FAQ)"),
            {},
            {},
            {
        {
            QStringLiteral("Dove vengono salvati i miei dati?"),
            {
            QStringLiteral("Attività e file di configurazione sono memorizzati in: ") + dataDir,
            QStringLiteral("Controlla quella cartella per trovare file di persistenza e log."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("supporto-it"),
            QStringLiteral("Come ottenere aiuto e supporto"),
            {},
            {
            QStringLiteral("Controlla la sezione Informazioni nell'applicazione per informazioni ufficiali e note di rilascio."),
            QStringLiteral("Per problemi complessi, genera i log e inviali al supporto. I file di log si trovano in: ") + dataDir,
            QStringLiteral("Invia i file di log e una descrizione dettagliata del problema all'email di supporto: linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
