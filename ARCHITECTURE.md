<!-- Multilanguage ARCHITECTURE.md for Linceu_Lighthouse -->

<p align="center">
  <b>Selecione o idioma / Select language / Seleccione el idioma / Sélectionnez la langue / Seleziona la lingua / Sprache wählen</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> |
  <a href="#enus">🇺🇸 English (US)</a> |
  <a href="#eses">🇪🇸 Español (ES)</a> |
  <a href="#frfr">🇫🇷 Français (FR)</a> |
  <a href="#itit">🇮🇹 Italiano (IT)</a> |
  <a href="#dede">🇩🇪 Deutsch (DE)</a>
</p>

---

<h2 id="ptbr">🇧🇷 Português (BR)</h2>

<details>
<summary>Clique para expandir a Arquitetura em português</summary>

# Arquitetura Técnica - Linceu Lighthouse C++/Qt6

Este documento descreve a arquitetura atual do projeto `Linceu_Lighthouse_cpp` a partir da árvore de código presente neste workspace. O objetivo é servir como referência para manutenção, evolução, auditoria de dependências e integração entre módulos.

## Visão Geral e Proposta Arquitetural

O **Linceu Lighthouse** é uma plataforma desktop nativa para Windows escrita em C++17 com Qt 6, projetada para fornecer visibilidade absoluta, histórico estruturado, inteligência de arquivos sem IA e auditoria de segurança local.

A arquitetura é orientada ao princípio **Local-First / Sovereign Privacy**: todos os dados de eventos, metadados enriquecidos, bancos SQLite, caches e relatórios são processados e armazenados estritamente na máquina local, sem conexão externa, sem dependências de nuvem, sem telemetria e sem modelos de linguagem (LLM).

Em termos arquiteturais e funcionais, o sistema opera como uma **pipeline de dados local** dividida em 7 domínios cooperantes:

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                      PIPELINE DE DADOS - LINCEU LIGHTHOUSE              │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. DOMÍNIO DE MONITORAMENTO (APIs Nativas Windows / ReadDirectoryChangesW)
                                     ↓
 2. DOMÍNIO DE EVENTOS E CORRELAÇÃO (Normalização / Detecção Move & Rename)
                                     ↓
 3. DOMÍNIO DE METADADOS (30+ categorias, Magic Numbers, Hashes Cripto)
                                     ↓
 4. DOMÍNIO DE PERSISTÊNCIA (SQLite Local Assíncrono / Batch Writers)
                                     ↓
 5. DOMÍNIO DE ANALYTICS & ESTATÍSTICAS (17 Gráficos Nativos / Determinismo)
                                     ↓
 6. DOMÍNIO DE SEGURANÇA E AUDITORIA (Baseline, Regras, Risco, Evidências)
                                     ↓
 7. DOMÍNIO DE APRESENTAÇÃO (Interface Qt6, Internacionalização, Exportação)
```

Em alto nível, o sistema é estruturado em:

- **Bootstrap e Ciclo de Vida:** Inicialização de ambiente, configuração OpenGL, splash e fallback;
- **Janela Principal `InterfaceMonitor`:** Orquestração central e integração entre serviços e UI;
- **Serviços de Observação, Scanner e Bufferização:** Captura multithread resiliente a rajadas;
- **Normalização, Metadados e Persistência:** Enriquecimento profundo e gravação em SQLite;
- **Camada de Segurança, Integridade e Forense:** Baselines, regras de risco, quarentena e evidências;
- **Infraestrutura Gráfica e Estatísticas:** Motor interno `Charts` com 17 geradores de diagramas;
- **Internacionalização e Assets:** Suporte nativo a 6 idiomas com recarregamento dinâmico;
- **Utilitários Transversais:** Gerenciamento de caminhos, logs, ícones e integrações com o shell.

## Dependências Efetivas

### Linguagem, build e plataforma

- C++17 definido em `CMakeLists.txt`.
- CMake com `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- Preset principal: `qt6-mingw-debug`, usando MinGW e Qt 6 no Windows.
- Aplicação Windows GUI criada como alvo `WIN32`.
- Recurso nativo: `source/Linceu_Lighthouse_cpp.rc`.
- Deploy pós-build com `windeployqt`, cópia de `assets` e cópia de arquivos `.qm`.

### Módulos Qt vinculados

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

Não há dependência direta dos módulos opcionais auditados anteriormente para gráficos externos, visualização 3D, servidor HTTP, mensageria, automação de teclado virtual, compositor Wayland ou autorização de rede. A renderização estatística atual é interna ao projeto.

### Bibliotecas nativas Windows

Em Windows, o alvo principal também vincula:

- `wintrust`
- `version`
- `crypt32`

Essas bibliotecas suportam verificação de assinaturas, leitura de metadados de executáveis e operações criptográficas ligadas ao subsistema de segurança.

### Variáveis operacionais relevantes

- `LINCEU_OPENGL_MODE=auto`: comportamento padrão.
- `LINCEU_OPENGL_MODE=software`: força OpenGL por software.
- `LINCEU_OPENGL_MODE=desktop`: solicita OpenGL desktop.
- `LINCEU_OPENGL_MODE=angle`: solicita ANGLE quando disponível.

O bootstrap também habilita compartilhamento de contextos OpenGL e configura `QSurfaceFormat` com double buffering e antialiasing.

## Mapa de Diretórios

| Caminho | Papel |
| --- | --- |
| `main.cpp` | Ponto de entrada mínimo. Chama `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Bootstrap principal, backend gráfico, registro QML, tradução, janela principal e writer do banco. |
| `source/src_02_SplashAppStarter.*` | Fluxo alternativo com splash, barra de progresso e carregamento visual. |
| `source/gui` | Janela principal, inicialização de UI, configuração e internacionalizador. |
| `source/ui` | Gerenciadores de botões, menus, estatísticas, eventos, progresso, dados, manual, dialogs e tradução de metadados. |
| `source/services` | Observação de diretórios, scanner, eventos, banco de eventos, metadados e modelo/tabela. |
| `source/security` | Perfil de segurança, baseline, regras, risco, atribuição, quarentena, evidências e dialogs de segurança. |
| `source/data` | Geração estatística e camada própria de gráficos/diagramas. |
| `source/tools` | Filtros, calendário e administração de filtros. |
| `source/locale` | `.ts`, `.qm` e compilação de traduções. |
| `source/assets` | Ícones, release notes, notices, EULA, privacy, copyright, licenças e textos de apoio. |
| `source/utils` | Caminhos, ícones, logs, trial e utilitários de aplicação. |
| `AdvancedDocumentation.qml` | Documentação visual navegável da arquitetura. |
| `ARCHITECTURE.md` | Este documento de arquitetura técnica. |

## Fluxo de Inicialização

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
  |      +--> configurar backend gráfico
  |      +--> inicializar traduções
  |      +--> exibir splash
  |      +--> criar InterfaceMonitor
  |      +--> iniciar DatabaseWriter se houver db_path
  |
  +--> fallback sem splash
         |
         +--> configurar backend gráfico
         +--> QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> AppUserModelID Windows
         +--> ícone da aplicação
         +--> Internacionalizador
         +--> InterfaceMonitor
         +--> DatabaseWriter eager
         +--> show()
         +--> app.exec()
```

O bootstrap separa responsabilidades importantes:

- `ajustar_cwd()` evita falhas por caminhos relativos de assets, traduções e banco;
- `configurar_backend_grafico()` prepara OpenGL antes da criação de widgets;
- `source::charts_lgpl::register_qml_types()` expõe `QuickChartItem` como tipo QML;
- `source::security::pg::prepare_runtime()` prepara o runtime local de persistência de segurança;
- `Internacionalizador::inicializar_sistema_traducao()` instala tradutores Qt/aplicação;
- `DatabaseWriter::get_instance(dbPath)` aquece o writer assíncrono de eventos.

## Janela Principal

`source::gui::InterfaceMonitor` é o hub da aplicação. Ela herda de `QMainWindow` e agrega ponteiros para todos os gerenciadores de alto nível.

Responsabilidades principais:

- criar e configurar a UI;
- expor comandos `Q_INVOKABLE` para menus, botões e dialogs;
- manter tabela, filtros, monitoramento, estatísticas, dados e segurança integrados;
- propagar mudanças de idioma;
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

A janela principal usa uma hierarquia de dialogs associados por propriedades Qt, especialmente `interface_principal`. Essa hierarquia inclui manual, sobre, ajuda, dialogs de cores, seletor avançado e janela de estatísticas.

O objetivo é:

- manter a GUI principal recuperável quando o usuário alterna foco;
- ordenar dialogs associados sem prender todos como filhos diretos;
- evitar que a janela `Estatisticas` fique acima da GUI quando estiver desativada;
- fechar dialogs vinculados ao encerrar a interface.

Mecanismos usados:

- `eventFilter` para eventos de ativação, foco, show/hide e mouse;
- propriedades dinâmicas para associar dialogs a `InterfaceMonitor`;
- `raise()` e `activateWindow()` apenas quando há intenção explícita;
- sincronização agendada para evitar reentrância em eventos de janela;
- isolamento de dialogs transientes que não devem participar da hierarquia.

## Serviços de Monitoramento e Eventos

### Observador

`source::services::Observador` encapsula o monitoramento de diretório. Ele combina estado atômico, filas, locks, thread própria, scanner e handlers de evento.

Ele controla:

- diretório monitorado;
- início, parada, pausa e reinício;
- detecção de operação massiva;
- bufferização de eventos;
- controle de falhas consecutivas;
- estimativa de eventos perdidos;
- snapshots de registros anteriores;
- deduplicação de eventos recentes;
- integração com scanner inicial.

### BaseEvento

`source::services::BaseEvento` normaliza eventos e persistência. Ela cria banco, registra eventos específicos, trata exclusões temporárias, processa movimentações e atualiza a interface.

Operações centrais:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Módulos de serviço

| Submódulo | Responsabilidade |
| --- | --- |
| `GerenciamentoBaseEvento` | Banco de eventos, registro, exclusão, movimentação, snapshots e writer. |
| `GerenciamentoDiretorioScanner` | Scan inicial/lote, fila de arquivos, worker e atualização de progresso. |
| `GerenciamentoEventoModificado` | Agrupamento, intervalo dinâmico, processamento normal/massivo e cache de metadados. |
| `GerenciamentoEventoMovido` | Correlação entre exclusão/adição, worker de movimentação e atualização de tabela. |
| `GerenciamentoMetadados` | Extração por categoria de arquivo, cache, configuração, tabela e lotes. |
| `GerenciamentoObservador` | Início/parada, buffer, monitoramento, erros, pausa e reinício. |
| `GerenciamentoTabela` | Modelo de tabela, cores, tema, colunas, cabeçalhos, retradução e processamento em background. |

## Persistência

O projeto usa `Qt::Sql` e drivers Qt para persistência local. O caminho principal de eventos é gerenciado por `BaseEvento` e `DatabaseWriter`. A camada de segurança usa helpers em `secore_01_postgres_utils.*`, mas no runtime atual abre cache local com `QSQLITE` quando disponível.

Padrões de persistência:

- conexões nomeadas por escopo;
- writers assíncronos para reduzir bloqueio da UI;
- schema criado sob demanda;
- eventos em `QVariantMap` para manter compatibilidade com UI e estatísticas;
- snapshots e tabelas auxiliares para segurança, baseline, quarentena, evidências e sessões.

## Modelo e Tabela de Eventos

`GerenciadorTabela` coordena a experiência visual da tabela. `EventTableModel` é o modelo Qt usado para streaming de eventos.

Capacidades:

- prepend e append em lote;
- substituição completa de eventos;
- snapshot de dados;
- ordenação por coluna;
- cache de cores por operação;
- cabeçalhos traduzidos;
- colunas visíveis/personalizadas;
- adaptação ao tema do Windows;
- retradução agendada após mudança de idioma;
- processamento pesado em background;
- sincronização com `QTableView`.

## Metadados

O módulo `GerenciamentoMetadados` é amplo e especializado por tipo de arquivo.

Ele cobre:

- código fonte;
- imagem;
- áudio;
- vídeo;
- documento;
- planilha;
- apresentação;
- banco de dados;
- executável;
- temporário;
- compactados;
- backup;
- log;
- configuração;
- OLE;
- dados estruturados;
- extração completa;
- tipo de arquivo;
- tamanho, unidade e bytes;
- atributos, autor, dimensões, duração, taxa de bits e proteção.

Também cuida de estado/configuração da tabela, invalidação de cache de diretórios relacionados, fila de metadados, callbacks e atualização de colunas.

## Segurança Forense

`source::security::SecurityManager` é a fachada do subsistema de segurança. Ele centraliza contexto, schema, enriquecimento de eventos, consulta de alertas/sessões/quarentena/baseline/evidências e controle dos observadores de runtime.

Componentes principais:

| Arquivo | Papel |
| --- | --- |
| `base_01_baseline_criptografico.*` | Baseline criptográfico, hashes e snapshots. |
| `exec_01_inteligencia_executaveis.*` | Análise de executáveis e metadados de assinatura. |
| `sess_01_correlacao_sessoes.*` | Correlação temporal de sessões e atividade. |
| `sdiff_01_diff_conteudo.*` | Diferenças de conteúdo e comparação de versões. |
| `attr_01_atribuicao_forense.*` | Atribuição/contexto forense. |
| `agre_01_motor_regras.*` | Regras e alertas. |
| `risk_01_score_risco.*` | Pontuação de risco. |
| `quar_01_quarentena.*` | Quarentena, blobs e restauração. |
| `evid_01_exportacao_evidencia.*` | Exportação de pacotes de evidência. |
| `mdir_01_multi_diretorio.*` | Perfis e múltiplos diretórios monitorados. |
| `secore_05_SecurityDbWriter.*` | Writer específico de segurança. |

Diálogos de segurança:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Geração de Estatísticas

`source::data::GeradorEstatisticas` recebe `db_path`, localizador e interface. Ele registra geradores especializados derivados de `BaseGerador`.

Gráficos/diagramas suportados:

- pizza de operações;
- barras por tipo de arquivo;
- timeline de operações;
- treemap por tipos;
- histograma por horários;
- pareto;
- linha/cluster;
- boxplots por distribuição, operação, tipo, hora, tamanho e eventos;
- radar de eventos;
- heatmap;
- scatter;
- sankey;
- radar;
- dotplot;
- sankey por evento/caminho;
- sankey por tipo/caminho;
- árvore de diretórios.

O contrato de saída continua baseado em `QVariant`, `QVariantMap` e `QVariantList`, o que permite manter compatibilidade com a UI histórica.

## Renderização de Gráficos e Diagramas

O subsistema em `source/data/Charts` substitui dependências externas de gráficos por uma implementação própria.

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
  +--> RasterChartView    (QWidget + QPainter, usado na janela Estatísticas)
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

Decisões importantes:

- `RasterChartView` evita instabilidade de `QOpenGLWidget` na janela `Estatísticas` em cenários multi-monitor.
- `QPainter` é usado com antialiasing para preservar qualidade visual.
- `WidgetChartView` continua disponível para fluxos OpenGL.
- `QuickChartItem` permite composição QML e animação via `transitionProgress`.
- O export de imagens usa modo determinístico, sem animações.

## UI e Comandos

Os gerenciadores em `source/ui` separam a janela principal em domínios menores.

| Módulo | Papel |
| --- | --- |
| `GerenciadorBotoes` | Ações de diretório, análise, exportação, limpeza e pausa. |
| `GerenciadorBotoesUI` | Criação visual dos botões. |
| `GerenciadorMenusUI` | Menus de arquivo, configurações, opções, cores, colunas, exportação, idiomas, sobre, manual e ajuda. |
| `GerenciadorEventosUI` | Filtros, visibilidade, cores, idioma, limpeza e reinício. |
| `GerenciadorProgressoUI` | Barra de progresso de scan. |
| `GerenciadorEstatisticasUI` | Dialog de estatísticas, painel de seleção, geração, regeneração e exportação de gráficos. |
| `GerenciadorDados` | Exportação e manipulação de dados. |
| `GerenciadorEventosArquivo` | Integração de eventos de arquivo com UI. |
| `GerenciadorMonitoramento` | Início/parada de observação. |
| `GerenciadorLimpeza` | Limpeza de dados monitorados. |
| `DialogoCores` | Seleção de cores por operação e seletor avançado. |
| `LocalizadorQt` | Traduções da aplicação. |
| `TradutorMetadadosQt` | Traduções de operação, tipo, atributos, autor, proteção e dimensões. |
| `OpcoesSobre`, `Manual` | Dialogs documentais. |

## Filtros e Ferramentas

`source/tools` agrega filtros e calendário.

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
- responder a mudanças de idioma;
- salvar estado de checkboxes;
- verificar filtros por tipo de operação.

## Internacionalização

A internacionalização combina:

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

Quando o idioma muda, a interface retraduz menus, cabeçalhos, filtros, estatísticas, dialogs e dados existentes quando necessário.

## Assets, Licenças e Documentação Legal

`source/assets` é empacotado no deploy. Ele contém:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Os notices listam os módulos Qt efetivamente usados e as obrigações de redistribuição. Release notes e textos legais ficam separados por idioma.

## Utilitários Transversais

| Arquivo | Responsabilidade |
| --- | --- |
| `ApplicationPathUtils.*` | Base path, localização e leitura de arquivos de texto. |
| `CaminhoPersistenteUtils.*` | Caminho persistente da aplicação. |
| `IconUtils.*` | Resolução de ícones em assets. |
| `LogManager.*` | Logger central, arquivo de log e normalização Unicode. |
| `TrialManager.*` | Trial, registro/configuração e URL da versão paga. |

## Concorrência e Responsividade

O projeto usa uma combinação de:

- `std::thread` para monitoramento contínuo;
- `QThread` para scanner;
- `QTimer` para flushes, retradução e sincronização de UI;
- `QtConcurrent` para processamento auxiliar;
- writers assíncronos para banco;
- caches em memória para cores, traduções, metadados e snapshots;
- atomics e mutexes no observador.

Cuidados arquiteturais:

- UI deve ser atualizada no thread principal do Qt;
- persistência deve evitar bloqueio do loop de eventos;
- eventos massivos devem ser agrupados;
- renderização de estatísticas deve preferir `RasterChartView` na janela principal de estatísticas;
- cache deve ser invalidado quando idioma, tema, colunas ou cores mudarem.

## Fluxo de Evento de Arquivo

```text
ReadDirectoryChangesW / scanner
  |
  v
Observador
  |
  +--> buffer de eventos
  +--> detecção de operação massiva
  +--> handlers Adicionado/Excluído/Modificado/Renomeado/Movido
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
  +--> filtros / cores / traduções
  +--> estatísticas quando solicitadas
```

## Fluxo de Estatísticas

```text
Usuário clica Estatísticas
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
  +--> RasterChartView para janela Estatísticas
  +--> widgets customizados para diagramas específicos
  |
  v
Dialog Estatísticas
```

## Regras para Evolução

### Adicionar novo gráfico

1. Criar gerador em `source/data/GeradorEstatisticas`.
2. Registrar no `GeradorEstatisticas::inicializar_geradores`.
3. Garantir payload `QVariantMap` estável.
4. Se o tipo for suportado pelo modelo comum, adaptar em `LegacyVariantChartAdapter`.
5. Se precisar geometria nova, expandir `ChartLayoutEngine`.
6. Expor no painel de seleção em `GerenciamentoEstatisticasUI`.
7. Atualizar traduções `.ts` e documentação.

### Adicionar nova coluna de tabela

1. Atualizar fontes de evento/metadados.
2. Registrar chave e label no gerenciador de colunas.
3. Ajustar tradução no `LocalizadorQt`/arquivos `.ts`.
4. Validar cor, visibilidade, ordenação e exportação.

### Adicionar novo recurso de segurança

1. Criar componente em `source/security`.
2. Integrar em `SecurityManager`.
3. Criar schema no método `ensure_schema`.
4. Usar `SecurityDbWriter` para escrita assíncrona quando aplicável.
5. Expor consultas/dialogs em `secui_*` se houver UI.
6. Atualizar evidence package se o dado for auditável.

### Adicionar novo idioma

1. Criar `.ts` e `.qm` em `source/locale`.
2. Adicionar textos em assets documentais.
3. Registrar idioma em `LocalizadorQt`.
4. Atualizar menus de idioma.
5. Validar cabeçalhos, filtros, dados existentes e manual.

## Pontos de Atenção

- `InterfaceMonitor` concentra muitos ponteiros de gerenciadores; novas features devem evitar aumentar acoplamento direto quando um gerenciador existente já possui o domínio.
- `QVariantMap` é o contrato histórico entre eventos, estatísticas e UI; alterar chaves exige migração cuidadosa.
- O caminho de gráficos usa `RasterChartView` para estabilidade multi-monitor na janela `Estatísticas`.
- O CMake inclui `source/data/Charts` por `GLOB_RECURSE`; novos arquivos nessa pasta entram automaticamente no build.
- O deploy copia `source/assets` inteiro; arquivos legais e traduções devem permanecer consistentes.
- A árvore atual possui arquivos não versionados e modificações locais; esta documentação descreve o workspace atual, não necessariamente um commit limpo.

## Referência Rápida de Arquivos

