# Arquitetura Tecnica - Linceu Lighthouse C++/Qt6

Este documento descreve a arquitetura atual do projeto `Linceu_Lighthouse_cpp` a partir da arvore de codigo presente neste workspace. O objetivo e servir como referencia para manutencao, evolucao, auditoria de dependencias e integracao entre modulos.

## Visao Geral

O Linceu Lighthouse e uma aplicacao desktop Windows escrita em C++17 com Qt 6. A aplicacao monitora diretorios, registra eventos de arquivos, extrai metadados, aplica filtros, colore e traduz tabelas, gera estatisticas e diagramas, acrescenta analises de seguranca e disponibiliza documentacao/legal notices em multiplos idiomas.

Em alto nivel, o sistema e dividido em:

- bootstrap e ciclo de vida da aplicacao;
- janela principal `InterfaceMonitor`;
- servicos de observacao, scanner, persistencia, metadados e tabela;
- camada de seguranca forense;
- camada de UI e comandos de menu/botoes/dialogos;
- geracao de estatisticas e renderizacao de graficos;
- internacionalizacao e assets documentais;
- utilitarios transversais de caminho, icones, logging e trial.

## Dependencias Efetivas

### Linguagem, build e plataforma

- C++17 definido em `CMakeLists.txt`.
- CMake com `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- Preset principal: `qt6-mingw-debug`, usando MinGW e Qt 6 no Windows.
- Aplicacao Windows GUI criada como alvo `WIN32`.
- Recurso nativo: `source/Linceu_Lighthouse_cpp.rc`.
- Deploy pos-build com `windeployqt`, copia de `assets` e copia de arquivos `.qm`.

### Modulos Qt vinculados

O build vincula explicitamente:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`
- `Qt::Sql`
- `Qt::Xml`
- `Qt::Concurrent`
- `Qt::OpenGL`
- `Qt::OpenGLWidgets`
- `Qt::Quick`
- `Qt::Qml`

Nao ha dependencia direta dos modulos opcionais auditados anteriormente para graficos externos, visualizacao 3D, servidor HTTP, mensageria, automacao de teclado virtual, compositor Wayland ou autorizacao de rede. A renderizacao estatistica atual e interna ao projeto.

### Bibliotecas nativas Windows

Em Windows, o alvo principal tambem vincula:

- `wintrust`
- `version`
- `crypt32`

Essas bibliotecas suportam verificacao de assinaturas, leitura de metadados de executaveis e operacoes criptograficas ligadas ao subsistema de seguranca.

### Variaveis operacionais relevantes

- `LINCEU_OPENGL_MODE=auto`: comportamento padrao.
- `LINCEU_OPENGL_MODE=software`: forca OpenGL por software.
- `LINCEU_OPENGL_MODE=desktop`: solicita OpenGL desktop.
- `LINCEU_OPENGL_MODE=angle`: solicita ANGLE quando disponivel.

O bootstrap tambem habilita compartilhamento de contextos OpenGL e configura `QSurfaceFormat` com double buffering e antialiasing.

## Mapa de Diretorios

