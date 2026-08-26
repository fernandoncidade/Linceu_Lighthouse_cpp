# LINCEU LIGHTHOUSE - OBSERVACAO TECNICA ATUALIZADA

## Estado atual do projeto

O repositório atual do **Linceu Lighthouse** não corresponde mais à implementação antiga em Python/PySide6. A base desta release foi consolidada como uma aplicação desktop **nativa em C++17 com Qt6**, compilada via CMake, com foco em monitoramento de integridade de arquivos, análise de eventos, extração de metadados, estatísticas e persistência local.

Versão consolidada observada nos assets e na documentação embarcada:

- `v2026.4.27.0`
Autor identificado no projeto:

- `Fernando Nillsson Cidade`

Licença do repositório:

- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Correcao principal desta observacao

O arquivo antigo descrevia uma arquitetura baseada em:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- cadeia de runtime dependente de bibliotecas Python

Essa descricao está defasada para esta arvore. A implementação atual:

- é compilada em `C++17`;
- usa `Qt6` como stack principal;
- utiliza `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` e `Qt Qml`;
- renderiza gráficos e diagramas com subsistema próprio em `source/data/Charts`, baseado em `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` e Qt Quick Scene Graph;
- usa APIs nativas do Windows para observação de diretório e metadados de executáveis;
- mantém, como herança histórica, a nomenclatura modular do projeto original.

Em outras palavras: a arquitetura funcional foi preservada, mas a plataforma de execução foi trocada.

## Resumo estrutural consolidado

Contagem validada diretamente na arvore `source/`:

| Camada | `.cpp` | `.h` | Observacao |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Interface principal, bootstrap visual, mensagens e integração da GUI |
| `source/services` | 162 | 163 | Observação, banco, metadados, eventos e tabela |
| `source/ui` | 125 | 125 | Menus, botões, diálogos, manual, estatísticas e localização |
| `source/data` | 26 | 27 | Fachada de estatísticas + infraestrutura `Charts` + 17 geradores |
| `source/tools` | 23 | 23 | Filtros e administração de filtros |
| `source/utils` | 5 | 5 | Caminhos, logs, ícones, trial e leitura de assets |
| `source/locale` | 1 | 1 | Ferramentas de tradução |
| `source/security` | 24 | 24 | Ferramentas de segurança |
| **Total em `source/`** | **376** | **378** | Base C++ atual |

Diretórios principais sob `source/`:

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Build, toolchain e dependencias reais

Dados validados em `CMakeLists.txt` e `mocks/CMakeLists.txt`:

- `cmake_minimum_required(VERSION 4.1)` no build principal
- `cmake_minimum_required(VERSION 3.24)` no build auxiliar de `mocks/`
- padrão de linguagem: `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` no build principal

Componentes Qt exigidos pelo build principal:

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

Bibliotecas nativas adicionais no Windows:

- `wintrust`
- `version`
- `crypt32`

No build auxiliar de `mocks/find_VersionEditor`, os componentes Qt requeridos são:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Executáveis definidos por CMake:

- `Linceu_Lighthouse_cpp` no `CMakeLists.txt` raiz
- `find_VersionEditor` em `mocks/CMakeLists.txt`

Observacoes:

- O `CMakeLists.txt` tenta localizar automaticamente uma instalação Qt6 em `C:/Qt/*/mingw_64/...` quando executado no Windows.
- O utilitário `find_VersionEditor` foi isolado em `mocks/`, com `CMakePresets.json` próprio e saída direcionada para `mocks/output`.
- O projeto atual não depende de runtime Python para a aplicação principal.
- A ferramenta `source/locale/compile_translations.cpp` existe no repositório como utilitário de linha de comando para `.ts`/`.qm`, mas não é o ponto de entrada principal da aplicação.

## Diretórios auxiliares fora de `source/`

Além da árvore principal em `source/`, o repositório atual também contém:

- `repo/`, com `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1` e um arquivo de apoio sobre o uso de `cmake --build`;
- `mocks/`, com `find_VersionEditor.cpp/.h`, `CMakeLists.txt` e `CMakePresets.json` para o utilitário auxiliar de atualização de versões;
- `.vscode/`, com `c_cpp_properties.json`, `settings.json` e `tasks.json` para a configuração local de IDE;
- `.github/workflows/`, com `update-readme-version.yml` para automação de atualização documental.

Esses diretórios não pertencem à árvore de runtime principal da aplicação, mas fazem parte da manutenção, automação e apoio ao desenvolvimento do projeto.

## Ponto de entrada e bootstrap

Fluxo confirmado no código:

1. `main.cpp`
   - delega imediatamente para `source::iniciar_aplicacao(argc, argv)`.

2. `source/src_01_InicializadorMain.cpp`
   - ajusta o diretório corrente;
   - define `AppUserModelID` no Windows;
   - tenta inicializar via `SplashAppStarter`;
   - em fallback, cria `QApplication`, configura ícone e traduções, instancia `InterfaceMonitor` e tenta iniciar o `DatabaseWriter` de forma antecipada.

3. `source/src_02_SplashAppStarter.cpp`
   - cria `QApplication`;
   - carrega ícone principal;
   - inicializa sistema de tradução;
   - monta splash screen com:
     - imagem;
     - barra de progresso;
     - área de log;
   - instancia `InterfaceMonitor`;
   - tenta iniciar o `DatabaseWriter`;
   - fecha a splash e entrega o controle para a janela principal.

Esse bootstrap mostra que a aplicação atual já nasceu para:

- diagnóstico de inicialização;
- fallback seguro;
- carregamento robusto de assets;
- compatibilidade com a distribuição binária no Windows.

## Estrutura atual por camada

### `source/gui`

Top-level validado:

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Subdiretórios:

- não há subdiretórios ativos sob `source/gui` nesta árvore;
- os antigos diretórios `GerenciadorDesempenho/` e `GerenciadorEstruturaDiretoriosWidget/` não aparecem mais na estrutura atual.

Responsabilidades:

- janela principal;
- composição de widgets;
- atualização visual;
- gerenciamento de progresso e mensagens;
- internacionalização da camada de GUI;
- integração com tabela e bootstrap visual da aplicação.

### `source/services`

Top-level validado:

- `ob_01_Observador.cpp/.h`
- `ob_02_BaseEvento.cpp/.h`
- `ob_03_DiretorioScanner.cpp/.h`
- `ob_04_EventoAdicionado.cpp/.h`
- `ob_05_EventoExcluido.cpp/.h`
- `ob_06_EventoModificado.cpp/.h`
- `ob_07_EventoRenomeado.cpp/.h`
- `ob_08_EventoMovido.cpp/.h`
- `ob_09_GerenciadorColunas.cpp/.h`
- `ob_10_GerenciadorTabela.cpp/.h`

Subdiretórios validados:

- `GerenciamentoBaseEvento/` com `21` módulos
- `GerenciamentoDiretorioScanner/` com `9` módulos
- `GerenciamentoEventoModificado/` com `7` módulos
- `GerenciamentoEventoMovido/` com `9` módulos
- `GerenciamentoMetadados/` com `47` módulos
- `GerenciamentoObservador/` com `13` módulos
- `GerenciamentoTabela/` com `47` módulos

Responsabilidades:

- observação do sistema de arquivos;
- varredura inicial;
- roteamento por tipo de evento;
- persistência SQLite;
- extração de metadados;
- atualização de tabela;
- coloração, tema e retradução.

### `source/ui`

Top-level validado:

- `ui_01_GerenciadorBotoes.cpp/.h`
- `ui_02_GerenciadorBotoesUI.cpp/.h`
- `ui_03_GerenciadorMenusUI.cpp/.h`
- `ui_04_GerenciadorEventosUI.cpp/.h`
- `ui_05_GerenciadorProgressoUI.cpp/.h`
- `ui_06_GerenciadorEstatisticasUI.cpp/.h`
- `ui_07_GerenciadorDados.cpp/.h`
- `ui_08_GerenciadorEventosArquivo.cpp/.h`
- `ui_09_GerenciadorMonitoramento.cpp/.h`
- `ui_10_GerenciadorLimpeza.cpp/.h`
- `ui_11_DialogoCores.cpp/.h`
- `ui_12_LocalizadorQt.cpp/.h`
- `ui_13_TradutorMetadadosQt.cpp/.h`
- `ui_14_OpcoesSobre.cpp/.h`
- `ui_15_Manual.cpp/.h`

Subdiretórios validados:

- `GerenciamentoBotoes/` com `6` módulos
- `GerenciamentoDialogoCores/` com `8` módulos
- `GerenciamentoEstatisticasUI/` com `31` módulos
- `GerenciamentoEventosUI/` com `11` módulos
- `GerenciamentoLocalizadorQt/` com `14` módulos
- `GerenciamentoMenusUI/` com `24` módulos
- `GerenciamentoTradutorMetadadosQt/` com `9` módulos
- `Manual/` com `7` módulos

Responsabilidades:

- menus e ações globais;
- exportação;
- janela de estatísticas;
- manual e ajuda;
- diálogo de cores;
- troca de idioma;
- textos de sobre, licença, avisos, política de privacidade e release notes.

### `source/data`