| Tema | Arquivos |
| --- | --- |
| Entrada | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Janela principal | `source/gui/ic_01_InterfaceMonitor.*` |
| Tradução | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Monitoramento | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Banco de eventos | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Tabela | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Metadados | `source/services/GerenciamentoMetadados/*` |
| Segurança | `source/security/*` |
| Estatísticas | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Gráficos | `source/data/Charts/*` |
| UI | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filtros | `source/tools/*` |
| Assets | `source/assets/*` |
| Utilitários | `source/utils/*` |

</details>

---

<h2 id="enus">🇺🇸 English (US)</h2>

<details>
<summary>Click to expand the Architecture in English</summary>

# Technical Architecture - Linceu Lighthouse C++/Qt6

This document describes the current architecture of the `Linceu_Lighthouse_cpp` project based on the codebase tree present in this workspace. Its purpose is to serve as a reference for maintenance, feature development, dependency auditing, and inter-module integration.

## Overview and Architectural Vision

**Linceu Lighthouse** is a native Windows desktop platform written in C++17 with Qt 6, designed to deliver absolute visibility, structured history, non-AI file intelligence, and local security auditing.

The architecture is guided by the **Local-First / Sovereign Privacy** principle: all event data, enriched metadata, SQLite databases, caches, and reports are processed and stored strictly on the local machine, with no external connections, no cloud dependencies, no telemetry, and no Large Language Models (LLMs).

Architecturally and functionally, the system operates as a **local data pipeline** divided into 7 cooperating domains:

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                      DATA PIPELINE - LINCEU LIGHTHOUSE                  │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. MONITORING DOMAIN (Native Windows APIs / ReadDirectoryChangesW)
                                     ↓
 2. EVENTS & CORRELATION DOMAIN (Normalization / Move & Rename Detection)
                                     ↓
 3. METADATA DOMAIN (30+ categories, Magic Numbers, Crypto Hashes)
                                     ↓
 4. PERSISTENCE DOMAIN (Asynchronous Local SQLite / Batch Writers)
                                     ↓
 5. ANALYTICS & STATISTICS DOMAIN (17 Native Charts / Determinism)
                                     ↓
 6. SECURITY & AUDIT DOMAIN (Baseline, Rules, Risk, Evidences)
                                     ↓
 7. PRESENTATION DOMAIN (Qt6 Interface, Internationalization, Export)
```

At a high level, the system is structured into:

- **Bootstrap and Lifecycle:** Environment initialization, OpenGL configuration, splash screen, and fallback;
- **Main Window `InterfaceMonitor`:** Central orchestration and integration between background services and UI;
- **Observer, Scanner, and Buffering Services:** Multi-threaded capture resilient to event bursts;
- **Normalization, Metadata, and Persistence:** Deep enrichment and batch persistence to SQLite;
- **Security, Integrity, and Forensics Layer:** Baselines, risk scoring rules, quarantine, and auditable evidences;
- **Graphics Infrastructure and Statistics:** Internal `Charts` rendering engine featuring 17 diagram generators;
- **Internationalization and Assets:** Native support for 6 languages with dynamic live reloading;
- **Cross-Cutting Utilities:** Path management, logging, icon resolution, and Windows shell integration.

## Effective Dependencies

### Language, build, and platform

- C++17 specified in `CMakeLists.txt`.
- CMake with `CMAKE_AUTOMOC` and `CMAKE_AUTORCC`.
- Main preset: `qt6-mingw-debug`, utilizing MinGW and Qt 6 on Windows.
- Windows GUI application created as a `WIN32` target.
- Native resource file: `source/Linceu_Lighthouse_cpp.rc`.
- Post-build deployment with `windeployqt`, `assets` copying, and compiled `.qm` translation distribution.

### Linked Qt Modules

The build explicitly links:

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

There is no direct runtime dependency on external graphing libraries, 3D visualization toolkits, HTTP servers, messaging brokers, virtual keyboards, Wayland compositors, or network authorization frameworks. All statistical rendering is entirely native and internal to the codebase.

### Native Windows Libraries

On Windows, the primary build target also links:

- `wintrust`
- `version`
- `crypt32`

These libraries support digital signature verification, executable metadata extraction, and cryptographic hashing operations within the security subsystem.

### Relevant Operational Environment Variables

- `LINCEU_OPENGL_MODE=auto`: default behavior.
- `LINCEU_OPENGL_MODE=software`: forces software OpenGL rendering.
- `LINCEU_OPENGL_MODE=desktop`: requests desktop OpenGL context.
- `LINCEU_OPENGL_MODE=angle`: requests ANGLE translation layer when available.

The bootstrap routine also enables OpenGL context sharing and configures `QSurfaceFormat` with double buffering and multi-sample antialiasing.

## Directory Map

| Path | Role |
| --- | --- |
| `main.cpp` | Minimal entry point. Invokes `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Primary bootstrap, graphics backend setup, QML registration, translations, main window, and database writer. |
| `source/src_02_SplashAppStarter.*` | Alternative visual startup pipeline with animated splash, progress reporting, and asset preload. |
| `source/gui` | Main application window, UI initialization, geometry management, and internationalizer. |
| `source/ui` | Managers for toolbar buttons, menu bars, statistics dialogs, UI events, progress bars, data operations, manuals, dialogs, and metadata translation. |
| `source/services` | Directory observation, scanner worker, event dispatching, event database management, metadata extraction, and table model. |
| `source/security` | Security profiles, baseline snapshots, compliance rules, risk scores, forensic attribution, quarantine, evidence exports, and security dialogs. |
| `source/data` | Statistical report generation and custom internal chart/diagram engine. |
| `source/tools` | Filtering subsystem, calendar manager, and filter administrative controls. |
| `source/locale` | Translation source files (`.ts`), binary catalogs (`.qm`), and translation compilation scripts. |
| `source/assets` | Application icons, release notes, third-party notices, EULA, privacy policies, copyright notices, licenses, and support documents. |
| `source/utils` | Path resolvers, icon loaders, centralized logging, trial enforcement, and general helpers. |
| `AdvancedDocumentation.qml` | Interactive browsable visual architecture documentation. |
| `ARCHITECTURE.md` | This technical architecture specification document. |

## Initialization Flow

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> adjust current working directory (cwd)
  +--> attempt SplashAppStarter
  |      |
  |      +--> instantiate QApplication
  |      +--> configure graphics backend
  |      +--> initialize translation system
  |      +--> display splash screen
  |      +--> instantiate InterfaceMonitor
  |      +--> initialize DatabaseWriter if db_path exists
  |
  +--> fallback headless/splashless bootstrap
         |
         +--> configure graphics backend
         +--> instantiate QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> set Windows AppUserModelID
         +--> set application window icon
         +--> initialize Internacionalizador
         +--> instantiate InterfaceMonitor
         +--> initialize DatabaseWriter eager instance
         +--> show()
         +--> app.exec()
```

The bootstrap cleanly decouples key responsibilities:

- `ajustar_cwd()` prevents runtime failures caused by relative paths to assets, translations, or database files;
- `configurar_backend_grafico()` configures OpenGL surface formats before any Qt widgets are allocated;
- `source::charts_lgpl::register_qml_types()` registers `QuickChartItem` as an instantiable QML component;
- `source::security::pg::prepare_runtime()` configures the local security storage persistence subsystem;
- `Internacionalizador::inicializar_sistema_traducao()` installs core Qt and application translation catalogs;
- `DatabaseWriter::get_instance(dbPath)` warms up the asynchronous, non-blocking SQLite worker thread.

## Main Window

`source::gui::InterfaceMonitor` serves as the central hub of the application. Inheriting from `QMainWindow`, it aggregates pointers to all high-level subsystem managers.

Key responsibilities:

- Create and configure the user interface layout;
- Expose `Q_INVOKABLE` slots for menus, toolbars, and contextual actions;
- Maintain real-time synchronization between the event table, active filters, monitoring state, statistics dialogs, and security manager;
- Propagate dynamic language switches across all visual controls;
- Ingest streaming file events smoothly;
- Apply customized color themes, row highlights, and column visibility preferences;
- Coordinate the window activation and z-order hierarchy of auxiliary dialogs;
- Safely clean up and close dependent windows on application exit.

Key aggregated components:

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

## Window Hierarchy and Management

The main window orchestrates auxiliary dialogs via dynamic Qt properties, notably `interface_principal`. This hierarchy covers the user manual, about box, help dialog, color picker, advanced filter selector, and the statistics window.

Objectives:

- Keep the main window easily restorable when users cycle focus between desktop windows;
- Properly stack associated dialogs without forcibly making every dialog a modal or rigid child;
- Prevent the `Estatisticas` window from obscuring the main window when deactivated;
- Automatically dismiss all associated child dialogs upon main window closure.

Employed mechanisms:

- Custom `eventFilter` monitoring activation, focus, show/hide, and mouse interactions;
- Dynamic Qt property binding to associate child dialogs with `InterfaceMonitor`;
- Controlled invocation of `raise()` and `activateWindow()` only on explicit user interaction;
- Scheduled deferred synchronization to avoid event recursion during window state changes;
- Clean isolation of transient, independent modal dialogs that do not belong to the hierarchy.

## Monitoring and Event Services

### Observador (Observer)

`source::services::Observador` encapsulates background file system monitoring. It combines atomic state flags, thread-safe queues, mutex locks, a dedicated worker thread, a folder scanner, and specialized event dispatchers.

It manages:

- Monitored root path and subtree filtering;
- Start, stop, pause, and resume lifecycles;
- High-volume burst detection (massive operation throttling);
- Thread-safe event queue buffering;
- Consecutive OS buffer overrun tracking;
- Dropped event estimation;
- Baseline registry snapshots;
- Deduplication of rapid recurring events;
- Handshake and handoff with the initial folder scanner.

### BaseEvento (Base Event Manager)

`source::services::BaseEvento` normalizes OS-level notifications and coordinates data persistence. It creates the database schema, records distinct file events, handles transient deletions, correlates file moves, and updates the table model.

Core operations:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Service Submodules

| Submodule | Responsibility |
| --- | --- |
| `GerenciamentoBaseEvento` | Event database lifecycle, insertion, deletion handling, move processing, snapshots, and async writer interface. |
| `GerenciamentoDiretorioScanner` | Initial/batch directory scanning, file queue ingestion, worker thread, and progress notification. |
| `GerenciamentoEventoModificado` | Event grouping, dynamic time-window debounce, normal vs. massive modification handling, and metadata cache. |
| `GerenciamentoEventoMovido` | Correlation between deletion and addition events, move worker execution, and table updates. |
| `GerenciamentoMetadados` | Category-specific metadata extraction, memory caching, table formatting, and bulk processing. |
| `GerenciamentoObservador` | Observer lifecycle (start/stop/pause/resume), buffer management, OS error handling, and recovery. |
| `GerenciamentoTabela` | Qt table model integration, color highlighting, theme synchronization, column headers, retranslation, and background workers. |

## Persistence

The project utilizes `Qt::Sql` and Qt database drivers for all local data persistence. The primary event pipeline is managed by `BaseEvento` and `DatabaseWriter`. The security layer includes helpers in `secore_01_postgres_utils.*`, while the current runtime defaults to a local `QSQLITE` database backend.

Persistence design patterns:

- Scoped, uniquely named database connections per thread;
- Asynchronous batch writers to avoid blocking the main UI thread;
- Schema auto-creation on demand;
- In-memory event representation via `QVariantMap` for seamless compatibility with Qt models, views, and analytics;
- Dedicated auxiliary tables and snapshots for baseline checks, security rules, quarantine, evidence packages, and audit sessions.

## Event Table and Data Model

`GerenciadorTabela` coordinates the visual presentation of the audit log. `EventTableModel` is the custom Qt model that feeds file events into the `QTableView`.

Key capabilities:

- High-throughput batch prepending and appending;
- Complete event set replacement;
- Instant data snapshot generation;
- Multi-column sorting;
- Operation-specific color caching;
- Dynamically localized column headers;
- Configurable column visibility and custom layouts;
- Native integration with Windows light and dark theme palettes;
- Deferred retranslation on runtime language switches;
- Asynchronous offloading of expensive formatting computations;
- Bidirectional synchronization with `QTableView`.

## Metadata Extraction

The `GerenciamentoMetadados` subsystem is comprehensive and specializes across distinct file categories.

Supported file types and domains:

- Source code;
- Images;
- Audio streams;
- Video files;
- Documents (PDF, Office);
- Spreadsheets;
- Presentations;
- Databases;
- Executables and PE binaries;
- Temporary files;
- Compressed archives;
- Backup files;
- System logs;
- Configuration files;
- Compound OLE storage;
- Structured data formats (JSON, XML, YAML);
- Deep metadata extraction;
- Content-based MIME categorization;
- File size, unit conversions, and exact byte counts;
- File attributes, author, dimensions, duration, bitrate, and security permissions.

It also governs table column configurations, cache invalidation for updated directories, metadata extraction queues, worker callbacks, and real-time column refreshing.

## Forensic Security Subsystem

`source::security::SecurityManager` acts as the facade for the local security subsystem. It centralizes security contexts, database schemas, event enrichment, alert queries, session correlation, quarantine management, baseline integrity checks, and runtime security observers.

Core components:

| File | Role |
| --- | --- |
| `base_01_baseline_criptografico.*` | Cryptographic baseline, SHA-256 hashes, and folder snapshot verification. |
| `exec_01_inteligencia_executaveis.*` | Executable binary analysis, PE headers, and digital signature metadata. |
| `sess_01_correlacao_sessoes.*` | Temporal correlation of user sessions, bursts, and system activity. |
| `sdiff_01_diff_conteudo.*` | Content-level diffing and version comparison. |
| `attr_01_atribuicao_forense.*` | Forensic attribution, user context, and process tracking. |
| `agre_01_motor_regras.*` | Local compliance and security rules engine. |
| `risk_01_score_risco.*` | Deterministic risk score calculation engine. |
| `quar_01_quarentena.*` | Local quarantine storage, encrypted blob management, and file restoration. |
| `evid_01_exportacao_evidencia.*` | Auditable evidence package bundling and export. |
| `mdir_01_multi_diretorio.*` | Multi-directory monitoring profiles and watch paths. |
| `secore_05_SecurityDbWriter.*` | Dedicated asynchronous security database writer. |

Security Dialogs:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Statistics and Analytics Generation

`source::data::GeradorEstatisticas` receives `db_path`, a localization manager, and the main window interface. It registers specialized generators inheriting from `BaseGerador`.

Supported charts and diagrams:

- Operation breakdown pie chart;
- Bar chart by file category;
- Operation timeline;
- Category treemap;
- Time-of-day histogram;
- Pareto distribution curve;
- Cluster and trend line charts;
- Boxplots (distribution by operation, type, hour, size, and event volume);
- Event radar chart;
- Temporal activity heatmap;
- Scatter plot;
- Sankey diagram;
- Dimensional radar;
- Dotplot;
- Event vs. Path Sankey diagram;
- Type vs. Path Sankey diagram;
- Hierarchical directory tree diagram.

All output payloads maintain strict compatibility with `QVariant`, `QVariantMap`, and `QVariantList`, ensuring seamless interoperability with legacy and modern UI layers.

## Chart and Diagram Rendering Engine

The internal subsystem in `source/data/Charts` replaces all external third-party charting libraries with a native, high-performance C++/Qt rendering pipeline.

Rendering layers:

```text
Legacy QVariantMap
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
  +--> RasterChartView    (QWidget + QPainter, utilized in the Statistics dialog)
  +--> WidgetChartView    (QOpenGLWidget + QPainter overlay)
  +--> QuickChartItem     (QQuickItem + Qt Scene Graph)