| Caminho | Papel |
| --- | --- |
| `main.cpp` | Ponto de entrada minimo. Chama `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Bootstrap principal, backend grafico, registro QML, traducao, janela principal e writer do banco. |
| `source/src_02_SplashAppStarter.*` | Fluxo alternativo com splash, barra de progresso e carregamento visual. |
| `source/gui` | Janela principal, inicializacao de UI, configuracao e internacionalizador. |
| `source/ui` | Gerenciadores de botoes, menus, estatisticas, eventos, progresso, dados, manual, dialogs e traducao de metadados. |
| `source/services` | Observacao de diretorios, scanner, eventos, banco de eventos, metadados e modelo/tabela. |
| `source/security` | Perfil de seguranca, baseline, regras, risco, atribuicao, quarentena, evidencias e dialogs de seguranca. |
| `source/data` | Geracao estatistica e camada propria de graficos/diagramas. |
| `source/tools` | Filtros, calendario e administracao de filtros. |
| `source/locale` | `.ts`, `.qm` e compilacao de traducoes. |
| `source/assets` | Icones, release notes, notices, EULA, privacy, copyright, licencas e textos de apoio. |
| `source/utils` | Caminhos, icones, logs, trial e utilitarios de aplicacao. |
| `AdvancedDocumentation.qml` | Documentacao visual navegavel da arquitetura. |
| `ARCHITECTURE.md` | Este documento de arquitetura tecnica. |

## Fluxo de Inicializacao

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> ajustar diretório de trabalho
  +--> tentar SplashAppStarter
  |      |
  |      +--> criar QApplication
  |      +--> configurar backend grafico
  |      +--> inicializar traducoes
  |      +--> exibir splash
  |      +--> criar InterfaceMonitor
  |      +--> iniciar DatabaseWriter se houver db_path
  |
  +--> fallback sem splash
         |
         +--> configurar backend grafico
         +--> QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> AppUserModelID Windows
         +--> icone da aplicacao
         +--> Internacionalizador
         +--> InterfaceMonitor
         +--> DatabaseWriter eager
         +--> show()
         +--> app.exec()
```

O bootstrap separa responsabilidades importantes:

- `ajustar_cwd()` evita falhas por caminhos relativos de assets, traducoes e banco;
- `configurar_backend_grafico()` prepara OpenGL antes da criacao de widgets;
- `source::charts_lgpl::register_qml_types()` expoe `QuickChartItem` como tipo QML;
- `source::security::pg::prepare_runtime()` prepara o runtime local de persistencia de seguranca;
- `Internacionalizador::inicializar_sistema_traducao()` instala tradutores Qt/aplicacao;
- `DatabaseWriter::get_instance(dbPath)` aquece o writer assincrono de eventos.

## Janela Principal

`source::gui::InterfaceMonitor` e o hub da aplicacao. Ela herda de `QMainWindow` e agrega ponteiros para todos os gerenciadores de alto nivel.

Responsabilidades principais:

- criar e configurar a UI;
- expor comandos `Q_INVOKABLE` para menus, botoes e dialogs;
- manter tabela, filtros, monitoramento, estatisticas, dados e seguranca integrados;
- propagar mudancas de idioma;
- inserir eventos em streaming;
- aplicar cores e visibilidade de colunas;
- coordenar hierarquia de janelas associadas;
- fechar dialogs relacionados ao encerrar.

Componentes agregados mais importantes:

- `LocalizadorQt`
- `GerenciadorTabela`
- `GerenciadorColunas`
- `GerenciadorEventosArquivo`
- `GerenciadorDados`
- `GerenciadorMonitoramento`
- `GerenciadorLimpeza`
- `GerenciadorBotoes`
- `GerenciadorBotoesUI`
- `GerenciadorMenusUI`
- `GerenciadorEventosUI`
- `GerenciadorProgressoUI`
- `GerenciadorEstatisticasUI`
- `Filtros`
- `BaseEvento`
- `Observador`
- `EventTableModel`
- `SecurityManager`

## Hierarquia de Janelas

A janela principal usa uma hierarquia de dialogs associados por propriedades Qt, especialmente `interface_principal`. Essa hierarquia inclui manual, sobre, ajuda, dialogs de cores, seletor avancado e janela de estatisticas.

O objetivo e:

- manter a GUI principal recuperavel quando o usuario alterna foco;
- ordenar dialogs associados sem prender todos como filhos diretos;
- evitar que a janela `Estatisticas` fique acima da GUI quando estiver desativada;
- fechar dialogs vinculados ao encerrar a interface.

Mecanismos usados:

- `eventFilter` para eventos de ativacao, foco, show/hide e mouse;
- propriedades dinamicas para associar dialogs a `InterfaceMonitor`;
- `raise()` e `activateWindow()` apenas quando ha intencao explicita;
- sincronizacao agendada para evitar reentrancia em eventos de janela;
- isolamento de dialogs transientes que nao devem participar da hierarquia.

