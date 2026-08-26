pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 1280
    height: 760

    property int currentIndex: 0
    readonly property var sections: [
        {
            title: "Visao geral",
            subtitle: "Aplicacao desktop C++17/Qt6 para monitoramento, auditoria, estatisticas e seguranca.",
            summary: [
                "Ponto de entrada em main.cpp, delegando para source::iniciar_aplicacao.",
                "InterfaceMonitor centraliza a janela principal e agrega os gerenciadores de UI, servicos e seguranca.",
                "Eventos de arquivo fluem por Observador, BaseEvento, DatabaseWriter, EventTableModel e SecurityManager.",
                "Graficos e diagramas usam subsistema proprio em source/data/Charts."
            ],
            files: [
                "main.cpp",
                "source/src_01_InicializadorMain.*",
                "source/src_02_SplashAppStarter.*",
                "source/gui/ic_01_InterfaceMonitor.*"
            ],
            dependencies: [
                "C++17",
                "Qt Widgets",
                "Qt SQL",
                "Qt Quick/Qml",
                "Qt OpenGL"
            ],
            flow: [
                "main.cpp",
                "iniciar_aplicacao",
                "QApplication",
                "InterfaceMonitor",
                "Observador + Tabela + Seguranca"
            ]
        },
        {
            title: "Build e deploy",
            subtitle: "CMake resolve Qt6, compila o alvo principal e empacota runtime/assets.",
            summary: [
                "CMakeLists.txt define C++17, AUTOMOC e AUTORCC.",
                "O preset qt6-mingw-debug aponta para Qt 6 MinGW no Windows.",
                "O alvo Linceu_Lighthouse_cpp vincula Core, Gui, Widgets, Sql, Xml, Concurrent, OpenGL, OpenGLWidgets, Quick e Qml.",
                "O pos-build executa windeployqt e copia assets e traducoes para o diretorio de deploy."
            ],
            files: [
                "CMakeLists.txt",
                "CMakePresets.json",
                "source/Linceu_Lighthouse_cpp.rc",
                "source/assets",
                "source/locale/*.qm"
            ],
            dependencies: [
                "Qt::Core",
                "Qt::Gui",
                "Qt::Widgets",
                "Qt::Sql",
                "Qt::Xml",
                "Qt::Concurrent",
                "Qt::OpenGL",
                "Qt::OpenGLWidgets",
                "Qt::Quick",
                "Qt::Qml",
                "wintrust",
                "version",
                "crypt32"
            ],
            flow: [
                "configure preset",
                "build Linceu_Lighthouse_cpp",
                "copy executable",
                "windeployqt",
                "copy assets",
                "copy locale"
            ]
        },
        {
            title: "Inicializacao",
            subtitle: "Bootstrap prepara caminhos, backend grafico, splash, traducao, QML e persistencia.",
            summary: [
                "ajustar_cwd garante que assets e banco sejam resolvidos a partir do diretorio correto.",
                "configurar_backend_grafico define OpenGL compartilhado, double buffering e antialiasing.",
                "SplashAppStarter cria QApplication, splash, progresso e InterfaceMonitor.",
                "register_qml_types expoe QuickChartItem para QML.",
                "prepare_runtime prepara a camada local de seguranca antes da UI operar."
            ],
            files: [
                "source/src_01_InicializadorMain.cpp",
                "source/src_02_SplashAppStarter.cpp",
                "source/data/Charts/ch_04_QmlRegistration.*",
                "source/security/secore_01_postgres_utils.*",
                "source/utils/IconUtils.*",
                "source/utils/LogManager.*"
            ],
            dependencies: [
                "QApplication",
                "QSurfaceFormat",
                "QIcon",
                "QTimer",
                "QTranslator",
                "Windows AppUserModelID"
            ],
            flow: [
                "cwd",
                "OpenGL mode",
                "QApplication",
                "translations",
                "InterfaceMonitor",
                "DatabaseWriter",
                "app.exec"
            ]
        },
        {
            title: "Interface principal",
            subtitle: "InterfaceMonitor e a fachada visual que coordena menus, botoes, dialogs, tabela e eventos.",
            summary: [
                "Expõe comandos Q_INVOKABLE para botoes, menus e dialogs.",
                "Coordena GerenciadorTabela, GerenciadorColunas, GerenciadorEventosUI, GerenciadorEstatisticasUI e SecurityManager.",
                "Mantem hierarquia de janelas associadas por propriedades dinamicas como interface_principal.",
                "Controla ativacao, foco e encerramento de dialogs associados."
            ],
            files: [
                "source/gui/ic_01_InterfaceMonitor.*",
                "source/ui/ui_01_GerenciadorBotoes.*",
                "source/ui/ui_03_GerenciadorMenusUI.*",
                "source/ui/ui_04_GerenciadorEventosUI.*",
                "source/ui/ui_06_GerenciadorEstatisticasUI.*",
                "source/ui/GerenciamentoMenusUI/*",
                "source/ui/GerenciamentoDialogoCores/*"
            ],
            dependencies: [
                "QMainWindow",
                "QDialog",
                "QMenu",
                "QAction",
                "QTableView",
                "QObject properties"
            ],
            flow: [
                "setup_ui",
                "setup_menu_bar",
                "gerenciadores",
                "eventFilter",
                "sincronizacao de janelas",
                "closeEvent"
            ]
        },
        {
            title: "Monitoramento e persistencia",
            subtitle: "Observador, scanner, eventos e banco formam o pipeline operacional.",
            summary: [
                "Observador mantem threads, locks, atomics, buffer de eventos e estado de pausa/parada.",
                "BaseEvento normaliza payloads e registra eventos no banco.",
                "DiretorioScanner processa scans iniciais e lotes.",
                "DatabaseWriter reduz bloqueios da UI com persistencia assincrona.",
                "EventTableModel recebe eventos em streaming e atualiza a tabela."
            ],
            files: [
                "source/services/ob_01_Observador.*",
                "source/services/ob_02_BaseEvento.*",
                "source/services/GerenciamentoBaseEvento/*",
                "source/services/GerenciamentoDiretorioScanner/*",
                "source/services/GerenciamentoEventoModificado/*",
                "source/services/GerenciamentoEventoMovido/*",
                "source/services/GerenciamentoTabela/gtab_47_EventTableModel.*"
            ],
            dependencies: [
                "std::thread",
                "std::atomic",
                "std::mutex",
                "QThread",
                "QSqlDatabase",
                "QVariantMap"
            ],
            flow: [
                "ReadDirectoryChanges/scanner",
                "Observador",
                "handlers de evento",
                "BaseEvento",
                "DatabaseWriter",
                "EventTableModel",
                "GerenciadorTabela"
            ]
        },
        {
            title: "Metadados e tabela",
            subtitle: "Extracao especializada alimenta as colunas, filtros, exportacao e estatisticas.",
            summary: [
                "GerenciamentoMetadados possui extratores por tipo de arquivo e helpers de tamanho, atributos, autor, duracao e dimensoes.",
                "GerenciadorTabela cuida de cores, tema Windows, cabecalhos, colunas, ordenacao e retraducao.",
                "Caches de cores e indices evitam recalculo excessivo.",
                "Processamentos pesados sao agendados para preservar a responsividade."
            ],
            files: [
                "source/services/GerenciamentoMetadados/*",
                "source/services/ob_09_GerenciadorColunas.*",
                "source/services/ob_10_GerenciadorTabela.*",
                "source/services/GerenciamentoTabela/*",
                "source/ui/GerenciamentoEventosUI/*",
                "source/ui/GerenciamentoBotoes/*"
            ],
            dependencies: [
                "QFileInfo",
                "QMime-like type detection",
                "QTimer",
                "QBrush",
                "QColor",
                "QAbstractTableModel"
            ],
            flow: [
                "evento",
                "extracao de metadados",
                "colunas",
                "cores",
                "modelo",
                "view",
                "exportacao"
            ]
        },
        {
            title: "Seguranca forense",
            subtitle: "SecurityManager enriquece eventos e centraliza baseline, risco, quarentena e evidencias.",
            summary: [
                "SecurityManager coordena contexto, schemas, consultas e observadores de runtime.",
                "Baseline criptografico registra snapshots e hashes.",
                "Motor de regras gera alertas; ScoreRisco calcula severidade.",
                "Quarentena captura metadados/blobs e RecoveryDialog restaura itens.",
                "ExportacaoEvidencia cria pacotes auditaveis."
            ],
            files: [
                "source/security/secore_02_SecurityManager.*",
                "source/security/base_01_baseline_criptografico.*",
                "source/security/exec_01_inteligencia_executaveis.*",
                "source/security/agre_01_motor_regras.*",
                "source/security/risk_01_score_risco.*",
                "source/security/quar_01_quarentena.*",
                "source/security/evid_01_exportacao_evidencia.*",
                "source/security/secui_*"
            ],
            dependencies: [
                "Qt SQL",
                "wintrust",
                "crypt32",
                "version",
                "SecurityDbWriter",
                "QVariantList"
            ],
            flow: [
                "evento normalizado",
                "enrich_event",
                "baseline/diff/regras",
                "risco",
                "alertas",
                "quarentena",
                "evidencia"
            ]
        },
        {
            title: "Estatisticas e graficos",
            subtitle: "GeradorEstatisticas consulta eventos e entrega payloads para renderizadores proprios.",
            summary: [
                "GeradorEstatisticas registra geradores gst_02 a gst_18.",
                "Payloads permanecem em QVariantMap/QVariantList para compatibilidade com a UI.",
                "LegacyVariantChartAdapter converte payload legado para LgplChartModel.",
                "ChartLayoutEngine calcula geometria, labels, legendas e regioes de picking.",
                "RasterChartView e usado na janela Estatisticas para estabilidade multi-monitor."
            ],
            files: [
                "source/data/st_01_GeradorEstatisticas.*",
                "source/data/GeradorEstatisticas/gst_*.{cpp,h}",
                "source/data/Charts/ch_01_ChartLayoutEngine.*",
                "source/data/Charts/ch_02_ChartWidgetFactory.*",
                "source/data/Charts/ch_03_LegacyVariantChartAdapter.*",
                "source/data/Charts/ch_08_RasterChartView.*",
                "source/ui/GerenciamentoEstatisticasUI/*"
            ],
            dependencies: [
                "QPainter",
                "QPainterPath",
                "QOpenGLWidget",
                "QQuickItem",
                "QSGGeometryNode",
                "QVariantAnimation"
            ],
            flow: [
                "botao Estatisticas",
                "GerenciadorEstatisticasUI",
                "GeradorEstatisticas",
                "QVariantMap",
                "ChartWidgetFactory",
                "RasterChartView",
                "export"
            ]
        },
        {
            title: "Internacionalizacao e assets",
            subtitle: "Textos de UI, metadados, manual e notices sao mantidos por idioma.",
            summary: [
                "LocalizadorQt carrega tradutores, preferencias de idioma e textos fallback.",
                "TradutorMetadadosQt traduz operacoes, tipos, atributos, autores, protecao e dimensoes.",
                "Arquivos .ts/.qm cobrem pt_BR, en_US, es_ES, fr_FR, it_IT e de_DE.",
                "Assets incluem ABOUT, CLC, COPYRIGHT, EULA, LICENSES, NOTICES, PRIVACY_POLICY, RELEASE e icones.",
                "Mudanca de idioma retraduz menus, cabecalhos, filtros, estatisticas e dados existentes."
            ],
            files: [
                "source/locale/*",
                "source/gui/ic_08_Internacionalizador.*",
                "source/ui/ui_12_LocalizadorQt.*",
                "source/ui/ui_13_TradutorMetadadosQt.*",
                "source/ui/Manual/*",
                "source/assets/*"
            ],
            dependencies: [
                "QTranslator",
                "QSettings-like persistence",
                "QHash",
                "QVariantMap",
                "assets textuais"
            ],
            flow: [
                "preferencia",
                "carregar .qm",
                "traducoes",
                "metadados",
                "menus",
                "tabela",
                "manual"
            ]
        },
        {
            title: "Evolucao segura",
            subtitle: "Pontos de extensao para novos graficos, colunas, idiomas e recursos de seguranca.",
            summary: [
                "Novo grafico: criar gerador, registrar em GeradorEstatisticas, adaptar payload e expor no painel.",
                "Nova coluna: atualizar evento/metadados, registrar chave, traduzir label e validar exportacao.",
                "Nova regra de seguranca: implementar componente, schema, writer e UI se necessario.",
                "Novo idioma: criar .ts/.qm, assets legais, entrada no LocalizadorQt e validar retraducao.",
                "Mudancas em dialogs devem respeitar a propriedade interface_principal e a hierarquia de janelas."
            ],
            files: [
                "ARCHITECTURE.md",
                "AdvancedDocumentation.qml",
                "source/data/GeradorEstatisticas",
                "source/data/Charts",
                "source/security",
                "source/locale",
                "source/assets"
            ],
            dependencies: [
                "contrato QVariantMap",
                "thread principal Qt",
                "writers assincronos",
                "cache invalidation",
                "documentacao atualizada"
            ],
            flow: [
                "planejar contrato",
                "implementar modulo",
                "ligar na fachada",
                "traduzir",
                "documentar",
                "validar build"
            ]
        }
    ]

    readonly property var activeSection: sections[currentIndex]

    Rectangle {
        anchors.fill: parent
        color: "#f3f6fb"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 16

        Rectangle {
            Layout.preferredWidth: Math.max(260, Math.min(340, root.width * 0.28))
            Layout.fillHeight: true
            radius: 8
            color: "#ffffff"
            border.color: "#d7deea"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 12

                Text {
                    Layout.fillWidth: true
                    text: "Linceu Lighthouse"
                    color: "#0f172a"
                    font.pixelSize: 22
                    font.bold: true
                    wrapMode: Text.Wrap
                }

                Text {
                    Layout.fillWidth: true
                    text: "Documentacao avancada de arquitetura, modulos, fluxos e dependencias."
                    color: "#475569"
                    font.pixelSize: 13
                    wrapMode: Text.Wrap
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#e2e8f0"
                }

                ListView {
                    id: sectionList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    spacing: 6
                    model: root.sections

                    delegate: Button {
                        id: sectionButton
                        required property int index
                        required property var modelData

                        width: ListView.view.width
                        implicitHeight: Math.max(46, sectionTitle.implicitHeight + sectionSubtitle.implicitHeight + 22)
                        padding: 0
                        flat: true
                        onClicked: root.currentIndex = sectionButton.index

                        background: Rectangle {
                            radius: 7
                            color: root.currentIndex === sectionButton.index ? "#e0edff" : (sectionButton.hovered ? "#f1f5f9" : "transparent")
                            border.color: root.currentIndex === sectionButton.index ? "#7aa7e8" : "transparent"
                            border.width: 1
                        }

                        contentItem: Column {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 2

                            Text {
                                id: sectionTitle
                                width: parent.width
                                text: sectionButton.modelData.title
                                color: "#0f172a"
                                font.pixelSize: 14
                                font.bold: root.currentIndex === sectionButton.index
                                wrapMode: Text.Wrap
                            }

                            Text {
                                id: sectionSubtitle
                                width: parent.width
                                text: sectionButton.modelData.subtitle
                                color: "#64748b"
                                font.pixelSize: 11
                                maximumLineCount: 2
                                elide: Text.ElideRight
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: "#ffffff"
            border.color: "#d7deea"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 14

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            Layout.fillWidth: true
                            text: root.activeSection.title
                            color: "#0f172a"
                            font.pixelSize: 28
                            font.bold: true
                            wrapMode: Text.Wrap
                        }

                        Text {
                            Layout.fillWidth: true
                            text: root.activeSection.subtitle
                            color: "#475569"
                            font.pixelSize: 14
                            wrapMode: Text.Wrap
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 150
                        Layout.preferredHeight: 42
                        radius: 7
                        color: "#f8fafc"
                        border.color: "#dbe4f0"

                        Text {
                            anchors.centerIn: parent
                            text: (root.currentIndex + 1) + " / " + root.sections.length
                            color: "#334155"
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#e2e8f0"
                }

                ScrollView {
                    id: docScroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        width: docScroll.availableWidth
                        spacing: 14

                        InfoPanel {
                            Layout.fillWidth: true
                            title: "Responsabilidades"
                            items: root.activeSection.summary
                        }

                        FlowPanel {
                            Layout.fillWidth: true
                            title: "Fluxo"
                            items: root.activeSection.flow
                        }

                        InfoPanel {
                            Layout.fillWidth: true
                            title: "Arquivos e modulos"
                            items: root.activeSection.files
                            monospace: true
                        }

                        TagPanel {
                            Layout.fillWidth: true
                            title: "Dependencias e conceitos"
                            items: root.activeSection.dependencies
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            radius: 8
                            color: "#f8fafc"
                            border.color: "#e2e8f0"
                            implicitHeight: noteColumn.implicitHeight + 28

                            Column {
                                id: noteColumn
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.margins: 14
                                spacing: 8

                                Text {
                                    width: parent.width
                                    text: "Contrato arquitetural"
                                    color: "#0f172a"
                                    font.pixelSize: 16
                                    font.bold: true
                                }

                                Text {
                                    width: parent.width
                                    text: "Preserve os contratos em QVariantMap/QVariantList, atualize traducoes e notices quando novas dependencias entrarem, e mantenha operacoes pesadas fora do thread visual. Dialogs associados devem carregar a propriedade interface_principal para participar corretamente da hierarquia de janelas."
                                    color: "#475569"
                                    font.pixelSize: 13
                                    lineHeight: 1.25
                                    wrapMode: Text.Wrap
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    component InfoPanel: Rectangle {
        id: infoPanel

        property string title: ""
        property var items: []
        property bool monospace: false

        radius: 8
        color: "#ffffff"
        border.color: "#e2e8f0"
        implicitHeight: contentColumn.implicitHeight + 28

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 14
            spacing: 10

            Text {
                width: parent.width
                text: infoPanel.title
                color: "#0f172a"
                font.pixelSize: 16
                font.bold: true
                wrapMode: Text.Wrap
            }

            Repeater {
                model: infoPanel.items

                delegate: Row {
                    id: infoItem

                    required property var modelData

                    width: parent.width
                    spacing: 8

                    Rectangle {
                        width: 5
                        height: 5
                        radius: 2
                        color: "#2563eb"
                        anchors.verticalCenter: itemText.verticalCenter
                    }

                    Text {
                        id: itemText
                        width: parent.width - 16
                        text: infoItem.modelData
                        color: "#334155"
                        font.pixelSize: 13
                        font.family: infoPanel.monospace ? "Consolas" : ""
                        wrapMode: Text.Wrap
                        lineHeight: 1.2
                    }
                }
            }
        }
    }

    component TagPanel: Rectangle {
        id: tagPanel

        property string title: ""
        property var items: []

        radius: 8
        color: "#ffffff"
        border.color: "#e2e8f0"
        implicitHeight: tagColumn.implicitHeight + 28

        Column {
            id: tagColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 14
            spacing: 10

            Text {
                width: parent.width
                text: tagPanel.title
                color: "#0f172a"
                font.pixelSize: 16
                font.bold: true
            }

            Flow {
                width: parent.width
                spacing: 8

                Repeater {
                    model: tagPanel.items

                    delegate: Rectangle {
                        id: tagItem

                        required property var modelData

                        radius: 6
                        color: "#eef6ff"
                        border.color: "#c7ddf8"
                        implicitWidth: tagText.implicitWidth + 18
                        implicitHeight: tagText.implicitHeight + 10

                        Text {
                            id: tagText
                            anchors.centerIn: parent
                            text: tagItem.modelData
                            color: "#1e3a8a"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                }
            }
        }
    }

    component FlowPanel: Rectangle {
        id: flowPanel

        property string title: ""
        property var items: []

        radius: 8
        color: "#f8fafc"
        border.color: "#e2e8f0"
        implicitHeight: flowColumn.implicitHeight + 28

        Column {
            id: flowColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 14
            spacing: 12

            Text {
                width: parent.width
                text: flowPanel.title
                color: "#0f172a"
                font.pixelSize: 16
                font.bold: true
            }

            Flow {
                width: parent.width
                spacing: 8

                Repeater {
                    model: flowPanel.items

                    delegate: Row {
                        id: flowStep

                        required property int index
                        required property var modelData

                        spacing: 8

                        Rectangle {
                            radius: 6
                            color: "#ffffff"
                            border.color: "#dbe4f0"
                            implicitWidth: flowText.implicitWidth + 18
                            implicitHeight: flowText.implicitHeight + 10

                            Text {
                                id: flowText
                                anchors.centerIn: parent
                                text: flowStep.modelData
                                color: "#334155"
                                font.pixelSize: 12
                                font.family: "Consolas"
                            }
                        }

                        Text {
                            visible: flowStep.index < flowPanel.items.length - 1
                            text: ">"
                            color: "#94a3b8"
                            font.pixelSize: 15
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }
    }
}