```

Key files:

- `ch_01_ChartLayoutEngine.*`: Computes 2D/3D geometry, labels, legends, hit-testing picks, and scene primitives.
- `ch_02_ChartWidgetFactory.*`: Instantiates visual chart widgets from structured datasets or legacy payloads.
- `ch_03_LegacyVariantChartAdapter.*`: Bridges untyped `QVariantMap` payloads to strongly typed chart models.
- `ch_04_QmlRegistration.*`: Registers `QuickChartItem` into the QML engine.
- `ch_05_QuickChartItem.*`: Qt Quick Scene Graph backend for high-speed hardware-accelerated animations.
- `ch_06_WidgetChartView.*`: Hardware-accelerated `QOpenGLWidget` canvas.
- `ch_07_LgplChartModel.h`: Fundamental scene, geometry, color, and data structures.
- `ch_08_RasterChartView.*`: Pure raster `QWidget` canvas with high-quality antialiasing.

Design decisions:

- `RasterChartView` is prioritized for the primary `Estatísticas` dialog to eliminate OpenGL context instability across multi-monitor configurations;
- `QPainter` with antialiasing preserves crisp vector typography and geometry;
- `WidgetChartView` remains available for dedicated OpenGL rendering pipelines;
- `QuickChartItem` allows dynamic QML integration with smooth `transitionProgress` interpolation;
- Image export utilizes a fully deterministic rendering pass, bypassing transient animation frames.

## UI Subsystems and Commands

Managers in `source/ui` segment the main window into modular, maintainable domains.

| Module | Role |
| --- | --- |
| `GerenciadorBotoes` | Handles directory selection, analysis, export, clear, and pause actions. |
| `GerenciadorBotoesUI` | Visual construction and layout of toolbar buttons. |
| `GerenciadorMenusUI` | Menu bar construction (File, Settings, Options, Colors, Columns, Export, Languages, About, Manual, Help). |
| `GerenciadorEventosUI` | Filter handling, column visibility toggles, color triggers, language switches, and reset actions. |
| `GerenciadorProgressoUI` | Scan progress bar and stage notification controls. |
| `GerenciadorEstatisticasUI` | Statistics dialog controller, chart selector panel, generation, live refresh, and image export. |
| `GerenciadorDados` | Multi-format data export and table manipulation. |
| `GerenciadorEventosArquivo` | Dispatches file system events into the UI pipeline. |
| `GerenciadorMonitoramento` | Controls observer start, pause, resume, and stop operations. |
| `GerenciadorLimpeza` | Clears monitored table data, caches, and working sets. |
| `DialogoCores` | Operation color mapping dialog and advanced palette picker. |
| `LocalizadorQt` | Centralized application translation catalog manager. |
| `TradutorMetadadosQt` | Dynamic translation of operations, MIME types, file attributes, author strings, and permissions. |
| `OpcoesSobre`, `Manual` | Documentation and about dialogs. |

## Filters and Tools Subsystem

`source/tools` bundles filtering capabilities and calendar integration.

Components:

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Key responsibilities:

- Apply and clear active table filters;
- Synchronize main menu toggle states with filter parameters;
- Update matching event counters in real time;
- Respond dynamically to UI language changes;
- Persist checkbox and custom criteria states across sessions;
- Verify and filter events by operation category, date range, or file extension.

## Internationalization (i18n)

The internationalization architecture combines:

- Translation source files: `source/locale/*.ts`;
- Compiled binary translation files: `source/locale/*.qm`;
- Automated compilation tooling: `compile_translations.*`;
- `Internacionalizador` class;
- `LocalizadorQt` class;
- `TradutorMetadadosQt` class;
- Localized markdown manuals in `source/ui/Manual`;
- Localized textual assets in `source/assets`.

Supported languages:

- `pt_BR` (Portuguese - Brazil)
- `en_US` (English - US)
- `es_ES` (Spanish - Spain)
- `fr_FR` (French - France)
- `it_IT` (Italian - Italy)
- `de_DE` (German - Germany)

When the language changes at runtime, the application dynamically retranslates menus, column headers, filter options, active statistics, modal dialogs, and existing table rows.

## Assets, Licensing, and Legal Documentation

The `source/assets` folder is bundled during the deployment step. It contains:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

The notices enumerate all active Qt modules and their respective open-source redistribution terms. Release notes and legal policies are stored separately for each supported language.

## Cross-Cutting Utilities

| File | Responsibility |
| --- | --- |
| `ApplicationPathUtils.*` | Base path resolution, asset discovery, and localized text loading. |
| `CaminhoPersistenteUtils.*` | Resolves standard persistent application paths (`%LOCALAPPDATA%`). |
| `IconUtils.*` | High-DPI icon resolution and asset caching. |
| `LogManager.*` | Centralized thread-safe logger, timestamped log rotation, and Unicode normalization. |
| `TrialManager.*` | Trial tracking, validation, licensing checks, and store link routing. |

## Concurrency and UI Responsiveness

The project achieves high responsiveness through a multi-threaded architecture:

- `std::thread` for low-level continuous file system monitoring;
- `QThread` for background recursive folder scanning;
- `QTimer` for debounced flushes, UI synchronization, and deferred retranslations;
- `QtConcurrent` for auxiliary off-thread processing;
- Dedicated asynchronous database writers;
- Fast in-memory caches for colors, translated strings, metadata, and snapshots;
- Lock-free atomic flags and mutex protection in observer queues.

Architectural guidelines:

- UI mutations must strictly execute on the Qt main GUI thread;
- Database persistence must never block the event loop;
- Massive file modifications must be debounced and batched;
- Chart rendering must prefer `RasterChartView` inside the main statistics dialog;
- Caches must be invalidated whenever language, theme, column layout, or color preferences change.

## File Event Pipeline

```text
ReadDirectoryChangesW / scanner
  |
  v
Observador
  |
  +--> event buffer
  +--> high-volume burst detection
  +--> handlers: Added / Deleted / Modified / Renamed / Moved
  |
  v
BaseEvento
  |
  +--> normalize payload
  +--> extract metadata
  +--> write to SQLite database
  +--> notify SecurityManager
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> filters / colors / translations
  +--> statistics when requested
```

## Statistics Pipeline

```text
User clicks Statistics
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
  +--> RasterChartView for Statistics dialog
  +--> specialized custom widgets for specific diagrams
  |
  v
Statistics Dialog
```

## Architectural Guidelines for Extension

### Adding a New Chart

1. Implement a specialized generator inheriting from `BaseGerador` under `source/data/GeradorEstatisticas`.
2. Register the new generator in `GeradorEstatisticas::inicializar_geradores`.
3. Ensure a stable and well-documented `QVariantMap` output payload.
4. If the chart fits the generic model, adapt it in `LegacyVariantChartAdapter`.
5. If custom geometry is needed, extend `ChartLayoutEngine`.
6. Add the chart entry to the selection panel in `GerenciamentoEstatisticasUI`.
7. Update `.ts` localization files and technical documentation.

### Adding a New Table Column

1. Update event extraction sources and metadata collectors.
2. Register the column identifier key and label in `GerenciadorColunas`.
3. Add translations to `LocalizadorQt` and `.ts` files.
4. Validate color highlighting, visibility toggling, sorting, and export routines.

### Adding a New Security Feature

1. Implement the feature module under `source/security`.
2. Register the component within `SecurityManager`.
3. Declare any required schema updates in `ensure_schema`.
4. Use `SecurityDbWriter` for asynchronous writes where appropriate.
5. Expose user queries and dialogs under `secui_*` if a GUI is needed.
6. Update evidence package bundling if the data is auditable.

### Adding a New Language

1. Create `.ts` and `.qm` catalogs under `source/locale`.
2. Add translated text documents to `source/assets` and `source/ui/Manual`.
3. Register the new language code in `LocalizadorQt`.
4. Update language selection menus in `GerenciadorMenusUI`.
5. Validate column headers, filter options, existing datasets, and manuals.

## Key Considerations

- `InterfaceMonitor` aggregates numerous managers; new features should avoid introducing direct coupling when an existing manager already owns the domain.
- `QVariantMap` is the established data contract between events, statistics, and UI; changing map keys requires coordinated refactoring.
- The charting subsystem utilizes `RasterChartView` for multi-monitor stability in the `Estatísticas` window.
- CMake discovers files in `source/data/Charts` via `GLOB_RECURSE`; newly added files in that folder are automatically included in the build.
- Deployment bundles the entire `source/assets` directory; legal notices and translations must remain consistent across releases.
- This documentation reflects the current state of the workspace codebase.

## Quick File Reference

| Topic | Key Files |
| --- | --- |
| Entry Point | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Main Window | `source/gui/ic_01_InterfaceMonitor.*` |
| Translation & i18n | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Monitoring & Observation | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Event Database | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Table Management | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Metadata Extraction | `source/services/GerenciamentoMetadados/*` |
| Security Subsystem | `source/security/*` |
| Statistics Generation | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Native Chart Engine | `source/data/Charts/*` |
| UI & Dialogs | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filters & Calendar | `source/tools/*` |
| Assets & Legal | `source/assets/*` |
| General Utilities | `source/utils/*` |

</details>

---

<h2 id="eses">🇪🇸 Español (ES)</h2>

<details>
<summary>Haga clic para expandir la Arquitectura en español</summary>

# Arquitectura Técnica - Linceu Lighthouse C++/Qt6

Este documento describe la arquitectura actual del proyecto `Linceu_Lighthouse_cpp` a partir del árbol de código presente en este espacio de trabajo. El objetivo es servir como referencia para el mantenimiento, evolución, auditoría de dependencias e integración entre módulos.

## Visión General y Propuesta Arquitectónica

El **Linceu Lighthouse** es una plataforma de escritorio nativa para Windows escrita en C++17 con Qt 6, diseñada para proporcionar visibilidad absoluta, historial estructurado, inteligencia de archivos sin IA y auditoría de seguridad local.

La arquitectura está orientada al principio **Local-First / Sovereign Privacy**: todos los datos de eventos, metadatos enriquecidos, bases de datos SQLite, cachés e informes se procesan y almacenan estrictamente en la máquina local, sin conexión externa, sin dependencias de la nube, sin telemetría y sin modelos de lenguaje (LLM).

En términos arquitectónicos y funcionales, el sistema opera como una **tubería de datos local (pipeline)** dividida en 7 dominios cooperantes:

```text
┌─────────────────────────────────────────────────────────────────────────┐
│              TUBERÍA DE DATOS (PIPELINE) - LINCEU LIGHTHOUSE            │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. DOMINIO DE MONITORIZACIÓN (APIs Nativas Windows / ReadDirectoryChangesW)
                                     ↓
 2. DOMINIO DE EVENTOS Y CORRELACIÓN (Normalización / Detección Move & Rename)
                                     ↓
 3. DOMINIO DE METADATOS (30+ categorías, Magic Numbers, Hashes Cripto)
                                     ↓
 4. DOMINIO DE PERSISTENCIA (SQLite Local Asíncrono / Batch Writers)
                                     ↓
 5. DOMINIO DE ANALYTICS Y ESTADÍSTICAS (17 Gráficos Nativos / Determinismo)
                                     ↓
 6. DOMINIO DE SEGURIDAD Y AUDITORÍA (Línea Base, Reglas, Riesgo, Evidencias)
                                     ↓
 7. DOMINIO DE PRESENTACIÓN (Interfaz Qt6, Internacionalización, Exportación)
```

En alto nivel, el sistema está estructurado en:

- **Bootstrap y Ciclo de Vida:** Inicialización de entorno, configuración de OpenGL, pantalla de bienvenida (splash) y respaldo (fallback);
- **Ventana Principal `InterfaceMonitor`:** Orquestación central e integración entre servicios y la interfaz de usuario;
- **Servicios de Observación, Escáner y Almacenamiento en Búfer:** Captura multihilo resistente a ráfagas intensas;
- **Normalización, Metadatos y Persistencia:** Enriquecimiento profundo y guardado en SQLite local;
- **Capa de Seguridad, Integridad y Forense:** Líneas base, reglas de puntuación de riesgo, cuarentena y evidencias;
- **Infraestructura Gráfica y Estadísticas:** Motor interno `Charts` con 17 generadores de diagramas nativos;
- **Internacionalización y Recursos (Assets):** Soporte nativo para 6 idiomas con recarga dinámica en tiempo de ejecución;
- **Utilidades Transversales:** Gestión de rutas, registros (logs), iconos e integraciones con el shell de Windows.

## Dependencias Efectivas

### Lenguaje, compilación y plataforma

- C++17 definido en `CMakeLists.txt`.
- CMake con `CMAKE_AUTOMOC` y `CMAKE_AUTORCC`.
- Ajuste predeterminado principal (preset): `qt6-mingw-debug`, utilizando MinGW y Qt 6 en Windows.
- Aplicación GUI de Windows creada como objetivo `WIN32`.
- Recurso nativo: `source/Linceu_Lighthouse_cpp.rc`.
- Despliegue posterior a la compilación con `windeployqt`, copia de `assets` y copia de archivos `.qm`.

### Módulos Qt vinculados

La compilación vincula explícitamente:

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

No existe dependencia directa de los módulos opcionales auditados previamente para gráficos externos, visualización 3D, servidor HTTP, mensajería, automatización de teclado virtual, compositor Wayland o autorización de red. Toda la renderización estadística actual es nativa e interna al proyecto.

### Bibliotecas nativas de Windows

En Windows, el objetivo principal también vincula:

- `wintrust`
- `version`
- `crypt32`

Estas bibliotecas admiten la verificación de firmas digitales, la lectura de metadados de ejecutables y operaciones criptográficas vinculadas al subsistema de seguridad.

### Variables operativas relevantes

- `LINCEU_OPENGL_MODE=auto`: comportamiento predeterminado.
- `LINCEU_OPENGL_MODE=software`: fuerza OpenGL por software.
- `LINCEU_OPENGL_MODE=desktop`: solicita OpenGL de escritorio.
- `LINCEU_OPENGL_MODE=angle`: solicita la capa de traducción ANGLE cuando está disponible.

El arranque también habilita el uso compartido de contextos OpenGL y configura `QSurfaceFormat` con doble búfer y suavizado de bordes (antialiasing).

## Mapa de Directorios

| Ruta | Rol |
| --- | --- |
| `main.cpp` | Punto de entrada mínimo. Llama a `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Arranque principal, backend gráfico, registro QML, traducción, ventana principal y escritor de la base de datos. |
| `source/src_02_SplashAppStarter.*` | Flujo alternativo con pantalla de inicio (splash), barra de progreso y carga visual. |
| `source/gui` | Ventana principal, inicialización de interfaz de usuario, configuración e internacionalizador. |
| `source/ui` | Gestores de botones, menús, estadísticas, eventos, progreso, datos, manual, diálogos y traducción de metadatos. |
| `source/services` | Observación de directorios, escáner, eventos, base de datos de eventos, metadados y modelo/tabla. |
| `source/security` | Perfil de seguridad, línea base, reglas, riesgo, atribución, cuarentena, evidencias y diálogos de seguridad. |
| `source/data` | Generación estadística y capa propia de gráficos/diagramas. |
| `source/tools` | Filtros, calendario y administración de filtros. |
| `source/locale` | Archivos fuente `.ts`, archivos binarios `.qm` y compilación de traducciones. |
| `source/assets` | Iconos, notas de versión, avisos, EULA, privacidad, derechos de autor, licencias y textos de soporte. |
| `source/utils` | Rutas, iconos, registros (logs), período de prueba y utilidades de la aplicación. |
| `AdvancedDocumentation.qml` | Documentación visual navegable de la arquitectura. |
| `ARCHITECTURE.md` | Este documento de arquitectura técnica. |

## Flujo de Inicialización

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> ajustar directorio de trabajo (cwd)
  +--> intentar SplashAppStarter
  |      |
  |      +--> crear QApplication
  |      +--> configurar backend gráfico
  |      +--> inicializar traducciones
  |      +--> mostrar splash
  |      +--> crear InterfaceMonitor
  |      +--> iniciar DatabaseWriter si existe db_path
  |
  +--> arranque de respaldo sin splash
         |
         +--> configurar backend gráfico
         +--> crear QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> AppUserModelID de Windows
         +--> icono de la aplicación
         +--> Internacionalizador
         +--> InterfaceMonitor
         +--> DatabaseWriter en modo inmediato (eager)
         +--> show()
         +--> app.exec()
```

El proceso de arranque separa responsabilidades clave:

- `ajustar_cwd()` previene fallos derivados de rutas relativas de recursos, traducciones y base de datos;
- `configurar_backend_grafico()` prepara OpenGL antes de la instanciación de cualquier widget;
- `source::charts_lgpl::register_qml_types()` expone `QuickChartItem` como un tipo QML instanciable;
- `source::security::pg::prepare_runtime()` prepara el subsistema local de persistencia de seguridad;
- `Internacionalizador::inicializar_sistema_traducao()` instala los traductores de Qt y de la aplicación;
- `DatabaseWriter::get_instance(dbPath)` inicializa el hilo de escritura asíncrona de eventos.

## Ventana Principal

`source::gui::InterfaceMonitor` es el núcleo central de la aplicación. Hereda de `QMainWindow` y agrega punteros a todos los administradores de alto nivel.

Responsabilidades principales:

- Crear y configurar la interfaz de usuario;
- Exponer comandos `Q_INVOKABLE` para menús, botones y cuadros de diálogo;
- Mantener integrados la tabla, filtros, monitorización, estadísticas, datos y seguridad;
- Propagar los cambios de idioma en tiempo de ejecución;
- Insertar eventos en flujo continuo (streaming);
- Aplicar colores y visibilidad de columnas;
- Coordinar la jerarquía de las ventanas asociadas;
- Cerrar los diálogos vinculados al finalizar la aplicación.

Componentes agregados más relevantes:

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

## Jerarquía de Ventanas

La ventana principal coordina una jerarquía de diálogos asociados mediante propiedades dinámicas de Qt, especialmente `interface_principal`. Esta jerarquía incluye el manual, acerca de, ayuda, diálogos de colores, selector avanzado y la ventana de estadísticas.

El objetivo es:

- Mantener la interfaz principal recuperable cuando el usuario alterna el foco entre ventanas;
- Ordenar los diálogos asociados sin forzar a todos a ser hijos directos o modales estrictos;
- Evitar que la ventana de `Estatísticas` quede por encima de la interfaz principal cuando esté inactiva;
- Cerrar automáticamente los diálogos vinculados al cerrar la interfaz.

Mecanismos utilizados:

- `eventFilter` para eventos de activación, foco, visualización/ocultación y eventos de ratón;
- Propiedades dinámicas para asociar diálogos con `InterfaceMonitor`;
- Invocación de `raise()` y `activateWindow()` únicamente ante una acción explícita del usuario;
- Sincronización programada y diferida para evitar reentradas en eventos de ventana;
- Aislamiento de diálogos transitorios que no deben participar en la jerarquía.

## Servicios de Monitorización y Eventos

### Observador (Observer)

`source::services::Observador` encapsula la monitorización de directorios. Combina estado atómico, colas seguras para subprocesos, bloqueos (locks), un hilo propio de ejecución, escáner y controladores de eventos.

Controla:

- Directorio monitorizado y subdirectorios;
- Ciclos de inicio, parada, pausa y reanudación;
- Detección de operaciones masivas por ráfagas;
- Almacenamiento en búfer de eventos en colas thread-safe;
- Control de fallos u omisiones consecutivas del sistema operativo;
- Estimación de eventos perdidos;
- Instantáneas (snapshots) de registros previos;
- Deduplicación de eventos recientes continuos;
- Integración y traspaso de datos con el escáner inicial.

### BaseEvento (Gestor Base de Eventos)

`source::services::BaseEvento` normaliza los eventos y gestiona la persistencia. Crea la base de datos, registra eventos específicos, gestiona eliminaciones temporales, procesa movimientos y actualiza el modelo de la tabla.

Operaciones centrales:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Submódulos de Servicio

| Submódulo | Responsabilidad |
| --- | --- |
| `GerenciamentoBaseEvento` | Base de datos de eventos, inserción, eliminación, movimiento, instantáneas y escritor asíncrono. |
| `GerenciamentoDiretorioScanner` | Escaneo inicial/lote, cola de archivos, hilo de trabajo (worker) y actualización de progreso. |
| `GerenciamentoEventoModificado` | Agrupación, intervalo dinámico de espera, procesamiento normal/masivo y caché de metadatos. |
| `GerenciamentoEventoMovido` | Correlación entre eventos de eliminación y adición, worker de movimiento y actualización de tabla. |
| `GerenciamentoMetadados` | Extracción por categoría de archivo, caché en memoria, configuración de tabla y procesamiento en lote. |
| `GerenciamentoObservador` | Inicio/parada, búfer, monitorización, gestión de errores, pausa y reanudación. |
| `GerenciamentoTabela` | Modelo de tabla, resaltado de colores, temas, columnas, encabezados, retraducción y procesamiento en segundo plano. |

## Persistencia

El proyecto utiliza `Qt::Sql` y controladores de Qt para la persistencia local. El flujo principal de eventos es gestionado por `BaseEvento` y `DatabaseWriter`. La capa de seguridad incluye funciones auxiliares en `secore_01_postgres_utils.*`, mientras que en el runtime actual utiliza una base de datos local `QSQLITE`.

Patrones de persistencia:

- Conexiones con nombres únicos por ámbito de hilo;
- Escritores asíncronos en lote para evitar bloqueos en la interfaz;
- Esquema de base de datos generado bajo demanda;
- Representación de eventos en `QVariantMap` para mantener compatibilidad con Qt y analíticas;
- Tablas auxiliares e instantáneas para líneas base, reglas de seguridad, cuarentena, evidencias y sesiones.

## Modelo y Tabla de Eventos

`GerenciadorTabela` coordena la experiencia visual de la tabla. `EventTableModel` es el modelo Qt utilizado para la alimentación en flujo de eventos.

Capacidades:

- Inserción al inicio (prepend) y al final (append) en lote de alto rendimiento;
- Sustitución completa del conjunto de eventos;
- Generación instantánea de snapshots de datos;
- Ordenación por múltiples columnas;
- Caché de colores por tipo de operación;
- Encabezados de columnas traducidos dinámicamente;
- Columnas visibles y personalizadas;
- Adaptación automática al tema claro u oscuro de Windows;
- Retraducción diferida tras cambios de idioma;
- Procesamiento pesado en segundo plano;
- Sincronización bidireccional con `QTableView`.

## Metadatos

El subsistema `GerenciamentoMetadados` es amplio y está especializado según el tipo de archivo.

Cubre:

- Código fuente;
- Imágenes;
- Archivos de audio;
- Vídeos;
- Documentos;
- Hojas de cálculo;
- Presentaciones;
- Bases de datos;
- Ejecutables y binarios PE;
- Archivos temporales;
- Archivos comprimidos;
- Copias de seguridad (backups);
- Archivos de registro (logs);
- Archivos de configuración;
- Almacenamiento estructurado OLE;
- Formatos de datos estructurados;
- Extracción completa de metadatos;
- Detección de tipo MIME real por contenido;
- Tamaño, unidades y conteo exacto de bytes;
- Atributos, autor, dimensiones, duración, tasa de bits y permisos de protección.

También administra la configuración de columnas, la invalidación de caché de directorios afectados, la cola de extracción de metadados, las devoluciones de llamada (callbacks) y la actualización de columnas.

## Seguridad Forense

`source::security::SecurityManager` es la fachada del subsistema de seguridad. Centraliza el contexto, esquemas, enriquecimiento de eventos, consultas de alertas, correlación de sesiones, cuarentena, líneas base de integridad y observadores de tiempo de ejecución.

Componentes principales:

| Archivo | Rol |
| --- | --- |
| `base_01_baseline_criptografico.*` | Línea base criptográfica, hashes SHA-256 e instantáneas de verificación. |
| `exec_01_inteligencia_executaveis.*` | Análisis de binarios ejecutables y metadados de firma digital. |
| `sess_01_correlacao_sessoes.*` | Correlación temporal de sesiones de usuario y actividad del sistema. |
| `sdiff_01_diff_conteudo.*` | Comparación de versiones y diferencias a nivel de contenido. |
| `attr_01_atribuicao_forense.*` | Atribución y contexto forense de procesos y usuarios. |
| `agre_01_motor_regras.*` | Motor de reglas de conformidad y alertas locales. |
| `risk_01_score_risco.*` | Cálculo determinista de la puntuación de riesgo. |
| `quar_01_quarentena.*` | Almacenamiento en cuarentena local, gestión de blobs y restauración. |
| `evid_01_exportacao_evidencia.*` | Empaquetado y exportación de evidencias auditables. |
| `mdir_01_multi_diretorio.*` | Perfiles de monitorización y múltiples directorios observados. |
| `secore_05_SecurityDbWriter.*` | Escritor asíncrono especializado para la base de datos de seguridad. |

Diálogos de seguridad:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Generación de Estadísticas

`source::data::GeradorEstatisticas` recibe `db_path`, el gestor de localización y la interfaz principal. Registra generadores especializados derivados de `BaseGerador`.

Gráficos y diagramas admitidos:

- Gráfico circular de operaciones;
- Gráfico de barras por tipo de archivo;
- Línea de tiempo de operaciones (timeline);
- Mapa de árbol por categorías (treemap);
- Histograma de actividad por horas;
- Curva de distribución de Pareto;
- Gráficos de líneas y agrupaciones (clusters);
- Diagramas de caja (boxplots por distribución, operación, tipo, hora, tamaño y volumen de eventos);
- Gráfico de radar de eventos;
- Mapa de calor temporal (heatmap);
- Diagrama de dispersión (scatter);
- Diagramas de Sankey;
- Radar multidimensional;
- Gráfico de puntos (dotplot);
- Diagrama de Sankey por evento y ruta;
- Diagrama de Sankey por tipo y ruta;
- Árbol jerárquico de directorios.

Todas las estructuras de salida mantienen total compatibilidad con `QVariant`, `QVariantMap` y `QVariantList`, garantizando interoperabilidad con las capas visuales.

## Motor de Renderizado de Gráficos y Diagramas

El subsistema en `source/data/Charts` reemplaza cualquier dependencia externa de bibliotecas gráficas mediante una implementación C++/Qt propia de alto rendimiento.

Capas de renderizado:

```text
QVariantMap heredado
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
  +--> RasterChartView    (QWidget + QPainter, utilizado en la ventana de Estadísticas)
  +--> WidgetChartView    (QOpenGLWidget + capa QPainter)
  +--> QuickChartItem     (QQuickItem + Scene Graph de Qt Quick)
```

Archivos principales:

- `ch_01_ChartLayoutEngine.*`: Genera geometría, etiquetas, leyendas, selección interactiva y escenas gráficas.
- `ch_02_ChartWidgetFactory.*`: Crea widgets visuales a partir de conjuntos de datos tipados o cargas heredadas.
- `ch_03_LegacyVariantChartAdapter.*`: Adapta cargas `QVariantMap` al modelo fuertemente tipado.
- `ch_04_QmlRegistration.*`: Registra `QuickChartItem` en el motor de QML.
- `ch_05_QuickChartItem.*`: Backend Qt Quick Scene Graph para renderizado acelerado.
- `ch_06_WidgetChartView.*`: Backend acelerado por hardware basado en `QOpenGLWidget`.
- `ch_07_LgplChartModel.h`: Estructuras fundamentales de datos, colores y geometría.
- `ch_08_RasterChartView.*`: Backend `QWidget` raster puro con suavizado de alta calidad.

Decisiones arquitectónicas:

- `RasterChartView` se prioriza en la ventana principal de `Estatísticas` para evitar inestabilidades de contexto OpenGL en entornos multimonitor;
- Se utiliza `QPainter` con antialiasing para garantizar una renderización vectorial nítida;
- `WidgetChartView` permanece disponible para flujos de renderizado OpenGL continuo;
- `QuickChartItem` permite la integración fluida con QML y transiciones animadas mediante `transitionProgress`;
- La exportación de imágenes emplea un paso de renderizado determinista, omitiendo fotogramas de transición.

## Módulos de UI y Comandos

Los administradores en `source/ui` dividen la ventana principal en dominios desacoplados y modulares.

| Módulo | Rol |
| --- | --- |
| `GerenciadorBotoes` | Acciones de selección de directorio, análisis, exportación, limpieza y pausa. |
| `GerenciadorBotoesUI` | Creación y disposición visual de los botones de la barra de herramientas. |
| `GerenciadorMenusUI` | Menús de archivo, configuración, opciones, colores, columnas, exportación, idiomas, acerca de, manual y ayuda. |
| `GerenciadorEventosUI` | Gestión de filtros, visibilidad de columnas, colores, selector de idioma y reinicio. |
| `GerenciadorProgressoUI` | Barra de progreso y notificaciones del estado del escaneo. |
| `GerenciadorEstatisticasUI` | Diálogo de estadísticas, panel selector, generación, regeneración en vivo y exportación. |
| `GerenciadorDados` | Exportación multiformato y manipulación de datos. |
| `GerenciadorEventosArquivo` | Despacho de eventos del sistema de archivos hacia la interfaz de usuario. |
| `GerenciadorMonitoramento` | Inicio, parada, pausa y reanudación de la monitorización. |
| `GerenciadorLimpeza` | Limpieza de datos monitorizados y cachés de trabajo. |
| `DialogoCores` | Mapeo de colores por tipo de operación y selector avanzado de paletas. |
| `LocalizadorQt` | Gestor central de catálogos de traducción de la aplicación. |
| `TradutorMetadadosQt` | Traducción dinámica de operaciones, tipos MIME, atributos, autor y permisos. |
| `OpcoesSobre`, `Manual` | Diálogos informativos y manual de usuario. |

## Filtros y Herramientas

`source/tools` centraliza el sistema de filtrado y la integración con el calendario.

Componentes:

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Responsabilidades:

- Aplicar y restablecer filtros activos;
- Sincronizar el menú principal con el estado de los filtros;
- Actualizar el recuento de eventos coincidentes en tiempo real;
- Responder a los cambios de idioma de la interfaz;
- Guardar el estado de las casillas de verificación entre sesiones;
- Comprobar y filtrar eventos por operación, rango de fechas o extensión.

## Internacionalización (i18n)

La arquitectura de internacionalización combina:

- Archivos fuente de traducción: `source/locale/*.ts`;
- Archivos binarios compilados: `source/locale/*.qm`;
- Herramienta de compilación automática: `compile_translations.*`;
- Clase `Internacionalizador`;
- Clase `LocalizadorQt`;
- Clase `TradutorMetadadosQt`;
- Documentos de manual localizados en `source/ui/Manual`;
- Textos y recursos legales localizados en `source/assets`.

Idiomas integrados:

- `pt_BR` (Portugués - Brasil)
- `en_US` (Inglés - EE. UU.)
- `es_ES` (Español - España)
- `fr_FR` (Francés - Francia)
- `it_IT` (Italiano - Italia)
- `de_DE` (Alemán - Alemania)

Al cambiar de idioma en tiempo de ejecución, la aplicación retraduce menús, encabezados de tabla, opciones de filtrado, estadísticas activas, cuadros de diálogo y registros existentes.

## Recursos, Licencias y Documentación Legal

La carpeta `source/assets` se empaqueta íntegramente durante el despliegue. Contiene:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Los avisos enumeran los módulos Qt utilizados y sus condiciones de redistribución de código abierto. Las notas de versión y los textos legales se mantienen separados por idioma.

## Utilidades Transversales

| Archivo | Responsabilidad |
| --- | --- |
| `ApplicationPathUtils.*` | Resolución de rutas base, detección de assets y lectura de textos locales. |
| `CaminhoPersistenteUtils.*` | Resolución de rutas estándar de datos persistentes (`%LOCALAPPDATA%`). |
| `IconUtils.*` | Carga de iconos compatibles con pantallas de alta densidad (High-DPI). |
| `LogManager.*` | Registrador centralizado seguro para hilos, rotación de logs con marca temporal y normalización Unicode. |
| `TrialManager.*` | Gestión del período de prueba, verificación de licencias y redirección a la tienda. |

## Concurrencia y Capacidad de Respuesta

El proyecto implementa una estrategia multihilo equilibrada:

- `std::thread` para la monitorización continua del sistema de archivos;
- `QThread` para el escaneo recursivo de directorios en segundo plano;
- `QTimer` para vaciado diferido (flushes), sincronización de interfaz y retraducciones;
- `QtConcurrent` para tareas auxiliares de procesamiento intensivo;
- Escritores asíncronos en hilos dedicados para la base de datos;
- Cachés en memoria ultrarrápidos para colores, cadenas traducidas, metadados e instantáneas;
- Variables atómicas libres de bloqueo y exclusión mutua (mutex) en colas de eventos.

Directrices arquitectónicas:

- Las actualizaciones de interfaz deben ejecutarse estrictamente en el hilo principal de Qt;
- Las operaciones de persistencia nunca deben bloquear el bucle de eventos (event loop);
- Las operaciones masivas deben agruparse y procesarse en lotes;
- El renderizado de gráficos debe preferir `RasterChartView` en el diálogo de estadísticas;
- Los cachés deben invalidarse ante cualquier cambio de idioma, tema, configuración de columnas o colores.

## Flujo de Eventos de Archivo

```text
ReadDirectoryChangesW / escáner
  |
  v
Observador
  |
  +--> búfer de eventos
  +--> detección de ráfagas masivas
  +--> controladores: Añadido / Eliminado / Modificado / Renombrado / Movido
  |
  v
BaseEvento
  |
  +--> normaliza los datos del evento
  +--> extrae metadados
  +--> registra en la base de datos SQLite
  +--> notifica a SecurityManager
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> filtros / colores / traducciones
  +--> estadísticas bajo demanda
```

## Flujo de Estadísticas

```text
El usuario hace clic en Estadísticas
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
  +--> RasterChartView para la ventana de Estadísticas
  +--> widgets personalizados para diagramas específicos
  |
  v
Diálogo de Estadísticas
```

## Reglas para la Evolución

### Añadir un nuevo gráfico

1. Implementar un generador especializado derivado de `BaseGerador` en `source/data/GeradorEstatisticas`.
2. Registrar el nuevo generador en `GeradorEstatisticas::inicializar_geradores`.
3. Garantizar una estructura de salida `QVariantMap` estable y bien definida.
4. Si el gráfico se ajusta al modelo común, adaptarlo en `LegacyVariantChartAdapter`.
5. Si requiere geometrías personalizadas, ampliar `ChartLayoutEngine`.
6. Exponerlo en el panel selector dentro de `GerenciamentoEstatisticasUI`.
7. Actualizar los archivos de traducción `.ts` y la documentación técnica.

### Añadir una nueva columna a la tabla

1. Actualizar las fuentes de eventos y la extracción de metadados.
2. Registrar el identificador y la etiqueta de la columna en `GerenciadorColunas`.
3. Incorporar la traducción en `LocalizadorQt` y en los archivos `.ts`.
4. Verificar el resaltado por color, visibilidad, ordenación y rutinas de exportación.

### Añadir una nueva funcionalidad de seguridad

1. Crear el componente dentro de `source/security`.
2. Integrarlo en la fachada `SecurityManager`.
3. Declarar las tablas o esquemas requeridos en `ensure_schema`.
4. Utilizar `SecurityDbWriter` para escrituras asíncronas cuando corresponda.
5. Crear diálogos y vistas de consulta en `secui_*` si requiere interfaz gráfica.
6. Actualizar el paquete de evidencias si la información debe ser auditable.

### Añadir un nuevo idioma

1. Crear los archivos `.ts` y `.qm` en `source/locale`.
2. Añadir la documentación traducida en `source/assets` y `source/ui/Manual`.
3. Registrar el código del nuevo idioma en `LocalizadorQt`.
4. Actualizar los menús de selección de idioma en `GerenciadorMenusUI`.
5. Validar encabezados de columna, filtros, datos cargados y manuales.

## Puntos de Atención

- `InterfaceMonitor` centraliza una gran cantidad de punteros; las nuevas características deben evitar incrementar el acoplamiento directo cuando un gestor existente ya es responsable del dominio.
- `QVariantMap` representa el contrato de datos histórico entre eventos, estadísticas e interfaz; modificar sus claves requiere una migración coordinada.
- El subsistema gráfico utiliza `RasterChartView` para garantizar estabilidad multimonitor en la ventana de `Estatísticas`.
- CMake incluye la carpeta `source/data/Charts` mediante `GLOB_RECURSE`; cualquier archivo nuevo en esta ruta se compila automáticamente.
- El despliegue copia todo el directorio `source/assets`; los textos legales y traducciones deben mantenerse consistentes en cada versión.
- Esta documentación describe la estructura real del espacio de trabajo actual.

## Referencia Rápida de Archivos

| Tema | Archivos Clave |
| --- | --- |
| Punto de entrada | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Ventana principal | `source/gui/ic_01_InterfaceMonitor.*` |
| Traducción e i18n | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Monitorización | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Base de datos de eventos | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Gestión de tabla | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Extracción de metadados | `source/services/GerenciamentoMetadados/*` |
| Seguridad forense | `source/security/*` |
| Generación estadística | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Motor de gráficos nativo | `source/data/Charts/*` |
| Interfaz y diálogos | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filtros y calendario | `source/tools/*` |
| Recursos y licencias | `source/assets/*` |
| Utilidades generales | `source/utils/*` |

</details>

---

<h2 id="frfr">🇫🇷 Français (FR)</h2>

<details>
<summary>Cliquez pour développer l'Architecture en français</summary>

# Architecture Technique - Linceu Lighthouse C++/Qt6

Ce document décrit l'architecture actuelle du projet `Linceu_Lighthouse_cpp` à partir de l'arborescence du code présente dans cet espace de travail. Son objectif est de servir de référence pour la maintenance, le développement de fonctionnalités, l'audit des dépendances et l'intégration entre modules.

## Vue d'Ensemble et Vision Architecturale

**Linceu Lighthouse** est une plateforme de bureau native pour Windows écrite en C++17 avec Qt 6, conçue pour offrir une visibilité absolue, un historique structuré, une intelligence des fichiers sans IA et un audit de sécurité local.

L'architecture repose sur le principe fondamental **Local-First / Sovereign Privacy** : toutes les données d'événements, métadonnées enrichies, bases de données SQLite, caches et rapports sont traités et stockés strictement sur la machine locale, sans connexion externe, sans dépendance au cloud, sans télémétrie et sans modèles de langage (LLM).

Sur le plan architectural et fonctionnel, le système fonctionne comme un **pipeline de données local** divisé en 7 domaines coopératifs :

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    PIPELINE DE DONNÉES - LINCEU LIGHTHOUSE              │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. DOMAINE DE SURVEILLANCE (APIs Natives Windows / ReadDirectoryChangesW)
                                     ↓
 2. DOMAINE DES ÉVÉNEMENTS ET CORRÉLATION (Normalisation / Détection Move & Rename)
                                     ↓
 3. DOMAINE DES MÉTADONNÉES (30+ catégories, Magic Numbers, Hashes Crypto)
                                     ↓
 4. DOMAINE DE PERSISTANCE (SQLite Local Asynchrone / Batch Writers)
                                     ↓
 5. DOMAINE D'ANALYTIQUE ET STATISTIQUES (17 Graphiques Natifs / Déterminisme)
                                     ↓
 6. DOMAINE DE SÉCURITÉ ET AUDIT (Ligne de Base, Règles, Risque, Preuves)
                                     ↓
 7. DOMAINE DE PRÉSENTATION (Interface Qt6, Internationalisation, Exportation)
```

À haut niveau, le système est structuré autour des modules suivants :

- **Bootstrap et Cycle de Vie :** Initialisation de l'environnement, configuration OpenGL, écran de démarrage (splash) et procédure de secours (fallback) ;
- **Fenêtre Principale `InterfaceMonitor` :** Orchestration centrale et intégration transparente entre services d'arrière-plan et interface utilisateur ;
- **Services d'Observation, Scanner et Mise en Mémoire Tampon :** Capture multithread résiliente aux rafales massives d'événements ;
- **Normalisation, Métadonnées et Persistance :** Enrichissement profond et écriture asynchrone dans SQLite ;
- **Couche de Sécurité, Intégrité et Analyse Forensique :** Lignes de base, règles d'évaluation du risque, mise en quarantaine et preuves auditables ;
- **Infrastructure Graphique et Statistiques :** Moteur interne `Charts` doté de 17 générateurs de diagrammes natifs ;
- **Internationalisation et Ressources (Assets) :** Prise en charge native de 6 langues avec rechargement dynamique à l'exécution ;
- **Utilitaires Transversaux :** Gestion des chemins d'accès, journalisation (logs), résolution d'icônes et intégration au shell Windows.

## Dépendances Effectives

### Langage, chaîne de compilation et plateforme

- C++17 spécifié dans `CMakeLists.txt`.
- CMake avec `CMAKE_AUTOMOC` et `CMAKE_AUTORCC`.
- Profil principal (preset) : `qt6-mingw-debug`, utilisant MinGW et Qt 6 sous Windows.
- Application GUI Windows créée en tant que cible `WIN32`.
- Fichier de ressources natif : `source/Linceu_Lighthouse_cpp.rc`.
- Déploiement post-compilation via `windeployqt`, copie des `assets` et distribution des fichiers de traduction compilés `.qm`.

### Modules Qt Liés

La compilation lie explicitement :

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

Il n'y a aucune dépendance directe envers des bibliothèques externes pour le rendu de graphiques, la visualisation 3D, un serveur HTTP, un bus de messages, un clavier virtuel, un compositeur Wayland ou une authentification réseau. L'ensemble du rendu statistique est développé de manière native et interne au projet.

### Bibliothèques Natives Windows

Sous Windows, la cible principale lie également :

- `wintrust`
- `version`
- `crypt32`

Ces bibliothèques prennent en charge la vérification des signatures numériques, la lecture des métadonnées des exécutables et les calculs cryptographiques du sous-système de sécurité.

### Variables d'Environnement Opérationnelles

- `LINCEU_OPENGL_MODE=auto` : comportement par défaut.
- `LINCEU_OPENGL_MODE=software` : force le rendu OpenGL logiciel.
- `LINCEU_OPENGL_MODE=desktop` : sollicite le contexte OpenGL de bureau.
- `LINCEU_OPENGL_MODE=angle` : sollicite la couche de traduction ANGLE si disponible.

Le programme d'amorce active également le partage des contextes OpenGL et configure `QSurfaceFormat` avec double mise en mémoire tampon (double buffering) et anticrénelage.

## Carte des Répertoires

| Chemin | Rôle |
| --- | --- |
| `main.cpp` | Point d'entrée minimal. Invoque `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Amorce principale, configuration graphique, enregistrement QML, traductions, fenêtre principale et rédacteur de base de données. |
| `source/src_02_SplashAppStarter.*` | Flux de démarrage alternatif avec écran de veille animé, indicateur de progression et chargement visuel. |
| `source/gui` | Fenêtre principale, initialisation de l'interface graphique, configuration et internationalisation. |
| `source/ui` | Gestionnaires de boutons, barres de menus, statistiques, événements UI, progression, données, manuel, boîtes de dialogue et traduction des métadonnées. |
| `source/services` | Surveillance de répertoires, scanner, gestion des événements, base de données, métadonnées et modèle de tableau. |
| `source/security` | Profils de sécurité, lignes de base, règles, score de risque, attribution forensique, quarantaine, preuves et dialogues de sécurité. |
| `source/data` | Génération de rapports statistiques et moteur graphique/diagrammes interne. |
| `source/tools` | Sous-système de filtres, gestionnaire de calendrier et commandes administratives de filtrage. |
| `source/locale` | Fichiers sources `.ts`, fichiers binaires `.qm` et scripts de compilation des traductions. |
| `source/assets` | Icônes, notes de version, mentions tierces, CLUF (EULA), politique de confidentialité, droits d'auteur, licences et documentations. |
| `source/utils` | Résolution des chemins, icônes, journaux (logs), période d'essai et utilitaires généraux. |
| `AdvancedDocumentation.qml` | Documentation visuelle interactive et navigable de l'architecture. |
| `ARCHITECTURE.md` | Ce document d'architecture technique. |

## Flux d'Initialisation

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> ajuster le répertoire de travail courant (cwd)
  +--> tenter SplashAppStarter
  |      |
  |      +--> instancier QApplication
  |      +--> configurer le backend graphique
  |      +--> initialiser le système de traduction
  |      +--> afficher l'écran de démarrage (splash)
  |      +--> instancier InterfaceMonitor
  |      +--> démarrer DatabaseWriter si db_path est défini
  |
  +--> procédure de secours sans splash
         |
         +--> configurer le backend graphique
         +--> instancier QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> définir l'AppUserModelID de Windows
         +--> charger l'icône de l'application
         +--> initialiser Internacionalizador
         +--> instancier InterfaceMonitor
         +--> instancier DatabaseWriter (mode direct)
         +--> show()
         +--> app.exec()
```

Le processus d'amorçage découple clairement les responsabilités majeures :

- `ajustar_cwd()` évite les erreurs causées par des chemins relatifs pour les ressources, les traductions et la base de données ;
- `configurar_backend_grafico()` configure les formats de surface OpenGL avant toute allocation de widgets ;
- `source::charts_lgpl::register_qml_types()` enregistre `QuickChartItem` en tant que type QML instanciable ;
- `source::security::pg::prepare_runtime()` initialise le runtime de persistance du sous-système de sécurité ;
- `Internacionalizador::inicializar_sistema_traducao()` charge les catalogues de traduction Qt et de l'application ;
- `DatabaseWriter::get_instance(dbPath)` initialise le thread d'écriture asynchrone des événements.

## Fenêtre Principale

`source::gui::InterfaceMonitor` constitue le centre névralgique de l'application. Elle hérite de `QMainWindow` et regroupe l'ensemble des pointeurs vers les gestionnaires de haut niveau.

Responsabilités principales :

- Créer et agencer l'interface utilisateur ;
- Exposer les commandes `Q_INVOKABLE` pour les menus, boutons et boîtes de dialogue ;
- Maintenir synchronisés en temps réel le tableau des événements, les filtres actifs, la surveillance, les statistiques et le module de sécurité ;
- Propager dynamiquement les changements de langue ;
- Insérer les événements en continu (streaming fluide) ;
- Appliquer les jeux de couleurs et la visibilité des colonnes ;
- Coordonner la hiérarchie et l'ordre d'affichage des fenêtres associées ;
- Fermer proprement les fenêtres secondaires lors de la fermeture de l'application.

Principaux composants agrégés :

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

## Hiérarchie des Fenêtres

La fenêtre principale gère une hiérarchie de dialogues associés par le biais de propriétés dynamiques Qt, en particulier `interface_principal`. Cette hiérarchie englobe le manuel, la boîte À propos, l'aide, les sélecteurs de couleurs, le sélecteur de filtres avancés et la fenêtre de statistiques.

Objectifs :

- Conserver l'accessibilité de l'interface principale lorsque l'utilisateur bascule entre différentes fenêtres du système ;
- Structurer l'ordre des boîtes de dialogue sans contraindre chaque boîte à être modale ou enfant direct ;
- Empêcher la fenêtre `Estatísticas` de recouvrir l'interface principale lorsqu'elle est inactive ;
- Fermer automatiquement tous les dialogues dépendants lors de la fermeture de la fenêtre principale.

Mécanismes mis en œuvre :

- `eventFilter` dédié à la gestion des événements d'activation, de focus, d'affichage/masquage et de souris ;
- Propriétés dynamiques pour rattacher formellement les dialogues à `InterfaceMonitor` ;
- Utilisation contrôlée de `raise()` et `activateWindow()` uniquement sur action explicite de l'utilisateur ;
- Synchronisation différée et programmée pour prévenir les boucles réentrantes lors des changements d'état de fenêtre ;
- Isolation des dialogues transitoires indépendants ne devant pas faire partie de la hiérarchie.

## Services de Surveillance et de Gestion des Événements

### Observateur (Observador)

`source::services::Observador` encapsule la surveillance du système de fichiers en arrière-plan. Il combine des indicateurs atomiques, des files d'attente sécurisées (thread-safe), des verrous (locks), un thread d'exécution dédié, un scanner et des gestionnaires d'événements.

Il supervise :

- Le répertoire surveillé et ses arborescences filles ;
- Les cycles de démarrage, d'arrêt, de pause et de reprise ;
- La détection de rafales d'activité massives ;
- La mise en mémoire tampon des événements dans des files sécurisées ;
- Le suivi des débordements de tampons du système d'exploitation ;
- L'estimation des événements perdus ou omis ;
- Les instantanés (snapshots) des états antérieurs ;
- La déduplication des événements récurrents rapprochés ;
- L'intégration harmonieuse avec le scanner initial.

### BaseEvento (Gestionnaire d'Événements de Base)

`source::services::BaseEvento` normalise les signaux du système d'exploitation et coordonne la persistance des données. Elle crée le schéma de base de données, enregistre les événements, gère les suppressions temporaires, suit les déplacements de fichiers et alimente la table d'audit.

Opérations clés :

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Sous-Modules de Service

| Sous-module | Responsabilité |
| --- | --- |
| `GerenciamentoBaseEvento` | Gestion de la base de données d'événements, insertion, suppression, déplacement, instantanés et rédacteur asynchrone. |
| `GerenciamentoDiretorioScanner` | Analyse initiale et par lots, ingestion dans la file d'attente, thread d'exécution et mise à jour de la progression. |
| `GerenciamentoEventoModificado` | Regroupement, temporisation dynamique, gestion des modifications massives et cache des métadonnées. |
| `GerenciamentoEventoMovido` | Corrélation des événements de suppression et de création, exécution du worker de déplacement et mise à jour de l'affichage. |
| `GerenciamentoMetadados` | Extraction spécialisée par type de fichier, mise en cache mémoire, formatage pour tableau et traitement par lots. |
| `GerenciamentoObservador` | Cycle de vie de l'observateur (démarrage/arrêt/pause/reprise), gestion des tampons, gestion des erreurs OS et récupération. |
| `GerenciamentoTabela` | Modèle de tableau Qt, surlignage couleur, thèmes visuels, colonnes, en-têtes, retraduction et traitement en arrière-plan. |

## Persistance

Le projet s'appuie sur `Qt::Sql` et les pilotes de base de données Qt pour toute la persistance locale. Le pipeline principal d'événements est pris en charge par `BaseEvento` et `DatabaseWriter`. La couche de sécurité fournit des assistants dans `secore_01_postgres_utils.*`, tandis que l'exécution standard utilise une base locale `QSQLITE`.

Principes de conception de la persistance :

- Connexions nommées de manière unique par contexte de thread ;
- Rédacteurs asynchrones par lots pour éviter tout gel de l'interface utilisateur ;
- Création automatique du schéma à la demande ;
- Représentation mémoire des événements via `QVariantMap` garantissant l'interopérabilité avec les composants Qt et statistiques ;
- Tables auxiliaires et instantanés dédiés aux lignes de base, règles de sécurité, mise en quarantaine, preuves et sessions.

## Modèle et Tableau d'Événements

`GerenciadorTabela` pilote l'expérience visuelle du journal d'audit. `EventTableModel` est le modèle Qt sur mesure qui transmet le flux d'événements à la vue `QTableView`.

Fonctionnalités majeures :

- Ajout en début (prepend) ou en fin (append) par lots à haut débit ;
- Remplacement intégral du jeu d'événements ;
- Génération instantanée d'instantanés de données ;
- Tri multicritère par colonne ;
- Mise en cache des couleurs par catégorie d'opération ;
- Traduction dynamique des en-têtes de colonnes ;
- Configuration des colonnes visibles et personnalisées ;
- Adaptation automatique aux thèmes clair et sombre de Windows ;
- Retraduction planifiée et différée lors des changements de langue ;
- Déport des calculs complexes de formatage en arrière-plan ;
- Synchronisation bidirectionnelle avec `QTableView`.

## Extraction de Métadonnées

Le sous-système `GerenciamentoMetadados` est complet et spécialisé selon la catégorie de fichier.

Domaines et formats couverts :

- Code source ;
- Images ;
- Fichiers audio ;
- Flux vidéo ;
- Documents textuels ;
- Feuilles de calcul ;
- Présentations ;
- Bases de données ;
- Exécutables et binaires PE ;
- Fichiers temporaires ;
- Archives compressées ;
- Fichiers de sauvegarde (backup) ;
- Fichiers journaux (logs) ;
- Fichiers de configuration ;
- Stockage structuré composé OLE ;
- Formats de données structurés (JSON, XML, etc.) ;
- Extraction approfondie de métadonnées ;
- Détection du type MIME réel par analyse de contenu ;
- Taille, conversions d'unités et décompte exact d'octets ;
- Attributs, auteur, dimensions, durée, débit binaire et droits de protection.

Il supervise également la configuration des colonnes de tableau, l'invalidation du cache pour les dossiers modifiés, les files d'attente d'extraction, les rappels de fin de traitement (callbacks) et la mise à jour des vues.

## Sécurité Forensique

`source::security::SecurityManager` constitue la façade principale du sous-système de sécurité. Il centralise le contexte, les schémas de données, l'enrichissement d'événements, la gestion des alertes, la corrélation de sessions, la quarantaine, la vérification d'intégrité par ligne de base et les observateurs de sécurité.

Composants essentiels :

| Fichier | Rôle |
| --- | --- |
| `base_01_baseline_criptografico.*` | Ligne de base cryptographique, empreintes SHA-256 et instantanés de contrôle. |
| `exec_01_inteligencia_executaveis.*` | Analyse des binaires exécutables, en-têtes PE et métadonnées de signatures numériques. |
| `sess_01_correlacao_sessoes.*` | Corrélation temporelle des sessions utilisateur et de l'activité du système. |
| `sdiff_01_diff_conteudo.*` | Comparaison de contenu et différenciation de versions. |
| `attr_01_atribuicao_forense.*` | Attribution et contexte forensique (processus et utilisateurs). |
| `agre_01_motor_regras.*` | Moteur de règles de sécurité et alertes locales. |
| `risk_01_score_risco.*` | Moteur de calcul déterministe du score de risque. |
| `quar_01_quarentena.*` | Gestion du stockage en quarantaine locale, manipulation de blobs et restauration. |
| `evid_01_exportacao_evidencia.*` | Création et exportation de packages de preuves auditables. |
| `mdir_01_multi_diretorio.*` | Profils de surveillance et répertoires multiples. |
| `secore_05_SecurityDbWriter.*` | Rédacteur asynchrone dédié à la base de données de sécurité. |

Boîtes de dialogue de sécurité :

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Génération de Statistiques

`source::data::GeradorEstatisticas` reçoit `db_path`, le gestionnaire de localisation et la fenêtre principale. Il enregistre des générateurs spécialisés héritant de `BaseGerador`.

Graphiques et diagrammes pris en charge :

- Diagramme circulaire des opérations ;
- Diagramme en barres par type de fichier ;
- Chronologie des opérations (timeline) ;
- Treemap par catégorie de fichier ;
- Histogramme d'activité par tranche horaire ;
- Courbe de distribution de Pareto ;
- Graphiques linéaires et analyse par grappes (clusters) ;
- Boîtes à moustaches (boxplots par distribution, opération, type, heure, taille et volume) ;
- Diagramme radar des événements ;
- Carte thermique temporelle (heatmap) ;
- Nuage de points (scatter plot) ;
- Diagrammes de Sankey ;
- Radar multidimensionnel ;
- Diagramme de dispersion (dotplot) ;
- Diagramme de Sankey par événement et chemin ;
- Diagramme de Sankey par type et chemin ;
- Arborescence hiérarchique de répertoires.

Les structures produites conservent une stricte compatibilité avec `QVariant`, `QVariantMap` et `QVariantList`.

## Moteur de Rendu Graphique et Diagrammes

Le sous-système situé dans `source/data/Charts` remplace l'intégralité des dépendances graphiques tierces par une solution interne C++/Qt hautement performante.

Couches de rendu :

```text
QVariantMap hérité
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
  +--> RasterChartView    (QWidget + QPainter, utilisé dans le dialogue Statistiques)
  +--> WidgetChartView    (QOpenGLWidget + surcouche QPainter)
  +--> QuickChartItem     (QQuickItem + Scene Graph Qt Quick)
```

Fichiers principaux :

- `ch_01_ChartLayoutEngine.*` : Calcule la géométrie 2D/3D, étiquettes, légendes, détection de clic et primitives visuelles.
- `ch_02_ChartWidgetFactory.*` : Instancie les composants visuels à partir de structures typées ou de flux hérités.
- `ch_03_LegacyVariantChartAdapter.*` : Convertit les structures `QVariantMap` vers les modèles de diagrammes fortement typés.
- `ch_04_QmlRegistration.*` : Déclare `QuickChartItem` au sein du moteur QML.
- `ch_05_QuickChartItem.*` : Backend Qt Quick Scene Graph pour un rendu matériellement accéléré.
- `ch_06_WidgetChartView.*` : Canevas accéléré basé sur `QOpenGLWidget`.
- `ch_07_LgplChartModel.h` : Structures centrales de modélisation de scène, géométries et couleurs.
- `ch_08_RasterChartView.*` : Canevas raster pur basé sur `QWidget` avec anticrénelage haute précision.

Choix d'implémentation majeurs :

- `RasterChartView` est privilégié pour la fenêtre `Estatísticas` afin d'éliminer toute instabilité liée aux contextes OpenGL sur les configurations multi-écrans ;
- `QPainter` avec anticrénelage assure un rendu vectoriel parfait de la typographie et des formes ;
- `WidgetChartView` reste disponible pour les besoins de rendu OpenGL continu ;
- `QuickChartItem` autorise l'intégration fluide dans QML et l'animation via `transitionProgress` ;
- L'exportation d'images exploite un mode de rendu déterministe qui ignore les images intermédiaires d'animation.

## Interface Utilisateur et Commandes

Les gestionnaires situés dans `source/ui` segmentent la fenêtre principale en domaines découplés et autonomes.

| Module | Rôle |
| --- | --- |
| `GerenciadorBotoes` | Traite les actions de sélection de dossier, d'analyse, d'exportation, de purge et de mise en pause. |
| `GerenciadorBotoesUI` | Création et agencement visuel des boutons de la barre d'outils. |
| `GerenciadorMenusUI` | Construction des menus (Fichier, Configuration, Options, Couleurs, Colonnes, Export, Langues, À propos, Manuel, Aide). |
| `GerenciadorEventosUI` | Gestion des filtres, visibilité des colonnes, règles de coloration, changement de langue et réinitialisation. |
| `GerenciadorProgressoUI` | Barre de progression et affichage des messages d'état du scan. |
| `GerenciadorEstatisticasUI` | Contrôleur du dialogue de statistiques, sélection de graphiques, génération, actualisation et export. |
| `GerenciadorDados` | Exportation multiformat et manipulation des données brutes. |
| `GerenciadorEventosArquivo` | Acheminement des événements du système de fichiers vers la couche graphique. |
| `GerenciadorMonitoramento` | Contrôle du démarrage, de l'arrêt, de la pause et de la reprise de la surveillance. |
| `GerenciadorLimpeza` | Nettoyage des événements surveillés et des structures temporaires. |
| `DialogoCores` | Configuration des couleurs par type d'opération et sélecteur de palettes avancé. |
| `LocalizadorQt` | Gestionnaire central des catalogues de traduction de l'application. |
| `TradutorMetadadosQt` | Traduction dynamique des opérations, types MIME, attributs, auteurs et permissions. |
| `OpcoesSobre`, `Manual` | Boîtes de dialogue d'information et manuel utilisateur. |

## Filtres et Outils

`source/tools` regroupe le moteur de filtrage et l'intégration du calendrier.

Composants :

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Responsabilités :

- Appliquer et réinitialiser les filtres actifs du tableau ;
- Synchroniser l'état des menus avec les paramètres de filtrage ;
- Mettre à jour le compteur d'événements correspondants en temps réel ;
- Réagir immédiatement aux changements de langue de l'interface ;
- Mémoriser l'état des cases à cocher entre les sessions ;
- Filtrer les événements par catégorie d'opération, plage de dates ou extension de fichier.

## Internationalisation (i18n)

L'architecture d'internationalisation réunit :

- Les fichiers sources de traduction : `source/locale/*.ts` ;
- Les catalogues binaires compilés : `source/locale/*.qm` ;
- Les utilitaires de compilation automatique : `compile_translations.*` ;
- La classe `Internacionalizador` ;
- La classe `LocalizadorQt` ;
- La classe `TradutorMetadadosQt` ;
- Les manuels d'utilisation traduits dans `source/ui/Manual` ;
- Les textes et documents légaux traduits dans `source/assets`.

Langues prises en charge :

- `pt_BR` (Portugais - Brésil)
- `en_US` (Anglais - États-Unis)
- `es_ES` (Espagnol - Espagne)
- `fr_FR` (Français - France)
- `it_IT` (Italien - Italie)
- `de_DE` (Allemand - Allemagne)

Lors d'un changement de langue à l'exécution, l'application retraduit dynamiquement l'ensemble des menus, en-têtes de colonnes, critères de filtres, graphiques actifs, boîtes de dialogue et lignes existantes du tableau.

## Ressources, Licences et Documents Légaux

Le répertoire `source/assets` est intégralement intégré lors du déploiement. Il contient :

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Les mentions énumèrent l'ensemble des modules Qt exploités et leurs conditions de redistribution open source. Les notes de version et documents légaux sont structurés par langue.

## Utilitaires Transversaux

| Fichier | Responsabilité |
| --- | --- |
| `ApplicationPathUtils.*` | Résolution des chemins de base, détection des ressources et lecture des fichiers texte. |
| `CaminhoPersistenteUtils.*` | Résolution du répertoire standard de données persistantes (`%LOCALAPPDATA%`). |
| `IconUtils.*` | Résolution des icônes compatibles avec les écrans haute densité (High-DPI). |
| `LogManager.*` | Journaliseur centralisé thread-safe, rotation horodatée des journaux et normalisation Unicode. |
| `TrialManager.*` | Gestion de la période d'essai, contrôle des licences et redirection vers la boutique. |

## Concurrence et Réactivité de l'Interface

Le projet adopte une gestion multithread rigoureuse :

- `std::thread` pour la surveillance continue et bas niveau du système de fichiers ;
- `QThread` pour l'analyse récursive des répertoires en arrière-plan ;
- `QTimer` pour les écritures différées (flushes), la synchronisation de l'interface et les retraductions ;
- `QtConcurrent` pour les traitements asynchrones lourds ;
- Rédacteurs asynchrones dédiés pour la persistance en base de données ;
- Caches en mémoire vive pour les palettes de couleurs, chaînes traduites, métadonnées et instantanés ;
- Variables atomiques sans verrou et verrous d'exclusion mutuelle (mutex) sur les files de l'observateur.

Bonnes pratiques architecturales :

- Toutes les modifications d'interface graphique doivent obligatoirement être effectuées sur le thread principal de Qt ;
- Les écritures en base de données ne doivent jamais bloquer la boucle d'événements (event loop) ;
- Les événements massifs doivent être regroupés et traités par lots ;
- Le rendu statistique doit privilégier `RasterChartView` dans la fenêtre principale de statistiques ;
- Les caches doivent être systématiquement invalidés lors d'un changement de langue, de thème, de colonnes ou de couleurs.

## Pipeline des Événements de Fichiers

```text
ReadDirectoryChangesW / scanner
  |
  v
Observador
  |
  +--> tampon d'événements
  +--> détection d'activité massive
  +--> gestionnaires : Ajouté / Supprimé / Modifié / Renommé / Déplacé
  |
  v
BaseEvento
  |
  +--> normalisation des données
  +--> extraction des métadonnées
  +--> écriture dans la base SQLite locale
  +--> notification de SecurityManager
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> filtres / couleurs / traductions
  +--> statistiques à la demande
```

## Pipeline des Statistiques

```text
L'utilisateur clique sur Statistiques
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
  +--> RasterChartView pour la fenêtre Statistiques
  +--> composants sur mesure pour diagrammes spécifiques
  |
  v
Dialogue de Statistiques
```

## Règles d'Évolution du Code

### Ajouter un nouveau graphique

1. Implémenter un générateur spécialisé dérivé de `BaseGerador` sous `source/data/GeradorEstatisticas`.
2. Déclarer le nouveau générateur dans `GeradorEstatisticas::inicializar_geradores`.
3. Assurer la production d'une charge `QVariantMap` stable et structurée.
4. Si le type de graphique est compatible avec le modèle générique, l'adapter dans `LegacyVariantChartAdapter`.
5. Si des formes visuelles inédites sont requises, enrichir `ChartLayoutEngine`.
6. Rendre le graphique disponible dans le panneau de sélection de `GerenciamentoEstatisticasUI`.
7. Mettre à jour les catalogues de traduction `.ts` et la documentation technique.

### Ajouter une nouvelle colonne au tableau

1. Mettre à jour les collecteurs d'événements et extracteurs de métadonnées.
2. Déclarer l'identifiant et l'intitulé de la colonne dans `GerenciadorColunas`.
3. Ajouter la traduction correspondante dans `LocalizadorQt` et les fichiers `.ts`.
4. Valider le surlignage par couleur, la visibilité, le tri et les modules d'exportation.

### Ajouter une nouvelle fonctionnalité de sécurité

1. Développer le composant sous `source/security`.
2. L'intégrer à la façade `SecurityManager`.
3. Déclarer le schéma de base de données nécessaire dans `ensure_schema`.
4. Utiliser `SecurityDbWriter` pour assurer des écritures asynchrones.
5. Créer les boîtes de dialogue et vues dans `secui_*` si une interface graphique est nécessaire.
6. Mettre à jour la génération de package de preuves si les données sont auditables.

### Ajouter une nouvelle langue

1. Créer les fichiers `.ts` et `.qm` dans `source/locale`.
2. Fournir les documentations et textes traduits dans `source/assets` et `source/ui/Manual`.
3. Déclarer la nouvelle langue dans `LocalizadorQt`.
4. Mettre à jour les menus de sélection de langue dans `GerenciadorMenusUI`.
5. Contrôler les en-têtes de tableau, filtres, données existantes et manuels d'utilisation.

## Points de Vigilance

- `InterfaceMonitor` centralise de nombreuses références ; toute nouvelle fonctionnalité doit éviter d'accroître le couplage direct lorsqu'un gestionnaire dédié possède déjà la compétence métier.
- `QVariantMap` représente le contrat d'échange historique entre événements, statistiques et interface graphique ; toute modification de clé exige une migration rigoureuse.
- Le moteur de graphiques utilise `RasterChartView` pour garantir une parfaite stabilité multi-écrans dans la boîte `Estatísticas`.
- CMake référence le dossier `source/data/Charts` avec `GLOB_RECURSE` ; tout nouveau fichier ajouté dans ce dossier est automatiquement intégré à la compilation.
- Le déploiement copie l'intégralité du répertoire `source/assets` ; les mentions légales et traductions doivent rester cohérentes à chaque livraison.
- Ce document reflète la structure exacte du code de l'espace de travail actuel.

## Référence Rapide des Fichiers

| Sujet | Fichiers Clés |
| --- | --- |
| Point d'entrée | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Fenêtre principale | `source/gui/ic_01_InterfaceMonitor.*` |
| Traduction et i18n | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Surveillance | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Base de données d'événements | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Gestion du tableau | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Extraction des métadonnées | `source/services/GerenciamentoMetadados/*` |
| Sécurité forensique | `source/security/*` |
| Génération statistique | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Moteur graphique natif | `source/data/Charts/*` |
| Interface et dialogues | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filtres et calendrier | `source/tools/*` |
| Ressources et licences | `source/assets/*` |
| Utilitaires généraux | `source/utils/*` |

</details>

---

<h2 id="itit">🇮🇹 Italiano (IT)</h2>

<details>
<summary>Fai clic per espandere l'Architettura in italiano</summary>

# Architettura Tecnica - Linceu Lighthouse C++/Qt6

Questo documento descrive l'architettura corrente del progetto `Linceu_Lighthouse_cpp` a partire dall'albero del codice sorgente presente in questo workspace. L'obiettivo è fungere da riferimento per la manutenzione, l'evoluzione delle funzionalità, l'audit delle dipendenze e l'integrazione tra i moduli.

## Panoramica e Visione Architetturale

**Linceu Lighthouse** è una piattaforma desktop nativa per Windows scritta in C++17 con Qt 6, progettata per offrire visibilità assoluta, cronologia strutturata, intelligence dei file senza intelligenza artificiale e audit di sicurezza locale.

L'architettura è guidata dal principio fondamentale **Local-First / Sovereign Privacy**: tutti i dati degli eventi, i metadati arricchiti, i database SQLite, le cache e i report vengono elaborati e memorizzati rigorosamente sul computer locale, senza alcuna connessione esterna, senza dipendenze dal cloud, senza telemetria e senza modelli di linguaggio (LLM).

Sotto il profilo architetturale e funzionale, il sistema opera come una **pipeline locale di dati** suddivisa in 7 domini cooperanti:

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    PIPELINE DEI DATI - LINCEU LIGHTHOUSE                │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. DOMINIO DI MONITORAGGIO (API Native Windows / ReadDirectoryChangesW)
                                     ↓
 2. DOMINIO DEGLI EVENTI E CORRELAZIONE (Normalizzazione / Rilevamento Move & Rename)
                                     ↓
 3. DOMINIO DEI METADATI (30+ categorie, Magic Numbers, Hash Crittografici)
                                     ↓
 4. DOMINIO DELLA PERSISTENZA (SQLite Locale Asincrono / Batch Writers)
                                     ↓
 5. DOMINIO DI ANALYTICS E STATISTICHE (17 Grafici Nativi / Determinismo)
                                     ↓
 6. DOMINIO DELLA SICUREZZA E AUDIT (Baseline, Regole, Rischio, Prove)
                                     ↓
 7. DOMINIO DELLA PRESENTAZIONE (Interfaccia Qt6, Internazionalizzazione, Esportazione)
```

Ad alto livello, il sistema è strutturato nei seguenti moduli:

- **Bootstrap e Ciclo di Vita:** Inizializzazione dell'ambiente, configurazione OpenGL, schermata di avvio (splash) e procedura di fallback;
- **Finestra Principale `InterfaceMonitor`:** Orchestrazione centrale e integrazione diretta tra i servizi in background e l'interfaccia utente;
- **Servizi di Osservazione, Scanner e Bufferizzazione:** Acquisizione multithread resiliente a picchi intensi di eventi;
- **Normalizzazione, Metadati e Persistenza:** Arricchimento approfondito e scrittura asincrona su SQLite locale;
- **Livello di Sicurezza, Integrità e Forense:** Linee di base, regole per il punteggio di rischio, quarantena e prove verificabili;
- **Infrastruttura Grafica e Statistiche:** Motore interno `Charts` dotato di 17 generatori di diagrammi nativi;
- **Internazionalizzazione e Risorse (Assets):** Supporto nativo per 6 lingue con ricaricamento dinamico a runtime;
- **Utilità Trasversali:** Gestione dei percorsi, log centralizzati, risoluzione delle icone e integrazione con la shell di Windows.

## Dipendenze Effettive

### Linguaggio, build e piattaforma

- C++17 specificato in `CMakeLists.txt`.
- CMake con `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- Preset principale: `qt6-mingw-debug`, con MinGW e Qt 6 su Windows.
- Applicazione GUI Windows creata come target `WIN32`.
- File di risorse nativo: `source/Linceu_Lighthouse_cpp.rc`.
- Distribuzione post-compilazione con `windeployqt`, copia degli `assets` e distribuzione dei file di traduzione compilati `.qm`.

### Moduli Qt Collegati

La compilazione collega esplicitamente:

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

Non vi è alcuna dipendenza diretta da librerie esterne per il rendering di grafici, visualizzazione 3D, server HTTP, broker di messaggi, tastiere virtuali, compositori Wayland o autorizzazioni di rete. L'intero rendering statistico è nativo e interno al progetto.

### Librerie Native di Windows

Su Windows, il target principale collega inoltre:

- `wintrust`
- `version`
- `crypt32`

Queste librerie supportano la verifica delle firme digitali, l'estrazione dei metadati dagli eseguibili e le operazioni crittografiche per il sottosistema di sicurezza.

### Variabili d'Ambiente Operative Rilevanti

- `LINCEU_OPENGL_MODE=auto`: comportamento predefinito.
- `LINCEU_OPENGL_MODE=software`: forza il rendering OpenGL software.
- `LINCEU_OPENGL_MODE=desktop`: richiede il contesto OpenGL desktop.
- `LINCEU_OPENGL_MODE=angle`: richiede il layer di traduzione ANGLE quando disponibile.

La procedura di avvio abilita inoltre la condivisione dei contesti OpenGL e configura `QSurfaceFormat` con doppio buffer (double buffering) e antialiasing.

## Mappa delle Directory

| Percorso | Ruolo |
| --- | --- |
| `main.cpp` | Punto di ingresso minimale. Invoca `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Bootstrap principale, configurazione grafica, registrazione QML, traduzioni, finestra principale e writer del database. |
| `source/src_02_SplashAppStarter.*` | Flusso di avvio alternativo con splash screen animato, barra di avanzamento e caricamento visivo. |
| `source/gui` | Finestra principale, inizializzazione della UI, configurazione e internazionalizzatore. |
| `source/ui` | Gestori di pulsanti, barre dei menu, statistiche, eventi UI, avanzamento, dati, manuale, finestre di dialogo e traduzione dei metadati. |
| `source/services` | Osservazione delle cartelle, scanner, eventi, database degli eventi, metadati e modello di tabella. |
| `source/security` | Profili di sicurezza, baseline, regole, punteggio di rischio, attribuzione forense, quarantena, prove ed export di sicurezza. |
| `source/data` | Generazione statistica e motore interno di grafici e diagrammi. |
| `source/tools` | Sottosistema dei filtri, calendario e controlli amministrativi dei filtri. |
| `source/locale` | File sorgente `.ts`, cataloghi compilati `.qm` e script di compilazione delle traduzioni. |
| `source/assets` | Icone, note di rilascio, comunicazioni di terze parti, EULA, informativa sulla privacy, copyright, licenze e testi di supporto. |
| `source/utils` | Risoluzione dei percorsi, icone, log, periodo di prova e utilità generali. |
| `AdvancedDocumentation.qml` | Documentazione visiva interattiva e navigabile dell'architettura. |
| `ARCHITECTURE.md` | Questo documento di architettura tecnica. |

## Flusso di Inizializzazione

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> imposta la cartella di lavoro corrente (cwd)
  +--> tenta SplashAppStarter
  |      |
  |      +--> istanzia QApplication
  |      +--> configura il backend grafico
  |      +--> inizializza il sistema di traduzione
  |      +--> visualizza la splash screen
  |      +--> istanzia InterfaceMonitor
  |      +--> avvia DatabaseWriter se è presente db_path
  |
  +--> fallback di avvio senza splash
         |
         +--> configura il backend grafico
         +--> istanzia QApplication
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> imposta l'AppUserModelID di Windows
         +--> assegna l'icona dell'applicazione
         +--> inizializza Internacionalizador
         +--> istanzia InterfaceMonitor
         +--> istanzia DatabaseWriter (modalità immediata)
         +--> show()
         +--> app.exec()
```

Il bootstrap separa con precisione le responsabilità principali:

- `ajustar_cwd()` previene errori causati da percorsi relativi verso risorse, traduzioni o file del database;
- `configurar_backend_grafico()` prepara i formati di superficie OpenGL prima della creazione di qualsiasi widget;
- `source::charts_lgpl::register_qml_types()` registra `QuickChartItem` come tipo QML istanziabile;
- `source::security::pg::prepare_runtime()` predispone il runtime di persistenza locale per la sicurezza;
- `Internacionalizador::inicializar_sistema_traducao()` carica i dizionari di traduzione Qt e dell'applicazione;
- `DatabaseWriter::get_instance(dbPath)` inizializza il thread di scrittura asincrono degli eventi.

## Finestra Principale

`source::gui::InterfaceMonitor` è il fulcro centrale dell'applicazione. Eredita da `QMainWindow` e aggrega i puntatori a tutti i gestori di alto livello.

Responsabilità primarie:

- Creare e configurare il layout dell'interfaccia utente;
- Esporre comandi `Q_INVOKABLE` per menu, pulsanti e finestre di dialogo;
- Mantenere la sincronizzazione in tempo reale tra tabella degli eventi, filtri, monitoraggio, statistiche e modulo di sicurezza;
- Propagare dinamicamente i cambi di lingua;
- Ricevere e inserire gli eventi in flusso continuo (streaming);
- Applicare i colori personalizzati e la visibilità delle colonne;
- Coordinare la gerarchia e l'ordinamento z-order delle finestre collegate;
- Chiudere ordinatamente le finestre dipendenti all'uscita dall'applicazione.

Componenti aggregati principali:

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

## Gerarchia delle Finestre

La finestra principale gestisce una gerarchia di finestre di dialogo associate tramite proprietà dinamiche Qt, in particolare `interface_principal`. Questa gerarchia comprende il manuale, la finestra Informazioni, l'aiuto, i selettori di colore, il selettore avanzato e la finestra delle statistiche.

Obiettivi:

- Mantenere l'interfaccia principale facilmente ripristinabile quando l'utente alterna il focus tra le finestre;
- Ordinare le finestre di dialogo collegate senza costringerle a essere tutte figlie dirette o modali rigide;
- Evitare che la finestra `Estatísticas` rimanga in primo piano sopra l'interfaccia principale quando è inattiva;
- Chiudere automaticamente tutte le finestre secondarie alla chiusura della finestra principale.

Meccanismi adottati:

- `eventFilter` dedicato per gli eventi di attivazione, focus, visualizzazione/nascondimento e mouse;
- Proprietà dinamiche per associare formalmente i dialoghi a `InterfaceMonitor`;
- Utilizzo controllato di `raise()` e `activateWindow()` solo su azione esplicita dell'utente;
- Sincronizzazione pianificata e differita per evitare ricorsioni durante i cambi di stato della finestra;
- Isolamento delle finestre di dialogo transitorie che non devono far parte della gerarchia.

## Servizi di Monitoraggio ed Eventi

### Osservatore (Observador)

`source::services::Observador` incapsula il monitoraggio del file system in background. Combina flag atomici, code thread-safe, blocchi (mutex), un thread dedicato, uno scanner e gestori di eventi.

Supervisiona:

- Percorso monitorato e sottoalberi di directory;
- Cicli di avvio, arresto, pausa e ripresa;
- Rilevamento di picchi massivi di operazioni;
- Bufferizzazione degli eventi in code thread-safe;
- Tracciamento degli errori e overrun dei buffer del sistema operativo;
- Stima degli eventi persi;
- Snapshot degli stati precedenti;
- Deduplicazione degli eventi ricorrenti ravvicinati;
- Integrazione e passaggio di consegne con lo scanner iniziale.

### BaseEvento (Gestore Base degli Eventi)

`source::services::BaseEvento` normalizza i segnali del sistema operativo e gestisce la persistenza dei dati. Crea lo schema del database, registra i singoli eventi, gestisce le eliminazioni temporanee, correla gli spostamenti e aggiorna il modello della tabella.

Operazioni fondamentali:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Sottomoduli di Servizio

| Sottomodulo | Responsabilità |
| --- | --- |
| `GerenciamentoBaseEvento` | Database degli eventi, inserimento, eliminazione, spostamento, snapshot e writer asincrono. |
| `GerenciamentoDiretorioScanner` | Scansione iniziale e per lotti, coda dei file, thread di lavoro e notifica di avanzamento. |
| `GerenciamentoEventoModificado` | Raggruppamento, debounce dinamico, gestione delle modifiche normali e massive e cache dei metadati. |
| `GerenciamentoEventoMovido` | Correlazione tra eventi di eliminazione e creazione, worker di spostamento e aggiornamento tabella. |
| `GerenciamentoMetadados` | Estrazione specializzata per categoria di file, cache in memoria, formattazione tabella e batch. |
| `GerenciamentoObservador` | Ciclo di vita dell'osservatore (avvio/arresto/pausa/ripresa), buffer, gestione errori OS e ripristino. |
| `GerenciamentoTabela` | Modello di tabella Qt, colori, temi, colonne, intestazioni, ritraduzione ed elaborazione in background. |

## Persistenza

Il progetto impiega `Qt::Sql` e i driver Qt per tutta la persistenza locale. La pipeline principale degli eventi è gestita da `BaseEvento` e `DatabaseWriter`. Il livello di sicurezza include helper in `secore_01_postgres_utils.*`, mentre il runtime predefinito opera su un database locale `QSQLITE`.

Pattern di persistenza adottati:

- Connessioni al database con nomi univoci per singolo thread;
- Scrittori asincroni a lotti per eliminare qualsiasi blocco della GUI;
- Creazione automatica dello schema su richiesta;
- Rappresentazione in memoria degli eventi tramite `QVariantMap` per garantire compatibilità con Qt e statistiche;
- Tabelle ausiliarie e snapshot dedicati a baseline, regole di sicurezza, quarantena, prove e sessioni di audit.

## Modello e Tabella degli Eventi

`GerenciadorTabela` coordina l'esperienza visiva del log di audit. `EventTableModel` è il modello Qt personalizzato che alimenta la vista `QTableView` con il flusso continuo di eventi.

Funzionalità principali:

- Inserimento ad alto rendimento in testa (prepend) e in coda (append) a lotti;
- Sostituzione completa del set di eventi;
- Generazione immediata di snapshot dei dati;
- Ordinamento su più colonne;
- Cache dei colori per tipologia di operazione;
- Intestazioni delle colonne tradotte dinamicamente;
- Gestione della visibilità e disposizione personalizzata delle colonne;
- Adattamento automatico ai temi chiaro e scuro di Windows;
- Ritraduzione differita al cambio di lingua a runtime;
- Elaborazione complessa demandata a thread di background;
- Sincronizzazione bidirezionale con `QTableView`.

## Estrazione dei Metadati

Il sottosistema `GerenciamentoMetadados` è completo e specializzato per ciascuna categoria di file.

Ambiti e formati gestiti:

- Codice sorgente;
- Immagini;
- File audio;
- Flussi video;
- Documenti di testo;
- Fogli di calcolo;
- Presentazioni;
- Database;
- Eseguibili e binari PE;
- File temporanei;
- Archivi compressi;
- File di backup;
- File di log;
- File di configurazione;
- Archiviazione strutturata composta OLE;
- Formati di dati strutturati (JSON, XML, ecc.);
- Estrazione approfondita di metadati;
- Identificazione del tipo MIME reale mediante analisi del contenuto;
- Dimensioni, conversioni di unità e conteggio esatto dei byte;
- Attributi, autore, dimensioni visive, durata, bitrate e permessi di protezione.

Gestisce inoltre la configurazione delle colonne della tabella, l'invalidazione della cache per le cartelle aggiornate, le code di estrazione, le callback di notifica e il refresh visivo.

## Sicurezza Forense

`source::security::SecurityManager` costituisce la facciata del sottosistema di sicurezza. Centralizza contesto, schemi del database, arricchimento degli eventi, gestione degli avvisi, correlazione delle sessioni, quarantena, verifica dell'integrità tramite baseline e osservatori di sicurezza.

Componenti fondamentali:

| File | Ruolo |
| --- | --- |
| `base_01_baseline_criptografico.*` | Baseline crittografica, hash SHA-256 e snapshot di verifica dell'integrità. |
| `exec_01_inteligencia_executaveis.*` | Analisi dei file eseguibili, intestazioni PE e metadati delle firme digitali. |
| `sess_01_correlacao_sessoes.*` | Correlazione temporale delle sessioni utente e dell'attività di sistema. |
| `sdiff_01_diff_conteudo.*` | Confronto delle versioni e differenze a livello di contenuto. |
| `attr_01_atribuicao_forense.*` | Attribuzione e contesto forense (processi e utenti). |
| `agre_01_motor_regras.*` | Motore di regole di conformità e sicurezza locale. |
| `risk_01_score_risco.*` | Calcolo deterministico del punteggio di rischio. |
| `quar_01_quarentena.*` | Gestione della quarantena locale, manipolazione dei blob e ripristino. |
| `evid_01_exportacao_evidencia.*` | Creazione ed esportazione di pacchetti di prove verificabili. |
| `mdir_01_multi_diretorio.*` | Profili di monitoraggio e cartelle multiple supervisionate. |
| `secore_05_SecurityDbWriter.*` | Scrittore asincrono dedicato al database di sicurezza. |

Finestre di dialogo di sicurezza:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Generazione delle Statistiche

`source::data::GeradorEstatisticas` riceve `db_path`, il gestore di localizzazione e l'interfaccia principale. Registra generatori specializzati derivati da `BaseGerador`.

Grafici e diagrammi supportati:

- Grafico a torta delle operazioni;
- Grafico a barre per tipo di file;
- Cronologia temporale delle operazioni (timeline);
- Mappa ad albero per categorie (treemap);
- Istogramma delle attività per fasce orarie;
- Curva di distribuzione di Pareto;
- Grafici a linee e analisi a gruppi (cluster);
- Diagrammi a scatola e baffi (boxplot per distribuzione, operazione, tipo, ora, dimensione e volume);
- Grafico radar degli eventi;
- Mappa termica temporale (heatmap);
- Diagramma di dispersione (scatter plot);
- Diagrammi di Sankey;
- Radar multidimensionale;
- Grafico a punti (dotplot);
- Diagramma di Sankey per evento e percorso;
- Diagramma di Sankey per tipo e percorso;
- Albero gerarchico delle directory.

Tutte le strutture di dati prodotte mantengono piena conformità con `QVariant`, `QVariantMap` e `QVariantList`.

## Motore di Rendering di Grafici e Diagrammi

Il sottosistema in `source/data/Charts` sostituisce qualsiasi libreria grafica di terze parti con una soluzione interna C++/Qt altamente performante.

Livelli di rendering:

```text
QVariantMap legacy
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
  +--> RasterChartView    (QWidget + QPainter, utilizzato nella finestra Statistiche)
  +--> WidgetChartView    (QOpenGLWidget + sovrapposizione QPainter)
  +--> QuickChartItem     (QQuickItem + Qt Quick Scene Graph)
```

File principali:

- `ch_01_ChartLayoutEngine.*`: Calcola geometria 2D/3D, etichette, legende, hit-testing e primitive di scena.
- `ch_02_ChartWidgetFactory.*`: Crea i widget visivi a partire da modelli tipizzati o strutture legacy.
- `ch_03_LegacyVariantChartAdapter.*`: Adatta le strutture `QVariantMap` al modello fortemente tipizzato.
- `ch_04_QmlRegistration.*`: Registra `QuickChartItem` all'interno del motore QML.
- `ch_05_QuickChartItem.*`: Backend Qt Quick Scene Graph per rendering accelerato via hardware.
- `ch_06_WidgetChartView.*`: Canvas con accelerazione hardware basato su `QOpenGLWidget`.
- `ch_07_LgplChartModel.h`: Strutture dati fondamentali per scene, geometrie e colori.
- `ch_08_RasterChartView.*`: Canvas raster puro basato su `QWidget` con antialiasing di precisione.

Scelte architetturali fondamentali:

- `RasterChartView` viene impiegato prioritariamente per la finestra `Estatísticas` per evitare instabilità legate ai contesti OpenGL in configurazioni multischermo;
- Viene utilizzato `QPainter` con antialiasing per garantire una nitidezza vettoriale impeccabile;
- `WidgetChartView` rimane disponibile per pipeline di rendering OpenGL continuo;
- `QuickChartItem` permette l'integrazione fluida con QML e animazioni tramite `transitionProgress`;
- L'esportazione delle immagini utilizza un ciclo di rendering deterministico, ignorando i fotogrammi transitori di animazione.

## Moduli UI e Comandi

I gestori in `source/ui` suddividono la finestra principale in domini modulari e disaccoppiati.

| Modulo | Ruolo |
| --- | --- |
| `GerenciadorBotoes` | Gestisce le azioni di selezione cartella, analisi, esportazione, pulizia e pausa. |
| `GerenciadorBotoesUI` | Creazione e disposizione grafica dei pulsanti della barra degli strumenti. |
| `GerenciadorMenusUI` | Creazione delle barre dei menu (File, Configurazione, Opzioni, Colori, Colonne, Esportazione, Lingue, Informazioni, Manuale, Aiuto). |
| `GerenciadorEventosUI` | Gestione dei filtri, visibilità delle colonne, colori, cambio di lingua e ripristino. |
| `GerenciadorProgressoUI` | Barra di avanzamento e messaggi sullo stato della scansione. |
| `GerenciadorEstatisticasUI` | Controller del dialogo statistiche, pannello di selezione dei grafici, generazione, aggiornamento ed export. |
| `GerenciadorDados` | Esportazione multiformato e manipolazione dei dati. |
| `GerenciadorEventosArquivo` | Inoltro degli eventi del file system verso l'interfaccia utente. |
| `GerenciadorMonitoramento` | Controllo dell'avvio, arresto, pausa e ripresa del monitoraggio. |
| `GerenciadorLimpeza` | Pulizia degli eventi monitorati e delle strutture temporanee. |
| `DialogoCores` | Mappatura dei colori per tipo di operazione e selettore avanzato di tavolozze. |
| `LocalizadorQt` | Gestore centrale dei cataloghi di traduzione dell'applicazione. |
| `TradutorMetadadosQt` | Traduzione dinamica di operazioni, tipi MIME, attributi, autori e permessi. |
| `OpcoesSobre`, `Manual` | Finestre di dialogo informative e manuale utente. |

## Filtri e Strumenti

`source/tools` centralizza le funzionalità di filtraggio e l'integrazione con il calendario.

Componenti:

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Responsabilità:

- Applicare e reimpostare i filtri attivi della tabella;
- Sincronizzare lo stato del menu principale con i parametri dei filtri;
- Aggiornare in tempo reale il conteggio degli eventi corrispondenti;
- Rispondere istantaneamente ai cambi di lingua dell'interfaccia;
- Salvare lo stato delle caselle di controllo tra le sessioni;
- Verificare e filtrare gli eventi per categoria di operazione, intervallo di date o estensione.

## Internazionalizzazione (i18n)

L'architettura di internazionalizzazione integra:

- File sorgente di traduzione: `source/locale/*.ts`;
- Cataloghi binari compilati: `source/locale/*.qm`;
- Strumenti di compilazione automatica: `compile_translations.*`;
- Classe `Internacionalizador`;
- Classe `LocalizadorQt`;
- Classe `TradutorMetadadosQt`;
- Manuali utente localizzati in `source/ui/Manual`;
- Testi e documenti legali localizzati in `source/assets`.

Lingue integrate:

- `pt_BR` (Portoghese - Brasile)
- `en_US` (Inglese - USA)
- `es_ES` (Spagnolo - Spagna)
- `fr_FR` (Francese - Francia)
- `it_IT` (Italiano - Italia)
- `de_DE` (Tedesco - Germania)

Quando la lingua cambia a runtime, l'applicazione ritraduce dinamicamente menu, intestazioni di tabella, opzioni di filtro, statistiche correnti, finestre di dialogo e righe caricate nella tabella.

## Risorse, Licenze e Documentazione Legale

La cartella `source/assets` viene inclusa integralmente durante la distribuzione. Contiene:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Le note enumerano tutti i moduli Qt utilizzati e le relative condizioni di ridistribuzione open source. Le note di rilascio e i documenti legali sono strutturati per singola lingua.

## Utilità Trasversali

| File | Responsabilità |
| --- | --- |
| `ApplicationPathUtils.*` | Risoluzione dei percorsi base, rilevamento degli asset e lettura dei file di testo. |
| `CaminhoPersistenteUtils.*` | Risoluzione della cartella standard per i dati persistenti (`%LOCALAPPDATA%`). |
| `IconUtils.*` | Caricamento di icone compatibili con schermi ad alta densità (High-DPI). |
| `LogManager.*` | Registratore di log centralizzato e thread-safe, rotazione con timestamp e normalizzazione Unicode. |
| `TrialManager.*` | Gestione del periodo di prova, controllo delle licenze e reindirizzamento allo store. |

## Concorrenza e Reattività dell'Interfaccia

Il progetto adotta una rigorosa strategia multithreading:

- `std::thread` per il monitoraggio continuo a basso livello del file system;
- `QThread` per la scansione ricorsiva delle cartelle in background;
- `QTimer` per scritture differite (flush), sincronizzazione della UI e ritraduzioni;
- `QtConcurrent` per elaborazioni ausiliarie pesanti;
- Scrittori asincroni dedicati per il salvataggio su database;
- Cache veloci in memoria per tavolozze di colori, stringhe tradotte, metadati e snapshot;
- Variabili atomiche lock-free e blocchi di esclusione mutua (mutex) nelle code dell'osservatore.

Linee guida architetturali:

- Le modifiche all'interfaccia utente devono essere eseguite tassativamente sul thread principale di Qt;
- Le operazioni su database non devono mai bloccare l'event loop della GUI;
- Gli eventi massivi devono essere raggruppati ed elaborati a lotti;
- Il rendering dei grafici deve privilegiare `RasterChartView` nella finestra principale delle statistiche;
- Le cache devono essere invalidate a fronte di qualsiasi modifica di lingua, tema, layout delle colonne o colori.

## Pipeline degli Eventi di File

```text
ReadDirectoryChangesW / scanner
  |
  v
Observador
  |
  +--> buffer degli eventi
  +--> rilevamento di attività massive
  +--> gestori: Aggiunto / Eliminato / Modificato / Ridenominato / Spostato
  |
  v
BaseEvento
  |
  +--> normalizza i dati dell'evento
  +--> estrae i metadati
  +--> scrive nel database SQLite locale
  +--> invia notifica a SecurityManager
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> filtri / colori / traduzioni
  +--> statistiche su richiesta
```

## Pipeline delle Statistiche

```text
L'utente fa clic su Statistiche
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
  +--> RasterChartView per la finestra Statistiche
  +--> widget personalizzati per diagrammi specifici
  |
  v
Finestra delle Statistiche
```

## Regole per l'Evoluzione del Codice

### Aggiungere un nuovo grafico

1. Creare un generatore specializzato derivato da `BaseGerador` in `source/data/GeradorEstatisticas`.
2. Registrarlo in `GeradorEstatisticas::inicializar_geradores`.
3. Garantire una struttura di output `QVariantMap` stabile e ben definita.
4. Se compatibile con il modello comune, adattarlo in `LegacyVariantChartAdapter`.
5. Se necessita di geometrie personalizzate, estendere `ChartLayoutEngine`.
6. Renderlo selezionabile nel pannello in `GerenciamentoEstatisticasUI`.
7. Aggiornare i file di traduzione `.ts` e la documentazione tecnica.

### Aggiungere una nuova colonna alla tabella

1. Aggiornare le sorgenti degli eventi e i moduli di estrazione dei metadati.
2. Registrare identificatore e intestazione della colonna in `GerenciadorColunas`.
3. Aggiungere le traduzioni in `LocalizadorQt` e nei file `.ts`.
4. Verificare evidenziazione a colori, visibilità, ordinamento e funzioni di esportazione.

### Aggiungere una nuova funzionalità di sicurezza

1. Sviluppare il componente all'interno di `source/security`.
2. Integrarlo nella facciata `SecurityManager`.
3. Dichiarare lo schema del database necessario in `ensure_schema`.
4. Utilizzare `SecurityDbWriter` per scritture asincrone dove opportuno.
5. Creare viste e finestre di dialogo in `secui_*` se è richiesta un'interfaccia grafica.
6. Aggiornare la generazione del pacchetto di prove se i dati sono soggetti ad audit.

### Aggiungere una nuova lingua

1. Creare i file `.ts` e `.qm` in `source/locale`.
2. Aggiungere i testi e i manuali tradotti in `source/assets` e `source/ui/Manual`.
3. Registrare il codice della lingua in `LocalizadorQt`.
4. Aggiornare i menu di selezione lingua in `GerenciadorMenusUI`.
5. Verificare intestazioni di tabella, filtri, dati già caricati e manuali utente.

## Punti di Attenzione

- `InterfaceMonitor` centralizza numerosi gestori; le nuove funzionalità devono evitare di incrementare l'accoppiamento diretto quando un gestor dedicato presidia già l'ambito specifico.
- `QVariantMap` è il contratto storico di interscambio dati tra eventi, statistiche e interfaccia; la modifica delle chiavi richiede una migrazione attenta.
- Il sottosistema grafico adotta `RasterChartView` per assicurare stabilità multischermo nella finestra `Estatísticas`.
- CMake include i file in `source/data/Charts` tramite `GLOB_RECURSE`; ogni nuovo file in questa directory viene compilato automaticamente.
- La distribuzione copia l'intero contenuto di `source/assets`; le note legali e le traduzioni devono rimanere coerenti a ogni rilascio.
- Questo documento descrive la configurazione reale del codice sorgente nel workspace corrente.

## Riferimento Rapido dei File

| Argomento | File Principali |
| --- | --- |
| Punto di ingresso | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Finestra principale | `source/gui/ic_01_InterfaceMonitor.*` |
| Traduzione e i18n | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Monitoraggio | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Database eventi | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Gestione tabella | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Estrazione metadati | `source/services/GerenciamentoMetadados/*` |
| Sicurezza forense | `source/security/*` |
| Generazione statistiche | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Motore grafico nativo | `source/data/Charts/*` |
| Interfaccia e dialoghi | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filtri e calendario | `source/tools/*` |
| Risorse e licenze | `source/assets/*` |
| Utilità generali | `source/utils/*` |

</details>

---

<h2 id="dede">🇩🇪 Deutsch (DE)</h2>

<details>
<summary>Klicken Sie hier, um die Architektur auf Deutsch zu erweitern</summary>

# Technische Architektur - Linceu Lighthouse C++/Qt6

Dieses Dokument beschreibt die aktuelle Architektur des Projekts `Linceu_Lighthouse_cpp` auf Basis der im Workspace vorhandenen Codebasis. Es dient als Referenz für Wartung, Weiterentwicklung, Abhängigkeitsprüfung und Modulintegration.

## Überblick und Architekturkonzept

**Linceu Lighthouse** ist eine native Windows-Desktop-Plattform, entwickelt in C++17 mit Qt 6, die absolute Transparenz, strukturierte Historie, KI-freie Datei-Intelligenz und lokale Sicherheitsaudits bietet.

Die Architektur basiert strikt auf dem **Local-First / Sovereign Privacy**-Prinzip: Sämtliche Ereignisdaten, angereicherten Metadaten, SQLite-Datenbanken, Caches und Berichte werden ausschließlich lokal auf dem Endgerät verarbeitet und gespeichert — ohne externe Netzwerkverbindungen, ohne Cloud-Abhängigkeiten, ohne Telemetrie und ohne Large Language Models (LLMs).

Architektonisch und funktional agiert das System als **lokale Datenpipeline**, unterteilt in 7 kooperierende Domänen:

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    DATEN-PIPELINE - LINCEU LIGHTHOUSE                   │
└─────────────────────────────────────────────────────────────────────────┘
                                     │
 1. ÜBERWACHUNGSDOMÄNE (Native Windows-APIs / ReadDirectoryChangesW)
                                     ↓
 2. EREIGNIS- UND KORRELATIONSDOMÄNE (Normalisierung / Move- & Rename-Erkennung)
                                     ↓
 3. METADATEN-DOMÄNE (30+ Kategorien, Magic Numbers, Krypto-Hashes)
                                     ↓
 4. PERSISTENZ-DOMÄNE (Asynchrones lokales SQLite / Batch-Writer)
                                     ↓
 5. ANALYSE- UND STATISTIKDOMÄNE (17 native Diagramme / Determinismus)
                                     ↓
 6. SICHERHEITS- UND AUDIT-DOMÄNE (Baseline, Regeln, Risiko, Beweismittel)
                                     ↓
 7. PRÄSENTATIONSDOMÄNE (Qt6-Oberfläche, Internationalisierung, Export)
```

Auf hoher Abstraktionsebene gliedert sich das System in folgende Bereiche:

- **Bootstrap und Lebenszyklus:** Umgebungseinrichtung, OpenGL-Konfiguration, Splash-Screen und Ausweichmechanismen (Fallback);
- **Hauptfenster `InterfaceMonitor`:** Zentrale Ablaufsteuerung und nahtlose Verknüpfung von Hintergrunddiensten und grafischer Oberfläche;
- **Beobachtungs-, Scanner- und Pufferungsdienste:** Multithread-Erfassung, robust gegenüber massiven Ereignisspitzen;
- **Normalisierung, Metadaten und Persistenz:** Tiefgehende Datenanreicherung und asynchrones Schreiben in SQLite;
- **Sicherheits-, Integritäts- und Forensik-Ebene:** Kryptografische Baselines, Risikobewertungsregeln, Quarantäne und Revisionsnachweise;
- **Grafik- und Statistik-Infrastruktur:** Proprietäre `Charts`-Rendering-Engine mit 17 nativen Diagrammgeneratoren;
- **Internationalisierung und Ressourcen (Assets):** Native Unterstützung für 6 Sprachen mit dynamischer Umschaltung zur Laufzeit;
- **Übergreifende Hilfsprogramme:** Pfadverwaltung, zentralisierte Protokollierung (Logging), Symbolauflösung und Windows-Shell-Integration.

## Tatsächliche Abhängigkeiten

### Programmiersprache, Buildsystem und Plattform

- C++17 festgelegt in `CMakeLists.txt`.
- CMake mit `CMAKE_AUTOMOC` und `CMAKE_AUTORCC`.
- Primäres Build-Preset: `qt6-mingw-debug` unter Nutzung von MinGW und Qt 6 auf Windows.
- Windows-GUI-Anwendung konfiguriert als `WIN32`-Target.
- Native Ressourcendatei: `source/Linceu_Lighthouse_cpp.rc`.
- Post-Build-Deployment über `windeployqt`, Kopieren von `assets` und Verteilung der kompilierten Sprachdateien `.qm`.

### Verknüpfte Qt-Module

Der Build bindet explizit folgende Module ein:

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

Es bestehen keinerlei direkte Abhängigkeiten von externen Bibliotheken für Diagrammerstellung, 3D-Visualisierung, HTTP-Server, Messaging-Broker, virtuelle Tastaturen, Wayland-Compositor oder Netzwerk-Authentifizierung. Das gesamte statistische Rendering erfolgt vollständig nativ und projektintern.

### Native Windows-Bibliotheken

Unter Windows bindet das Hauptziel zusätzlich ein:

- `wintrust`
- `version`
- `crypt32`

Diese Bibliotheken unterstützen die Prüfung digitaler Signaturen, das Auslesen von Metadaten ausführbarer Dateien sowie kryptografische Operationen im Sicherheitssubsystem.

### Relevante Umgebungsvariablen

- `LINCEU_OPENGL_MODE=auto`: Standardverhalten.
- `LINCEU_OPENGL_MODE=software`: erzwingt Software-OpenGL-Rendering.
- `LINCEU_OPENGL_MODE=desktop`: fordert Desktop-OpenGL-Kontext an.
- `LINCEU_OPENGL_MODE=angle`: fordert die ANGLE-Übersetzungsschicht an, falls verfügbar.

Die Initialisierungsroutine aktiviert zudem die gemeinsame Nutzung von OpenGL-Kontexten und konfiguriert `QSurfaceFormat` mit Doppelpufferung (Double Buffering) und Antialiasing.

## Verzeichnisstruktur

| Pfad | Rolle |
| --- | --- |
| `main.cpp` | Minimaler Einstiegspunkt. Ruft `source::iniciar_aplicacao` auf. |
| `source/src_01_InicializadorMain.*` | Haupt-Bootstrap, Grafikkonfiguration, QML-Registrierung, Übersetzung, Hauptfenster und Datenbank-Writer. |
| `source/src_02_SplashAppStarter.*` | Alternativer Startablauf mit animiertem Ladebildschirm, Fortschrittsanzeige und Ressourcen-Vorabladen. |
| `source/gui` | Hauptanwendungsfenster, UI-Initialisierung, Layout-Verwaltung und Internationalisierer. |
| `source/ui` | Manager für Schaltflächen, Menüs, Statistiken, UI-Ereignisse, Fortschritt, Daten, Handbuch, Dialoge und Metadaten-Übersetzung. |
| `source/services` | Verzeichnisbeobachtung, Scanner, Ereignisverarbeitung, Ereignisdatenbank, Metadaten und Tabellenmodell. |
| `source/security` | Sicherheitsprofile, Baselines, Richtlinien, Risikobewertung, forensische Zuordnung, Quarantäne, Beweise und Sicherheitsdialoge. |
| `source/data` | Statistische Berichterstellung und proprietäre Diagramm-Engine. |
| `source/tools` | Filtersubsystem, Kalenderverwaltung und administrative Filtersteuerung. |
| `source/locale` | Übersetzungsquelldateien (`.ts`), kompilierte Kataloge (`.qm`) und Kompilierungsskripte. |
| `source/assets` | Symbole, Versionshinweise, Drittanbieter-Hinweise, EULA, Datenschutzrichtlinie, Urheberrecht, Lizenzen und Hilfetexte. |
| `source/utils` | Pfadauflösung, Symbolverwaltung, Protokollierung, Testzeitraum und allgemeine Hilfsfunktionen. |
| `AdvancedDocumentation.qml` | Interaktive, navigierbare visuelle Architekturdokumentation. |
| `ARCHITECTURE.md` | Dieses technische Architekturdokument. |

## Initialisierungsablauf

```text
main.cpp
  |
  v
source::iniciar_aplicacao(argc, argv)
  |
  +--> Arbeitsverzeichnis anpassen (cwd)
  +--> Versuch von SplashAppStarter
  |      |
  |      +--> QApplication instanziieren
  |      +--> Grafik-Backend konfigurieren
  |      +--> Übersetzungssystem initialisieren
  |      +--> Ladebildschirm anzeigen
  |      +--> InterfaceMonitor instanziieren
  |      +--> DatabaseWriter starten, falls db_path vorhanden
  |
  +--> Fallback-Start ohne Ladebildschirm
         |
         +--> Grafik-Backend konfigurieren
         +--> QApplication instanziieren
         +--> register_qml_types()
         +--> security::pg::prepare_runtime()
         +--> Windows AppUserModelID zuweisen
         +--> Anwendungssymbol setzen
         +--> Internacionalizador initialisieren
         +--> InterfaceMonitor instanziieren
         +--> DatabaseWriter im Sofortmodus (eager) instanziieren
         +--> show()
         +--> app.exec()
```

Die Startsequenz entkoppelt zentrale Verantwortlichkeiten sauber:

- `ajustar_cwd()` verhindert Laufzeitfehler durch relative Pfade zu Assets, Übersetzungen oder Datenbankdateien;
- `configurar_backend_grafico()` richtet OpenGL-Oberflächenformate ein, bevor Widgets instanziiert werden;
- `source::charts_lgpl::register_qml_types()` registriert `QuickChartItem` als instanziierbaren QML-Typ;
- `source::security::pg::prepare_runtime()` bereitet die Persistenzschicht des Sicherheitsmoduls vor;
- `Internacionalizador::inicializar_sistema_traducao()` lädt die Übersetzungskataloge von Qt und der Anwendung;
- `DatabaseWriter::get_instance(dbPath)` initialisiert den asynchronen Worker-Thread für Datenbank-Schreibvorgänge.

## Hauptfenster

`source::gui::InterfaceMonitor` bildet das organisatorische Zentrum der Anwendung. Es erbt von `QMainWindow` und bündelt Zeiger auf sämtliche übergeordneten Manager.

Hauptaufgaben:

- Erstellung und Konfiguration des Benutzeroberflächen-Layouts;
- Bereitstellung von `Q_INVOKABLE`-Befehlen für Menüs, Schaltflächen und Dialoge;
- Gewährleistung der Echtzeit-Synchronisation zwischen Ereignistabelle, aktiven Filtern, Überwachung, Statistiken und Sicherheitsmodul;
- Dynamische Weitergabe von Sprachwechseln an alle UI-Elemente;
- Reibungslose Verarbeitung einströmender Dateiereignisse (Streaming);
- Zuweisung von Farbprofilen und Steuerung der Spaltensichtbarkeit;
- Koordination der Z-Reihenfolge und Hierarchie abhängiger Dialogfenster;
- Zuverlässiges Schließen aller untergeordneten Fenster beim Beenden der Anwendung.

Wichtigste integrierte Komponenten:

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

## Fensterhierarchie

Das Hauptfenster verwaltet eine Hierarchie zugeordneter Dialoge über dynamische Qt-Eigenschaften, insbesondere `interface_principal`. Diese Hierarchie umfasst das Handbuch, den Info-Dialog, die Hilfe, Farbdialoge, den erweiterten Filterwähler und das Statistikfenster.

Ziele:

- Das Hauptfenster stets wiederherstellbar halten, wenn Benutzer zwischen Anwendungsfenstern wechseln;
- Dialoge sinnvoll staffeln, ohne jedes Fenster zwingend als direktes Kind oder strikt modal zu blockieren;
- Verhindern, dass das Fenster `Estatísticas` im inaktiven Zustand das Hauptfenster überdeckt;
- Automatisches Schließen aller abhängigen Fenster beim Beenden der Hauptoberfläche.

Eingesetzte Mechanismen:

- Eigener `eventFilter` für Aktivierungs-, Fokus-, Anzeige-/Ausblend- und Mausereignisse;
- Dynamische Qt-Property-Bindungen zur logischen Verknüpfung von Dialogen mit `InterfaceMonitor`;
- Kontrollierter Aufruf von `raise()` und `activateWindow()` ausschließlich bei expliziter Benutzerinteraktion;
- Zeitlich entkoppelte Synchronisation zur Vermeidung von Reentranz-Schleifen bei Statusänderungen;
- Vollständige Trennung flüchtiger, unabhängiger Modaldialoge, die nicht Teil der Hierarchie sein sollen.

## Überwachungs- und Ereignisdienste

### Beobachter (Observador)

`source::services::Observador` kapselt die Dateisystemüberwachung im Hintergrund. Er vereint atomare Statusflags, threadsichere Warteschlangen, Mutex-Sperren, einen eigenen Ausführungs-Thread, Scanner und Ereignishandler.

Verantwortungsbereich:

- Überwachter Verzeichnispfad und Unterverzeichnisbäume;
- Start-, Stopp-, Pausen- und Fortsetzungszyklen;
- Erkennung massiver Ereignisspitzen (Throttling);
- Pufferung von Ereignissen in threadsicheren Queues;
- Erfassung und Protokollierung von Betriebssystem-Pufferüberläufen;
- Schätzung verworfener oder verpasster Ereignisse;
- Schnappschüsse (Snapshots) früherer Verzeichniszustände;
- Deduplizierung dicht aufeinanderfolgender Ereignisse;
- Nahtlose Übergabe und Synchronisation mit dem initialen Scanner.

### BaseEvento (Basis-Ereignismanager)

`source::services::BaseEvento` normalisiert Betriebssystemmeldungen und steuert die Datenpersistenz. Sie erzeugt das Datenbankschema, protokolliert Einzelereignisse, behandelt temporäre Löschungen, verarbeitet Datei-Verschiebungen und aktualisiert das Tabellenmodell.

Kernoperationen:

- `criar_banco_de_dados()`
- `registrar_evento_no_banco()`
- `scan_directory()`
- `processar_exclusao()`
- `processar_eventos_movimentacao()`
- `_criar_evento_padrao()`
- `_criar_evento_exclusao()`
- `_inserir_evento_movido()`

### Service-Submodule

| Submodul | Verantwortung |
| --- | --- |
| `GerenciamentoBaseEvento` | Ereignisdatenbank, Protokollierung, Löschung, Verschiebung, Snapshots und asynchroner Writer. |
| `GerenciamentoDiretorioScanner` | Initial- und Stapel-Scan, Datei-Warteschlange, Worker-Thread und Fortschrittsmeldung. |
| `GerenciamentoEventoModificado` | Ereignisgruppierung, dynamische Entprellung (Debounce), Normal- vs. Massenverarbeitung und Metadaten-Cache. |
| `GerenciamentoEventoMovido` | Korrelation von Lösch- und Hinzufüge-Ereignissen, Ausführung des Verschiebungs-Workers und Tabellenaktualisierung. |
| `GerenciamentoMetadados` | Kategoriespezifische Metadatenextraktion, Speichercaching, Tabellenformatierung und Stapelverarbeitung. |
| `GerenciamentoObservador` | Beobachter-Lebenszyklus (Start/Stopp/Pause/Fortsetzung), Puffer, Fehlerbehandlung und Wiederherstellung. |
| `GerenciamentoTabela` | Qt-Tabellenmodell, Farbcodierung, Themes, Spalten, Kopfzeilen, Neuübersetzung und Hintergrundverarbeitung. |

## Persistenz

Das Projekt nutzt `Qt::Sql` und Qt-Treiber für sämtliche lokale Datenpersistenz. Die primäre Ereignis-Pipeline wird von `BaseEvento` und `DatabaseWriter` gesteuert. Das Sicherheitsmodul stellt Hilfsfunktionen in `secore_01_postgres_utils.*` bereit, während die Standardausführung auf einer lokalen `QSQLITE`-Datenbank operiert.

Persistenz-Muster:

- Eindeutig benannte Datenbankverbindungen pro Thread-Kontext;
- Asynchrone Stapelschreiber (Batch-Writer) zur Vermeidung von GUI-Blockaden;
- Automatische Schemaerstellung bei Bedarf;
- Speicherung von Ereignissen als `QVariantMap` zur Gewährleistung vollständiger Kompatibilität mit Qt und Statistikmodulen;
- Dedizierte Hilfstabellen und Snapshots für Baselines, Sicherheitsregeln, Quarantäne, Beweise und Auditsitzungen.

## Modell und Ereignistabelle

`GerenciadorTabela` steuert die visuelle Darstellung des Audit-Protokolls. `EventTableModel` ist das maßgeschneiderte Qt-Modell, das Dateiereignisse in die `QTableView` einspeist.

Hauptmerkmale:

- Hochperformantes Voranstellen (Prepend) und Anhängen (Append) von Datensätzen in Batches;
- Vollständiger Austausch des gesamten Ereignissatzes;
- Blitzschnelle Erstellung von Daten-Snapshots;
- Sortierung über mehrere Spalten;
- Caching von Farbwerten nach Operationsart;
- Dynamisch übersetzte Spaltenüberschriften;
- Konfigurierbare Spaltensichtbarkeit und individuelle Layouts;
- Automatische Anpassung an helle und dunkle Windows-Themes;
- Zeitverzögerte Neuübersetzung bei Sprachwechseln;
- Auslagerung rechenintensiver Formatierungen in den Hintergrund;
- Bidirektionale Synchronisation mit `QTableView`.

## Metadatenextraktion

Das Subsystem `GerenciamentoMetadados` ist umfangreich und nach Dateikategorien modularisiert.

Abgedeckte Dateiformate und Bereiche:

- Quellcode;
- Bilddateien;
- Audiodateien;
- Videostreams;
- Textdokumente;
- Tabellenkalkulationen;
- Präsentationen;
- Datenbanken;
- Ausführbare Dateien und PE-Binaries;
- Temporäre Dateien;
- Komprimierte Archive;
- Sicherungsdateien (Backups);
- Protokolldateien (Logs);
- Konfigurationsdateien;
- Strukturierter Verbundspeicher (OLE);
- Strukturierte Datenformate (JSON, XML usw.);
- Tiefgehende Metadatenanalyse;
- Erkennung des tatsächlichen MIME-Typs anhand des Dateiinhalts;
- Dateigröße, Einheitenumrechnung und exakte Byte-Anzahl;
- Dateiattribute, Autor, Abmessungen, Dauer, Bitrate und Schutzrechte.

Das Modul regelt zudem die Konfiguration von Tabellenspalten, die Cache-Invalidierung bei geänderten Verzeichnissen, Extraktions-Warteschlangen, Worker-Rückrufe und Live-Aktualisierungen der Ansicht.

## Forensische Sicherheit

`source::security::SecurityManager` fungiert als Fassade des lokalen Sicherheitssubsystems. Er bündelt Sicherheitskontexte, Schemata, Ereignisanreicherung, Warnungsabfragen, Sitzungskorrelation, Quarantäne, Baseline-Integritätsprüfungen und Sicherheitsbeobachter.

Kernkomponenten:

| Datei | Rolle |
| --- | --- |
| `base_01_baseline_criptografico.*` | Kryptografische Baseline, SHA-256-Hashes und Integritäts-Snapshots. |
| `exec_01_inteligencia_executaveis.*` | Analyse ausführbarer Binärdateien, PE-Header und Metadaten digitaler Signaturen. |
| `sess_01_correlacao_sessoes.*` | Zeitliche Korrelation von Benutzersitzungen und Systemaktivitäten. |
| `sdiff_01_diff_conteudo.*` | Versionsvergleich und Differenzanalyse auf Inhaltsebene. |
| `attr_01_atribuicao_forense.*` | Forensische Zuordnung und Kontext (Prozesse und Benutzer). |
| `agre_01_motor_regras.*` | Lokale Sicherheitsrichtlinien- und Alarmierungs-Engine. |
| `risk_01_score_risco.*` | Deterministische Berechnung von Risikobewertungen. |
| `quar_01_quarentena.*` | Lokaler Quarantänespeicher, Blob-Verwaltung und Wiederherstellung. |
| `evid_01_exportacao_evidencia.*` | Bündelung und Export revisionssicherer Beweispakete. |
| `mdir_01_multi_diretorio.*` | Überwachungsprofile und Verwaltung mehrerer Verzeichnispfade. |
| `secore_05_SecurityDbWriter.*` | Dedizierter asynchroner Writer für die Sicherheitsdatenbank. |

Sicherheitsdialoge:

- `SecurityUiController`
- `MultiDirectoryDialog`
- `AlertCenterDialog`
- `SessionCenterDialog`
- `RecoveryDialog`
- `EvidenceDialog`
- `NotificationPopupConfigDialog`

## Statistik- und Analysegenerierung

`source::data::GeradorEstatisticas` erhält `db_path`, den Lokalisierungsmanager und die Hauptoberfläche. Er registriert spezialisierte Generatoren, die von `BaseGerador` erben.

Unterstützte Diagrammtypen:

- Kreisdiagramm der Operationen;
- Balkendiagramm nach Dateityp;
- Zeitstrahl der Operationen (Timeline);
- Kachel-Treemap nach Dateikategorien;
- Aktivitätshistogramm nach Uhrzeiten;
- Pareto-Verteilungskurve;
- Liniendiagramme und Clusteranalysen;
- Boxplots (Verteilung nach Operation, Typ, Uhrzeit, Größe und Ereignisvolumen);
- Radar-Diagramm für Ereignisse;
- Zeitliche Heatmap (Aktivitäts-Wärmebild);
- Streudiagramm (Scatterplot);
- Sankey-Diagramme;
- Mehrdimensionales Radar-Diagramm;
- Punktdiagramm (Dotplot);
- Sankey-Diagramm nach Ereignis und Pfad;
- Sankey-Diagramm nach Typ und Pfad;
- Hierarchischer Verzeichnisbaum.

Alle erzeugten Datenstrukturen sind strikt kompatibel mit `QVariant`, `QVariantMap` und `QVariantList`.

## Diagramm- und Grafik-Rendering-Engine

Das interne Subsystem in `source/data/Charts` ersetzt alle externen Diagrammbibliotheken durch eine native, hocheffiziente C++/Qt-Lösung.

Rendering-Ebenen:

```text
Legacy QVariantMap
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
  +--> RasterChartView    (QWidget + QPainter, verwendet im Statistik-Dialog)
  +--> WidgetChartView    (QOpenGLWidget + QPainter-Überlagerung)
  +--> QuickChartItem     (QQuickItem + Qt Quick Scene Graph)
```

Zentrale Dateien:

- `ch_01_ChartLayoutEngine.*`: Berechnet 2D/3D-Geometrie, Beschriftungen, Legenden, Klick-Prüfungen und Szenenelemente.
- `ch_02_ChartWidgetFactory.*`: Erzeugt visuelle Widgets aus typisierten Datensätzen oder Altdatenstrukturen.
- `ch_03_LegacyVariantChartAdapter.*`: Wandelt `QVariantMap`-Strukturen in das stark typisierte Diagrammmodell um.
- `ch_04_QmlRegistration.*`: Registriert `QuickChartItem` in der QML-Engine.
- `ch_05_QuickChartItem.*`: Qt Quick Scene Graph-Backend für hardwarebeschleunigte Darstellung.
- `ch_06_WidgetChartView.*`: Hardwarebeschleunigte Zeichenfläche auf Basis von `QOpenGLWidget`.
- `ch_07_LgplChartModel.h`: Grundlegende Datenstrukturen für Szenen, Geometrien und Farbpaletten.
- `ch_08_RasterChartView.*`: Reine Raster-`QWidget`-Zeichenfläche mit präzisem Antialiasing.

Architektonische Entscheidungen:

- `RasterChartView` wird für den Dialog `Estatísticas` priorisiert, um OpenGL-Kontextinstabilitäten auf Mehrschirmsystemen auszuschließen;
- `QPainter` mit Antialiasing sorgt für gestochen scharfe Vektordarstellung von Texten und Formen;
- `WidgetChartView` bleibt für dedizierte OpenGL-Renderingabläufe verfügbar;
- `QuickChartItem` erlaubt dynamische QML-Einbindung und weiche Animationen über `transitionProgress`;
- Der Bildexport nutzt ein deterministisches Einzel-Rendering ohne transiente Animationszwischenstufen.

## Benutzeroberfläche und Befehle

Die Manager in `source/ui` gliedern das Hauptfenster in eigenständige, modular aufgebaute Domänen.

| Modul | Rolle |
| --- | --- |
| `GerenciadorBotoes` | Verarbeitet Aktionen für Verzeichniswahl, Analyse, Export, Bereinigung und Pause. |
| `GerenciadorBotoesUI` | Grafischer Aufbau und Layout der Symbolleistenschaltflächen. |
| `GerenciadorMenusUI` | Aufbau der Menüleisten (Datei, Einstellungen, Optionen, Farben, Spalten, Export, Sprachen, Info, Handbuch, Hilfe). |
| `GerenciadorEventosUI` | Filterverarbeitung, Spaltensichtbarkeit, Farbverwaltung, Sprachumschaltung und Zurücksetzen. |
| `GerenciadorProgressoUI` | Fortschrittsbalken und Statusmeldungen für Suchläufe. |
| `GerenciadorEstatisticasUI` | Steuerung des Statistikdialogs, Diagrammauswahl, Generierung, Live-Aktualisierung und Export. |
| `GerenciadorDados` | Multiformat-Export und Datenmanipulation. |
| `GerenciadorEventosArquivo` | Weiterleitung von Dateisystemereignissen an die Benutzeroberfläche. |
| `GerenciadorMonitoramento` | Steuerung von Start, Stopp, Pause und Fortsetzung der Überwachung. |
| `GerenciadorLimpeza` | Bereinigung überwachter Tabellendaten und flüchtiger Caches. |
| `DialogoCores` | Farbzuordnung nach Operationsart und erweiterte Farbpalettenauswahl. |
| `LocalizadorQt` | Zentraler Verwalter der Übersetzungskataloge der Anwendung. |
| `TradutorMetadadosQt` | Dynamische Übersetzung von Operationen, MIME-Typen, Dateiattributen, Autoren und Rechten. |
| `OpcoesSobre`, `Manual` | Informationsdialoge und Benutzerhandbuch. |

## Filter und Werkzeuge

`source/tools` bündelt das Filtersystem und die Kalenderintegration.

Komponenten:

- `Filtros`
- `AdministradorCalendario`
- `AdministradorFiltros`
- `GerenciamentoFiltros`
- `GerenciamentoAdministradorFiltros`

Verantwortlichkeiten:

- Anwenden und Zurücksetzen aktiver Tabellenfilter;
- Synchronisation der Menüeinstellungen mit aktiven Filterparametern;
- Aktualisierung der Trefferzähler in Echtzeit;
- Unmittelbare Reaktion auf Änderungen der Oberflächensprache;
- Beibehaltung des Zustands von Auswahlkästchen über Sitzungen hinweg;
- Filterung von Ereignissen nach Operationsart, Datumsbereich oder Dateiendung.

## Internationalisierung (i18n)

Die Internationalisierungsarchitektur umfasst:

- Übersetzungsquelldateien: `source/locale/*.ts`;
- Kompilierte Binärkataloge: `source/locale/*.qm`;
- Automatisierte Build-Werkzeuge: `compile_translations.*`;
- Klasse `Internacionalizador`;
- Klasse `LocalizadorQt`;
- Klasse `TradutorMetadadosQt`;
- Übersetzte Handbücher unter `source/ui/Manual`;
- Lokalisierte Rechtstexte und Dokumente unter `source/assets`.

Unterstützte Sprachen:

- `pt_BR` (Portugiesisch - Brasilien)
- `en_US` (Englisch - USA)
- `es_ES` (Spanisch - Spanien)
- `fr_FR` (Französisch - Frankreich)
- `it_IT` (Italienisch - Italien)
- `de_DE` (Deutsch - Deutschland)

Bei einem Sprachwechsel zur Laufzeit übersetzt die Anwendung Menüs, Spaltenüberschriften, Filteroptionen, Diagramme, Dialoge und vorhandene Tabellenzeilen dynamisch neu.

## Ressourcen, Lizenzen und Rechtliche Dokumentation

Das Verzeichnis `source/assets` wird beim Deployment vollständig gebündelt. Es enthält:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Die Hinweise führen alle genutzten Qt-Module und deren Open-Source-Weitergabebedingungen auf. Versionshinweise und Rechtstexte sind nach Sprachen getrennt abgelegt.

## Übergreifende Hilfsprogramme

| Datei | Verantwortung |
| --- | --- |
| `ApplicationPathUtils.*` | Basispfadauflösung, Auffinden von Assets und Einlesen von Textdateien. |
| `CaminhoPersistenteUtils.*` | Auflösung von Standardpfaden für persistente Anwendungsdaten (`%LOCALAPPDATA%`). |
| `IconUtils.*` | Laden von Symbolen optimiert für hochauflösende Bildschirme (High-DPI). |
| `LogManager.*` | Zentralisierter threadsicherer Logger, zeitgestempelte Protokollrotation und Unicode-Normalisierung. |
| `TrialManager.*` | Verwaltung des Testzeitraums, Lizenzprüfung und Weiterleitung zum Microsoft Store. |

## Nebenläufigkeit und Reaktionsfähigkeit

Das Projekt setzt auf eine ausgewogene Multithreading-Architektur:

- `std::thread` für die kontinuierliche Low-Level-Dateisystemüberwachung;
- `QThread` für rekursive Verzeichnis-Suchläufe im Hintergrund;
- `QTimer` für verzögerte Schreibvorgänge (Flushes), UI-Synchronisation und Neuübersetzungen;
- `QtConcurrent` für rechenintensive Hintergrundaufgaben;
- Dedizierte asynchrone Writer für Datenbankzugriffe;
- Schnelle Arbeitsspeichercaches für Farbwerte, übersetzte Zeichenketten, Metadaten und Snapshots;
- Lock-freie atomare Variablen und Mutex-Sperren in den Warteschlangen des Beobachters.

Architekturrichtlinien:

- GUI-Aktualisierungen dürfen ausschließlich auf dem Haupt-Thread von Qt ausgeführt werden;
- Datenbankoperationen dürfen den Ereignis-Loop der Benutzeroberfläche niemals blockieren;
- Massenhafte Dateiänderungen müssen gebündelt in Stapeln verarbeitet werden;
- Das Diagramm-Rendering muss im Statistikdialog `RasterChartView` bevorzugen;
- Caches müssen bei jeder Änderung von Sprache, Theme, Spaltenlayout oder Farben invalidiert werden.

## Dateiereignis-Pipeline

```text
ReadDirectoryChangesW / Scanner
  |
  v
Observador
  |
  +--> Ereignispuffer
  +--> Erkennung von Massenaktivität
  +--> Handler: Hinzugefügt / Gelöscht / Geändert / Umbenannt / Verschoben
  |
  v
BaseEvento
  |
  +--> Daten normalisieren
  +--> Metadaten extrahieren
  +--> In lokale SQLite-Datenbank schreiben
  +--> SecurityManager benachrichtigen
  |
  v
InterfaceMonitor
  |
  +--> EventTableModel
  +--> GerenciadorTabela
  +--> Filter / Farben / Übersetzungen
  +--> Statistiken bei Bedarf
```

## Statistik-Pipeline

```text
Benutzer klickt auf Statistiken
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
  +--> RasterChartView für das Statistikfenster
  +--> maßgeschneiderte Widgets für spezielle Diagramme
  |
  v
Statistik-Dialog
```

## Richtlinien für Weiterentwicklungen

### Neues Diagramm hinzufügen

1. Einen spezialisierten Generator implementieren, der von `BaseGerador` erbt (in `source/data/GeradorEstatisticas`).
2. Den neuen Generator in `GeradorEstatisticas::inicializar_geradores` registrieren.
3. Eine stabile und klar definierte `QVariantMap`-Ausgabestruktur gewährleisten.
4. Falls diagrammtypisch passend, in `LegacyVariantChartAdapter` einbinden.
5. Bei Bedarf an neuartigen grafischen Formen `ChartLayoutEngine` erweitern.
6. Im Auswahlfeld von `GerenciamentoEstatisticasUI` verfügbar machen.
7. Übersetzungsdateien (`.ts`) und technische Dokumentation aktualisieren.

### Neue Spalte zur Tabelle hinzufügen

1. Ereignisquellen und Metadaten-Extraktionsmodule erweitern.
2. Spalten-Schlüssel und Beschriftung in `GerenciadorColunas` registrieren.
3. Übersetzung in `LocalizadorQt` und den `.ts`-Dateien ergänzen.
4. Farbliche Hervorhebung, Sichtbarkeit, Sortierung und Exportfunktionen validieren.

### Neues Sicherheitsfeature hinzufügen

1. Komponentenmodul unter `source/security` implementieren.
2. In die Fassade `SecurityManager` integrieren.
3. Erforderliche Datenbankschemata in `ensure_schema` deklarieren.
4. `SecurityDbWriter` für asynchrone Schreibvorgänge einsetzen.
5. Dialoge und Ansichten unter `secui_*` anlegen, falls eine Benutzeroberfläche erforderlich ist.
6. Die Beweispaket-Generierung aktualisieren, falls die Daten revisionsrelevant sind.

### Neue Sprache hinzufügen

1. Neue `.ts`- und `.qm`-Dateien unter `source/locale` anlegen.
2. Übersetzte Handbücher und Dokumente unter `source/assets` und `source/ui/Manual` hinterlegen.
3. Sprachcode in `LocalizadorQt` registrieren.
4. Sprachauswahlmenüs in `GerenciadorMenusUI` ergänzen.
5. Spaltenüberschriften, Filter, bestehende Daten und Handbücher verifizieren.

## Wichtige Hinweise

- `InterfaceMonitor` bündelt viele Manager; neue Funktionen sollten keine unnötige direkte Kopplung einführen, wenn ein spezialisierter Manager bereits die fachliche Zuständigkeit besitzt.
- `QVariantMap` ist der bewährte Datenvertrag zwischen Ereignissen, Statistiken und Benutzeroberfläche; Anpassungen an Schlüsseln erfordern sorgfältige Koordination.
- Die Diagramm-Engine nutzt `RasterChartView`, um Mehrschirm-Stabilität im Fenster `Estatísticas` sicherzustellen.
- CMake bindet Dateien in `source/data/Charts` über `GLOB_RECURSE` ein; neue Dateien in diesem Ordner werden automatisch kompiliert.
- Das Deployment kopiert den gesamten Ordner `source/assets`; Lizenzhinweise und Übersetzungen müssen bei jedem Release konsistent gehalten werden.
- Dieses Dokument spiegelt den aktuellen Zustand der Codebasis im Workspace wider.

## Schnelle Dateireferenz

| Bereich | Wichtige Dateien |
| --- | --- |
| Einstiegspunkt | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| Hauptfenster | `source/gui/ic_01_InterfaceMonitor.*` |
| Übersetzung und i18n | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| Überwachung | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| Ereignisdatenbank | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| Tabellenverwaltung | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| Metadatenextraktion | `source/services/GerenciamentoMetadados/*` |
| Forensische Sicherheit | `source/security/*` |
| Statistikgenerierung | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| Native Diagramm-Engine | `source/data/Charts/*` |
| UI und Dialoge | `source/ui/*`, `source/ui/Gerenciamento*/*` |
| Filter und Kalender | `source/tools/*` |
| Assets und Lizenzen | `source/assets/*` |
| Allgemeine Hilfsprogramme | `source/utils/*` |

</details>

---