Estrutura validada:

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` com `8` módulos de infraestrutura gráfica
- `GeradorEstatisticas/` com `18` módulos (`1` base comum + `17` geradores especializados)

Geradores confirmados:

- pizza
- barras
- timeline
- treemap
- histograma
- pareto
- linha
- boxplot
- radar de eventos
- heatmap
- scatter
- sankey
- radar
- dotplot
- sankey evento-caminho
- sankey tipo-caminho
- árvore de diretórios

Observacao:

- A camada `source/data` possui `27` módulos ao todo: `1` fachada + `8` módulos em `Charts/` + `18` módulos em `GeradorEstatisticas/`.

### `source/tools`

Top-level validado:

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Subdiretórios:

- `GerenciamentoAdministradorFiltros/` com `10` módulos
- `GerenciamentoFiltros/` com `10` módulos

Responsabilidades:

- filtros avançados;
- calendário;
- sincronização com menu principal;
- contagem de resultados;
- persistência do estado de checkboxes.

### `source/utils`

Módulos validados:

- `ApplicationPathUtils.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`
- `TrialManager.cpp/.h`

Responsabilidades:

- localização de assets;
- caminhos persistentes;
- logs locais;
- ícones;
- infraestrutura de trial.

### `source/locale`

Arquivos principais validados:

- `compile_translations.cpp/.h`
- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsabilidades:

- validação de arquivos `.ts`;
- compilação para `.qm` via `lrelease`;
- listagem, limpeza e estatísticas das traduções.

## Arquitetura funcional atual

### 1. Monitoramento nativo do sistema de arquivos

O módulo `source/services/ob_01_Observador.*` é o núcleo do monitoramento.

Elementos confirmados:

- diretório observado;
- callback para integração com UI;
- buffers e hashes para deduplicação e reconciliação;
- controle de pausa;
- contadores de eventos;
- `thread_scan` para a varredura inicial;
- `std::thread` para monitoramento contínuo;
- `std::thread` para processamento do buffer de eventos.

As chamadas nativas do Windows aparecem na camada de observação:

- `CreateFileW`
- `ReadDirectoryChangesW`

Flags de notificação observadas:

- nome de arquivo;
- nome de diretório;
- atributos;
- tamanho;
- última escrita;
- segurança;
- criação.

Tipos de operação cobertos:

- adicionado
- excluído
- modificado
- renomeado
- movido
- escaneado

### 2. Separação entre baseline e fluxo em tempo real

O desenho atual separa:

- varredura inicial recursiva;
- captura contínua por API nativa;
- processamento em buffer;
- persistência assíncrona;
- atualização da interface.

Essa separação reduz interferência entre:

- construção do snapshot inicial;
- ingestão de eventos sob carga;
- renderização da UI.

### 3. Persistência local com SQLite

O banco principal é:

- `monitoramento.db`

Local padrão confirmado no código:

- `%LOCALAPPDATA%\\LinceuLighthouse\\monitoramento.db`

Pontos confirmados em `gbank_02_criar_banco_de_dados.cpp` e correlatos:

- `QSQLITE`
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- criação de índices para tabelas de monitoramento e snapshot

Existe uma camada de escrita assíncrona dedicada:

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

Características do `DatabaseWriter`:

- singleton;
- fila interna;
- `std::thread` própria;
- persistência em lote;
- espera por timeout;
- encerramento ordenado.

### 4. Reconciliação de eventos complexos

O projeto possui tratamento especializado para:

- movimentações;
- exclusões temporárias;
- renomeações;
- modificações sob alta frequência;
- operações em massa.

Subcamadas importantes:

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

Isso indica que a aplicação não apenas registra notificações brutas do SO, mas tenta consolidar eventos em operações semanticamente coerentes.

## Tabela, streaming e compatibilidade de interface

O estado atual da aplicação é híbrido e explícito no código:

- ainda há suporte a `QTableWidget`;
- há migração progressiva para `QTableView + QAbstractTableModel`.

Modelo nativo confirmado:

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Capacidades confirmadas no modelo:

- `rowCount`
- `columnCount`
- `headerData`
- `data`
- `prepend_event`
- `append_events`
- `replace_events`
- caches de rótulos e brushes
- invalidação de cache de cores

Conclusão importante:

- o projeto já tem base para streaming via model/view;
- vários fluxos continuam compatíveis com o caminho legado;
- a migração foi pensada para reduzir regressão funcional, não para quebrar a UI antiga de uma vez.

## Colunas, cores, tema e retraducao

Na camada `GerenciamentoTabela/` existem módulos para:

- configuração de tabela;
- ajuste de largura e altura de cabeçalho;
- quebra de linha em cabeçalhos;
- atualização de dados;
- atualização da linha mais recente;
- visibilidade de colunas;
- coloração por operação ou coluna;
- persistência de cores;
- atualização assíncrona de cores;
- detecção e monitoramento do tema do Windows;
- retradução de dados existentes;
- atualização de cabeçalhos após mudança de idioma.

Pontos relevantes confirmados:

- leitura do tema do Windows via `QSettings` na chave `HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize`;
- ponte específica `gtab_43_AtualizarCabecalhosBridge`;
- worker threads e execuções em background para tarefas visuais mais pesadas.

## Extração de metadados

O subdiretório `GerenciamentoMetadados/` possui `47` módulos e cobre, entre outros:

- código-fonte;
- imagens;
- áudio;
- vídeo;
- documentos;
- planilhas;
- apresentações;
- bancos de dados;
- executáveis;
- temporários;
- compactados;
- backups;
- logs;
- arquivos de configuração;
- OLE;
- dados estruturados.

Aspectos técnicos confirmados:

- `QImageReader` para dimensões e leitura de imagens;
- inspeção de ZIP/OpenXML para documentos, planilhas e apresentações;
- `QSqlDatabase` para leitura de bancos SQLite e enumeração de tabelas;
- detecção por header para arquivos SQLite;
- `WinVerifyTrust`, `GetFileVersionInfoW` e `VerQueryValueW` para executáveis no Windows.

Isso mostra que a extração foi reimplementada de modo nativo, substituindo boa parte da antiga cadeia de bibliotecas Python.

## Estatísticas e geração de gráficos

A UI de estatísticas está distribuída entre:

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Capacidades confirmadas:

- geração de múltiplos gráficos em janela dedicada;
- painel de seleção;
- atualização sem fechar a interface;
- regeneração após mudança de idioma;
- salvamento de gráficos;
- worker de geração;
- adaptação de payloads legados por `LegacyVariantChartAdapter`;
- processamento geométrico por `ChartLayoutEngine` e modelo tipado `LgplChartModel`;
- renderização QWidget por `RasterChartView` com `QPainter` antialiasing para a janela `Estatísticas`;
- backends complementares por `WidgetChartView` (`QOpenGLWidget`) e `QuickChartItem` (Qt Quick Scene Graph).

Quantidade consolidada:

- `17` tipos de gráficos

## Exportação de dados

O fluxo de exportação atual passa principalmente por:

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Formatos confirmados em código:

- `.xlsx`
- `.csv`
- `.txt`
- `.json`
- `.xml`
- `.db`

Opções de exportação confirmadas:

- apenas colunas ativas;
- apenas filtros ativos;
- apenas seleção.

Observacao importante:

- a geração de `.xlsx` é montada manualmente com estrutura OpenXML e `QXmlStreamWriter`, sem depender de bibliotecas Python.

## Filtros e painel auxiliar

A ferramenta `Filtros` oferece:

- filtros por operação;
- aplicação combinada;
- atualização de contagem;
- sincronização com o menu principal;
- integração com mudança de idioma;
- persistência do estado dos checkboxes.

A arquitetura de filtros está dividida entre:

- widget principal de filtros;
- administrador de calendário;
- administrador de filtros;
- módulos auxiliares especializados para cada subação.

## Estruturas removidas e estruturas que entraram

Com base na árvore atual, alguns diretórios antes descritos nesta observação não estão mais presentes como camadas próprias:

- `source/gui/GerenciadorEstruturaDiretoriosWidget/` não aparece mais na árvore atual;
- `source/gui/GerenciadorDesempenho/` também não aparece mais na árvore atual;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*` saiu da estrutura presente nesta release.

Em contrapartida, a estrutura explicitamente presente e relevante que entrou no desenho atual é:

- `source/data/Charts/`, com a infraestrutura de renderização e adaptação gráfica reutilizável do projeto;
- `mocks/`, com um `CMake` auxiliar próprio para `find_VersionEditor`;
- `repo/`, `.vscode/` e `.github/workflows/` como diretórios auxiliares de empacotamento, IDE e automação.

Portanto, este arquivo não deve mais tratar desempenho e árvore de diretórios como subsistemas ativos com diretórios próprios dentro de `source/gui`; a árvore atual documenta essas responsabilidades de forma diferente e concentra a infraestrutura gráfica em `source/data/Charts/`.

## Internacionalização

Idiomas suportados e validados:

- `pt_BR`
- `en_US`
- `es_ES`
- `fr_FR`
- `it_IT`
- `de_DE`

Pontos confirmados na implementação:

- `LocalizadorQt` gerencia idioma atual, tradução e persistência;
- a preferência de idioma é salva em `language_config.json`;
- o projeto possui tradução de interface;
- o projeto possui tradução de tipos de operação;
- o projeto possui tradutor de metadados;
- há retradução de conteúdos já exibidos;
- menus, manual, ajuda e diálogo sobre reagem à mudança de idioma.

## Assets e conteudos embarcados

O projeto contém assets multilíngues sob `source/assets/`, incluindo:

- `ABOUT`
- `CLC`
- `COPYRIGHT`
- `EULA`
- `LICENSES`
- `LISTAGEM`
- `NOTICES`
- `PRIVACY_POLICY`
- `RELEASE`
- `icones`

Os textos de "Sobre" são carregados por `ui_14_OpcoesSobre.cpp` e incluem:

- sobre do produto;
- licença;
- avisos;
- política de privacidade;
- histórico do aplicativo;
- notas de lançamento.

O manual interno é montado a partir de:

- `source/ui/Manual/common_Manual.*`
- `source/ui/Manual/DOC_PT_BR.*`
- `source/ui/Manual/DOC_EN_US.*`
- `source/ui/Manual/DOC_ES_ES.*`
- `source/ui/Manual/DOC_FR_FR.*`
- `source/ui/Manual/DOC_IT_IT.*`
- `source/ui/Manual/DOC_DE_DE.*`

## Persistência local e arquivos relevantes

Diretório persistente confirmado em código:

- `%LOCALAPPDATA%\\LinceuLighthouse`

Arquivos relevantes confirmados:

- `monitoramento.db`
- `language_config.json`
- `colunas_config.json`
- `colunas_coloridas.json`
- `cores_config.json`
- `trial_info.json`

Logs:

- `%LOCALAPPDATA%\\LinceuLighthouse\\logs\\file_monitor_YYYYMMDD_hhmmss.log`

Observacao sobre trial:

- existe infraestrutura de trial em `source/utils/TrialManager.*`;
- ela usa `trial_info.json` e também a chave de registro `HKCU\\SOFTWARE\\LinceuLighthouse\\FirstRunTimestamp`;
- o valor padrão observado é `7` dias;
- não foi encontrada chamada direta de `enforce_trial()` no bootstrap atual, então a infraestrutura existe, mas não aparece conectada ao ponto de entrada principal desta arvore.

## Fluxo principal de execução observado

Fluxo funcional consolidado:

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. criação da `InterfaceMonitor`
5. inicialização de localização, ícones e componentes de UI
6. seleção de diretório e/ou restauração de contexto
7. criação do `Observador`
8. varredura inicial em `QThread`
9. monitoramento contínuo em `std::thread`
10. reconciliação e registro de eventos
11. persistência SQLite
12. atualização da tabela
13. filtros, exportação e estatísticas sobre a base local

## Pontos técnicos importantes para manutenção

1. A nomenclatura modular preserva a origem Python.
   Isso explica nomes como `ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_` etc.

2. O `README.md` ainda contém trechos técnicos descrevendo a implementação antiga.
   Esta observação foi atualizada com base no código real do projeto atual, não no texto legado do README.

3. A aplicação atual é fortemente orientada a Windows.
   Isso aparece em:
   - `ReadDirectoryChangesW`;
   - `CreateFileW`;
   - `WinVerifyTrust`;
   - `GetFileVersionInfoW`;
   - uso de `QSettings` para o tema do Windows;
   - bibliotecas `wintrust`, `version` e `crypt32`.

4. A migração para model/view ainda é parcial.
   Existem caminhos novos com `QTableView`/`EventTableModel`, mas muitos fluxos seguem compatíveis com `QTableWidget`.

5. O projeto usa persistência local como centro da arquitetura.
   A base SQLite não é apenas exportação: ela é parte da operação do produto.

6. O utilitário auxiliar `find_VersionEditor`
   foi isolado em `mocks/`, com `CMakeLists.txt` e `CMakePresets.json` próprios, e serve para atualizar versões e datas em assets e documentação multilíngue.

## Conclusao

O **Linceu Lighthouse** desta arvore é uma aplicação **C++17/Qt6**, desktop e nativa para Windows, com arquitetura modular extensa, forte separação por camadas e foco em:

- monitoramento de integridade de arquivos;
- persistência local em SQLite;
- extração nativa de metadados;
- visualização estatística por gráficos;
- internacionalização em seis idiomas;
- compatibilidade progressiva entre UI legada e model/view moderno.

A principal correção necessária neste arquivo era remover a leitura antiga de que o projeto atual ainda seria uma aplicação Python/PySide6. Essa não é mais a realidade técnica da base presente neste repositório.

# LINCEU LIGHTHOUSE: source

**Root Path:** `c:\Users\ferna\DEV\CPP\Linceu_Lighthouse_cpp\source`