## Servicos de Monitoramento e Eventos

### Observador

`source::services::Observador` encapsula o monitoramento de diretorio. Ele combina estado atomico, filas, locks, thread propria, scanner e handlers de evento.

Ele controla:

- diretorio monitorado;
- inicio, parada, pausa e reinicio;
- deteccao de operacao massiva;
- bufferizacao de eventos;
- controle de falhas consecutivas;
- estimativa de eventos perdidos;
- snapshots de registros anteriores;
- deduplicacao de eventos recentes;
- integracao com scanner inicial.

### BaseEvento

`source::services::BaseEvento` normaliza eventos e persistencia. Ela cria banco, registra eventos especificos, trata exclusoes temporarias, processa movimentacoes e atualiza a interface.

Operacoes centrais:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Modulos de servico

| Submodulo | Responsabilidade |
| --- | --- |
| `GerenciamentoBaseEvento` | Banco de eventos, registro, exclusao, movimentacao, snapshots e writer. |
| `GerenciamentoDiretorioScanner` | Scan inicial/lote, fila de arquivos, worker e atualizacao de progresso. |
| `GerenciamentoEventoModificado` | Agrupamento, intervalo dinamico, processamento normal/massivo e cache de metadados. |
| `GerenciamentoEventoMovido` | Correlacao entre exclusao/adicao, worker de movimentacao e atualizacao de tabela. |
| `GerenciamentoMetadados` | Extracao por categoria de arquivo, cache, configuracao, tabela e lotes. |
| `GerenciamentoObservador` | Inicio/parada, buffer, monitoramento, erros, pausa e reinicio. |
| `GerenciamentoTabela` | Modelo de tabela, cores, tema, colunas, cabecalhos, retraducao e processamento em background. |

## Persistencia

O projeto usa `Qt::Sql` e drivers Qt para persistencia local. O caminho principal de eventos e gerenciado por `BaseEvento` e `DatabaseWriter`. A camada de seguranca usa helpers em `secore_01_postgres_utils.*`, mas no runtime atual abre cache local com `QSQLITE` quando disponivel.

Padroes de persistencia:

- conexoes nomeadas por escopo;
- writers assincronos para reduzir bloqueio da UI;
- schema criado sob demanda;
- eventos em `QVariantMap` para manter compatibilidade com UI e estatisticas;
- snapshots e tabelas auxiliares para seguranca, baseline, quarentena, evidencias e sessoes.

## Modelo e Tabela de Eventos

`GerenciadorTabela` coordena a experiencia visual da tabela. `EventTableModel` e o modelo Qt usado para streaming de eventos.

Capacidades:

- prepend e append em lote;
- substituicao completa de eventos;
- snapshot de dados;
- ordenacao por coluna;
- cache de cores por operacao;
- cabecalhos traduzidos;
- colunas visiveis/personalizadas;
- adaptacao ao tema do Windows;
- retraducao agendada apos mudanca de idioma;
- processamento pesado em background;
- sincronizacao com `QTableView`.

## Metadados

O modulo `GerenciamentoMetadados` e amplo e especializado por tipo de arquivo.

Ele cobre:

- codigo fonte;
- imagem;
- audio;
- video;
- documento;
- planilha;
- apresentacao;
- banco de dados;
- executavel;
- temporario;
- compactados;
- backup;
- log;
- configuracao;
- OLE;
- dados estruturados;
- extracao completa;
- tipo de arquivo;
- tamanho, unidade e bytes;
- atributos, autor, dimensoes, duracao, taxa de bits e protecao.

Tambem cuida de estado/configuracao da tabela, invalidacao de cache de diretorios relacionados, fila de metadados, callbacks e atualizacao de colunas.

## Seguranca Forense

