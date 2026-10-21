#include "DOC_EN_US.h"

namespace source::ui::manual {

std::vector<ManualSection> doc_en_us() {
    const QString dataDir = data_dir();
    return {
        {
            QStringLiteral("new-features-and-capabilities"),
            QStringLiteral("New features and capabilities"),
            {
                QStringLiteral("Version 2026.10.21.0 introduces the dedicated console CLI tool (linceu-lighthouse-cli), new command-line parameters for the desktop graphical application, visual asset optimization for High-DPI screens, and formalization of terms under the MIT License."),
                QStringLiteral("Version 2026.9.28.0 introduced core structural reliability improvements to real-time monitoring, non-blocking asynchronous background loading for real-time sessions, flicker-free differential table synchronization in quarantine, and safe restoration with cryptographic SHA-256 validation."),
                QStringLiteral("The instructions below guide you through using these capabilities safely, predictably, and efficiently during daily operations."),
            },
            {
                QStringLiteral("Standalone CLI utility (linceu-lighthouse-cli.exe): command-line execution for scan, real-time monitoring, PE metadata, baseline audit, and quarantine."),
                QStringLiteral("Desktop startup parameters: direct folder opening, saved profile loading, scan-only execution, and automatic report export."),
                QStringLiteral("Visual asset optimization: faster startup and lower memory footprint by removing obsolete low-res icons in favor of native High-DPI assets."),
                QStringLiteral("Non-blocking real-time sessions: asynchronous background loading with progress indicators and cancellation support."),
                QStringLiteral("Smart quarantine & recovery: differential table updates that preserve selections, sorting order, and scroll position."),
                QStringLiteral("Safe file restoration with target directory picker and cryptographic SHA-256 integrity verification."),
                QStringLiteral("Real-time alert center with deterministic risk triage and correlated security rule evaluation."),
                QStringLiteral("Enhanced multi-directory management with persistent profiles for simultaneous monitoring."),
            },
            {
                {
                    QStringLiteral("Recommended flow for continuous monitoring"),
                    {
                        QStringLiteral("1) Add target directories and run a full initial scan."),
                        QStringLiteral("2) Confirm that the table is populated and status indicators are updated."),
                        QStringLiteral("3) Enable continuous monitoring to capture new events in real time."),
                        QStringLiteral("4) Use filters to reduce noise and speed up audits."),
                    },
                    {}
                },
                {
                    QStringLiteral("Using pause, resume, and stop safely"),
                    {
                        QStringLiteral("During long operations, pause monitoring to perform an intermediate review."),
                        QStringLiteral("Resume only after validating the current visible dataset."),
                        QStringLiteral("Before shutdown, stop monitoring/scanning explicitly and wait for final status update."),
                    },
                    {}
                },
                {
                    QStringLiteral("How to use Asynchronous Real-Time Sessions"),
                    {
                        QStringLiteral("Open Settings > Security > Real-time sessions."),
                        QStringLiteral("The session history loads in a background worker thread, keeping the user interface smooth and responsive even across thousands of records."),
                        QStringLiteral("Review the table columns: Updated at, Session label, Total events, Monitored directories, and Evidence ID."),
                        QStringLiteral("Hover over the directories column to view the complete list of monitored paths when multiple directories are present."),
                        QStringLiteral("Click Refresh at any time to query new incremental event blocks without closing the window."),
                    },
                    {}
                },
                {
                    QStringLiteral("How to use Smart Quarantine & Recovery"),
                    {
                        QStringLiteral("Open Settings > Security > Quarantine and restoration."),
                        QStringLiteral("Navigate between the Quarantine tab (isolated suspect files) and Monitored versions tab (cryptographic baseline snapshots)."),
                        QStringLiteral("Table polling is differential: newly arrived records will not disrupt your active row selection or scroll position."),
                        QStringLiteral("Select any item and click Open selected folder to view the isolated file directly in Windows Explorer."),
                        QStringLiteral("Click Restore selected copy (or Restore monitored version) to choose a safe destination folder and recover the verified file with cryptographic integrity validation."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("command-line-cli-utility"),
            QStringLiteral("Command-Line Utility (CLI) and Automation"),
            {
                QStringLiteral("The 'linceu-lighthouse-cli.exe' is the dedicated console tool of the application, engineered to execute directly within PowerShell, Windows Command Prompt (CMD), Task Scheduler, and CI/CD automation pipelines without requiring a graphical display."),
                QStringLiteral("Both the desktop application (Linceu_Lighthouse.exe) and the CLI utility operate 100% locally, sharing the same cryptographic engine, deterministic security rules, and local SQLite database at %LOCALAPPDATA%/LinceuLighthouse/monitoramento.db."),
            },
            {
                QStringLiteral("General CLI syntax: linceu-lighthouse-cli.exe <command> [options]"),
                QStringLiteral("Supported commands: scan, monitor, metadata, baseline, alerts, quarantine, and sessions."),
                QStringLiteral("Global options: --format <text|json|csv>, --depth <n>, --risks-only, --duration <seconds>, --no-banner, --help, and --version."),
                QStringLiteral("Graphical desktop parameters: Linceu_Lighthouse.exe [path], --dir <path>, --profile <name>, --scan-only, and --export <format> <file>."),
                QStringLiteral("Graphic optimization: streamlined startup and reduced RAM consumption through removal of legacy low-resolution assets."),
                QStringLiteral("MIT Licensing: open-source freedom for source code modification and integration, preserving official binary distribution rights."),
            },
            {
                {
                    QStringLiteral("How to run scans via terminal (scan)"),
                    {
                        QStringLiteral("The 'scan' command catalogs and inspects files in the target directory, calculating SHA-256 hashes and risk scores without opening a GUI."),
                        QStringLiteral("Basic scan: linceu-lighthouse-cli.exe scan \"C:\\MyData\""),
                        QStringLiteral("Scan with depth limit and JSON output: linceu-lighthouse-cli.exe scan \"C:\\MyData\" --depth 2 --format json"),
                        QStringLiteral("Scan filtered only for risky items: linceu-lighthouse-cli.exe scan \"C:\\MyData\" --risks-only"),
                    },
                    {}
                },
                {
                    QStringLiteral("How to monitor real-time file events in console (monitor)"),
                    {
                        QStringLiteral("The 'monitor' command observes file system changes continuously via native Windows APIs (ReadDirectoryChangesW)."),
                        QStringLiteral("Continuous monitoring: linceu-lighthouse-cli.exe monitor \"C:\\inetpub\\wwwroot\""),
                        QStringLiteral("Timed monitoring (e.g., 1 hour): linceu-lighthouse-cli.exe monitor \"D:\\Data\" --duration 3600"),
                        QStringLiteral("Press Ctrl+C at any moment to cleanly terminate the monitoring session."),
                    },
                    {}
                },
                {
                    QStringLiteral("How to inspect binary metadata and Authenticode (metadata)"),
                    {
                        QStringLiteral("The 'metadata' command extracts deep technical details from PE executable files, including architecture, sections, timestamp, and digital signature status."),
                        QStringLiteral("Example: linceu-lighthouse-cli.exe metadata \"C:\\Windows\\notepad.exe\""),
                    },
                    {}
                },
                {
                    QStringLiteral("How to audit cryptographic drift and baselines (baseline)"),
                    {
                        QStringLiteral("The 'baseline' command detects drift by comparing live disk contents against the stored SQLite cryptographic state."),
                        QStringLiteral("Audit drift: linceu-lighthouse-cli.exe baseline \"C:\\Configurations\""),
                        QStringLiteral("Create or update baseline: linceu-lighthouse-cli.exe baseline \"C:\\Configurations\" --update"),
                    },
                    {}
                },
                {
                    QStringLiteral("How to manage security alerts and quarantine (alerts and quarantine)"),
                    {
                        QStringLiteral("Query security alerts: linceu-lighthouse-cli.exe alerts --format json"),
                        QStringLiteral("List files in quarantine vault: linceu-lighthouse-cli.exe quarantine list"),
                        QStringLiteral("Restore isolated file with cryptographic check: linceu-lighthouse-cli.exe quarantine restore <id> \"C:\\SafeDestination\""),
                    },
                    {}
                },
                {
                    QStringLiteral("Desktop parameters for the graphical app (Linceu_Lighthouse.exe)"),
                    {
                        QStringLiteral("Launch directly into a directory: Linceu_Lighthouse.exe \"D:\\Projects\""),
                        QStringLiteral("Automatically load saved profile: Linceu_Lighthouse.exe --profile \"ProductionAudit\""),
                        QStringLiteral("Run silent scan and export immediately: Linceu_Lighthouse.exe --dir \"C:\\Data\" --scan-only --export xlsx report.xlsx"),
                        QStringLiteral("Graphic acceleration mode via LINCEU_OPENGL_MODE environment variable (auto, desktop, angle, software)."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("open-files-mime-folder-dates"),
            QStringLiteral("Opening files, MIME, and folder dates"),
            {
                QStringLiteral("The table can now help open reported items even when they are not text files. When you use Open or double-click a file row, the application tries the Windows default app and may offer other compatible choices."),
                QStringLiteral("The MIME column shows the real content type in readable language for the current interface language, while keeping the original identifier in parentheses for audits. Creation Date, Modification Date, and Access Date are also filled for folders, subfolders, and child directories during scans."),
            },
            {
                QStringLiteral("Use Open or double-click to open documents, images, videos, spreadsheets, executables, archives, and other system-recognized file types."),
                QStringLiteral("When Windows reports more than one option, choose the desired application in the selection window."),
                QStringLiteral("Enable the MIME column to review the detected content family and subtype."),
                QStringLiteral("Enable the three date columns to audit when folders and files were created, modified, or accessed."),
            },
            {
                {
                    QStringLiteral("Open any file type"),
                    {
                        QStringLiteral("1) Select a row that represents an existing file."),
                        QStringLiteral("2) Double-click it or use the Open action."),
                        QStringLiteral("3) If an application chooser appears, select the desired application and confirm."),
                        QStringLiteral("4) If the item is a folder, use the open-folder action to navigate to it in File Explorer."),
                    },
                    {}
                },
                {
                    QStringLiteral("Read the MIME column"),
                    {
                        QStringLiteral("The first part shows the content family, such as Text, Image, Video, Audio, or Application."),
                        QStringLiteral("The second part shows the subtype in simple language, such as PDF, PNG, ZIP, or Plain text."),
                        QStringLiteral("The text in parentheses keeps the original identifier, for example text/plain or application/pdf."),
                    },
                    {}
                },
                {
                    QStringLiteral("Check dates for folders and subfolders"),
                    {
                        QStringLiteral("1) Open the column menu and enable Creation Date, Modification Date, and Access Date."),
                        QStringLiteral("2) Run a new scan on the selected directory."),
                        QStringLiteral("3) Sort or filter the table to review child folders and deeper directories."),
                        QStringLiteral("4) Use these dates to compare recent changes with recorded events."),
                    },
                    {}
                },
                {
                    QStringLiteral("If a file does not open"),
                    {
                        QStringLiteral("Confirm that the path still exists and that you have read permission."),
                        QStringLiteral("Check whether Windows has an application associated with that file type."),
                        QStringLiteral("When needed, manually choose another compatible application in the selection window."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("high-volume-operation"),
            QStringLiteral("High-volume operation"),
            {
                QStringLiteral("When many files change in sequence, use short operating cycles: capture, review, filter, and export."),
            },
            {
                QStringLiteral("Sort by date/event to prioritize recent changes."),
                QStringLiteral("Apply directory/type filters before exporting datasets."),
                QStringLiteral("Use strategic pauses to create clear review checkpoints."),
            },
            {
                {
                    QStringLiteral("Language change during heavy activity"),
                    {
                        QStringLiteral("Language can be changed while monitoring is active, but pausing is recommended during critical visual validation."),
                        QStringLiteral("After changing language, verify menu labels, table headers, help, and statistics text."),
                    },
                    {}
                },
                {
                    QStringLiteral("Quick stability validation"),
                    {
                        QStringLiteral("Ensure new events continue to appear after monitoring resumes."),
                        QStringLiteral("Check that status labels and progress indicators transition correctly between states."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("complementary-usage-resources"),
            QStringLiteral("Complementary usage resources"),
            {
                QStringLiteral("This release also adds practical daily-use resources: configurable security popup behavior, refined event classification, and statistics opening with consistency checks."),
            },
            {
                QStringLiteral("Security popup controls: minimum trigger threshold, maximum lines, and cooldown between alerts."),
                QStringLiteral("Clearer separation between Renamed and Moved events for auditing."),
                QStringLiteral("Statistics flow with improved consistency when pending writes still exist."),
            },
            {
                {
                    QStringLiteral("Configure the security popup"),
                    {
                        QStringLiteral("Open the Security menu and access popup/notification settings."),
                        QStringLiteral("Set the minimum threshold required to trigger an alert."),
                        QStringLiteral("Set the maximum number of lines shown per popup."),
                        QStringLiteral("Configure cooldown between alerts and save preferences."),
                    },
                    {}
                },
                {
                    QStringLiteral("Interpret Renamed vs Moved events"),
                    {
                        QStringLiteral("Renamed: same directory, only the item name changes."),
                        QStringLiteral("Moved: directory changes, with or without a name change."),
                        QStringLiteral("For audits, filter by Event and review source/destination paths in the details panel."),
                    },
                    {}
                },
                {
                    QStringLiteral("Open statistics with stable data"),
                    {
                        QStringLiteral("Finish heavy operations or pause monitoring before opening statistics."),
                        QStringLiteral("After opening, verify there are no pending processing statuses."),
                        QStringLiteral("If numbers look inconsistent, close and reopen after processing stabilizes."),
                    },
                    {}
                },
                {
                    QStringLiteral("Advanced rendering mode (optional)"),
                    {
                        QStringLiteral("If you face graphics compatibility issues, set LINCEU_OPENGL_MODE before launching the app."),
                        QStringLiteral("Accepted values: auto, desktop, angle, and software."),
                        QStringLiteral("Use software only for troubleshooting, as performance may decrease."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("updated-menu-operational-flow"),
            QStringLiteral("Operational flow for the updated menus"),
            {
                QStringLiteral("The File, Settings and Options menus now cover the full working cycle: select directories, start or pause analysis, prepare the table, configure statistics, export data and review security resources."),
            },
            {
                QStringLiteral("Use File > Select Directory or Ctrl+Shift+O before running the initial scan."),
                QStringLiteral("Use File > Start/Stop or Ctrl+R to control analysis, and File > Pause or Ctrl+P to create a review point without clearing the current state."),
                QStringLiteral("Use Settings > Filters, Columns and Colored columns before exporting or opening statistics, so the table highlights only the relevant data."),
                QStringLiteral("Use Options > Manual whenever you need to reopen these instructions without restarting the application."),
            },
            {
                {
                    QStringLiteral("Recommended sequence"),
                    {
                        QStringLiteral("1. Select a directory and run the initial scan."),
                        QStringLiteral("2. Adjust filters, columns and colors to keep only the useful data visible."),
                        QStringLiteral("3. Open Settings > Statistics > Plot Charts and choose the charts needed for the audit."),
                        QStringLiteral("4. Open File > Statistics or press Ctrl+G to generate the visual analysis."),
                        QStringLiteral("5. Export results only after confirming that the table and charts show the intended scope."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("using-statistics-chart-menu"),
            QStringLiteral("Using Settings > Statistics > Plot Charts"),
            {
                QStringLiteral("This submenu controls which charts are generated in the statistics window. Checked items are saved in statistics_charts_config.json inside the persistent application data folder and are reused the next time statistics are opened."),
            },
            {
                QStringLiteral("Check only the charts you want to generate to reduce processing time and keep the statistics window focused."),
                QStringLiteral("Uncheck heavy or unnecessary charts when analyzing many events."),
                QStringLiteral("After changing the selection, open File > Statistics or press Ctrl+G to generate the window with the selected chart set."),
            },
            {
                {
                    QStringLiteral("Choosing chart groups"),
                    {
                        QStringLiteral("Use Pie, Bars, Pareto and Distributions to compare volumes by type, size or operation."),
                        QStringLiteral("Use Timeline, Operations by day, Week/Hour and Heatmap to investigate temporal concentration."),
                        QStringLiteral("Use Sankey, Event by directory and Type by directory to discover flows between operation, file type and monitored folder."),
                        QStringLiteral("Use Directory Tree to locate which folder levels concentrate more activity."),
                        QStringLiteral("Use Scatter, Radar and Dotplot to compare size behavior, frequency and operation distribution."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("using-statistics-window"),
            QStringLiteral("Using the Statistics window"),
            {
                QStringLiteral("The statistics window reads the monitoring database and shows the selected charts. Open it only after the table has data; during very heavy scans, wait for the status to stabilize before refreshing charts."),
            },
            {
                QStringLiteral("Use the Select charts panel to show or hide charts during the current session."),
                QStringLiteral("Use Select all to enable every chart available in the window."),
                QStringLiteral("Use Save selected to export only checked charts as PNG files."),
                QStringLiteral("Use Save all to export every generated chart to a timestamped folder."),
                QStringLiteral("Use Refresh after new events, after relevant filter changes or after changing the chart selection menu."),
                QStringLiteral("Hide the selection panel when you need more room to compare chart tabs and visual details."),
            },
            {}
        },
        {
            QStringLiteral("using-security-menu"),
            QStringLiteral("Using Settings > Security"),
            {
                QStringLiteral("The Security submenu brings together audit resources, real-time session tracking, quarantine, restoration, cryptographic baselines, and evidence packages. Use it after configuring directories and preferably after an initial scan."),
            },
            {},
            {
                {
                    QStringLiteral("Profiles and multi-directory"),
                    {
                        QStringLiteral("Open Settings > Security > Profiles and multi-directory to manage the active profile."),
                        QStringLiteral("Use Add directory to include another folder in the same profile and Remove selected to remove the highlighted folder."),
                        QStringLiteral("Edit Profile name and click Save profile to identify the monitored folder set."),
                        QStringLiteral("Click Refresh to reload the main directory, directory count and active profile summary."),
                    },
                    {}
                },
                {
                    QStringLiteral("Real-time alert center"),
                    {
                        QStringLiteral("Open Settings > Security > Alert center to review security occurrences triaged by risk levels: Low, Medium, High, and Critical."),
                        QStringLiteral("Inspect the alert summary, triggered rules, target path, filename, associated session, and evidence ID."),
                        QStringLiteral("Prioritize investigating Critical or High alerts before reviewing routine events in the main table."),
                        QStringLiteral("Click Refresh to query newly detected alerts without disrupting active file monitoring."),
                    },
                    {}
                },
                {
                    QStringLiteral("Asynchronous real-time sessions"),
                    {
                        QStringLiteral("Open Settings > Security > Real-time sessions to track filesystem activity blocks."),
                        QStringLiteral("Loading occurs in a dedicated background worker, avoiding visual freezes or lag even with large historical databases."),
                        QStringLiteral("Inspect the timestamp, user-friendly session label, total event count, monitored directories summary, and unique session ID."),
                        QStringLiteral("Hover over the directories column to view the full path list when a session spans multiple folders."),
                        QStringLiteral("Use Refresh to load newly recorded incremental sessions while monitoring remains active."),
                    },
                    {}
                },
                {
                    QStringLiteral("Smart quarantine and recovery"),
                    {
                        QStringLiteral("Open Settings > Security > Quarantine and restoration to access the Quarantine and Monitored versions tabs."),
                        QStringLiteral("Tables use flicker-free differential synchronization: your active selection, scroll position, and sorting order stay intact during automatic updates."),
                        QStringLiteral("Under the Quarantine tab, select an isolated file and click Open selected folder to view its directory in Windows Explorer."),
                        QStringLiteral("Click Restore selected copy to open a target destination dialog: restore to the original path or choose an alternative safe folder for analysis."),
                        QStringLiteral("Under the Monitored versions tab, select a baseline snapshot version and click Restore monitored version to recover it."),
                        QStringLiteral("The system validates cryptographic SHA-256 integrity and displays an immediate success confirmation upon restoration."),
                    },
                    {}
                },
                {
                    QStringLiteral("Evidence package and audit"),
                    {
                        QStringLiteral("Open Settings > Security > Evidence package to generate a structured, auditable export."),
                        QStringLiteral("Choose a destination folder, click Export now, and inspect the generated package containing cryptographic baseline reports and structured logs."),
                        QStringLiteral("Use Clear security database exclusively after saving evidence packages and verifying that historical audit logs are no longer needed."),
                    },
                    {}
                },
            }
        },
        {
            QStringLiteral("using-notifications-and-export-options"),
            QStringLiteral("Using notifications and export options"),
            {
                QStringLiteral("Security notifications and export options help control information volume, alert frequency and the shape of saved data."),
            },
            {
                QStringLiteral("Open Settings > Security > Notifications to enable footer/status security summaries or configure popup types."),
                QStringLiteral("In alert, session or quarantine popups, use Enable this popup, Trigger at, Maximum summary lines and Cooldown between popups to tune how often notices appear."),
                QStringLiteral("Open Settings > Export options to decide whether saved files should respect active columns, active filters or only selected rows."),
                QStringLiteral("Use Reset export options when you want the default behavior before generating CSV, TXT, JSON, XML, DB, XLSX or evidence packages."),
            },
            {}
        },
        {
            QStringLiteral("overview"),
            QStringLiteral("Overview"),
            {
            QStringLiteral("Linceu Lighthouse helps you inspect, monitor and analyze files and folders across multiple locations on your computer. It centralizes item information and provides tools to filter, generate statistics and extract metadata."),
        },
            {},
            {}
        },
        {
            QStringLiteral("main-screen"),
            QStringLiteral("Main Screen — Elements"),
            {},
            {
            QStringLiteral("𝐓𝐨𝐨𝐥𝐛𝐚𝐫: Quick access to Add Directory, Start Scan, Stop, Refresh and Preferences."),
            QStringLiteral("𝐃𝐢𝐫𝐞𝐜𝐭𝐨𝐫𝐲 𝐬𝐞𝐥𝐞𝐜𝐭𝐨𝐫: Area to add, remove or switch monitored folders."),
            QStringLiteral("𝐈𝐭𝐞𝐦 𝐭𝐚𝐛𝐥𝐞: Central list with columns (Name, Type, Size, Date, Event) supporting sort and filter."),
            QStringLiteral("𝐃𝐞𝐭𝐚𝐢𝐥𝐬 𝐩𝐚𝐧𝐞𝐥: Shows metadata and quick actions for the selected item."),
            QStringLiteral("𝐌𝐞𝐬𝐬𝐚𝐠𝐞𝐬 & 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬: Displays notifications, operation logs and progress bars."),
            QStringLiteral("𝐒𝐭𝐚𝐭𝐢𝐬𝐭𝐢𝐜𝐬: Area for charts and reports generated from collected data."),
        },
            {}
        },
        {
            QStringLiteral("getting-started"),
            QStringLiteral("Getting Started"),
            {},
            {
            QStringLiteral("Open the application using the shortcut or executable."),
            QStringLiteral("Click 𝐀𝐝𝐝 𝐃𝐢𝐫𝐞𝐜𝐭𝐨𝐫𝐲 and choose the folder to monitor."),
            QStringLiteral("Run an initial scan with 𝐒𝐭𝐚𝐫𝐭/𝐒𝐜𝐚𝐧."),
            QStringLiteral("Wait for the process to finish; items will appear with available metadata."),
            QStringLiteral("Enable 𝐂𝐨𝐧𝐭𝐢𝐧𝐮𝐨𝐮𝐬 𝐦𝐨𝐧𝐢𝐭𝐨𝐫𝐢𝐧𝐠 to detect changes automatically."),
        },
            {}
        },
        {
            QStringLiteral("operation-modes"),
            QStringLiteral("Operation Modes"),
            {},
            {
            QStringLiteral("𝐒𝐢𝐧𝐠𝐥𝐞 𝐒𝐜𝐚𝐧: Immediate check of the selected folder(s)."),
            QStringLiteral("𝐂𝐨𝐧𝐭𝐢𝐧𝐮𝐨𝐮𝐬 𝐌𝐨𝐧𝐢𝐭𝐨𝐫𝐢𝐧𝐠: Watch for real-time changes and update the table automatically."),
            QStringLiteral("𝐁𝐚𝐭𝐜𝐡 𝐏𝐫𝐨𝐜𝐞𝐬𝐬𝐢𝐧𝐠: Extract metadata or run actions on multiple files at once."),
        },
            {}
        },
        {
            QStringLiteral("understanding-table"),
            QStringLiteral("Understanding the Item Table"),
            {
            QStringLiteral("Each column shows a property of the item. Use sorting, filtering and selection to find and inspect items efficiently."),
        },
            {
            QStringLiteral("𝐍𝐚𝐦𝐞: File or folder name; double-click to open."),
            QStringLiteral("𝐓𝐲𝐩𝐞: Detected type (image, video, document, folder)."),
            QStringLiteral("𝐒𝐢𝐳𝐞: Shown in human-readable units (KB/MB/GB)."),
            QStringLiteral("𝐃𝐚𝐭𝐞: Last modification or recorded event date."),
            QStringLiteral("𝐄𝐯𝐞𝐧𝐭: Detected action (created, modified, moved, deleted)."),
        },
            {}
        },
        {
            QStringLiteral("details-panel"),
            QStringLiteral("Details Panel & Metadata"),
            {
            QStringLiteral("Selecting an item displays relevant metadata and quick actions (open folder, copy path, refresh metadata)."),
        },
            {
            QStringLiteral("𝐅𝐮𝐥𝐥 𝐩𝐚𝐭𝐡: File location on disk."),
            QStringLiteral("𝐀𝐮𝐭𝐡𝐨𝐫 / 𝐂𝐫𝐞𝐚𝐭𝐨𝐫: When present in metadata."),
            QStringLiteral("𝐃𝐢𝐦𝐞𝐧𝐬𝐢𝐨𝐧𝐬: For images (width × height)."),
            QStringLiteral("𝐃𝐮𝐫𝐚𝐭𝐢𝐨𝐧: For audio/video files."),
            QStringLiteral("𝐁𝐢𝐭𝐫𝐚𝐭𝐞: For media files, if available."),
            QStringLiteral("𝐏𝐞𝐫𝐦𝐢𝐬𝐬𝐢𝐨𝐧𝐬: Indicates protection or write restrictions."),
        },
            {}
        },
        {
            QStringLiteral("file-actions"),
            QStringLiteral("File Actions"),
            {},
            {
            QStringLiteral("𝐎𝐩𝐞𝐧: Open with the system default app (double-click or Open button)."),
            QStringLiteral("𝐂𝐨𝐩𝐲 / 𝐌𝐨𝐯𝐞: Use copy/paste or drag-and-drop to move files between folders."),
            QStringLiteral("𝐑𝐞𝐧𝐚𝐦𝐞 / 𝐃𝐞𝐥𝐞𝐭𝐞: Use the context menu; permanent actions require confirmation."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐨𝐫𝐞: When available, recover recent deletions using restore functionality."),
            QStringLiteral("𝐏𝐫𝐨𝐩𝐞𝐫𝐭𝐢𝐞𝐬: View full metadata and event history for the file."),
        },
            {}
        },
        {
            QStringLiteral("search-filters"),
            QStringLiteral("Search and Filters"),
            {},
            {
            QStringLiteral("𝐒𝐞𝐚𝐫𝐜𝐡: Enter terms in the search box to find items by name or content."),
            QStringLiteral("𝐅𝐢𝐥𝐭𝐞𝐫𝐬: Apply filters by type, size, date or event to narrow results."),
            QStringLiteral("𝐂𝐨𝐦𝐛𝐢𝐧𝐞 𝐟𝐢𝐥𝐭𝐞𝐫𝐬: Use multiple filters together for precise results."),
        },
            {}
        },
        {
            QStringLiteral("save-restore"),
            QStringLiteral("Saving and Restoring Views"),
            {},
            {
            QStringLiteral("𝐒𝐚𝐯𝐞 𝐬𝐭𝐚𝐭𝐞: Store current column layout and filters as a profile."),
            QStringLiteral("𝐑𝐞𝐬𝐭𝐨𝐫𝐞 𝐬𝐭𝐚𝐭𝐞: Load a saved profile to recover layout and filters."),
        },
            {}
        },
        {
            QStringLiteral("batch-processing"),
            QStringLiteral("Batch Processing"),
            {
            QStringLiteral("Select multiple items to extract metadata or perform batch actions. Progress and controls are available to pause or cancel."),
        },
            {
            QStringLiteral("𝐒𝐭𝐚𝐫𝐭 𝐞𝐱𝐭𝐫𝐚𝐜𝐭𝐢𝐨𝐧: Choose items and run batch metadata extraction."),
            QStringLiteral("𝐌𝐨𝐧𝐢𝐭𝐨𝐫 𝐩𝐫𝐨𝐠𝐫𝐞𝐬𝐬: Track via the progress bar and messages panel."),
            QStringLiteral("𝐏𝐚𝐮𝐬𝐞/𝐂𝐚𝐧𝐜𝐞𝐥: Use the provided controls to interrupt processing."),
        },
            {}
        },
        {
            QStringLiteral("reports-statistics"),
            QStringLiteral("Reports & Statistics"),
            {
            QStringLiteral("The statistics section produces charts for file types, disk usage and event trends. Export images or CSV for external use."),
        },
            {
            QStringLiteral("𝐆𝐞𝐧𝐞𝐫𝐚𝐭𝐞 𝐜𝐡𝐚𝐫𝐭𝐬: Select metrics to visualize distributions and trends."),
            QStringLiteral("𝐄𝐱𝐩𝐨𝐫𝐭: Save charts as images or export data as CSV."),
        },
            {}
        },
        {
            QStringLiteral("event-monitoring"),
            QStringLiteral("Event Monitoring"),
            {},
            {
            QStringLiteral("𝐄𝐯𝐞𝐧𝐭 𝐭𝐲𝐩𝐞𝐬: Creation, modification, removal and movement."),
            QStringLiteral("𝐑𝐞𝐚𝐥-𝐭𝐢𝐦𝐞 𝐯𝐢𝐞𝐰: Recent events are highlighted for quick inspection."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐭𝐢𝐨𝐧𝐬: Configure visual or sound alerts in Preferences."),
        },
            {}
        },
        {
            QStringLiteral("preferences"),
            QStringLiteral("Preferences"),
            {
            QStringLiteral("Adjust language, scan behavior, notifications and update preferences. Review these settings after installation."),
        },
            {
            QStringLiteral("𝐋𝐚𝐧𝐠𝐮𝐚𝐠𝐞: Choose the UI language."),
            QStringLiteral("𝐒𝐜𝐚𝐧 𝐛𝐞𝐡𝐚𝐯𝐢𝐨𝐫: Configure schedule, depth and exclusions."),
            QStringLiteral("𝐍𝐨𝐭𝐢𝐟𝐢𝐜𝐚𝐭𝐢𝐨𝐧𝐬: Enable or disable alerts."),
            QStringLiteral("𝐔𝐩𝐝𝐚𝐭𝐞𝐬: Choose automatic or manual checks."),
        },
            {}
        },
        {
            QStringLiteral("common-issues"),
            QStringLiteral("Common Issues & Fixes"),
            {},
            {
            QStringLiteral("𝐒𝐜𝐚𝐧 𝐬𝐭𝐮𝐜𝐤 𝐨𝐫 𝐞𝐫𝐫𝐨𝐫: Wait briefly; if it persists, restart the scan or the application."),
            QStringLiteral("𝐈𝐭𝐞𝐦𝐬 𝐦𝐢𝐬𝐬𝐢𝐧𝐠: Verify the folder is added and the app has read permission."),
            QStringLiteral("𝐔𝐧𝐚𝐛𝐥𝐞 𝐭𝐨 𝐨𝐩𝐞𝐧 𝐟𝐢𝐥𝐞: Ensure an associated application exists for the file type."),
            QStringLiteral("𝐌𝐢𝐬𝐬𝐢𝐧𝐠 𝐦𝐞𝐭𝐚𝐝𝐚𝐭𝐚: Not all files include metadata; try refreshing or batch processing."),
        },
            {}
        },
        {
            QStringLiteral("shortcuts"),
            QStringLiteral("Shortcuts"),
            {},
            shortcut_lines(QStringLiteral("en_US")),
            {}
        },
        {
            QStringLiteral("best-practices"),
            QStringLiteral("Best Practices"),
            {},
            {
            QStringLiteral("Run an initial scan on a small folder before large operations."),
            QStringLiteral("Save states and settings before batch tasks."),
            QStringLiteral("Use filters to focus on relevant items."),
        },
            {}
        },
        {
            QStringLiteral("logs-and-diagnostics"),
            QStringLiteral("Logs & diagnostics"),
            {},
            {
            QStringLiteral("The application logs can help diagnose issues; check the log file generated by the app at: ") + dataDir,
        },
            {}
        },
        {
            QStringLiteral("faq"),
            QStringLiteral("Frequently Asked Questions"),
            {},
            {},
            {
        {
            QStringLiteral("Where are my data saved?"),
            {
            QStringLiteral("Tasks and configuration files are stored in: ") + dataDir,
            QStringLiteral("Check that directory to find persistence files and logs."),
        },
            {}
        },
    }
        },
        {
            QStringLiteral("support"),
            QStringLiteral("How to get help and support"),
            {},
            {
            QStringLiteral("Check the About section inside the application for official information and release notes."),
            QStringLiteral("For complex issues, generate logs and send them to support. Log files are located at: ") + dataDir,
            QStringLiteral("Send the log files and a detailed description of the issue to the support email: linceu_lighthouse@outlook.com."),
        },
            {}
        },
    };
}

}  // namespace source::ui::manual