```
├── 📁 assets
│   ├── 📁 ABOUT
│   │   ├── 📄 ABOUT_de_DE.txt
│   │   ├── 📄 ABOUT_en_US.txt
│   │   ├── 📄 ABOUT_es_ES.txt
│   │   ├── 📄 ABOUT_fr_FR.txt
│   │   ├── 📄 ABOUT_it_IT.txt
│   │   ├── 📄 ABOUT_pt_BR.txt
│   │   ├── 📄 History_APP_de_DE.txt
│   │   ├── 📄 History_APP_en_US.txt
│   │   ├── 📄 History_APP_es_ES.txt
│   │   ├── 📄 History_APP_fr_FR.txt
│   │   ├── 📄 History_APP_it_IT.txt
│   │   └── 📄 History_APP_pt_BR.txt
│   ├── 📁 CLC
│   │   ├── 📄 CLC_de_DE - Linceu Lighthouse.txt
│   │   ├── 📄 CLC_en_US - Linceu Lighthouse.txt
│   │   ├── 📄 CLC_es_ES - Linceu Lighthouse.txt
│   │   ├── 📄 CLC_fr_FR - Linceu Lighthouse.txt
│   │   ├── 📄 CLC_it_IT - Linceu Lighthouse.txt
│   │   └── 📄 CLC_pt_BR - Linceu Lighthouse.txt
│   ├── 📁 COPYRIGHT
│   │   ├── 📄 AVIS DE COPYRIGHT ET DE MARQUE DÉPOSÉE_fr_FR.txt
│   │   ├── 📄 AVISO DE COPYRIGHT E MARCA REGISTRA_pt_BR.txt
│   │   ├── 📄 AVISO DE DERECHOS DE AUTOR Y MARCA REGISTRADA_es_ES.txt
│   │   ├── 📄 AVVISO SU COPYRIGHT E MARCHIO REGISTRATO_it_IT.txt
│   │   ├── 📄 COPYRIGHT AND TRADEMARK NOTICE_en_US.txt
│   │   └── 📄 HINWEIS ZU URHEBERRECHT UND MARKENZEICHEN_de_DE.txt
│   ├── 📁 EULA
│   │   ├── 📄 EULA_de_DE - Linceu Lighthouse.txt
│   │   ├── 📄 EULA_en_US - Linceu Lighthouse.txt
│   │   ├── 📄 EULA_es_ES - Linceu Lighthouse.txt
│   │   ├── 📄 EULA_fr_FR - Linceu Lighthouse.txt
│   │   ├── 📄 EULA_it_IT - Linceu Lighthouse.txt
│   │   └── 📄 EULA_pt_BR - Linceu Lighthouse.txt
│   ├── 📁 LICENSES
│   │   ├── 📄 Apache License Version 2.0.txt
│   │   ├── 📄 BSD License Clause-2.txt
│   │   ├── 📄 BSD License Clause-3.txt
│   │   ├── 📄 GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt
│   │   ├── 📄 GNU GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt
│   │   ├── 📄 GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt
│   │   ├── 📄 ISC LICENSE.txt
│   │   ├── 📄 MIT License.txt
│   │   ├── 📕 MS-Store-ADA-v8.9-FINAL-EN.pdf
│   │   ├── 📄 PYTHON SOFTWARE FOUNDATION LICENSE.txt
│   │   └── 📄 SQLite Is Public Domain.txt
│   ├── 📁 NOTICES
│   │   ├── 📄 NOTICE_de_DE.txt
│   │   ├── 📄 NOTICE_en_US.txt
│   │   ├── 📄 NOTICE_es_ES.txt
│   │   ├── 📄 NOTICE_fr_FR.txt
│   │   ├── 📄 NOTICE_it_IT.txt
│   │   └── 📄 NOTICE_pt_BR.txt
│   ├── 📁 PRIVACY_POLICY
│   │   ├── 📄 Privacy_Policy_de_DE.txt
│   │   ├── 📄 Privacy_Policy_en_US.txt
│   │   ├── 📄 Privacy_Policy_es_ES.txt
│   │   ├── 📄 Privacy_Policy_fr_FR.txt
│   │   ├── 📄 Privacy_Policy_it_IT.txt
│   │   └── 📄 Privacy_Policy_pt_BR.txt
│   ├── 📁 RELEASE
│   │   ├── 📄 RELEASE NOTES_de_DE.txt
│   │   ├── 📄 RELEASE NOTES_en_US.txt
│   │   ├── 📄 RELEASE NOTES_es_ES.txt
│   │   ├── 📄 RELEASE NOTES_fr_FR.txt
│   │   ├── 📄 RELEASE NOTES_it_IT.txt
│   │   └── 📄 RELEASE NOTES_pt_BR.txt
│   └── 📁 images
│       ├── 📁 icons
│       │   ├── 📄 analyze.ico
│       │   ├── 📄 atualizar.ico
│       │   ├── 📄 blocos-de-cores.ico
│       │   ├── 📄 calendar.ico
│       │   ├── 📄 clear.ico
│       │   ├── 📄 clear_button3.ico
│       │   ├── 📄 filtro.ico
│       │   ├── 📄 linceu_com_fundo.ico
│       │   ├── 📄 linceu_com_fundo_1440_2160.ico
│       │   ├── 📄 linceu_com_fundo_150_150.ico
│       │   ├── 📄 linceu_com_fundo_2160_2160.ico
│       │   ├── 📄 linceu_com_fundo_300_300.ico
│       │   ├── 📄 linceu_com_fundo_71_71.ico
│       │   ├── 📄 linceu_sem_fundo.ico
│       │   ├── 📄 linceu_sem_fundo_1440_2160.ico
│       │   ├── 📄 linceu_sem_fundo_150_150.ico
│       │   ├── 📄 linceu_sem_fundo_2160_2160.ico
│       │   ├── 📄 linceu_sem_fundo_300_300.ico
│       │   ├── 📄 linceu_sem_fundo_71_71.ico
│       │   ├── 📄 paleta-de-cores.ico
│       │   ├── 📄 pause1.ico
│       │   ├── 📄 pause2.ico
│       │   ├── 📄 pause3.ico
│       │   ├── 📄 pause4.ico
│       │   ├── 📄 salvar.ico
│       │   ├── 📄 save_as.ico
│       │   ├── 📄 scan.ico
│       │   ├── 📄 selecione.ico
│       │   ├── 📄 start.ico
│       │   ├── 📄 statistics.ico
│       │   └── 📄 stop.ico
│       └── 📁 imgs
│           ├── 🖼️ analyze.png
│           ├── 🖼️ atualizar.png
│           ├── 🖼️ blocos-de-cores.png
│           ├── 🖼️ calendar.png
│           ├── 🖼️ clear.png
│           ├── 🖼️ clear_button3.png
│           ├── 🖼️ filtro.png
│           ├── 🖼️ linceu_com_fundo.png
│           ├── 🖼️ linceu_com_fundo_1440_2160.png
│           ├── 🖼️ linceu_com_fundo_150_150.png
│           ├── 🖼️ linceu_com_fundo_2160_2160.png
│           ├── 🖼️ linceu_com_fundo_300_300.png
│           ├── 🖼️ linceu_com_fundo_71_71.png
│           ├── 🖼️ linceu_sem_fundo.png
│           ├── 🖼️ linceu_sem_fundo_1440_2160.png
│           ├── 🖼️ linceu_sem_fundo_150_150.png
│           ├── 🖼️ linceu_sem_fundo_2160_2160.png
│           ├── 🖼️ linceu_sem_fundo_300_300.png
│           ├── 🖼️ linceu_sem_fundo_71_71.png
│           ├── 🖼️ paleta-de-cores.png
│           ├── 🖼️ pause1.png
│           ├── 🖼️ pause2.png
│           ├── 🖼️ pause3.png
│           ├── 🖼️ pause4.png
│           ├── 🖼️ salvar.png
│           ├── 🖼️ save_as.png
│           ├── 🖼️ scan.png
│           ├── 🖼️ selecione.png
│           ├── 🖼️ start.png
│           ├── 🖼️ statistics.png
│           └── 🖼️ stop.png
├── 📁 data
│   ├── 📁 Charts
│   │   ├── ⚡ ch_01_ChartLayoutEngine.cpp
│   │   ├── ⚡ ch_01_ChartLayoutEngine.h
│   │   ├── ⚡ ch_02_ChartWidgetFactory.cpp
│   │   ├── ⚡ ch_02_ChartWidgetFactory.h
│   │   ├── ⚡ ch_03_LegacyVariantChartAdapter.cpp
│   │   ├── ⚡ ch_03_LegacyVariantChartAdapter.h
│   │   ├── ⚡ ch_04_QmlRegistration.cpp
│   │   ├── ⚡ ch_04_QmlRegistration.h
│   │   ├── ⚡ ch_05_QuickChartItem.cpp
│   │   ├── ⚡ ch_05_QuickChartItem.h
│   │   ├── ⚡ ch_06_WidgetChartView.cpp
│   │   ├── ⚡ ch_06_WidgetChartView.h
│   │   ├── ⚡ ch_07_LgplChartModel.h
│   │   ├── ⚡ ch_08_RasterChartView.cpp
│   │   └── ⚡ ch_08_RasterChartView.h
│   ├── 📁 GeradorEstatisticas
│   │   ├── ⚡ gst_01_BaseGerador.cpp
│   │   ├── ⚡ gst_01_BaseGerador.h
│   │   ├── ⚡ gst_02_GraficoPizza.cpp
│   │   ├── ⚡ gst_02_GraficoPizza.h
│   │   ├── ⚡ gst_03_GraficoBarras.cpp
│   │   ├── ⚡ gst_03_GraficoBarras.h
│   │   ├── ⚡ gst_04_GraficoTimeline.cpp
│   │   ├── ⚡ gst_04_GraficoTimeline.h
│   │   ├── ⚡ gst_05_GraficoTreemap.cpp
│   │   ├── ⚡ gst_05_GraficoTreemap.h
│   │   ├── ⚡ gst_06_GraficoHistograma.cpp
│   │   ├── ⚡ gst_06_GraficoHistograma.h
│   │   ├── ⚡ gst_07_GraficoPareto.cpp
│   │   ├── ⚡ gst_07_GraficoPareto.h
│   │   ├── ⚡ gst_08_GraficoLinha.cpp
│   │   ├── ⚡ gst_08_GraficoLinha.h
│   │   ├── ⚡ gst_09_GraficoBoxplot.cpp
│   │   ├── ⚡ gst_09_GraficoBoxplot.h
│   │   ├── ⚡ gst_10_GraficoRadarEventos.cpp
│   │   ├── ⚡ gst_10_GraficoRadarEventos.h
│   │   ├── ⚡ gst_11_GraficoHeatmap.cpp
│   │   ├── ⚡ gst_11_GraficoHeatmap.h
│   │   ├── ⚡ gst_12_GraficoScatter.cpp
│   │   ├── ⚡ gst_12_GraficoScatter.h
│   │   ├── ⚡ gst_13_GraficoSankey.cpp
│   │   ├── ⚡ gst_13_GraficoSankey.h
│   │   ├── ⚡ gst_14_GraficoRadar.cpp
│   │   ├── ⚡ gst_14_GraficoRadar.h
│   │   ├── ⚡ gst_15_GraficoDotplot.cpp
│   │   ├── ⚡ gst_15_GraficoDotplot.h
│   │   ├── ⚡ gst_16_GraficoSankeyEventoCaminho.cpp
│   │   ├── ⚡ gst_16_GraficoSankeyEventoCaminho.h
│   │   ├── ⚡ gst_17_GraficoSankeyTipoCaminho.cpp
│   │   ├── ⚡ gst_17_GraficoSankeyTipoCaminho.h
│   │   ├── ⚡ gst_18_GraficoArvoreDiretorios.cpp
│   │   └── ⚡ gst_18_GraficoArvoreDiretorios.h
│   ├── ⚡ st_01_GeradorEstatisticas.cpp
│   └── ⚡ st_01_GeradorEstatisticas.h
├── 📁 gui
│   ├── ⚡ ic_01_InterfaceMonitor.cpp
│   ├── ⚡ ic_01_InterfaceMonitor.h
│   ├── ⚡ ic_02_Inicializador.cpp
│   ├── ⚡ ic_02_Inicializador.h
│   ├── ⚡ ic_03_Configurador.cpp
│   ├── ⚡ ic_03_Configurador.h
│   ├── ⚡ ic_04_Atualizador.cpp
│   ├── ⚡ ic_04_Atualizador.h
│   ├── ⚡ ic_05_GerenciadorProgresso.cpp
│   ├── ⚡ ic_05_GerenciadorProgresso.h
│   ├── ⚡ ic_06_GerenciadorMensagens.cpp
│   ├── ⚡ ic_06_GerenciadorMensagens.h
│   ├── ⚡ ic_07_ManipuladorTabela.cpp
│   ├── ⚡ ic_07_ManipuladorTabela.h
│   ├── ⚡ ic_08_Internacionalizador.cpp
│   └── ⚡ ic_08_Internacionalizador.h
├── 📁 locale
│   ├── 📄 linceu_de_DE.qm
│   ├── 📄 linceu_de_DE.ts
│   ├── 📄 linceu_en_US.qm
│   ├── 📄 linceu_en_US.ts
│   ├── 📄 linceu_es_ES.qm
│   ├── 📄 linceu_es_ES.ts
│   ├── 📄 linceu_fr_FR.qm
│   ├── 📄 linceu_fr_FR.ts
│   ├── 📄 linceu_it_IT.qm
│   ├── 📄 linceu_it_IT.ts
│   ├── 📄 linceu_pt_BR.qm
│   └── 📄 linceu_pt_BR.ts
├── 📁 security
│   ├── ⚡ agre_01_motor_regras.cpp
│   ├── ⚡ agre_01_motor_regras.h
│   ├── ⚡ attr_01_atribuicao_forense.cpp
│   ├── ⚡ attr_01_atribuicao_forense.h
│   ├── ⚡ base_01_baseline_criptografico.cpp
│   ├── ⚡ base_01_baseline_criptografico.h
│   ├── ⚡ evid_01_exportacao_evidencia.cpp
│   ├── ⚡ evid_01_exportacao_evidencia.h
│   ├── ⚡ exec_01_inteligencia_executaveis.cpp
│   ├── ⚡ exec_01_inteligencia_executaveis.h
│   ├── ⚡ mdir_01_multi_diretorio.cpp
│   ├── ⚡ mdir_01_multi_diretorio.h
│   ├── ⚡ quar_01_quarentena.cpp
│   ├── ⚡ quar_01_quarentena.h
│   ├── ⚡ risk_01_score_risco.cpp
│   ├── ⚡ risk_01_score_risco.h
│   ├── ⚡ sdiff_01_diff_conteudo.cpp
│   ├── ⚡ sdiff_01_diff_conteudo.h
│   ├── ⚡ secore_01_postgres_utils.cpp
│   ├── ⚡ secore_01_postgres_utils.h
│   ├── ⚡ secore_02_SecurityManager.cpp
│   ├── ⚡ secore_02_SecurityManager.h
│   ├── ⚡ secore_03_evento_campos.cpp
│   ├── ⚡ secore_03_evento_campos.h
│   ├── ⚡ secore_04_traduzir_strings_ui.cpp
│   ├── ⚡ secore_04_traduzir_strings_ui.h
│   ├── ⚡ secore_05_SecurityDbWriter.cpp
│   ├── ⚡ secore_05_SecurityDbWriter.h
│   ├── ⚡ secui_00_helpers.cpp
│   ├── ⚡ secui_00_helpers.h
│   ├── ⚡ secui_01_SecurityUiController.cpp
│   ├── ⚡ secui_01_SecurityUiController.h
│   ├── ⚡ secui_02_MultiDirectoryDialog.cpp
│   ├── ⚡ secui_02_MultiDirectoryDialog.h
│   ├── ⚡ secui_03_AlertCenterDialog.cpp
│   ├── ⚡ secui_03_AlertCenterDialog.h
│   ├── ⚡ secui_04_SessionCenterDialog.cpp
│   ├── ⚡ secui_04_SessionCenterDialog.h
│   ├── ⚡ secui_05_RecoveryDialog.cpp
│   ├── ⚡ secui_05_RecoveryDialog.h
│   ├── ⚡ secui_06_EvidenceDialog.cpp
│   ├── ⚡ secui_06_EvidenceDialog.h
│   ├── ⚡ secui_07_configurar_menu_seguranca.cpp
│   ├── ⚡ secui_07_configurar_menu_seguranca.h
│   ├── ⚡ secui_08_NotificationPopupConfigDialog.cpp
│   ├── ⚡ secui_08_NotificationPopupConfigDialog.h
│   ├── ⚡ sess_01_correlacao_sessoes.cpp
│   └── ⚡ sess_01_correlacao_sessoes.h
├── 📁 services
│   ├── 📁 GerenciamentoBaseEvento
│   │   ├── ⚡ gbank_00_operacoes.h
│   │   ├── ⚡ gbank_01_set_callback.cpp
│   │   ├── ⚡ gbank_01_set_callback.h
│   │   ├── ⚡ gbank_02_criar_banco_de_dados.cpp
│   │   ├── ⚡ gbank_02_criar_banco_de_dados.h
│   │   ├── ⚡ gbank_03_processar_exclusao.cpp
│   │   ├── ⚡ gbank_03_processar_exclusao.h
│   │   ├── ⚡ gbank_04_registrar_evento_especifico.cpp
│   │   ├── ⚡ gbank_04_registrar_evento_especifico.h
│   │   ├── ⚡ gbank_05_obter_metadados_arquivo_excluido.cpp
│   │   ├── ⚡ gbank_05_obter_metadados_arquivo_excluido.h
│   │   ├── ⚡ gbank_06_registrar_evento_no_banco.cpp
│   │   ├── ⚡ gbank_06_registrar_evento_no_banco.h
│   │   ├── ⚡ gbank_07_atualizar_interface_apos_evento.cpp
│   │   ├── ⚡ gbank_07_atualizar_interface_apos_evento.h
│   │   ├── ⚡ gbank_08_scan_directory.cpp
│   │   ├── ⚡ gbank_08_scan_directory.h
│   │   ├── ⚡ gbank_09_get_tipo_from_snapshot.cpp
│   │   ├── ⚡ gbank_09_get_tipo_from_snapshot.h
│   │   ├── ⚡ gbank_10_is_directory.cpp
│   │   ├── ⚡ gbank_10_is_directory.h
│   │   ├── ⚡ gbank_11_limpar_registros.cpp
│   │   ├── ⚡ gbank_11_limpar_registros.h
│   │   ├── ⚡ gbank_12_obter_tipo_anterior.cpp
│   │   ├── ⚡ gbank_12_obter_tipo_anterior.h
│   │   ├── ⚡ gbank_13_notificar_evento.cpp
│   │   ├── ⚡ gbank_13_notificar_evento.h
│   │   ├── ⚡ gbank_14_remover_exclusao_temporaria.cpp
│   │   ├── ⚡ gbank_14_remover_exclusao_temporaria.h
│   │   ├── ⚡ gbank_15_criar_evento_exclusao.cpp
│   │   ├── ⚡ gbank_15_criar_evento_exclusao.h
│   │   ├── ⚡ gbank_16_criar_evento_padrao.cpp
│   │   ├── ⚡ gbank_16_criar_evento_padrao.h
│   │   ├── ⚡ gbank_17_atualizar_interface_apos_exclusao.cpp
│   │   ├── ⚡ gbank_17_atualizar_interface_apos_exclusao.h
│   │   ├── ⚡ gbank_18_processar_eventos_movimentacao.cpp
│   │   ├── ⚡ gbank_18_processar_eventos_movimentacao.h
│   │   ├── ⚡ gbank_19_inserir_evento_movido.cpp
│   │   ├── ⚡ gbank_19_inserir_evento_movido.h
│   │   ├── ⚡ gbank_20_db_writer.cpp
│   │   └── ⚡ gbank_20_db_writer.h
│   ├── 📁 GerenciamentoDiretorioScanner
│   │   ├── ⚡ gscanner_01_process_batch.cpp
│   │   ├── ⚡ gscanner_01_process_batch.h
│   │   ├── ⚡ gscanner_02_scan_directory.cpp
│   │   ├── ⚡ gscanner_02_scan_directory.h
│   │   ├── ⚡ gscanner_03_processar_fila.cpp
│   │   ├── ⚡ gscanner_03_processar_fila.h
│   │   ├── ⚡ gscanner_04_get_file_type.cpp
│   │   ├── ⚡ gscanner_04_get_file_type.h
│   │   ├── ⚡ gscanner_05_processar_item.cpp
│   │   ├── ⚡ gscanner_05_processar_item.h
│   │   ├── ⚡ gscanner_06_atualizar_progresso.cpp
│   │   ├── ⚡ gscanner_06_atualizar_progresso.h
│   │   ├── ⚡ gscanner_07_atualizar_interface.cpp
│   │   ├── ⚡ gscanner_07_atualizar_interface.h
│   │   ├── ⚡ gscanner_08_finalizar_scan.cpp
│   │   ├── ⚡ gscanner_08_finalizar_scan.h
│   │   ├── ⚡ gscanner_09_scan_worker_run.cpp
│   │   └── ⚡ gscanner_09_scan_worker_run.h
│   ├── 📁 GerenciamentoEventoModificado
│   │   ├── ⚡ gevmod_01_calcular_intervalo.cpp
│   │   ├── ⚡ gevmod_01_calcular_intervalo.h
│   │   ├── ⚡ gevmod_02_calcular_intervalo_original.cpp
│   │   ├── ⚡ gevmod_02_calcular_intervalo_original.h
│   │   ├── ⚡ gevmod_03_is_arquivo_codigo_grande.cpp
│   │   ├── ⚡ gevmod_03_is_arquivo_codigo_grande.h
│   │   ├── ⚡ gevmod_04_limpar_cache_metadados.cpp
│   │   ├── ⚡ gevmod_04_limpar_cache_metadados.h
│   │   ├── ⚡ gevmod_05_processar.cpp
│   │   ├── ⚡ gevmod_05_processar.h
│   │   ├── ⚡ gevmod_06_processar_massivo.cpp
│   │   ├── ⚡ gevmod_06_processar_massivo.h
│   │   ├── ⚡ gevmod_07_processar_normal.cpp
│   │   └── ⚡ gevmod_07_processar_normal.h
│   ├── 📁 GerenciamentoEventoMovido
│   │   ├── ⚡ gevmov_01_MovimentacaoWorker.cpp
│   │   ├── ⚡ gevmov_01_MovimentacaoWorker.h
│   │   ├── ⚡ gevmov_02_verificar_movimentacao.cpp
│   │   ├── ⚡ gevmov_02_verificar_movimentacao.h
│   │   ├── ⚡ gevmov_03_remover_exclusao.cpp
│   │   ├── ⚡ gevmov_03_remover_exclusao.h
│   │   ├── ⚡ gevmov_04_atualizar_linha_recente.cpp
│   │   ├── ⚡ gevmov_04_atualizar_linha_recente.h
│   │   ├── ⚡ gevmov_05_atualizar_tabela_completa.cpp
│   │   ├── ⚡ gevmov_05_atualizar_tabela_completa.h
│   │   ├── ⚡ gevmov_06_inicializar_sistema_evento.cpp
│   │   ├── ⚡ gevmov_06_inicializar_sistema_evento.h
│   │   ├── ⚡ gevmov_07_processar_exclusoes_pendentes.cpp
│   │   ├── ⚡ gevmov_07_processar_exclusoes_pendentes.h
│   │   ├── ⚡ gevmov_08_adicionar_item_tabela.cpp
│   │   ├── ⚡ gevmov_08_adicionar_item_tabela.h
│   │   ├── ⚡ gevmov_09_adicionar_evento.cpp
│   │   └── ⚡ gevmov_09_adicionar_evento.h
│   ├── 📁 GerenciamentoMetadados
│   │   ├── ⚡ gmet_01_ExtrairMetadados.cpp
│   │   ├── ⚡ gmet_01_ExtrairMetadados.h
│   │   ├── ⚡ gmet_02_ExtrairMetadadosCodigoFonte.cpp
│   │   ├── ⚡ gmet_02_ExtrairMetadadosCodigoFonte.h
│   │   ├── ⚡ gmet_03_ExtrairMetadadosImagem.cpp
│   │   ├── ⚡ gmet_03_ExtrairMetadadosImagem.h
│   │   ├── ⚡ gmet_04_ExtrairMetadadosAudio.cpp
│   │   ├── ⚡ gmet_04_ExtrairMetadadosAudio.h
│   │   ├── ⚡ gmet_05_ExtrairMetadadosVideo.cpp
│   │   ├── ⚡ gmet_05_ExtrairMetadadosVideo.h
│   │   ├── ⚡ gmet_06_ExtrairMetadadosDocumento.cpp
│   │   ├── ⚡ gmet_06_ExtrairMetadadosDocumento.h
│   │   ├── ⚡ gmet_07_ExtrairMetadadosPlanilha.cpp
│   │   ├── ⚡ gmet_07_ExtrairMetadadosPlanilha.h
│   │   ├── ⚡ gmet_08_ExtrairMetadadosApresentacao.cpp
│   │   ├── ⚡ gmet_08_ExtrairMetadadosApresentacao.h
│   │   ├── ⚡ gmet_09_ExtrairMetadadosBancoDados.cpp
│   │   ├── ⚡ gmet_09_ExtrairMetadadosBancoDados.h
│   │   ├── ⚡ gmet_10_ExtrairMetadadosExecutavel.cpp
│   │   ├── ⚡ gmet_10_ExtrairMetadadosExecutavel.h
│   │   ├── ⚡ gmet_11_ExtrairMetadadosTemporario.cpp
│   │   ├── ⚡ gmet_11_ExtrairMetadadosTemporario.h
│   │   ├── ⚡ gmet_12_ExtrairMetadadosCompactados.cpp
│   │   ├── ⚡ gmet_12_ExtrairMetadadosCompactados.h
│   │   ├── ⚡ gmet_13_ExtrairMetadadosBackup.cpp
│   │   ├── ⚡ gmet_13_ExtrairMetadadosBackup.h
│   │   ├── ⚡ gmet_14_ExtrairMetadadosLog.cpp
│   │   ├── ⚡ gmet_14_ExtrairMetadadosLog.h
│   │   ├── ⚡ gmet_15_ExtrairMetadadosConfig.cpp
│   │   ├── ⚡ gmet_15_ExtrairMetadadosConfig.h
│   │   ├── ⚡ gmet_16_ExtrairMetadadosOlefile.cpp
│   │   ├── ⚡ gmet_16_ExtrairMetadadosOlefile.h
│   │   ├── ⚡ gmet_17_ExtrairMetadadosDadosEstruturados.cpp
│   │   ├── ⚡ gmet_17_ExtrairMetadadosDadosEstruturados.h
│   │   ├── ⚡ gmet_18_ExtrairMetadadosCompletos.cpp
│   │   ├── ⚡ gmet_18_ExtrairMetadadosCompletos.h
│   │   ├── ⚡ gmet_19_GetTipoArquivo.cpp
│   │   ├── ⚡ gmet_19_GetTipoArquivo.h
│   │   ├── ⚡ gmet_20_GetTamanhoDiretorioArquivo.cpp
│   │   ├── ⚡ gmet_20_GetTamanhoDiretorioArquivo.h
│   │   ├── ⚡ gmet_21_GetFormataTamanho.cpp
│   │   ├── ⚡ gmet_21_GetFormataTamanho.h
│   │   ├── ⚡ gmet_22_GetAtributosArquivo.cpp
│   │   ├── ⚡ gmet_22_GetAtributosArquivo.h
│   │   ├── ⚡ gmet_23_GetAutorArquivo.cpp
│   │   ├── ⚡ gmet_23_GetAutorArquivo.h
│   │   ├── ⚡ gmet_24_GetDimensoesArquivo.cpp
│   │   ├── ⚡ gmet_24_GetDimensoesArquivo.h
│   │   ├── ⚡ gmet_25_GetDuracaoArquivo.cpp
│   │   ├── ⚡ gmet_25_GetDuracaoArquivo.h
│   │   ├── ⚡ gmet_26_GetTaxaBitsArquivo.cpp
│   │   ├── ⚡ gmet_26_GetTaxaBitsArquivo.h
│   │   ├── ⚡ gmet_27_GetProtecaoArquivo.cpp
│   │   ├── ⚡ gmet_27_GetProtecaoArquivo.h
│   │   ├── ⚡ gmet_28_AtualizarInterface.cpp
│   │   ├── ⚡ gmet_28_AtualizarInterface.h
│   │   ├── ⚡ gmet_29_SalvarEstadoTabela.cpp
│   │   ├── ⚡ gmet_29_SalvarEstadoTabela.h
│   │   ├── ⚡ gmet_30_RestaurarEstadoTabela.cpp
│   │   ├── ⚡ gmet_30_RestaurarEstadoTabela.h
│   │   ├── ⚡ gmet_31_CarregarConfiguracoes.cpp
│   │   ├── ⚡ gmet_31_CarregarConfiguracoes.h
│   │   ├── ⚡ gmet_32_SalvarConfiguracoes.cpp
│   │   ├── ⚡ gmet_32_SalvarConfiguracoes.h
│   │   ├── ⚡ gmet_33_ProcessarFilaMetadados.cpp
│   │   ├── ⚡ gmet_33_ProcessarFilaMetadados.h
│   │   ├── ⚡ gmet_34_CallbackMetadados.cpp
│   │   ├── ⚡ gmet_34_CallbackMetadados.h
│   │   ├── ⚡ gmet_35_ConfigurarTabela.cpp
│   │   ├── ⚡ gmet_35_ConfigurarTabela.h
│   │   ├── ⚡ gmet_36_MostrarDialogoConfiguracao.cpp
│   │   ├── ⚡ gmet_36_MostrarDialogoConfiguracao.h
│   │   ├── ⚡ gmet_37_InvalidarCacheDiretoriosRelacionados.cpp
│   │   ├── ⚡ gmet_37_InvalidarCacheDiretoriosRelacionados.h
│   │   ├── ⚡ gmet_38_GetMetadados.cpp
│   │   ├── ⚡ gmet_38_GetMetadados.h
│   │   ├── ⚡ gmet_39_IdentificarTipoArquivo.cpp
│   │   ├── ⚡ gmet_39_IdentificarTipoArquivo.h
│   │   ├── ⚡ gmet_40_AdicionarMetadadosAoItem.cpp
│   │   ├── ⚡ gmet_40_AdicionarMetadadosAoItem.h
│   │   ├── ⚡ gmet_41_ProcessarColunaThread.cpp
│   │   ├── ⚡ gmet_41_ProcessarColunaThread.h
│   │   ├── ⚡ gmet_42_AdicionarItemParaColuna.cpp
│   │   ├── ⚡ gmet_42_AdicionarItemParaColuna.h
│   │   ├── ⚡ gmet_43_AtualizarColunaInterface.cpp
│   │   ├── ⚡ gmet_43_AtualizarColunaInterface.h
│   │   ├── ⚡ gmet_44_ExtrairMetadadosEmLote.cpp
│   │   ├── ⚡ gmet_44_ExtrairMetadadosEmLote.h
│   │   ├── ⚡ gmet_45_MetadadosExtraidosCallback.cpp
│   │   ├── ⚡ gmet_45_MetadadosExtraidosCallback.h
│   │   ├── ⚡ gmet_46_GetSizeUnit.cpp
│   │   ├── ⚡ gmet_46_GetSizeUnit.h
│   │   ├── ⚡ gmet_47_GetTamanhoBytes.cpp
│   │   └── ⚡ gmet_47_GetTamanhoBytes.h
│   ├── 📁 GerenciamentoObservador
│   │   ├── ⚡ gob_01_DetectarOperacaoMassiva.cpp
│   │   ├── ⚡ gob_01_DetectarOperacaoMassiva.h
│   │   ├── ⚡ gob_02_Iniciar.cpp
│   │   ├── ⚡ gob_02_Iniciar.h
│   │   ├── ⚡ gob_03_HandleScanError.cpp
│   │   ├── ⚡ gob_03_HandleScanError.h
│   │   ├── ⚡ gob_04_IniciarMonitoramento.cpp
│   │   ├── ⚡ gob_04_IniciarMonitoramento.h
│   │   ├── ⚡ gob_05_ProcessarBufferEventos.cpp
│   │   ├── ⚡ gob_05_ProcessarBufferEventos.h
│   │   ├── ⚡ gob_06_PararScan.cpp
│   │   ├── ⚡ gob_06_PararScan.h
│   │   ├── ⚡ gob_07_Parar.cpp
│   │   ├── ⚡ gob_07_Parar.h
│   │   ├── ⚡ gob_08_LimparEstado.cpp
│   │   ├── ⚡ gob_08_LimparEstado.h
│   │   ├── ⚡ gob_09_Monitorar.cpp
│   │   ├── ⚡ gob_09_Monitorar.h
│   │   ├── ⚡ gob_10_ProcessarEventoInterno.cpp
│   │   ├── ⚡ gob_10_ProcessarEventoInterno.h
│   │   ├── ⚡ gob_11_ProcessarEvento.cpp
│   │   ├── ⚡ gob_11_ProcessarEvento.h
│   │   ├── ⚡ gob_12_ReiniciarMonitoramento.cpp
│   │   ├── ⚡ gob_12_ReiniciarMonitoramento.h
│   │   ├── ⚡ gob_13_PausarMonitoramentoEscaneamento.cpp
│   │   └── ⚡ gob_13_PausarMonitoramentoEscaneamento.h
│   ├── 📁 GerenciamentoTabela
│   │   ├── ⚡ gtab_01_DetectarTemaWindows.cpp
│   │   ├── ⚡ gtab_01_DetectarTemaWindows.h
│   │   ├── ⚡ gtab_02_CalcularCorTextoIdeal.cpp
│   │   ├── ⚡ gtab_02_CalcularCorTextoIdeal.h
│   │   ├── ⚡ gtab_03_ConfigurarTabela.cpp
│   │   ├── ⚡ gtab_03_ConfigurarTabela.h
│   │   ├── ⚡ gtab_04_AjustarLargurasColunas.cpp
│   │   ├── ⚡ gtab_04_AjustarLargurasColunas.h
│   │   ├── ⚡ gtab_05_AplicarQuebraLinhaCabecalho.cpp
│   │   ├── ⚡ gtab_05_AplicarQuebraLinhaCabecalho.h
│   │   ├── ⚡ gtab_06_AplicarQuebraLinhaTodosCabecalhos.cpp
│   │   ├── ⚡ gtab_06_AplicarQuebraLinhaTodosCabecalhos.h
│   │   ├── ⚡ gtab_07_AjustarAlturaCabecalho.cpp
│   │   ├── ⚡ gtab_07_AjustarAlturaCabecalho.h
│   │   ├── ⚡ gtab_08_RedimensionarCabecalho.cpp
│   │   ├── ⚡ gtab_08_RedimensionarCabecalho.h
│   │   ├── ⚡ gtab_09_AtualizarCabecalhos.cpp
│   │   ├── ⚡ gtab_09_AtualizarCabecalhos.h
│   │   ├── ⚡ gtab_10_AtualizarDadosTabela.cpp
│   │   ├── ⚡ gtab_10_AtualizarDadosTabela.h
│   │   ├── ⚡ gtab_11_AtualizarLinhaMaisRecente.cpp
│   │   ├── ⚡ gtab_11_AtualizarLinhaMaisRecente.h
│   │   ├── ⚡ gtab_12_AtualizarVisualizacaoTabela.cpp
│   │   ├── ⚡ gtab_12_AtualizarVisualizacaoTabela.h
│   │   ├── ⚡ gtab_13_AtualizarVisibilidadeColunas.cpp
│   │   ├── ⚡ gtab_13_AtualizarVisibilidadeColunas.h
│   │   ├── ⚡ gtab_14_InvalidarCacheCores.cpp
│   │   ├── ⚡ gtab_14_InvalidarCacheCores.h
│   │   ├── ⚡ gtab_15_ObterCoresOperacao.cpp
│   │   ├── ⚡ gtab_15_ObterCoresOperacao.h
│   │   ├── ⚡ gtab_16_ObterIndicesColunas.cpp
│   │   ├── ⚡ gtab_16_ObterIndicesColunas.h
│   │   ├── ⚡ gtab_17_AtivarCores.cpp
│   │   ├── ⚡ gtab_17_AtivarCores.h
│   │   ├── ⚡ gtab_18_OcultarCores.cpp
│   │   ├── ⚡ gtab_18_OcultarCores.h
│   │   ├── ⚡ gtab_19_AplicarCoresLinhaEspecifica.cpp
│   │   ├── ⚡ gtab_19_AplicarCoresLinhaEspecifica.h
│   │   ├── ⚡ gtab_20_AplicarCoresTodasColunas.cpp
│   │   ├── ⚡ gtab_20_AplicarCoresTodasColunas.h
│   │   ├── ⚡ gtab_21_RedefinirCoresTodasColunas.cpp
│   │   ├── ⚡ gtab_21_RedefinirCoresTodasColunas.h
│   │   ├── ⚡ gtab_22_RemoverCorColuna.cpp
│   │   ├── ⚡ gtab_22_RemoverCorColuna.h
│   │   ├── ⚡ gtab_23_AtualizarCoresColunas.cpp
│   │   ├── ⚡ gtab_23_AtualizarCoresColunas.h
│   │   ├── ⚡ gtab_24_SetColunaColorir.cpp
│   │   ├── ⚡ gtab_24_SetColunaColorir.h
│   │   ├── ⚡ gtab_25_SetColunasColorirEmMassa.cpp
│   │   ├── ⚡ gtab_25_SetColunasColorirEmMassa.h
│   │   ├── ⚡ gtab_26_RemoverTodasCoresColunas.cpp
│   │   ├── ⚡ gtab_26_RemoverTodasCoresColunas.h
│   │   ├── ⚡ gtab_27_SalvarConfiguracoesCores.cpp
│   │   ├── ⚡ gtab_27_SalvarConfiguracoesCores.h
│   │   ├── ⚡ gtab_28_CarregarConfiguracoesCores.cpp
│   │   ├── ⚡ gtab_28_CarregarConfiguracoesCores.h
│   │   ├── ⚡ gtab_29_EhColunaPersonalizadaColorida.cpp
│   │   ├── ⚡ gtab_29_EhColunaPersonalizadaColorida.h
│   │   ├── ⚡ gtab_30_MostrarDialogoConfiguracao.cpp
│   │   ├── ⚡ gtab_30_MostrarDialogoConfiguracao.h
│   │   ├── ⚡ gtab_31_AjustarCorSelecao.cpp
│   │   ├── ⚡ gtab_31_AjustarCorSelecao.h
│   │   ├── ⚡ gtab_32_WorkerThread.cpp
│   │   ├── ⚡ gtab_32_WorkerThread.h
│   │   ├── ⚡ gtab_33_AtualizarEstiloTema.cpp
│   │   ├── ⚡ gtab_33_AtualizarEstiloTema.h
│   │   ├── ⚡ gtab_34_MonitorTemaWindows.cpp
│   │   ├── ⚡ gtab_34_MonitorTemaWindows.h
│   │   ├── ⚡ gtab_35_IniciarProcessamentoPesado.cpp
│   │   ├── ⚡ gtab_35_IniciarProcessamentoPesado.h
│   │   ├── ⚡ gtab_36_AplicarCoresTodasColunasProcessamento.cpp
│   │   ├── ⚡ gtab_36_AplicarCoresTodasColunasProcessamento.h
│   │   ├── ⚡ gtab_37_ProcessarCoresEmBackground.cpp
│   │   ├── ⚡ gtab_37_ProcessarCoresEmBackground.h
│   │   ├── ⚡ gtab_38_OnCoresProcessadas.cpp
│   │   ├── ⚡ gtab_38_OnCoresProcessadas.h
│   │   ├── ⚡ gtab_39_AtualizarCoresNaInterface.cpp
│   │   ├── ⚡ gtab_39_AtualizarCoresNaInterface.h
│   │   ├── ⚡ gtab_40_AtualizarInterfacePosProcessamento.cpp
│   │   ├── ⚡ gtab_40_AtualizarInterfacePosProcessamento.h
│   │   ├── ⚡ gtab_41_ProcessarSelecaoBackground.cpp
│   │   ├── ⚡ gtab_41_ProcessarSelecaoBackground.h
│   │   ├── ⚡ gtab_42_ShutdownExecutors.cpp
│   │   ├── ⚡ gtab_42_ShutdownExecutors.h
│   │   ├── ⚡ gtab_43_AtualizarCabecalhosBridge.cpp
│   │   ├── ⚡ gtab_43_AtualizarCabecalhosBridge.h
│   │   ├── ⚡ gtab_44_OnIdiomaAlterado.cpp
│   │   ├── ⚡ gtab_44_OnIdiomaAlterado.h
│   │   ├── ⚡ gtab_45_RetraduzirDadosExistentes.cpp
│   │   ├── ⚡ gtab_45_RetraduzirDadosExistentes.h
│   │   ├── ⚡ gtab_46_ExecutarRetraducaoAgendada.cpp
│   │   ├── ⚡ gtab_46_ExecutarRetraducaoAgendada.h
│   │   ├── ⚡ gtab_47_EventTableModel.cpp
│   │   └── ⚡ gtab_47_EventTableModel.h
│   ├── ⚡ ob_01_Observador.cpp
│   ├── ⚡ ob_01_Observador.h
│   ├── ⚡ ob_02_BaseEvento.cpp
│   ├── ⚡ ob_02_BaseEvento.h
│   ├── ⚡ ob_03_DiretorioScanner.cpp
│   ├── ⚡ ob_03_DiretorioScanner.h
│   ├── ⚡ ob_04_EventoAdicionado.cpp
│   ├── ⚡ ob_04_EventoAdicionado.h
│   ├── ⚡ ob_05_EventoExcluido.cpp
│   ├── ⚡ ob_05_EventoExcluido.h
│   ├── ⚡ ob_06_EventoModificado.cpp
│   ├── ⚡ ob_06_EventoModificado.h
│   ├── ⚡ ob_07_EventoRenomeado.cpp
│   ├── ⚡ ob_07_EventoRenomeado.h
│   ├── ⚡ ob_08_EventoMovido.cpp
│   ├── ⚡ ob_08_EventoMovido.h
│   ├── ⚡ ob_09_GerenciadorColunas.cpp
│   ├── ⚡ ob_09_GerenciadorColunas.h
│   ├── ⚡ ob_10_GerenciadorTabela.cpp
│   └── ⚡ ob_10_GerenciadorTabela.h
├── 📁 tools
│   ├── 📁 GerenciamentoAdministradorFiltros
│   │   ├── ⚡ gadmfil_00_operacao_utils.h
│   │   ├── ⚡ gadmfil_01_desconectar_sinais.cpp
│   │   ├── ⚡ gadmfil_01_desconectar_sinais.h
│   │   ├── ⚡ gadmfil_02_aplicar_filtros.cpp
│   │   ├── ⚡ gadmfil_02_aplicar_filtros.h
│   │   ├── ⚡ gadmfil_03_limpar_filtros.cpp
│   │   ├── ⚡ gadmfil_03_limpar_filtros.h
│   │   ├── ⚡ gadmfil_04_sincronizar_menu_principal_com_filtros.cpp
│   │   ├── ⚡ gadmfil_04_sincronizar_menu_principal_com_filtros.h
│   │   ├── ⚡ gadmfil_05_atualizar_contagem.cpp
│   │   ├── ⚡ gadmfil_05_atualizar_contagem.h
│   │   ├── ⚡ gadmfil_06_atualizar_contagem_eventos_monitorados.cpp
│   │   ├── ⚡ gadmfil_06_atualizar_contagem_eventos_monitorados.h
│   │   ├── ⚡ gadmfil_07_verificar_filtro_operacao.cpp
│   │   ├── ⚡ gadmfil_07_verificar_filtro_operacao.h
│   │   ├── ⚡ gadmfil_08_atualizar_contagem_apos_idioma.cpp
│   │   ├── ⚡ gadmfil_08_atualizar_contagem_apos_idioma.h
│   │   ├── ⚡ gadmfil_09_notificar_alteracao_idioma.cpp
│   │   ├── ⚡ gadmfil_09_notificar_alteracao_idioma.h
│   │   ├── ⚡ gadmfil_10_salvar_estado_checkboxes.cpp
│   │   └── ⚡ gadmfil_10_salvar_estado_checkboxes.h
│   ├── 📁 GerenciamentoFiltros
│   │   ├── ⚡ gfil_01_on_idioma_alterado.cpp
│   │   ├── ⚡ gfil_01_on_idioma_alterado.h
│   │   ├── ⚡ gfil_02_desconectar_sinais.cpp
│   │   ├── ⚡ gfil_02_desconectar_sinais.h
│   │   ├── ⚡ gfil_03_atualizar_status.cpp
│   │   ├── ⚡ gfil_03_atualizar_status.h
│   │   ├── ⚡ gfil_04_setup_ui.cpp
│   │   ├── ⚡ gfil_04_setup_ui.h
│   │   ├── ⚡ gfil_05_on_filtro_alterado.cpp
│   │   ├── ⚡ gfil_05_on_filtro_alterado.h
│   │   ├── ⚡ gfil_06_sincronizar_com_menu_principal.cpp
│   │   ├── ⚡ gfil_06_sincronizar_com_menu_principal.h
│   │   ├── ⚡ gfil_07_verificar_filtro_operacao.cpp
│   │   ├── ⚡ gfil_07_verificar_filtro_operacao.h
│   │   ├── ⚡ gfil_08_atualizar_contagem.cpp
│   │   ├── ⚡ gfil_08_atualizar_contagem.h
│   │   ├── ⚡ gfil_09_atualizar_interface.cpp
│   │   ├── ⚡ gfil_09_atualizar_interface.h
│   │   ├── ⚡ gfil_10_limpar_filtros.cpp
│   │   └── ⚡ gfil_10_limpar_filtros.h
│   ├── ⚡ fil_01_Filtros.cpp
│   ├── ⚡ fil_01_Filtros.h
│   ├── ⚡ fil_02_AdministradorCalendario.cpp
│   ├── ⚡ fil_02_AdministradorCalendario.h
│   ├── ⚡ fil_03_AdministradorFiltros.cpp
│   └── ⚡ fil_03_AdministradorFiltros.h
├── 📁 ui
│   ├── 📁 GerenciamentoBotoes
│   │   ├── ⚡ geb_01_selecionar_diretorio.cpp
│   │   ├── ⚡ geb_01_selecionar_diretorio.h
│   │   ├── ⚡ geb_02_alternar_analise_diretorio.cpp
│   │   ├── ⚡ geb_02_alternar_analise_diretorio.h
│   │   ├── ⚡ geb_03_exportar_dados.cpp
│   │   ├── ⚡ geb_03_exportar_dados.h
│   │   ├── ⚡ geb_04_exportar_para_sqlite.cpp
│   │   ├── ⚡ geb_04_exportar_para_sqlite.h
│   │   ├── ⚡ geb_05_limpar_dados.cpp
│   │   ├── ⚡ geb_05_limpar_dados.h
│   │   ├── ⚡ geb_06_pausar_monitoramento_ou_escaneamento.cpp
│   │   └── ⚡ geb_06_pausar_monitoramento_ou_escaneamento.h
│   ├── 📁 GerenciamentoDialogoCores
│   │   ├── ⚡ gdc_01_setup_ui.cpp
│   │   ├── ⚡ gdc_01_setup_ui.h
│   │   ├── ⚡ gdc_02_adicionar_grid_cores.cpp
│   │   ├── ⚡ gdc_02_adicionar_grid_cores.h
│   │   ├── ⚡ gdc_03_selecionar_cor.cpp
│   │   ├── ⚡ gdc_03_selecionar_cor.h
│   │   ├── ⚡ gdc_04_abrir_seletor_avancado.cpp
│   │   ├── ⚡ gdc_04_abrir_seletor_avancado.h
│   │   ├── ⚡ gdc_05_accept.cpp
│   │   ├── ⚡ gdc_05_accept.h
│   │   ├── ⚡ gdc_06_traduzir_dialogo_cores.cpp
│   │   ├── ⚡ gdc_06_traduzir_dialogo_cores.h
│   │   ├── ⚡ gdc_07_obter_cor.cpp
│   │   ├── ⚡ gdc_07_obter_cor.h
│   │   ├── ⚡ gdc_08_atualizar_traducoes.cpp
│   │   └── ⚡ gdc_08_atualizar_traducoes.h
│   ├── 📁 GerenciamentoEstatisticasUI
│   │   ├── ⚡ geui_01_mostrar_estatisticas.cpp
│   │   ├── ⚡ geui_01_mostrar_estatisticas.h
│   │   ├── ⚡ geui_02_salvar_todos_graficos.cpp
│   │   ├── ⚡ geui_02_salvar_todos_graficos.h
│   │   ├── ⚡ geui_03_limpar_referencia_dialog.cpp
│   │   ├── ⚡ geui_03_limpar_referencia_dialog.h
│   │   ├── ⚡ geui_04_atualizar_graficos_apos_mudanca_idioma.cpp
│   │   ├── ⚡ geui_04_atualizar_graficos_apos_mudanca_idioma.h
│   │   ├── ⚡ geui_05_criar_painel_selecao.cpp
│   │   ├── ⚡ geui_05_criar_painel_selecao.h
│   │   ├── ⚡ geui_06_criar_painel_graficos.cpp
│   │   ├── ⚡ geui_06_criar_painel_graficos.h
│   │   ├── ⚡ geui_07_criar_botao_toggle_painel.cpp
│   │   ├── ⚡ geui_07_criar_botao_toggle_painel.h
│   │   ├── ⚡ geui_08_criar_lista_graficos.cpp
│   │   ├── ⚡ geui_08_criar_lista_graficos.h
│   │   ├── ⚡ geui_09_criar_mapeamento_funcoes.cpp
│   │   ├── ⚡ geui_09_criar_mapeamento_funcoes.h
│   │   ├── ⚡ geui_10_popular_checkboxes.cpp
│   │   ├── ⚡ geui_10_popular_checkboxes.h
│   │   ├── ⚡ geui_11_verificar_estado_checkbox_todos.cpp
│   │   ├── ⚡ geui_11_verificar_estado_checkbox_todos.h
│   │   ├── ⚡ geui_12_alternar_todos_checkboxes.cpp
│   │   ├── ⚡ geui_12_alternar_todos_checkboxes.h
│   │   ├── ⚡ geui_13_obter_estados_checkboxes.cpp
│   │   ├── ⚡ geui_13_obter_estados_checkboxes.h
│   │   ├── ⚡ geui_14_gerar_todos_graficos.cpp
│   │   ├── ⚡ geui_14_gerar_todos_graficos.h
│   │   ├── ⚡ geui_15_atualizar_graficos.cpp
│   │   ├── ⚡ geui_15_atualizar_graficos.h
│   │   ├── ⚡ geui_16_atualizar_graficos_sem_fechar.cpp
│   │   ├── ⚡ geui_16_atualizar_graficos_sem_fechar.h
│   │   ├── ⚡ geui_17_regenerar_graficos_existentes.cpp
│   │   ├── ⚡ geui_17_regenerar_graficos_existentes.h
│   │   ├── ⚡ geui_18_salvar_graficos_selecionados.cpp
│   │   ├── ⚡ geui_18_salvar_graficos_selecionados.h
│   │   ├── ⚡ geui_19_toggle_painel_selecao.cpp
│   │   ├── ⚡ geui_19_toggle_painel_selecao.h
│   │   ├── ⚡ geui_20_atualizar_textos_painel_selecao.cpp
│   │   ├── ⚡ geui_20_atualizar_textos_painel_selecao.h
│   │   ├── ⚡ geui_21_atualizar_textos_checkboxes.cpp
│   │   ├── ⚡ geui_21_atualizar_textos_checkboxes.h
│   │   ├── ⚡ geui_22_atualizar_layout_apos_mudanca_botao.cpp
│   │   ├── ⚡ geui_22_atualizar_layout_apos_mudanca_botao.h
│   │   ├── ⚡ geui_23_atualizar_checkboxes_graficos.cpp
│   │   ├── ⚡ geui_23_atualizar_checkboxes_graficos.h
│   │   ├── ⚡ geui_24_atualizar_abas_graficos.cpp
│   │   ├── ⚡ geui_24_atualizar_abas_graficos.h
│   │   ├── ⚡ geui_25_atualizar_dados_graficos_com_novos_titulos.cpp
│   │   ├── ⚡ geui_25_atualizar_dados_graficos_com_novos_titulos.h
│   │   ├── ⚡ geui_26_calcular_largura_ideal.cpp
│   │   ├── ⚡ geui_26_calcular_largura_ideal.h
│   │   ├── ⚡ geui_27_ajustar_largura_painel_selecao.cpp
│   │   ├── ⚡ geui_27_ajustar_largura_painel_selecao.h
│   │   ├── ⚡ geui_28_traduzir_botoes_detalhes.cpp
│   │   ├── ⚡ geui_28_traduzir_botoes_detalhes.h
│   │   ├── ⚡ geui_29_abrir_diretorio.cpp
│   │   ├── ⚡ geui_29_abrir_diretorio.h
│   │   ├── ⚡ geui_30_botao_rotacionado.cpp
│   │   ├── ⚡ geui_30_botao_rotacionado.h
│   │   ├── ⚡ geui_31_worker_grafico.cpp
│   │   └── ⚡ geui_31_worker_grafico.h
│   ├── 📁 GerenciamentoEventosUI
│   │   ├── ⚡ geve_01_alternar_filtro.cpp
│   │   ├── ⚡ geve_01_alternar_filtro.h
│   │   ├── ⚡ geve_02_alternar_visibilidade_coluna.cpp
│   │   ├── ⚡ geve_02_alternar_visibilidade_coluna.h
│   │   ├── ⚡ geve_03_resetar_colunas.cpp
│   │   ├── ⚡ geve_03_resetar_colunas.h
│   │   ├── ⚡ geve_04_resetar_cores.cpp
│   │   ├── ⚡ geve_04_resetar_cores.h
│   │   ├── ⚡ geve_05_redefinir_todas_colunas_cores.cpp
│   │   ├── ⚡ geve_05_redefinir_todas_colunas_cores.h
│   │   ├── ⚡ geve_06_alterar_idioma.cpp
│   │   ├── ⚡ geve_06_alterar_idioma.h
│   │   ├── ⚡ geve_07_confirmar_mudanca_idioma.cpp
│   │   ├── ⚡ geve_07_confirmar_mudanca_idioma.h
│   │   ├── ⚡ geve_08_limpar_dados_monitorados.cpp
│   │   ├── ⚡ geve_08_limpar_dados_monitorados.h
│   │   ├── ⚡ geve_09_limpar_dados_interface.cpp
│   │   ├── ⚡ geve_09_limpar_dados_interface.h
│   │   ├── ⚡ geve_10_reiniciar_aplicativo.cpp
│   │   ├── ⚡ geve_10_reiniciar_aplicativo.h
│   │   ├── ⚡ geve_11_atualizar_interface.cpp
│   │   └── ⚡ geve_11_atualizar_interface.h
│   ├── 📁 GerenciamentoLocalizadorQt
│   │   ├── ⚡ glqt_01_inicializar_tradutor_metadados.cpp
│   │   ├── ⚡ glqt_01_inicializar_tradutor_metadados.h
│   │   ├── ⚡ glqt_02_criar_mapa_compatibilidade.cpp
│   │   ├── ⚡ glqt_02_criar_mapa_compatibilidade.h
│   │   ├── ⚡ glqt_03_carregar_tradutor.cpp
│   │   ├── ⚡ glqt_03_carregar_tradutor.h
│   │   ├── ⚡ glqt_04_set_idioma.cpp
│   │   ├── ⚡ glqt_04_set_idioma.h
│   │   ├── ⚡ glqt_05_get_text.cpp
│   │   ├── ⚡ glqt_05_get_text.h
│   │   ├── ⚡ glqt_06_get_fallback_text.cpp
│   │   ├── ⚡ glqt_06_get_fallback_text.h
│   │   ├── ⚡ glqt_07_tr.cpp
│   │   ├── ⚡ glqt_07_tr.h
│   │   ├── ⚡ glqt_08_salvar_preferencia_idioma.cpp
│   │   ├── ⚡ glqt_08_salvar_preferencia_idioma.h
│   │   ├── ⚡ glqt_09_carregar_preferencia_idioma.cpp
│   │   ├── ⚡ glqt_09_carregar_preferencia_idioma.h
│   │   ├── ⚡ glqt_10_get_idiomas_disponiveis.cpp
│   │   ├── ⚡ glqt_10_get_idiomas_disponiveis.h
│   │   ├── ⚡ glqt_11_traduzir_tipo_operacao.cpp
│   │   ├── ⚡ glqt_11_traduzir_tipo_operacao.h
│   │   ├── ⚡ glqt_12_traduzir_metadados.cpp
│   │   ├── ⚡ glqt_12_traduzir_metadados.h
│   │   ├── ⚡ glqt_13_traduzir_tipo_operacao_fallback.cpp
│   │   ├── ⚡ glqt_13_traduzir_tipo_operacao_fallback.h
│   │   ├── ⚡ glqt_14_criar_arquivos_traducao.cpp
│   │   └── ⚡ glqt_14_criar_arquivos_traducao.h
│   ├── 📁 GerenciamentoMenusUI
│   │   ├── ⚡ gmui_01_MenuPersistente.cpp
│   │   ├── ⚡ gmui_01_MenuPersistente.h
│   │   ├── ⚡ gmui_02_GerenciadorCores.cpp
│   │   ├── ⚡ gmui_02_GerenciadorCores.h
│   │   ├── ⚡ gmui_03_SobreDialog.cpp
│   │   ├── ⚡ gmui_03_SobreDialog.h
│   │   ├── ⚡ gmui_04_criar_menu_principal.cpp
│   │   ├── ⚡ gmui_04_criar_menu_principal.h
│   │   ├── ⚡ gmui_05_configurar_menu_arquivo.cpp
│   │   ├── ⚡ gmui_05_configurar_menu_arquivo.h
│   │   ├── ⚡ gmui_06_configurar_menu_configuracoes.cpp
│   │   ├── ⚡ gmui_06_configurar_menu_configuracoes.h
│   │   ├── ⚡ gmui_07_configurar_menu_opcoes.cpp
│   │   ├── ⚡ gmui_07_configurar_menu_opcoes.h
│   │   ├── ⚡ gmui_08_criar_submenu_cores.cpp
│   │   ├── ⚡ gmui_08_criar_submenu_cores.h
│   │   ├── ⚡ gmui_09_criar_icone_cor.cpp
│   │   ├── ⚡ gmui_09_criar_icone_cor.h
│   │   ├── ⚡ gmui_10_abrir_dialogo_cor.cpp
│   │   ├── ⚡ gmui_10_abrir_dialogo_cor.h
│   │   ├── ⚡ gmui_11_criar_submenu_colunas.cpp
│   │   ├── ⚡ gmui_11_criar_submenu_colunas.h
│   │   ├── ⚡ gmui_12_alternar_ordenacao_linhas.cpp
│   │   ├── ⚡ gmui_12_alternar_ordenacao_linhas.h
│   │   ├── ⚡ gmui_13_criar_submenu_colunas_coloridas.cpp
│   │   ├── ⚡ gmui_13_criar_submenu_colunas_coloridas.h
│   │   ├── ⚡ gmui_14_selecionar_todas_colunas.cpp
│   │   ├── ⚡ gmui_14_selecionar_todas_colunas.h
│   │   ├── ⚡ gmui_15_selecionar_todas_cores_colunas.cpp
│   │   ├── ⚡ gmui_15_selecionar_todas_cores_colunas.h
│   │   ├── ⚡ gmui_16_criar_submenu_exportacao.cpp
│   │   ├── ⚡ gmui_16_criar_submenu_exportacao.h
│   │   ├── ⚡ gmui_17_resetar_opcoes_exportacao.cpp
│   │   ├── ⚡ gmui_17_resetar_opcoes_exportacao.h
│   │   ├── ⚡ gmui_18_criar_submenu_idiomas.cpp
│   │   ├── ⚡ gmui_18_criar_submenu_idiomas.h
│   │   ├── ⚡ gmui_19_get_texto_traduzido_para_idioma.cpp
│   │   ├── ⚡ gmui_19_get_texto_traduzido_para_idioma.h
│   │   ├── ⚡ gmui_20_confirmar_alteracao_idioma.cpp
│   │   ├── ⚡ gmui_20_confirmar_alteracao_idioma.h
│   │   ├── ⚡ gmui_21_on_traducoes_carregadas.cpp
│   │   ├── ⚡ gmui_21_on_traducoes_carregadas.h
│   │   ├── ⚡ gmui_22_exibir_sobre.cpp
│   │   ├── ⚡ gmui_22_exibir_sobre.h
│   │   ├── ⚡ gmui_24_exibir_manual.cpp
│   │   ├── ⚡ gmui_24_exibir_manual.h
│   │   ├── ⚡ gmui_25_exibir_ajuda.cpp
│   │   └── ⚡ gmui_25_exibir_ajuda.h
│   ├── 📁 GerenciamentoTradutorMetadadosQt
│   │   ├── ⚡ gtmqt_01_verificar_idioma_cache.cpp
│   │   ├── ⚡ gtmqt_01_verificar_idioma_cache.h
│   │   ├── ⚡ gtmqt_02_traduzir_tipo_operacao.cpp
│   │   ├── ⚡ gtmqt_02_traduzir_tipo_operacao.h
│   │   ├── ⚡ gtmqt_03_obter_chave_traducao_reversa.cpp
│   │   ├── ⚡ gtmqt_03_obter_chave_traducao_reversa.h
│   │   ├── ⚡ gtmqt_04_traduzir_metadados.cpp
│   │   ├── ⚡ gtmqt_04_traduzir_metadados.h
│   │   ├── ⚡ gtmqt_05_traduzir_tipo_arquivo.cpp
│   │   ├── ⚡ gtmqt_05_traduzir_tipo_arquivo.h
│   │   ├── ⚡ gtmqt_06_traduzir_atributos.cpp
│   │   ├── ⚡ gtmqt_06_traduzir_atributos.h
│   │   ├── ⚡ gtmqt_07_traduzir_autor.cpp
│   │   ├── ⚡ gtmqt_07_traduzir_autor.h
│   │   ├── ⚡ gtmqt_08_traduzir_protegido.cpp
│   │   ├── ⚡ gtmqt_08_traduzir_protegido.h
│   │   ├── ⚡ gtmqt_09_traduzir_dimensoes.cpp
│   │   └── ⚡ gtmqt_09_traduzir_dimensoes.h
│   ├── 📁 Manual
│   │   ├── ⚡ DOC_DE_DE.cpp
│   │   ├── ⚡ DOC_DE_DE.h
│   │   ├── ⚡ DOC_EN_US.cpp
│   │   ├── ⚡ DOC_EN_US.h
│   │   ├── ⚡ DOC_ES_ES.cpp
│   │   ├── ⚡ DOC_ES_ES.h
│   │   ├── ⚡ DOC_FR_FR.cpp
│   │   ├── ⚡ DOC_FR_FR.h
│   │   ├── ⚡ DOC_IT_IT.cpp
│   │   ├── ⚡ DOC_IT_IT.h
│   │   ├── ⚡ DOC_PT_BR.cpp
│   │   ├── ⚡ DOC_PT_BR.h
│   │   ├── ⚡ common_Manual.cpp
│   │   └── ⚡ common_Manual.h
│   ├── ⚡ ui_01_GerenciadorBotoes.cpp
│   ├── ⚡ ui_01_GerenciadorBotoes.h
│   ├── ⚡ ui_02_GerenciadorBotoesUI.cpp
│   ├── ⚡ ui_02_GerenciadorBotoesUI.h
│   ├── ⚡ ui_03_GerenciadorMenusUI.cpp
│   ├── ⚡ ui_03_GerenciadorMenusUI.h
│   ├── ⚡ ui_04_GerenciadorEventosUI.cpp
│   ├── ⚡ ui_04_GerenciadorEventosUI.h
│   ├── ⚡ ui_05_GerenciadorProgressoUI.cpp
│   ├── ⚡ ui_05_GerenciadorProgressoUI.h
│   ├── ⚡ ui_06_GerenciadorEstatisticasUI.cpp
│   ├── ⚡ ui_06_GerenciadorEstatisticasUI.h
│   ├── ⚡ ui_07_GerenciadorDados.cpp
│   ├── ⚡ ui_07_GerenciadorDados.h
│   ├── ⚡ ui_08_GerenciadorEventosArquivo.cpp
│   ├── ⚡ ui_08_GerenciadorEventosArquivo.h
│   ├── ⚡ ui_09_GerenciadorMonitoramento.cpp
│   ├── ⚡ ui_09_GerenciadorMonitoramento.h
│   ├── ⚡ ui_10_GerenciadorLimpeza.cpp
│   ├── ⚡ ui_10_GerenciadorLimpeza.h
│   ├── ⚡ ui_11_DialogoCores.cpp
│   ├── ⚡ ui_11_DialogoCores.h
│   ├── ⚡ ui_12_LocalizadorQt.cpp
│   ├── ⚡ ui_12_LocalizadorQt.h
│   ├── ⚡ ui_13_TradutorMetadadosQt.cpp
│   ├── ⚡ ui_13_TradutorMetadadosQt.h
│   ├── ⚡ ui_14_OpcoesSobre.cpp
│   ├── ⚡ ui_14_OpcoesSobre.h
│   ├── ⚡ ui_15_Manual.cpp
│   └── ⚡ ui_15_Manual.h
├── 📁 utils
│   ├── ⚡ ApplicationPathUtils.cpp
│   ├── ⚡ ApplicationPathUtils.h
│   ├── ⚡ CaminhoPersistenteUtils.cpp
│   ├── ⚡ CaminhoPersistenteUtils.h
│   ├── ⚡ IconUtils.cpp
│   ├── ⚡ IconUtils.h
│   ├── ⚡ LogManager.cpp
│   └── ⚡ LogManager.h
├── 📄 Linceu_Lighthouse_cpp.rc
├── ⚡ src_01_InicializadorMain.cpp
├── ⚡ src_01_InicializadorMain.h
├── ⚡ src_02_SplashAppStarter.cpp
└── ⚡ src_02_SplashAppStarter.h
```