`source::security::SecurityManager` e a fachada do subsistema de seguranca. Ele centraliza contexto, schema, enriquecimento de eventos, consulta de alertas/sessoes/quarentena/baseline/evidencias e controle dos observadores de runtime.

Componentes principais:

| Arquivo | Papel |
| --- | --- |
| `base_01_baseline_criptografico.*` | Baseline criptografico, hashes e snapshots. |
| `exec_01_inteligencia_executaveis.*` | Analise de executaveis e metadados de assinatura. |
| `sess_01_correlacao_sessoes.*` | Correlacao temporal de sessoes e atividade. |
| `sdiff_01_diff_conteudo.*` | Diferencas de conteudo e comparacao de versoes. |
| `attr_01_atribuicao_forense.*` | Atribuicao/contexto forense. |
| `agre_01_motor_regras.*` | Regras e alertas. |
| `risk_01_score_risco.*` | Pontuacao de risco. |
| `quar_01_quarentena.*` | Quarentena, blobs e restauracao. |
| `evid_01_exportacao_evidencia.*` | Exportacao de pacotes de evidencia. |
| `mdir_01_multi_diretorio.*` | Perfis e multiplos diretorios monitorados. |
| `secore_05_SecurityDbWriter.*` | Writer especifico de seguranca. |

Dialogs de seguranca:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Geracao de Estatisticas

`source::data::GeradorEstatisticas` recebe `db_path`, localizador e interface. Ele registra geradores especializados derivados de `BaseGerador`.

Graficos/diagramas suportados:

- pizza de operacoes;
- barras por tipo de arquivo;
- timeline de operacoes;
- treemap por tipos;
- histograma por horarios;
- pareto;
- linha/cluster;
- boxplots por distribuicao, operacao, tipo, hora, tamanho e eventos;
- radar de eventos;
- heatmap;
- scatter;
- sankey;
- radar;
- dotplot;
- sankey por evento/caminho;
- sankey por tipo/caminho;
- arvore de diretorios.

O contrato de saida continua baseado em `QVariant`, `QVariantMap` e `QVariantList`, o que permite manter compatibilidade com a UI historica.

## Renderizacao de Graficos e Diagramas

O subsistema em `source/data/Charts` substitui dependencias externas de graficos por uma implementacao propria.

Camadas:

```text
QVariantMap legado
  |
  v
LegacyVariantChartAdapter
  |
  v
LgplChartModel
  |
  v
ChartLayoutEngine
  |
  +--> RasterChartView    (QWidget + QPainter, usado na janela Estatisticas)
  +--> WidgetChartView    (QOpenGLWidget + QPainter overlay)
  +--> QuickChartItem     (QQuickItem + Scene Graph)
```

Arquivos:

- `ch_01_ChartLayoutEngine.*`: gera geometria, labels, legendas, picks e cena.
- `ch_02_ChartWidgetFactory.*`: cria widgets a partir de datasets ou payloads legados.
- `ch_03_LegacyVariantChartAdapter.*`: adapta `QVariantMap` para modelo tipado.
- `ch_04_QmlRegistration.*`: registra `QuickChartItem` para QML.
- `ch_05_QuickChartItem.*`: backend Qt Quick Scene Graph.
- `ch_06_WidgetChartView.*`: backend `QOpenGLWidget`.
- `ch_07_LgplChartModel.h`: tipos centrais da cena.
- `ch_08_RasterChartView.*`: backend QWidget raster com antialiasing.

Decisoes importantes:

- `RasterChartView` evita instabilidade de `QOpenGLWidget` na janela `Estatisticas` em cenarios multi-monitor.
- `QPainter` e usado com antialiasing para preservar qualidade visual.
- `WidgetChartView` continua disponivel para fluxos OpenGL.
- `QuickChartItem` permite composicao QML e animacao via `transitionProgress`.
- O export de imagens usa modo deterministico, sem animacoes.

## UI e Comandos

Os gerenciadores em `source/ui` separam a janela principal em dominios menores.