**Root Path:** `c:\Users\ferna\DEV\CPP\Linceu_Lighthouse_cpp\repo`

```
├── 📁 LISTAGEM
│   ├── 📁 en_US
│   │   ├── 🖼️ Captura de tela 2026-04-26 235704.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002013.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002037.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002109.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002222.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002242.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002305.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002332.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 002353.png
│   │   ├── 🖼️ Captura de tela 2026-04-27 015939.png
│   │   ├── 🎬 linceu_lighthouse_1.mp4
│   │   ├── 🎬 linceu_lighthouse_2.mp4
│   │   └── 🎬 linceu_lighthouse_3.mp4
│   └── 📁 pt_BR
│       ├── 🖼️ Captura de tela 2026-04-27 015127.png
│       ├── 🖼️ Captura de tela 2026-04-27 015146.png
│       ├── 🖼️ Captura de tela 2026-04-27 015204.png
│       ├── 🖼️ Captura de tela 2026-04-27 015222.png
│       ├── 🖼️ Captura de tela 2026-04-27 015241.png
│       ├── 🖼️ Captura de tela 2026-04-27 015300.png
│       ├── 🖼️ Captura de tela 2026-04-27 015319.png
│       ├── 🖼️ Captura de tela 2026-04-27 015345.png
│       ├── 🖼️ Captura de tela 2026-04-27 015410.png
│       ├── 🖼️ Captura de tela 2026-04-27 015902.png
│       ├── 🎬 linceu_lighthouse_1.mp4
│       ├── 🎬 linceu_lighthouse_2.mp4
│       └── 🎬 linceu_lighthouse_3.mp4
├── 📄 Linceu_Lighthouse.iss
├── 📄 Linceu_Lighthouse_mingw.iss
├── 📄 Linceu_Lighthouse_msvc.iss
└── 📄 Para preservar o cmake --build sem rodar cmake --preset.txt
```

**Root Path:** `c:\Users\ferna\DEV\CPP\Linceu_Lighthouse_cpp\mocks`

```
├── 📄 CMakeLists.txt
├── ⚙️ CMakePresets.json
├── ⚡ compile_translations.cpp
├── ⚡ compile_translations.h
├── ⚡ find_VersionEditor.cpp
├── ⚡ find_VersionEditor.h
└── 📄 limpar_build_qt6_regeneravel.ps1
```

**Root Path:** `c:\Users\ferna\DEV\CPP\Linceu_Lighthouse_cpp\.vscode`

```
├── ⚙️ c_cpp_properties.json
├── ⚙️ cmake-kits.json
├── ⚙️ settings.json
└── ⚙️ tasks.json
```

**Root Path:** `c:\Users\ferna\DEV\CPP\Linceu_Lighthouse_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```