| Modulo | Papel |
| --- | --- |
| `GerenciadorBotoes` | Acoes de diretorio, analise, exportacao, limpeza e pausa. |
| `GerenciadorBotoesUI` | Criacao visual dos botoes. |
| `GerenciadorMenusUI` | Menus de arquivo, configuracoes, opcoes, cores, colunas, exportacao, idiomas, sobre, manual e ajuda. |
| `GerenciadorEventosUI` | Filtros, visibilidade, cores, idioma, limpeza e reinicio. |
| `GerenciadorProgressoUI` | Barra de progresso de scan. |
| `GerenciadorEstatisticasUI` | Dialog de estatisticas, painel de selecao, geracao, regeneracao e exportacao de graficos. |
| `GerenciadorDados` | Exportacao e manipulacao de dados. |
| `GerenciadorEventosArquivo` | Integracao de eventos de arquivo com UI. |
| `GerenciadorMonitoramento` | Inicio/parada de observacao. |
| `GerenciadorLimpeza` | Limpeza de dados monitorados. |
| `DialogoCores` | Selecao de cores por operacao e seletor avancado. |
| `LocalizadorQt` | Traducoes da aplicacao. |
| `TradutorMetadadosQt` | Traducoes de operacao, tipo, atributos, autor, protecao e dimensoes. |
| `OpcoesSobre`, `Manual` | Dialogs documentais. |

## Filtros e Ferramentas

`source/tools` agrega filtros e calendario.

Componentes:

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Responsabilidades:

- aplicar/limpar filtros;
- sincronizar menu principal com filtros;
- atualizar contagem de eventos;
- responder a mudancas de idioma;
- salvar estado de checkboxes;
- verificar filtros por tipo de operacao.

## Internacionalizacao

A internacionalizacao combina:

- `source/locale/*.ts`;
- `source/locale/*.qm`;
- `compile_translations.*`;
- `Internacionalizador`;
- `LocalizadorQt`;
- `TradutorMetadadosQt`;
- documentos de manual em `source/ui/Manual`;
- assets textuais por idioma em `source/assets`.

Idiomas presentes:

- `pt_BR`
- `en_US`
- `es_ES`
- `fr_FR`
- `it_IT`
- `de_DE`

Quando o idioma muda, a interface retraduz menus, cabecalhos, filtros, estatisticas, dialogs e dados existentes quando necessario.

## Assets, Licencas e Documentacao Legal

`source/assets` e empacotado no deploy. Ele contem:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Os notices listam os modulos Qt efetivamente usados e as obrigacoes de redistribuicao. Release notes e textos legais ficam separados por idioma.

## Utilitarios Transversais

| Arquivo | Responsabilidade |
| --- | --- |
| `ApplicationPathUtils.*` | Base path, localizacao e leitura de arquivos de texto. |
| `CaminhoPersistenteUtils.*` | Caminho persistente da aplicacao. |
| `IconUtils.*` | Resolucao de icones em assets. |
| `LogManager.*` | Logger central, arquivo de log e normalizacao Unicode. |
| `TrialManager.*` | Trial, registro/configuracao e URL da versao paga. |

## Concorrencia e Responsividade

O projeto usa uma combinacao de:

- `std::thread` para monitoramento continuo;
- `QThread` para scanner;
- `QTimer` para flushes, retraducao e sincronizacao de UI;
- `QtConcurrent` para processamento auxiliar;
- writers assincronos para banco;
- caches em memoria para cores, traducoes, metadados e snapshots;
- atomics e mutexes no observador.

Cuidados arquiteturais:

- UI deve ser atualizada no thread principal do Qt;
- persistencia deve evitar bloqueio do loop de eventos;
- eventos massivos devem ser agrupados;
- renderizacao de estatisticas deve preferir `RasterChartView` na janela principal de estatisticas;
- cache deve ser invalidado quando idioma, tema, colunas ou cores mudarem.

## Fluxo de Evento de Arquivo

```text
ReadDirectoryChangesW / scanner
  |
  v
Observador
  |
  +--> buffer de eventos
  +--> deteccao de operacao massiva
  +--> handlers Adicionado/Excluido/Modificado/Renomeado/Movido
  |
  v
BaseEvento
  |
  +--> normaliza payload
  +--> extrai metadados
  +--> registra no banco
  +--> aciona SecurityManager
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> filtros / cores / traducoes
  +--> estatisticas quando solicitadas
```

## Fluxo de Estatisticas

```text
Usuario clica Estatisticas
  |
  v
GerenciadorEstatisticasUI::mostrar_estatisticas()
  |
  v
GeradorEstatisticas(db_path, loc, interface)
  |
  v
gst_*_Grafico*.cpp
  |
  v
QVariantMap / QVariantList
  |
  v
ChartWidgetFactory
  |
  +--> RasterChartView para janela Estatisticas
  +--> widgets customizados para diagramas especificos
  |
  v
Dialog Estatisticas
```

## Regras para Evolucao

### Adicionar novo grafico

1. Criar gerador em `source/data/GeradorEstatisticas`.
2. Registrar no `GeradorEstatisticas::inicializar_geradores`.
3. Garantir payload `QVariantMap` estavel.
4. Se o tipo for suportado pelo modelo comum, adaptar em `LegacyVariantChartAdapter`.
5. Se precisar geometria nova, expandir `ChartLayoutEngine`.
6. Expor no painel de selecao em `GerenciamentoEstatisticasUI`.
7. Atualizar traducoes `.ts` e documentacao.

### Adicionar nova coluna de tabela

1. Atualizar fontes de evento/metadados.
2. Registrar chave e label no gerenciador de colunas.
3. Ajustar traducao no `LocalizadorQt`/arquivos `.ts`.
4. Validar cor, visibilidade, ordenacao e exportacao.

### Adicionar novo recurso de seguranca

1. Criar componente em `source/security`.
2. Integrar em `SecurityManager`.
3. Criar schema no metodo `ensure_schema`.
4. Usar `SecurityDbWriter` para escrita assincrona quando aplicavel.
5. Expor consultas/dialogs em `secui_*` se houver UI.
6. Atualizar evidence package se o dado for auditavel.

### Adicionar novo idioma

1. Criar `.ts` e `.qm` em `source/locale`.
2. Adicionar textos em assets documentais.
3. Registrar idioma em `LocalizadorQt`.
4. Atualizar menus de idioma.
5. Validar cabecalhos, filtros, dados existentes e manual.

## Pontos de Atencao

- `InterfaceMonitor` concentra muitos ponteiros de gerenciadores; novas features devem evitar aumentar acoplamento direto quando um gerenciador existente ja possui o dominio.
- `QVariantMap` e o contrato historico entre eventos, estatisticas e UI; alterar chaves exige migracao cuidadosa.
- O caminho de graficos usa `RasterChartView` para estabilidade multi-monitor na janela `Estatisticas`.
- O CMake inclui `source/data/Charts` por `GLOB_RECURSE`; novos arquivos nessa pasta entram automaticamente no build.
- O deploy copia `source/assets` inteiro; arquivos legais e traducoes devem permanecer consistentes.
- A arvore atual possui arquivos nao versionados e modificacoes locais; esta documentacao descreve o workspace atual, nao necessariamente um commit limpo.

## Referencia Rapida de Arquivos

| Tema | Arquivos |
| --- | --- |
| Entrada | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Janela principal | `source/gui/ic_01_InterfaceMonitor.*` |
| Traducao | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Monitoramento | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Banco de eventos | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Tabela | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Metadados | `source/services/GerenciamentoMetadados/*` |
| Seguranca | `source/security/*` |
| Estatisticas | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Graficos | `source/data/Charts/*` |
| UI | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filtros | `source/tools/*` |
| Assets | `source/assets/*` |
| Utilitarios | `source/utils/*` |
