<!-- Multilanguage OBSERVACAO.md for Linceu_Lighthouse -->

<p align="center">
  <b>Selecione o idioma / Select language / Seleccione el idioma / Sélectionnez la langue / Seleziona la lingua / Sprache wählen</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> |
  <a href="#enus">🇺🇸 English (US)</a> |
  <a href="#eses">🇪🇸 Español (ES)</a> |
  <a href="#frfr">🇫🇷 Français (FR)</a> |
  <a href="#itit">🇮🇹 Italiano (IT)</a> |
  <a href="#dede">🇩🇪 Deutsch (DE)</a> |
  <a href="#tree">🌳 Árvore de Arquivos / File Tree</a>
</p>

---

<h2 id="ptbr">🇧🇷 Português (BR)</h2>

<details>
<summary>Clique para expandir a Observação Técnica em português</summary>

# LINCEU LIGHTHOUSE - OBSERVAÇÃO TÉCNICA E POSICIONAMENTO ESTRATÉGICO

## Estado atual do projeto e Posicionamento

O repositório atual do **Linceu Lighthouse** consolida uma plataforma desktop **nativa em C++17 com Qt6** compilada via CMake. O produto transcende a categoria restrita de monitor de arquivos ou FIM corporativo tradicional, posicionando-se como uma **Plataforma Local de Inteligência, Histórico e Auditoria de Atividades de Arquivos** (*Local File Activity Intelligence, History & Audit Platform*).

Sua proposta de valor central responde diretamente à pergunta mais frequente de qualquer usuário diante do computador:
> **"Saiba exatamente o que aconteceu com seus arquivos — localmente, visualmente e com histórico pesquisável."**
> *(“Know exactly what happened to your files — locally, visually, deterministically.”)*

O produto atua como uma solução soberana **100% Local-First** (sem nuvem, sem telemetria, sem contas e sem IA externa), estruturada em 3 camadas integradas (Monitoramento Contínuo com detecção correta de Move/Rename, Histórico Pesquisável com 17 gráficos nativos e Segurança/Auditoria Local).

Versão consolidada observada nos assets e na documentação embarcada:
- `v2026.8.25.0`
Autor identificado no projeto:
- `Fernando Nillsson Cidade`

Licença do repositório:
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Atualização funcional observada na versão 2026.7.4.0

Além da base C++/Qt6 já consolidada, esta versão acrescenta e corrige pontos diretamente ligados à experiência de uso da tabela principal:

- abertura de arquivos reportados na tabela com suporte a diferentes tipos de arquivo, usando a associação do Windows e uma janela de escolha quando houver alternativas;
- coluna `MIME` persistida, exibida e traduzida dinamicamente, com preservação do identificador original para auditoria;
- preenchimento de `data_criacao`, `data_modificacao` e `data_acesso` para arquivos, pastas, subpastas e diretórios filhos durante a varredura;
- retradução de metadados da tabela a partir do valor bruto preservado, evitando que a mudança de idioma reaproveite texto já traduzido;
- atualização dos manuais embarcados para explicar essas funcionalidades em seis idiomas.

## Correção principal desta observação

O arquivo antigo descrevia uma arquitetura baseada em:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- cadeia de runtime dependente de bibliotecas Python

Essa descrição está defasada para esta árvore. A implementação atual:

- é compilada em `C++17`;
- usa `Qt6` como stack principal;
- utiliza `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` e `Qt Qml`;
- renderiza gráficos e diagramas com subsistema próprio em `source/data/Charts`, baseado em `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` e Qt Quick Scene Graph;
- usa APIs nativas do Windows para observação de diretório e metadados de executáveis;
- mantém, como herança histórica, a nomenclatura modular do projeto original.

Em outras palavras: a arquitetura funcional foi preservada, mas a plataforma de execução foi trocada.

## Resumo estrutural consolidado

Contagem validada diretamente na árvore `source/`:

| Camada | `.cpp` | `.h` | Observação |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Interface principal, bootstrap visual, mensagens e integração da GUI |
| `source/services` | 162 | 164 | Observação, banco, metadados, eventos e tabela |
| `source/ui` | 126 | 126 | Menus, botões, diálogos, manual, estatísticas e localização |
| `source/data` | 26 | 27 | Fachada de estatísticas + infraestrutura `Charts` + 17 geradores |
| `source/tools` | 23 | 24 | Filtros e administração de filtros |
| `source/utils` | 5 | 5 | Caminhos, logs, ícones, aplicativos associados e leitura de assets |
| `source/locale` | 0 | 0 | Arquivos de tradução `.ts` e `.qm` |
| `source/security` | 24 | 24 | Ferramentas de segurança |
| raiz de `source/` | 2 | 2 | Inicialização principal e splash |
| **Total em `source/`** | **376** | **380** | Base C++ atual |

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

## Build, toolchain e dependências reais

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

Observações:

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

- `AppChooserDialog.cpp/.h`
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
- seleção de aplicativo para abrir arquivos reportados na tabela;
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

Observação:

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
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Responsabilidades:

- localização de assets;
- caminhos persistentes;
- logs locais;
- ícones;
- descoberta de aplicativos associados no Windows.

### `source/locale`

Arquivos principais validados:

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsabilidades:

- armazenamento dos arquivos-fonte de tradução `.ts`;
- armazenamento dos arquivos compilados `.qm`;
- suporte à tradução da interface, metadados, MIME e textos do manual.

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

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

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

Pontos funcionais recentes:

- a tabela passa a carregar a coluna `mime` como metadado persistido e traduzível;
- as colunas `data_criacao`, `data_modificacao` e `data_acesso` são relevantes tanto para arquivos quanto para diretórios;
- ações de abertura de itens usam associação de aplicativos do Windows e uma janela de escolha quando necessário.

Conclusão importante:

- o projeto já tem base para streaming via model/view;
- vários fluxos continuam compatíveis com o caminho legado;
- a migração foi pensada para reduzir regressão funcional, não para quebrar a UI antiga de uma vez.

## Colunas, cores, tema e retradução

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

- leitura do tema do Windows via `QSettings` na chave `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize`;
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
- `QMimeDatabase` para identificação de MIME real e fallback por extensão;
- leitura de datas de criação, modificação e acesso por `QFileInfo`, inclusive para diretórios;
- `WinVerifyTrust`, `GetFileVersionInfoW` e `VerQueryValueW` para executáveis no Windows.

Observações de comportamento:

- o cache de metadados só deve ser usado como resultado completo quando contém as datas base esperadas;
- estatísticas de diretórios podem ser cacheadas separadamente, mas não devem impedir a leitura posterior das datas do próprio diretório;
- MIME e datas fazem parte do evento apresentado na tabela e também do conteúdo persistido/exportável quando as colunas estão disponíveis.

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

Observação importante:

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
- a coluna MIME possui chaves traduzíveis para famílias e subtipos comuns;
- há retradução de conteúdos já exibidos;
- menus, manual, ajuda e diálogo sobre reagem à mudança de idioma.

## Assets e conteúdos embarcados

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

- `%LOCALAPPDATA%\LinceuLighthouse`

Arquivos relevantes confirmados:

- `monitoramento.db`
- `language_config.json`
- `colunas_config.json`
- `colunas_coloridas.json`
- `cores_config.json`
- `trial_info.json`

Logs:

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`

Observação sobre trial:

- existe infraestrutura de trial em `source/utils/TrialManager.*`;
- ela usa `trial_info.json` e também a chave de registro `HKCU\SOFTWARE\LinceuLighthouse\FirstRunTimestamp`;
- o valor padrão observado é `7` dias;
- não foi encontrada chamada direta de `enforce_trial()` no bootstrap atual, então a infraestrutura existe, mas não aparece conectada ao ponto de entrada principal desta árvore.

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

2. O `README.md` anteriormente continha resquícios técnicos descrevendo a implementação antiga.
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

## Conclusão

O **Linceu Lighthouse** desta árvore é uma aplicação **C++17/Qt6**, desktop e nativa para Windows, com arquitetura modular extensa, forte separação por camadas e foco em:

- monitoramento de integridade de arquivos;
- persistência local em SQLite;
- extração nativa de metadados;
- visualização estatística por gráficos;
- internacionalização em seis idiomas;
- compatibilidade progressiva entre UI legada e model/view moderno.

A principal correção necessária neste arquivo era remover a leitura antiga de que o projeto atual ainda seria uma aplicação Python/PySide6. Essa não é mais a realidade técnica da base presente neste repositório.

</details>

---

<h2 id="enus">🇺🇸 English (US)</h2>

<details>
<summary>Click to expand the Technical Observation in English</summary>

# LINCEU LIGHTHOUSE - TECHNICAL OBSERVATION AND STRATEGIC POSITIONING

## Current Project State and Positioning

The current repository of **Linceu Lighthouse** consolidates a native desktop platform in **C++17 with Qt6** compiled via CMake. The product transcends the narrow category of simple file monitor or traditional corporate FIM, positioning itself as a **Local File Activity Intelligence, History & Audit Platform**.

Its core value proposition directly answers the most frequent question of any user in front of a computer:
> **"Know exactly what happened to your files — locally, visually, deterministically."**
> *(“Saiba exatamente o que aconteceu com seus arquivos — localmente, visualmente e com histórico pesquisável.”)*

The product operates as a sovereign **100% Local-First** solution (no cloud, no telemetry, no accounts, and no external AI), structured around 3 integrated layers (Continuous Monitoring with accurate Move/Rename detection, Searchable History with 17 native charts, and Local Security/Auditing).

Consolidated version observed in assets and embedded documentation:
- `v2026.8.25.0`
Identified author in the project:
- `Fernando Nillsson Cidade`

Repository license:
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Functional Update Observed in Version 2026.7.4.0

Beyond the established C++/Qt6 foundation, this version adds and refines aspects directly tied to the primary event table user experience:

- opening files reported in the table across different file formats, leveraging Windows shell file association and an application chooser dialog when alternatives exist;
- `MIME` column persisted, displayed, and dynamically translated, preserving the raw identifier for audit purposes;
- population of `data_criacao`, `data_modificacao`, and `data_acesso` for files, folders, subfolders, and child directories during directory scanning;
- retranslation of table metadata from preserved raw values, preventing language changes from reusing already translated text;
- updated embedded user manuals explaining these capabilities across all six languages.

## Main Correction in this Observation

The legacy document described an architecture based on:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- a runtime chain dependent on Python libraries

That description is obsolete for this codebase tree. The current implementation:

- is compiled in `C++17`;
- utilizes `Qt6` as the core stack;
- uses `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick`, and `Qt Qml`;
- renders charts and diagrams via a proprietary subsystem in `source/data/Charts`, based on `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem`, and Qt Quick Scene Graph;
- utilizes native Windows APIs for directory monitoring and executable metadata extraction;
- retains modular naming conventions as historical heritage from the original project design.

In other words: the functional architecture was preserved, but the underlying execution platform was completely modernized to native C++.

## Consolidated Structural Summary

File count validated directly within the `source/` tree:

| Layer | `.cpp` | `.h` | Notes |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Main interface, visual bootstrap, notifications, and GUI integration |
| `source/services` | 162 | 164 | Observation, database, metadata, events, and table logic |
| `source/ui` | 126 | 126 | Menus, buttons, dialogs, manual, statistics, and localization |
| `source/data` | 26 | 27 | Statistics facade + `Charts` infrastructure + 17 generators |
| `source/tools` | 23 | 24 | Filters and filter administrative controllers |
| `source/utils` | 5 | 5 | Paths, logs, icons, associated apps, and asset loaders |
| `source/locale` | 0 | 0 | Translation catalogs (`.ts` and `.qm`) |
| `source/security` | 24 | 24 | Security tools and forensics |
| `source/` root | 2 | 2 | Main initialization and splash screen |
| **Total in `source/`** | **376** | **380** | Current C++ codebase |

Primary directories under `source/`:

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Build, Toolchain, and Actual Dependencies

Data validated in root `CMakeLists.txt` and `mocks/CMakeLists.txt`:

- `cmake_minimum_required(VERSION 4.1)` in primary build
- `cmake_minimum_required(VERSION 3.24)` in auxiliary `mocks/` build
- Language standard: `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` in primary build

Qt components required by the primary build:

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

Additional native Windows libraries:

- `wintrust`
- `version`
- `crypt32`

In the auxiliary build of `mocks/find_VersionEditor`, required Qt components are:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Executables defined by CMake:

- `Linceu_Lighthouse_cpp` in root `CMakeLists.txt`
- `find_VersionEditor` in `mocks/CMakeLists.txt`

Observations:

- `CMakeLists.txt` automatically detects Qt6 installations under `C:/Qt/*/mingw_64/...` when running on Windows.
- The `find_VersionEditor` tool was isolated under `mocks/`, with its own `CMakePresets.json` and output directed to `mocks/output`.
- The current application does not depend on a Python runtime.
- The command-line utility `source/locale/compile_translations.cpp` compiles `.ts`/`.qm` files, but is not the main application entry point.

## Auxiliary Directories Outside `source/`

Beyond the main codebase under `source/`, the repository contains:

- `repo/`, with `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1`, and CMake deployment helpers;
- `mocks/`, with `find_VersionEditor.cpp/.h`, `CMakeLists.txt`, and `CMakePresets.json` for the version updater utility;
- `.vscode/`, with `c_cpp_properties.json`, `settings.json`, and `tasks.json` for local IDE setup;
- `.github/workflows/`, with `update-readme-version.yml` for documentation automation.

These directories do not belong to the application runtime tree, but support project build automation, maintenance, and developer workflow.

## Entry Point and Bootstrap

Confirmed execution flow in source:

1. `main.cpp`
   - Immediately delegates to `source::iniciar_aplicacao(argc, argv)`.

2. `source/src_01_InicializadorMain.cpp`
   - Adjusts the current working directory;
   - Sets Windows `AppUserModelID`;
   - Attempts startup via `SplashAppStarter`;
   - In fallback mode, instantiates `QApplication`, configures icons and translations, constructs `InterfaceMonitor`, and eagerly warms up `DatabaseWriter`.

3. `source/src_02_SplashAppStarter.cpp`
   - Instantiates `QApplication`;
   - Loads application icon;
   - Initializes translation subsystem;
   - Builds animated splash screen featuring:
     - Image display;
     - Progress bar;
     - Log status output area;
   - Instantiates `InterfaceMonitor`;
   - Initializes `DatabaseWriter`;
   - Closes splash screen and yields execution to the main window.

This bootstrap routine demonstrates intentional engineering for:

- Startup diagnostics;
- Reliable fallback mechanisms;
- Robust asset discovery;
- Smooth Windows binary distribution.

## Current Layered Architecture

### `source/gui`

Validated top-level headers/sources:

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Subdirectories:

- No active subdirectories under `source/gui` in this tree;
- Former directories `GerenciadorDesempenho/` and `GerenciadorEstruturaDiretoriosWidget/` are no longer present.

Responsibilities:

- Main application window;
- Widget layout composition;
- Visual state updates;
- Progress and notification management;
- GUI-layer internationalization;
- Table coordination and visual bootstrap.

### `source/services`

Validated top-level headers/sources:

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

Validated subdirectories:

- `GerenciamentoBaseEvento/` with `21` modules
- `GerenciamentoDiretorioScanner/` with `9` modules
- `GerenciamentoEventoModificado/` with `7` modules
- `GerenciamentoEventoMovido/` with `9` modules
- `GerenciamentoMetadados/` with `47` modules
- `GerenciamentoObservador/` with `13` modules
- `GerenciamentoTabela/` with `47` modules

Responsibilities:

- File system observation;
- Recursive baseline directory scanning;
- Event type dispatching;
- SQLite local persistence;
- Metadata extraction;
- Table updates;
- Highlighting colors, theming, and retranslation.

### `source/ui`

Validated top-level headers/sources:

- `AppChooserDialog.cpp/.h`
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

Validated subdirectories:

- `GerenciamentoBotoes/` with `6` modules
- `GerenciamentoDialogoCores/` with `8` modules
- `GerenciamentoEstatisticasUI/` with `31` modules
- `GerenciamentoEventosUI/` with `11` modules
- `GerenciamentoLocalizadorQt/` with `14` modules
- `GerenciamentoMenusUI/` with `24` modules
- `GerenciamentoTradutorMetadadosQt/` with `9` modules
- `Manual/` with `7` modules

Responsibilities:

- Global menus and toolbar actions;
- Multi-format data export;
- Dedicated statistics window;
- User manual and help dialogs;
- Color management dialog;
- Application chooser for opening reported files;
- Runtime language switching;
- About box, licenses, third-party notices, privacy policy, and release notes.

### `source/data`

Validated structure:

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` with `8` graphics infrastructure modules
- `GeradorEstatisticas/` with `18` modules (`1` common base + `17` specialized generators)

Confirmed chart generators:

- Pie chart
- Bar chart
- Timeline
- Treemap
- Histogram
- Pareto chart
- Line chart
- Boxplot
- Event radar
- Heatmap
- Scatter plot
- Sankey diagram
- Dimensional radar
- Dotplot
- Event vs. Path Sankey
- Type vs. Path Sankey
- Directory tree diagram

Observation:

- The `source/data` layer contains `27` modules in total: `1` facade + `8` modules in `Charts/` + `18` modules in `GeradorEstatisticas/`.

### `source/tools`

Validated top-level headers/sources:

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Subdirectories:

- `GerenciamentoAdministradorFiltros/` with `10` modules
- `GerenciamentoFiltros/` with `10` modules

Responsibilities:

- Advanced multi-criteria filters;
- Calendar selection integration;
- Main menu synchronization;
- Matching item counter;
- Checkbox state persistence.

### `source/utils`

Validated modules:

- `ApplicationPathUtils.cpp/.h`
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Responsibilities:

- Asset path discovery;
- Standard persistent paths;
- Centralized thread-safe logging;
- High-DPI icon loading;
- Windows shell associated application discovery.

### `source/locale`

Validated files:

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsibilities:

- Storage of translation XML sources (`.ts`);
- Storage of compiled binary catalogs (`.qm`);
- Support for dynamic UI, metadata, MIME types, and manual translations.

## Current Functional Architecture

### 1. Native File System Monitoring

`source/services/ob_01_Observador.*` forms the monitoring core.

Confirmed mechanisms:

- Monitored directory root;
- UI integration callbacks;
- Ring buffers and cryptographic hashes for deduplication;
- Pause and resume controls;
- Real-time event counters;
- `thread_scan` dedicated to initial scanning;
- `std::thread` for continuous native event polling;
- `std::thread` for batch buffer processing.

Native Windows API calls used in the observation layer:

- `CreateFileW`
- `ReadDirectoryChangesW`

Monitored notification flags:

- File name;
- Directory name;
- Attributes;
- Size;
- Last write time;
- Security descriptors;
- Creation time.

Supported operation categories:

- Added
- Deleted
- Modified
- Renamed
- Moved
- Scanned

### 2. Baseline Scanning vs. Real-Time Streaming Separation

The architectural design decouples:

- Initial recursive folder scanning;
- Continuous native API event capture;
- Multi-threaded buffer processing;
- Asynchronous database persistence;
- UI table streaming.

This separation prevents contention between:

- Baseline snapshot construction;
- Ingestion under heavy file activity bursts;
- UI thread rendering.

### 3. Local SQLite Persistence

Primary database:

- `monitoramento.db`

Standard location:

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

Validated SQLite configurations in `gbank_02_criar_banco_de_dados.cpp`:

- `QSQLITE` driver
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- Automatic index creation on audit and snapshot tables

Dedicated asynchronous database writer:

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

`DatabaseWriter` properties:

- Singleton design pattern;
- Thread-safe internal queue;
- Dedicated `std::thread` worker;
- Batch transactions;
- Timeout-based flush loops;
- Safe graceful shutdown.

### 4. Complex Event Correlation

The system provides specialized correlation for:

- File moves across monitored trees;
- Transient deletions;
- File renames;
- High-frequency burst modifications;
- Bulk file operations.

Key sub-layers:

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

This ensures that raw operating system events are correlated into semantically coherent operations rather than fragmented notifications.

## Table, Streaming, and Interface Compatibility

The application architecture features a hybrid model/view design:

- Full legacy support for `QTableWidget`;
- Progressive modern architecture using `QTableView + QAbstractTableModel`.

Confirmed native table model:

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Confirmed model capabilities:

- `rowCount`
- `columnCount`
- `headerData`
- `data`
- `prepend_event`
- `append_events`
- `replace_events`
- Internal brush and label caches;
- Color cache invalidation.

Recent functional capabilities:

- `MIME` column loaded and persisted as dynamic translatable metadata;
- `data_criacao`, `data_modificacao`, and `data_acesso` populated for both files and directories;
- File opening actions integrating native Windows associations with a chooser fallback dialog.

Key takeaway:

- The project provides high-throughput model/view streaming;
- Existing workflows retain full backward compatibility;
- The refactoring prioritized non-breaking stability.

## Columns, Colors, Theming, and Retranslation

`GerenciamentoTabela/` implements dedicated modules for:

- Table layout configuration;
- Header width and height adjustments;
- Multi-line word wrapping on headers;
- Live data updates;
- Latest row highlighting;
- Column visibility toggling;
- Per-operation and per-column color highlights;
- Persistent color storage;
- Asynchronous background color styling;
- Windows light/dark theme detection and monitoring;
- Live retranslation of loaded event records;
- Header retranslation on language switch.

Key verified highlights:

- Windows theme detection via `QSettings` targeting `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize`;
- Dedicated header update bridge `gtab_43_AtualizarCabecalhosBridge`;
- Worker threads offloading heavy formatting tasks from the GUI.

## Metadata Extraction

`GerenciamentoMetadados/` comprises `47` specialized modules covering:

- Source code;
- Images;
- Audio;
- Video;
- Documents;
- Spreadsheets;
- Presentations;
- Databases;
- Executable binaries;
- Temporary files;
- Compressed archives;
- Backups;
- Logs;
- Configuration files;
- Compound OLE;
- Structured data formats.

Verified technical implementations:

- `QImageReader` for image dimensions and headers;
- ZIP/OpenXML parsing for documents, spreadsheets, and presentations;
- `QSqlDatabase` for SQLite schema and table inspection;
- Magic byte detection for SQLite databases;
- `QMimeDatabase` for content-based MIME categorization with extension fallback;
- `QFileInfo` extracting creation, modification, and access timestamps for files and folders;
- `WinVerifyTrust`, `GetFileVersionInfoW`, and `VerQueryValueW` for Windows PE binaries.

Behavioral notes:

- Metadata cache is validated only when expected core timestamps are present;
- Directory stats are cached independently without blocking folder timestamp updates;
- MIME categories and timestamps form part of both the visual table and exported report formats.

## Statistics and Graphical Visualizations

Statistics subsystems are distributed across:

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Confirmed features:

- Multi-chart generation in a dedicated window;
- Interactive chart selection panel;
- Dynamic updates without closing the dialog;
- Live retranslation on language switch;
- High-resolution chart image export;
- Background generation worker;
- Adaptation of legacy data payloads via `LegacyVariantChartAdapter`;
- Geometric generation via `ChartLayoutEngine` and `LgplChartModel`;
- High-quality `RasterChartView` canvas with `QPainter` antialiasing in the Statistics dialog;
- Alternative backends via `WidgetChartView` (`QOpenGLWidget`) and `QuickChartItem` (Qt Quick Scene Graph).

Consolidated total:

- `17` distinct chart visualizations

## Data Export

The data export pipeline is orchestrated by:

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Supported export formats:

- `.xlsx`
- `.csv`
- `.txt`
- `.json`
- `.xml`
- `.db`

Configurable export options:

- Export active columns only;
- Export filtered rows only;
- Export selected rows only.

Important note:

- Native `.xlsx` export is constructed using pure OpenXML schemas and `QXmlStreamWriter`, completely independent of Python libraries.

## Filters and Tools Subsystem

The `Filtros` subsystem provides:

- Operation type filtering;
- Combined multi-criteria expressions;
- Live match counter;
- Main menu synchronization;
- Runtime language responsiveness;
- Session-persistent checkbox states.

Divided into:

- Main filter widget;
- Calendar manager;
- Filter administrator;
- Specialized action modules.

## Architecture Evolution: Removed and Added Structures

Based on the current tree, several components previously documented in legacy notes are no longer standalone layers:

- `source/gui/GerenciadorEstruturaDiretoriosWidget/` is no longer present;
- `source/gui/GerenciadorDesempenho/` is no longer present;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*` was removed.

Conversely, significant new structures have been added:

- `source/data/Charts/`, containing the project's native C++/Qt rendering infrastructure;
- `mocks/`, containing an isolated CMake project for `find_VersionEditor`;
- `repo/`, `.vscode/`, and `.github/workflows/` for packaging, IDE configurations, and CI automation.

## Internationalization (i18n)

Supported and validated languages:

- `pt_BR`
- `en_US`
- `es_ES`
- `fr_FR`
- `it_IT`
- `de_DE`

Implementation highlights:

- `LocalizadorQt` manages current language state, translation catalogs, and persistence;
- Language preference is stored in `language_config.json`;
- Full UI string localization;
- Operation category translation;
- Metadata value translation;
- MIME column translates broad categories while preserving raw identifiers;
- Dynamic live retranslation of currently displayed table rows;
- Menu bars, manuals, help dialogs, and about boxes react immediately to language switches.

## Embedded Assets and Documentation

The project bundles multilingual assets under `source/assets/`, including:

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

The About dialog (`ui_14_OpcoesSobre.cpp`) loads:

- Product overview;
- Licenses;
- Notices;
- Privacy policy;
- Application history;
- Release notes.

The integrated user manual is built from:

- `source/ui/Manual/common_Manual.*`
- `source/ui/Manual/DOC_PT_BR.*`
- `source/ui/Manual/DOC_EN_US.*`
- `source/ui/Manual/DOC_ES_ES.*`
- `source/ui/Manual/DOC_FR_FR.*`
- `source/ui/Manual/DOC_IT_IT.*`
- `source/ui/Manual/DOC_DE_DE.*`

## Local Persistence and Key Configuration Files

Persistent storage directory:

- `%LOCALAPPDATA%\LinceuLighthouse`

Key persistent files:

- `monitoramento.db`
- `language_config.json`
- `colunas_config.json`
- `colunas_coloridas.json`
- `cores_config.json`
- `trial_info.json`

Log path:

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`

Trial notes:

- Trial infrastructure exists under `source/utils/TrialManager.*`;
- Utilizes `trial_info.json` and registry key `HKCU\SOFTWARE\LinceuLighthouse\FirstRunTimestamp`;
- Default trial period is `7` days.

## Main Observed Execution Flow

Consolidated functional flow:

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. Creation of `InterfaceMonitor`
5. Initialization of localization, icons, and UI components
6. Directory selection and/or context restoration
7. Creation of `Observador`
8. Initial scan in `QThread`
9. Continuous monitoring in `std::thread`
10. Event reconciliation and logging
11. SQLite persistence
12. Table update
13. Filters, export, and statistics over the local database

## Important Technical Points for Maintenance

1. Modular naming preserves Python origin.
   This explains prefixes like `ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_`, etc.

2. `README.md` previously contained technical remnants of the legacy implementation.
   This observation was updated based on the actual code of the current project, not on legacy README text.

3. The current application is strongly Windows-oriented.
   This is evident in:
   - `ReadDirectoryChangesW`;
   - `CreateFileW`;
   - `WinVerifyTrust`;
   - `GetFileVersionInfoW`;
   - Usage of `QSettings` for Windows theme detection;
   - Libraries `wintrust`, `version`, and `crypt32`.

4. Migration to model/view is ongoing.
   New flows leverage `QTableView`/`EventTableModel`, but many paths retain compatibility with `QTableWidget`.

5. The project relies on local persistence as the architectural core.
   The SQLite database is not merely for export: it is an active component of product operation.

6. The auxiliary utility `find_VersionEditor`
   was isolated under `mocks/`, with its own `CMakeLists.txt` and `CMakePresets.json`, and serves to update versions and dates across multilingual assets and documentation.

## Conclusion

The **Linceu Lighthouse** in this tree is a native **C++17/Qt6** Windows desktop application, with an extensive modular architecture, strong layer separation, and focused on:

- File activity and integrity monitoring;
- Local persistence in SQLite;
- Native metadata extraction;
- Visual statistical chart generation;
- Internationalization across six languages;
- Progressive compatibility between legacy UI and modern model/view.

The primary correction needed in this file was eliminating the outdated notion that the project remains a Python/PySide6 application. That is no longer the technical reality of the codebase in this repository.

</details>

---

<h2 id="eses">🇪🇸 Español (ES)</h2>

<details>
<summary>Haga clic para expandir la Observación Técnica en español</summary>

# LINCEU LIGHTHOUSE - OBSERVACIÓN TÉCNICA Y POSICIONAMIENTO ESTRATÉGICO

## Estado actual del proyecto y Posicionamiento

El repositorio actual de **Linceu Lighthouse** consolida una plataforma de escritorio **nativa en C++17 con Qt6** compilada mediante CMake. El producto trasciende la categoría restringida de un simple monitor de archivos o FIM corporativo tradicional, posicionándose como una **Plataforma Local de Inteligencia, Historial y Auditoría de Actividades de Archivos** (*Local File Activity Intelligence, History & Audit Platform*).

Su propuesta de valor central responde directamente a la pregunta más frecuente de cualquier usuario frente a su ordenador:
> **"Sepa exactamente qué ocurrió con sus archivos — localmente, visualmente y con historial consultable."**
> *(“Know exactly what happened to your files — locally, visually, deterministically.”)*

El producto actúa como una solución soberana **100% Local-First** (sin nube, sin telemetría, sin cuentas y sin IA externa), estructurada en 3 capas integradas (Monitorización Continua con detección correcta de Move/Rename, Historial Consultable con 17 gráficos nativos y Seguridad/Auditoría Local).

Versión consolidada observada en los recursos y en la documentación integrada:
- `v2026.8.25.0`
Autor identificado en el proyecto:
- `Fernando Nillsson Cidade`

Licencia del repositorio:
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Actualización funcional observada en la versión 2026.7.4.0

Además de la base consolidada en C++/Qt6, esta versión añade y corrige aspectos directamente vinculados con la experiencia de uso de la tabla principal:

- apertura de archivos reportados en la tabla con soporte para múltiples formatos, utilizando las asociaciones de Windows y un diálogo de selección cuando existan alternativas;
- columna `MIME` guardada, mostrada y traducida dinámicamente, preservando el identificador original para fines de auditoría;
- asignación de `data_criacao`, `data_modificacao` y `data_acesso` para archivos, carpetas, subcarpetas y directorios secundarios durante el escaneo;
- retraducción de metadatos de la tabla a partir del valor original preservado, evitando que el cambio de idioma reutilice texto previamente traducido;
- actualización de los manuales integrados para documentar estas funciones en seis idiomas.

## Corrección principal de esta observación

El documento antiguo describía una arquitectura basada en:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- entorno de ejecución dependiente de bibliotecas Python

Dicha descripción está completamente desfasada para este árbol de código. La implementación actual:

- está compilada en `C++17`;
- utiliza `Qt6` como entorno de desarrollo principal;
- hace uso de `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` y `Qt Qml`;
- renderiza gráficos y diagramas mediante un subsistema propio en `source/data/Charts`, basado en `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` y Qt Quick Scene Graph;
- emplea APIs nativas de Windows para la observación de directorios y la extracción de metadatos de ejecutables;
- conserva, como herencia histórica, la nomenclatura modular del diseño original.

En otras palabras: se preservó la arquitectura funcional, pero la plataforma de ejecución se modernizó por completo a C++ nativo.

## Resumen estructural consolidado

Conteo de archivos validado directamente en el árbol `source/`:

| Capa | `.cpp` | `.h` | Observaciones |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Interfaz principal, arranque visual, notificaciones e integración de UI |
| `source/services` | 162 | 164 | Observación, base de datos, metadados, eventos y tabla |
| `source/ui` | 126 | 126 | Menús, botones, cuadros de diálogo, manual, estadísticas y localización |
| `source/data` | 26 | 27 | Fachada de estadísticas + infraestructura `Charts` + 17 generadores |
| `source/tools` | 23 | 24 | Filtros y administración de filtros |
| `source/utils` | 5 | 5 | Rutas, logs, iconos, aplicaciones asociadas y lectura de recursos |
| `source/locale` | 0 | 0 | Archivos de traducción `.ts` y `.qm` |
| `source/security` | 24 | 24 | Herramientas de seguridad forense |
| raíz de `source/` | 2 | 2 | Inicialización principal y pantalla de inicio |
| **Total en `source/`** | **376** | **380** | Base de código C++ actual |

Directorios principales bajo `source/`:

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Compilación, toolchain y dependencias reales

Datos validados en `CMakeLists.txt` y `mocks/CMakeLists.txt`:

- `cmake_minimum_required(VERSION 4.1)` en la compilación principal
- `cmake_minimum_required(VERSION 3.24)` en la compilación auxiliar de `mocks/`
- Estándar de lenguaje: `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` en la compilación principal

Componentes de Qt requeridos por la compilación principal:

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

Bibliotecas nativas adicionales de Windows:

- `wintrust`
- `version`
- `crypt32`

En la compilación auxiliar de `mocks/find_VersionEditor`, los componentes de Qt requeridos son:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Ejecutables definidos por CMake:

- `Linceu_Lighthouse_cpp` en el `CMakeLists.txt` raíz
- `find_VersionEditor` en `mocks/CMakeLists.txt`

Observaciones:

- `CMakeLists.txt` localiza automáticamente instalaciones de Qt6 en `C:/Qt/*/mingw_64/...` al ejecutarse en Windows.
- La utilidad `find_VersionEditor` se aisló en `mocks/`, con su propio `CMakePresets.json` y salida dirigida a `mocks/output`.
- El proyecto actual no depende del entorno de ejecución de Python.
- La herramienta `source/locale/compile_translations.cpp` existe en el repositorio como utilidad de línea de comandos para `.ts`/`.qm`, pero no es el punto de entrada principal.

## Directorios auxiliares fuera de `source/`

Además del árbol principal en `source/`, el repositorio contiene:

- `repo/`, con `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1` y archivos de soporte para CMake;
- `mocks/`, con `find_VersionEditor.cpp/.h`, `CMakeLists.txt` y `CMakePresets.json` para la herramienta de actualización de versiones;
- `.vscode/`, con `c_cpp_properties.json`, `settings.json` y `tasks.json` para el entorno de desarrollo local;
- `.github/workflows/`, con `update-readme-version.yml` para la automatización documental.

Estos directorios no forman parte del runtime de la aplicación, sino que brindan soporte al mantenimiento, compilación y empaquetado del proyecto.

## Punto de entrada y arranque

Flujo confirmado en el código fuente:

1. `main.cpp`
   - Delega inmediatamente en `source::iniciar_aplicacao(argc, argv)`.

2. `source/src_01_InicializadorMain.cpp`
   - Establece el directorio de trabajo;
   - Configura el `AppUserModelID` de Windows;
   - Intenta el inicio mediante `SplashAppStarter`;
   - En modo de respaldo, inicializa `QApplication`, configura iconos y traducciones, instancia `InterfaceMonitor` e inicializa `DatabaseWriter` de forma inmediata.

3. `source/src_02_SplashAppStarter.cpp`
   - Crea `QApplication`;
   - Carga el icono principal;
   - Inicializa el sistema de localización;
   - Construye la pantalla de inicio con:
     - Imagen;
     - Barra de progreso;
     - Área de registro de eventos;
   - Instancia `InterfaceMonitor`;
   - Inicia `DatabaseWriter`;
   - Cierra la pantalla de bienvenida y transfiere el control a la ventana principal.

Este diseño de arranque demuestra una preparación orientada a:

- Diagnóstico de inicio;
- Mecanismos de recuperación seguros;
- Carga fiable de recursos;
- Distribución binaria en Windows.

## Estructura actual por capas

### `source/gui`

Archivos principales validados:

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Subdirectorios:

- No existen subdirectorios activos bajo `source/gui` en este árbol;
- Los antiguos directorios `GerenciadorDesempenho/` y `GerenciadorEstruturaDiretoriosWidget/` ya no forman parte de la estructura.

Responsabilidades:

- Ventana principal;
- Composición de la interfaz gráfica;
- Actualización visual;
- Gestión de progreso y notificaciones;
- Internacionalización de la interfaz;
- Coordinación de la tabla de eventos y arranque visual.

### `source/services`

Archivos principales validados:

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

Subdirectorios validados:

- `GerenciamentoBaseEvento/` con `21` módulos
- `GerenciamentoDiretorioScanner/` con `9` módulos
- `GerenciamentoEventoModificado/` con `7` módulos
- `GerenciamentoEventoMovido/` con `9` módulos
- `GerenciamentoMetadados/` con `47` módulos
- `GerenciamentoObservador/` con `13` módulos
- `GerenciamentoTabela/` con `47` módulos

Responsabilidades:

- Monitorización del sistema de archivos;
- Escaneo inicial recursivo;
- Enrutamiento por categoría de evento;
- Persistencia en SQLite local;
- Extracción de metadados;
- Actualización de la tabla;
- Resaltado por colores, temas y retraducción.

### `source/ui`

Archivos principales validados:

- `AppChooserDialog.cpp/.h`
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

Subdirectorios validados:

- `GerenciamentoBotoes/` con `6` módulos
- `GerenciamentoDialogoCores/` con `8` módulos
- `GerenciamentoEstatisticasUI/` con `31` módulos
- `GerenciamentoEventosUI/` con `11` módulos
- `GerenciamentoLocalizadorQt/` con `14` módulos
- `GerenciamentoMenusUI/` con `24` módulos
- `GerenciamentoTradutorMetadadosQt/` con `9` módulos
- `Manual/` con `7` módulos

Responsabilidades:

- Menús principales y botones de acción;
- Exportación multiformato;
- Ventana de estadísticas;
- Manual y ayuda integrados;
- Diálogo de configuración de colores;
- Selector de aplicaciones para abrir archivos reportados;
- Cambio dinámico de idioma;
- Textos de Acerca de, licencias, avisos, política de privacidad y notas de versión.

### `source/data`

Estructura validada:

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` con `8` módulos de infraestructura gráfica
- `GeradorEstatisticas/` con `18` módulos (`1` base común + `17` generadores especializados)

Generadores de gráficos confirmados:

- Gráfico circular (pie)
- Gráfico de barras
- Línea temporal (timeline)
- Treemap
- Histograma
- Diagrama de Pareto
- Gráfico de líneas
- Boxplot
- Radar de eventos
- Mapa de calor (heatmap)
- Gráfico de dispersión (scatter)
- Diagrama de Sankey
- Radar multidimensional
- Dotplot
- Sankey de evento y ruta
- Sankey de tipo y ruta
- Árbol de directorios

Observación:

- La capa `source/data` contiene `27` módulos en total: `1` fachada + `8` módulos en `Charts/` + `18` módulos en `GeradorEstatisticas/`.

### `source/tools`

Archivos principales validados:

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Subdirectorios:

- `GerenciamentoAdministradorFiltros/` con `10` módulos
- `GerenciamentoFiltros/` con `10` módulos

Responsabilidades:

- Filtros avanzados;
- Integración con calendario;
- Sincronización con el menú principal;
- Conteo de elementos coincidentes;
- Persistencia del estado de las casillas de verificación.

### `source/utils`

Módulos validados:

- `ApplicationPathUtils.cpp/.h`
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Responsabilidades:

- Detección de rutas de recursos;
- Rutas persistentes estándar;
- Registro de logs seguro para hilos;
- Carga de iconos High-DPI;
- Detección de aplicaciones asociadas en Windows.

### `source/locale`

Archivos validados:

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsabilidades:

- Almacenamiento de fuentes de traducción XML (`.ts`);
- Almacenamiento de catálogos binarios compilados (`.qm`);
- Localización dinámica de interfaz, metadados, tipos MIME y manuales.

## Arquitectura funcional actual

### 1. Monitorización nativa del sistema de archivos

El módulo `source/services/ob_01_Observador.*` constituye el núcleo del subsistema de observación.

Mecanismos confirmados:

- Directorio raíz observado;
- Devolución de llamadas (callbacks) hacia la UI;
- Búferes circulares y hashes para deduplicación;
- Controles de pausa y reanudación;
- Contadores de eventos en tiempo real;
- `thread_scan` dedicado al escaneo inicial;
- `std::thread` para la lectura continua de eventos del sistema operativo;
- `std::thread` para el procesamiento en lotes del búfer.

Llamadas nativas de Windows en la capa de observación:

- `CreateFileW`
- `ReadDirectoryChangesW`

Indicadores de notificación monitorizados:

- Nombre de archivo;
- Nombre de directorio;
- Atributos;
- Tamaño;
- Fecha de última modificación;
- Descriptores de seguridad;
- Fecha de creación.

Operaciones reconocidas:

- Añadido
- Eliminado
- Modificado
- Renombrado
- Movido
- Escaneado

### 2. Separación entre línea base y flujo en tiempo real

El diseño desacopla con claridad:

- Escaneo recursivo inicial de directorios;
- Captura continua mediante API nativa;
- Procesamiento en búfer multihilo;
- Persistencia asíncrona en base de datos;
- Visualización en la tabla de la interfaz.

Esta separación elimina interferencias entre:

- Construcción de la instantánea (snapshot) inicial;
- Recepción de ráfagas intensas de eventos;
- Renderizado de la interfaz gráfica.

### 3. Persistencia local con SQLite

Base de datos principal:

- `monitoramento.db`

Ubicación estándar:

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

Configuraciones validadas de SQLite en `gbank_02_criar_banco_de_dados.cpp`:

- Controlador `QSQLITE`
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- Creación automática de índices en tablas de auditoría e instantáneas

Escritor asíncrono especializado:

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

Características de `DatabaseWriter`:

- Patrón Singleton;
- Cola interna segura para subprocesos;
- Hilo `std::thread` dedicado;
- Escrituras en transacciones por lote;
- Bucles de vaciado por tiempo límite (timeout);
- Cierre ordenado y seguro.

### 4. Reconciliación de eventos complejos

El sistema incorpora lógica especializada para:

- Movimiento de archivos entre carpetas;
- Eliminaciones transitorias;
- Renombrado de elementos;
- Modificaciones a alta frecuencia;
- Operaciones masivas por ráfagas.

Subcapas relevantes:

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

Esto garantiza que las notificaciones del sistema operativo se transformen en operaciones semánticamente comprensibles.

## Tabla, streaming y compatibilidad de interfaz

La aplicación cuenta con un diseño de interfaz híbrido:

- Compatibilidad con `QTableWidget`;
- Migración hacia `QTableView + QAbstractTableModel`.

Modelo nativo confirmado:

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Capacidades del modelo:

- `rowCount`
- `columnCount`
- `headerData`
- `data`
- `prepend_event`
- `append_events`
- `replace_events`
- Cachés internas de texto y colores;
- Invalidación de caché de colores.

Funcionalidades recientes:

- Columna `MIME` persistida y traducida dinámicamente;
- Fechas de creación, modificación y acceso rellenadas tanto para archivos como para directorios;
- Apertura de archivos integrando asociaciones de Windows y ventana de selección.

Conclusión principal:

- El proyecto cuenta con una sólida base para streaming vía model/view;
- Los flujos existentes mantienen total compatibilidad;
- La modernización preservó la estabilidad operativa.

## Columnas, colores, temas y retraducción

`GerenciamentoTabela/` ofrece módulos dedicados para:

- Configuración de la estructura de la tabla;
- Ajuste de anchos y alturas de cabeceras;
- Ajuste de texto multilínea en encabezados;
- Actualización de registros en vivo;
- Resaltado de la fila más reciente;
- Control de visibilidad de columnas;
- Esquemas de color por operación y columna;
- Almacenamiento persistente de configuraciones de color;
- Aplicación asíncrona de estilos de color en segundo plano;
- Detección y monitorización del tema claro/oscuro de Windows;
- Retraducción en tiempo real de registros existentes;
- Actualización de encabezados tras el cambio de idioma.

Aspectos destacados:

- Lectura del tema de Windows mediante `QSettings` en `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize`;
- Puente de sincronización de encabezados `gtab_43_AtualizarCabecalhosBridge`;
- Hilos de trabajo que descargan a la interfaz de tareas de formato pesado.

## Extracción de metadados

`GerenciamentoMetadados/` incluye `47` módulos especializados que cubren:

- Código fuente;
- Imágenes;
- Audio;
- Vídeo;
- Documentos;
- Hojas de cálculo;
- Presentaciones;
- Bases de datos;
- Binarios ejecutables;
- Archivos temporales;
- Archivos comprimidos;
- Copias de seguridad;
- Registros (logs);
- Archivos de configuración;
- Almacenamiento compuesto OLE;
- Formatos estructurados.

Implementaciones técnicas verificadas:

- `QImageReader` para dimensiones e información de imágenes;
- Inspección de ZIP/OpenXML para documentos y hojas de cálculo;
- `QSqlDatabase` para análisis de esquemas y tablas SQLite;
- Detección de firmas binarias para bases de datos SQLite;
- `QMimeDatabase` para tipificación MIME por contenido con respaldo por extensión;
- `QFileInfo` para marcas temporales de archivos y directorios;
- `WinVerifyTrust`, `GetFileVersionInfoW` y `VerQueryValueW` para binarios PE de Windows.

## Estadísticas y visualizaciones gráficas

El subsistema de estadísticas se reparte entre:

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Características principales:

- Generación de múltiples diagramas en ventana independiente;
- Panel interactivo de selección de gráficos;
- Actualización dinámica sin cerrar la vista;
- Retraducción inmediata al cambiar de idioma;
- Exportación de imágenes en alta resolución;
- Hilo worker para cálculo y renderizado;
- Adaptación de estructuras antiguas con `LegacyVariantChartAdapter`;
- Cálculo geométrico con `ChartLayoutEngine` y `LgplChartModel`;
- Lienzo `RasterChartView` con `QPainter` antialiasing en el diálogo de estadísticas;
- Backends alternativos mediante `WidgetChartView` (`QOpenGLWidget`) y `QuickChartItem` (Qt Quick Scene Graph).

Total consolidado:

- `17` tipos de gráficos visuales

## Exportación de datos

El flujo de exportación es gestionado por:

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Formatos admitidos:

- `.xlsx`
- `.csv`
- `.txt`
- `.json`
- `.xml`
- `.db`

Opciones de filtrado para exportación:

- Únicamente columnas activas;
- Únicamente registros filtrados;
- Únicamente filas seleccionadas.

Nota técnica relevante:

- La exportación a `.xlsx` se genera de forma nativa mediante OpenXML y `QXmlStreamWriter`, sin depender de librerías externas de Python.

## Filtros y herramientas auxiliares

El componente `Filtros` proporciona:

- Filtrado por tipo de operación;
- Expresiones combinadas multicriterio;
- Contador de coincidencias en vivo;
- Sincronización con el menú principal;
- Compatibilidad con el cambio de idioma;
- Persistencia del estado de selección entre sesiones.

## Evolución arquitectónica: componentes eliminados e incorporados

En comparación con notas técnicas previas, varios elementos ya no existen como capas independientes:

- `source/gui/GerenciadorEstruturaDiretoriosWidget/` ha sido retirado;
- `source/gui/GerenciadorDesempenho/` ha sido retirado;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*` fue removido.

Por otro lado, se añadieron componentes estratégicos:

- `source/data/Charts/`, que centraliza el motor de renderizado gráfico C++/Qt del proyecto;
- `mocks/`, con proyecto CMake aislado para `find_VersionEditor`;
- `repo/`, `.vscode/` y `.github/workflows/` para empaquetado, configuración de IDE y CI.

## Internacionalización (i18n)

Idiomas soportados y validados:

- `pt_BR`
- `en_US`
- `es_ES`
- `fr_FR`
- `it_IT`
- `de_DE`

Detalles de implementación:

- `LocalizadorQt` administra el catálogo de traducción, el idioma activo y su persistencia;
- La preferencia de idioma se guarda en `language_config.json`;
- Localización integral de la interfaz de usuario;
- Traducción de categorías de operación y metadatos;
- La columna MIME traduce familias comunes preservando el identificador original;
- Retraducción dinámica de las filas mostradas en la tabla;
- Menús, manuales, diálogos de ayuda y Acerca de responden de inmediato al cambio de idioma.

## Recursos integrados y documentación

El proyecto incluye recursos multilíngues bajo `source/assets/`, entre los que destacan:

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

El diálogo Acerca de (`ui_14_OpcoesSobre.cpp`) muestra:

- Información general del producto;
- Licencias y avisos legales;
- Política de privacidad;
- Historial de la aplicación;
- Notas de la versión.

El manual de usuario integrado se compila a partir de:

- `source/ui/Manual/common_Manual.*`
- `source/ui/Manual/DOC_PT_BR.*`
- `source/ui/Manual/DOC_EN_US.*`
- `source/ui/Manual/DOC_ES_ES.*`
- `source/ui/Manual/DOC_FR_FR.*`
- `source/ui/Manual/DOC_IT_IT.*`
- `source/ui/Manual/DOC_DE_DE.*`

## Persistencia local y archivos clave

Directorio de almacenamiento persistente:

- `%LOCALAPPDATA%\LinceuLighthouse`

Archivos persistentes principales:

- `monitoramento.db`
- `language_config.json`
- `colunas_config.json`
- `colunas_coloridas.json`
- `cores_config.json`
- `trial_info.json`

Ruta de logs:

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`

## Flujo principal de ejecución observado

Flujo funcional consolidado:

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. Creación de `InterfaceMonitor`
5. Inicialización de localización, iconos y componentes de interfaz
6. Selección de directorio o restauración del contexto
7. Creación de `Observador`
8. Escaneo inicial en `QThread`
9. Monitorización continua en `std::thread`
10. Reconciliación y registro de eventos
11. Persistencia en SQLite
12. Actualización de la tabla
13. Filtros, exportación y estadísticas sobre la base de datos local

## Puntos técnicos importantes para mantenimiento

1. La nomenclatura modular preserva su origen en Python (`ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_`, etc.).
2. El archivo `README.md` contenía fragmentos de la implementación antigua; esta observación describe el código real del proyecto actual.
3. La aplicación está fuertemente orientada a Windows (`ReadDirectoryChangesW`, `CreateFileW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `wintrust`, `version`, `crypt32`).
4. La migración a model/view continúa de forma progresiva (`QTableView`/`EventTableModel` conviviendo con `QTableWidget`).
5. La persistencia local en SQLite es el núcleo operativo de la plataforma.
6. La utilidad `find_VersionEditor` se mantiene aislada bajo `mocks/` para la sincronización de versiones y fechas en la documentación.

## Conclusión

El **Linceu Lighthouse** de este árbol es una aplicación de escritorio nativa para Windows en **C++17/Qt6**, con una extensa arquitectura modular, clara separación por capas y enfoque en:

- Monitorización e integridad del sistema de archivos;
- Persistencia local en SQLite;
- Extracción nativa de metadados;
- Generación estadística visual mediante gráficos nativos;
- Internacionalización completa en seis idiomas;
- Compatibilidad progresiva hacia el patrón model/view moderno.

</details>

---

<h2 id="frfr">🇫🇷 Français (FR)</h2>

<details>
<summary>Cliquez pour développer l'Observation Technique en français</summary>

# LINCEU LIGHTHOUSE - OBSERVATION TECHNIQUE ET POSITIONNEMENT STRATÉGIQUE

## État actuel du projet et Positionnement

Le référentiel actuel de **Linceu Lighthouse** consolide une plateforme de bureau **native en C++17 avec Qt6** compilée via CMake. Le produit dépasse la catégorie restreinte d'un simple moniteur de fichiers ou d'un FIM d'entreprise traditionnel, se positionnant comme une **Plateforme Locale d'Intelligence, d'Historique et d'Audit d'Activités de Fichiers** (*Local File Activity Intelligence, History & Audit Platform*).

Sa proposition de valeur fondamentale répond directement à la question la plus fréquente de tout utilisateur devant un ordinateur :
> **« Sachez exactement ce qui est arrivé à vos fichiers — localement, visuellement et avec un historique consultable. »**
> *(“Know exactly what happened to your files — locally, visually, deterministically.”)*

Le produit fonctionne comme une solution souveraine **100% Local-First** (sans cloud, sans télémétrie, sans comptes et sans IA externe), structurée en 3 couches intégrées (Surveillance Continue avec détection précise des opérations Move/Rename, Historique Consultable avec 17 graphiques natifs et Sécurité/Audit Local).

Version consolidée observée dans les ressources et la documentation intégrée :
- `v2026.8.25.0`
Auteur identifié dans le projet :
- `Fernando Nillsson Cidade`

Licence du référentiel :
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Mise à jour fonctionnelle observée dans la version 2026.7.4.0

Au-delà du socle C++/Qt6 consolidé, cette version apporte des améliorations directement liées à l'expérience utilisateur du tableau d'événements :

- ouverture des fichiers listés dans le tableau avec prise en charge de nombreux formats, via l'association système Windows et un dialogue de choix en cas d'alternatives multiples ;
- colonne `MIME` persistée, affichée et traduite dynamiquement, préservant l'identifiant brut pour les besoins d'audit ;
- renseignement des champs `data_criacao`, `data_modificacao` et `data_acesso` pour les fichiers, dossiers, sous-dossiers et répertoires enfants lors de l'analyse ;
- retraduction des métadonnées du tableau à partir des valeurs brutes stockées, évitant ainsi la réutilisation de textes déjà traduits lors d'un changement de langue ;
- mise à jour des manuels d'utilisation intégrés pour documenter ces fonctionnalités dans les six langues.

## Correction principale de cette observation

La documentation précédente décrivait une architecture reposant sur :

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- une chaîne d'exécution tributaire de dépendances Python

Cette description est obsolète pour l'arborescence actuelle. L'implémentation contemporaine :

- est compilée en `C++17` ;
- utilise `Qt6` comme pile logicielle principale ;
- exploite `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` et `Qt Qml` ;
- génère des graphiques et diagrammes via un moteur interne dans `source/data/Charts`, reposant sur `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` et Qt Quick Scene Graph ;
- emploie les API natives Windows pour la surveillance des répertoires et l'inspection des exécutables ;
- conserve, par héritage historique, la nomenclature modulaire de la conception originale.

En somme : l'architecture fonctionnelle a été préservée, tandis que la plateforme d'exécution a été entièrement réécrite en C++ natif de haute performance.

## Résumé structurel consolidé

Décompte des fichiers validé directement dans l'arborescence `source/` :

| Couche | `.cpp` | `.h` | Notes |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Interface principale, amorçage visuel, notifications et intégration UI |
| `source/services` | 162 | 164 | Surveillance, base de données, métadonnées, événements et tableau |
| `source/ui` | 126 | 126 | Menus, boutons, boîtes de dialogue, manuel, statistiques et localisation |
| `source/data` | 26 | 27 | Façade de statistiques + infrastructure `Charts` + 17 générateurs |
| `source/tools` | 23 | 24 | Filtres et commandes d'administration des filtres |
| `source/utils` | 5 | 5 | Chemins, journaux (logs), icônes, applications associées et chargement d'assets |
| `source/locale` | 0 | 0 | Catalogues de traduction `.ts` et `.qm` |
| `source/security` | 24 | 24 | Outils de sécurité forensique |
| racine de `source/` | 2 | 2 | Initialisation principale et écran de démarrage (splash) |
| **Total dans `source/`** | **376** | **380** | Base de code C++ actuelle |

Principaux répertoires sous `source/` :

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Chaîne de compilation, build et dépendances réelles

Informations validées dans `CMakeLists.txt` et `mocks/CMakeLists.txt` :

- `cmake_minimum_required(VERSION 4.1)` dans le build principal
- `cmake_minimum_required(VERSION 3.24)` dans le build auxiliaire de `mocks/`
- Standard C++ : `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` dans le build principal

Modules Qt requis par le build principal :

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

Bibliothèques natives Windows complémentaires :

- `wintrust`
- `version`
- `crypt32`

Dans le build auxiliaire de `mocks/find_VersionEditor`, les modules Qt nécessaires sont :

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Exécutables définis via CMake :

- `Linceu_Lighthouse_cpp` dans le `CMakeLists.txt` racine
- `find_VersionEditor` dans `mocks/CMakeLists.txt`

Remarques :

- Le fichier `CMakeLists.txt` détecte automatiquement les installations Qt6 sous `C:/Qt/*/mingw_64/...` sous Windows.
- L'utilitaire `find_VersionEditor` a été isolé dans `mocks/`, avec son propre `CMakePresets.json` et des sorties dirigées vers `mocks/output`.
- L'application actuelle ne requiert aucun interpréteur Python.
- L'outil en ligne de commande `source/locale/compile_translations.cpp` permet de compiler les fichiers `.ts`/`.qm`, mais ne constitue pas le point d'entrée de l'application.

## Répertoires auxiliaires hors de `source/`

Outre le code source applicatif sous `source/`, le référentiel contient :

- `repo/`, avec `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1` et des guides de build ;
- `mocks/`, avec `find_VersionEditor.cpp/.h`, `CMakeLists.txt` et `CMakePresets.json` pour la mise à jour des versions ;
- `.vscode/`, avec `c_cpp_properties.json`, `settings.json` et `tasks.json` pour l'environnement de développement ;
- `.github/workflows/`, avec `update-readme-version.yml` pour l'automatisation des documentations.

Ces répertoires ne font pas partie du runtime applicatif mais accompagnent l'outillage et la maintenance du projet.

## Point d'entrée et amorçage

Flux d'exécution validé dans le code :

1. `main.cpp`
   - Transmet immédiatement le contrôle à `source::iniciar_aplicacao(argc, argv)`.

2. `source/src_01_InicializadorMain.cpp`
   - Définit le répertoire de travail ;
   - Configure l'`AppUserModelID` de Windows ;
   - Initie le démarrage via `SplashAppStarter` ;
   - En mode de secours, instancie `QApplication`, charge les icônes et traductions, crée `InterfaceMonitor` et démarre `DatabaseWriter` en mode direct.

3. `source/src_02_SplashAppStarter.cpp`
   - Crée `QApplication` ;
   - Charge l'icône de l'application ;
   - Initialise le système de traduction ;
   - Affiche l'écran de démarrage comprenant :
     - Image ;
     - Barre de progression ;
     - Zone de journalisation de statut ;
   - Instancie `InterfaceMonitor` ;
   - Initialise `DatabaseWriter` ;
   - Ferme l'écran de démarrage et confie l'exécution à la fenêtre principale.

Cette procédure d'amorce met en évidence :

- Un diagnostic fin du démarrage ;
- Des mécanismes de reprise fiables ;
- Un chargement sécurisé des ressources ;
- Une adéquation parfaite avec le packaging binaire Windows.

## Organisation actuelle par couche

### `source/gui`

Fichiers principaux :

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Sous-répertoires :

- Aucun sous-dossier actif sous `source/gui` dans cette arborescence ;
- Les anciens modules `GerenciadorDesempenho/` et `GerenciadorEstruturaDiretoriosWidget/` ont été supprimés.

Responsabilités :

- Fenêtre principale ;
- Agencement des widgets ;
- Rafraîchissement graphique ;
- Gestion de la progression et des alertes ;
- Internationalisation de la couche GUI ;
- Synchronisation du tableau et amorçage visuel.

### `source/services`

Fichiers principaux :

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

Sous-répertoires validés :

- `GerenciamentoBaseEvento/` avec `21` modules
- `GerenciamentoDiretorioScanner/` avec `9` modules
- `GerenciamentoEventoModificado/` avec `7` modules
- `GerenciamentoEventoMovido/` avec `9` modules
- `GerenciamentoMetadados/` avec `47` modules
- `GerenciamentoObservador/` avec `13` modules
- `GerenciamentoTabela/` avec `47` modules

Responsabilités :

- Surveillance du système de fichiers ;
- Analyse récursive initiale ;
- Distribution des événements ;
- Persistance SQLite locale ;
- Extraction de métadonnées ;
- Mise à jour du tableau ;
- Coloration thématique et retraduction.

### `source/ui`

Fichiers principaux :

- `AppChooserDialog.cpp/.h`
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

Sous-répertoires validés :

- `GerenciamentoBotoes/` avec `6` modules
- `GerenciamentoDialogoCores/` avec `8` modules
- `GerenciamentoEstatisticasUI/` avec `31` modules
- `GerenciamentoEventosUI/` avec `11` modules
- `GerenciamentoLocalizadorQt/` avec `14` modules
- `GerenciamentoMenusUI/` avec `24` modules
- `GerenciamentoTradutorMetadadosQt/` avec `9` modules
- `Manual/` avec `7` modules

Responsabilités :

- Menus généraux et barres d'outils ;
- Exportation de données ;
- Boîte de statistiques ;
- Manuel et aide intégrés ;
- Dialogue de personnalisation des couleurs ;
- Sélecteur d'application pour les fichiers du tableau ;
- Changement de langue à chaud ;
- Boîtes de dialogue À propos, licences, mentions légales et notes de version.

### `source/data`

Structure validée :

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` avec `8` modules d'infrastructure graphique
- `GeradorEstatisticas/` avec `18` modules (`1` base commune + `17` générateurs spécialisés)

Générateurs de diagrammes confirmés :

- Camembert (pie)
- Barres
- Ligne temporelle (timeline)
- Treemap
- Histogramme
- Diagramme de Pareto
- Lignes
- Boîte à moustaches (boxplot)
- Radar d'événements
- Carte thermique (heatmap)
- Nuage de points (scatter)
- Diagramme de Sankey
- Radar multidimensionnel
- Dotplot
- Sankey événement-chemin
- Sankey type-chemin
- Arborescence de répertoires

Remarque :

- La couche `source/data` totalise `27` modules : `1` façade + `8` modules dans `Charts/` + `18` modules dans `GeradorEstatisticas/`.

### `source/tools`

Fichiers principaux :

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Sous-répertoires :

- `GerenciamentoAdministradorFiltros/` avec `10` modules
- `GerenciamentoFiltros/` avec `10` modules

Responsabilités :

- Filtres multicritères ;
- Sélecteur de calendrier ;
- Synchronisation avec les menus ;
- Décompte dynamique des résultats ;
- Sauvegarde de l'état des cases à cocher.

### `source/utils`

Modules validés :

- `ApplicationPathUtils.cpp/.h`
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Responsabilités :

- Détection des répertoires de ressources ;
- Chemins d'accès persistants standards ;
- Journalisation centralisée thread-safe ;
- Chargement d'icônes High-DPI ;
- Détection des applications associées sous Windows.

### `source/locale`

Catalogues validés :

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsabilités :

- Fichiers sources de traduction XML (`.ts`) ;
- Fichiers binaires compilés (`.qm`) ;
- Traduction de l'interface, des métadonnées, des types MIME et des manuels.

## Architecture fonctionnelle actuelle

### 1. Surveillance native du système de fichiers

Le module `source/services/ob_01_Observador.*` constitue le cœur de la surveillance.

Composants clés :

- Répertoire racine sous observation ;
- Rappels (callbacks) vers l'interface graphique ;
- Tampons d'événements et empreintes de déduplication ;
- Commandes de mise en pause et reprise ;
- Compteurs d'événements en temps réel ;
- `thread_scan` dédié à la passe initiale ;
- `std::thread` pour l'écoute continue des API système ;
- `std::thread` pour le dépilement des tampons en arrière-plan.

Appels système Windows natifs :

- `CreateFileW`
- `ReadDirectoryChangesW`

Drapeaux de notification surveillés :

- Nom de fichier ;
- Nom de répertoire ;
- Attributs ;
- Taille ;
- Date de dernière modification ;
- Descripteurs de sécurité ;
- Date de création.

Opérations prises en charge :

- Ajouté
- Supprimé
- Modifié
- Renommé
- Déplacé
- Scanné

### 2. Découplage entre analyse initiale et flux continu

L'architecture isole :

- L'analyse récursive initiale ;
- La capture continue via les API natives ;
- Le traitement multithread des tampons ;
- La persistance en base de données ;
- L'affichage dans la vue du tableau.

Ce découplage garantit la fluidité de l'interface même lors de pics massifs d'activité.

### 3. Persistance locale avec SQLite

Base principale :

- `monitoramento.db`

Emplacement par défaut :

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

Paramètres SQLite vérifiés dans `gbank_02_criar_banco_de_dados.cpp` :

- Pilote `QSQLITE`
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- Indexation automatique des tables d'audit et de snapshot

Module d'écriture asynchrone :

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

Propriétés de `DatabaseWriter` :

- Patron Singleton ;
- File d'attente interne thread-safe ;
- Thread dédié `std::thread` ;
- Écritures par lots (batch) ;
- Gestion d'expiration (timeout) ;
- Arrêt propre et sécurisé.

### 4. Corrélation d'événements complexes

Le système intègre des algorithmes dédiés pour :

- Les déplacements de fichiers ;
- Les suppressions temporaires ;
- Les renommages ;
- Les modifications rapprochées à haute fréquence ;
- Les opérations de masse.

Couches concernées :

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

Ces composants transforment les signaux bruts de l'OS en événements métier intelligibles.

## Tableau, streaming et compatibilité d'interface

L'application associe :

- Une compatibilité historique avec `QTableWidget` ;
- Une migration moderne vers `QTableView + QAbstractTableModel`.

Modèle de données natif :

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Fonctionnalités du modèle :

- `rowCount`, `columnCount`, `headerData`, `data` ;
- `prepend_event`, `append_events`, `replace_events` ;
- Mise en cache des textes et des couleurs ;
- Invalidation dynamique des couleurs.

Évolutions récentes :

- La colonne `MIME` est conservée et traduite dynamiquement ;
- Les horodatages de création, modification et accès s'appliquent aux fichiers et aux dossiers ;
- L'ouverture de fichiers exploite les associations Windows avec dialogue de secours.

## Colonnes, couleurs, thèmes et retraduction

`GerenciamentoTabela/` gère :

- La configuration de la vue tabulaire ;
- Le redimensionnement des colonnes et des en-têtes ;
- Le retour à la ligne automatique dans les titres ;
- L'insertion en flux continu ;
- La mise en surbrillance de la dernière ligne ;
- La visibilité des colonnes ;
- Les palettes de couleurs par opération ;
- La détection du thème Windows clair/sombre (`QSettings`) ;
- La retraduction instantanée des lignes affichées.

## Extraction des métadonnées

`GerenciamentoMetadados/` regroupe `47` modules spécialisés couvrant :

- Code source, images, audio, vidéo, documents, tableurs, présentations, bases de données, exécutables, fichiers temporaires, archives, sauvegardes, journaux, fichiers de configuration, structures composées OLE et formats structurés.

Spécificités techniques :

- `QImageReader` pour les dimensions d'images ;
- Décompression ZIP/OpenXML pour la bureautique ;
- `QSqlDatabase` pour l'inspection des tables SQLite ;
- `QMimeDatabase` pour la reconnaissance de type de contenu ;
- `QFileInfo` pour les dates système ;
- `WinVerifyTrust`, `GetFileVersionInfoW` et `VerQueryValueW` pour les binaires Windows.

## Statistiques et génération graphique

L'infrastructure de statistiques repose sur :

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Fonctionnalités :

- Génération multi-graphique dans une boîte dédiée ;
- Sélecteur dynamique de vues ;
- Retraduction en direct des visualisations ;
- Export d'images haute définition ;
- Moteur de calcul géométrique `ChartLayoutEngine` et modèle typé `LgplChartModel` ;
- Rendu vectoriel de précision via `RasterChartView` avec `QPainter` dans le dialogue Statistiques ;
- Backends `WidgetChartView` (`QOpenGLWidget`) et `QuickChartItem` (Qt Quick Scene Graph).

Total disponible :

- `17` types de diagrammes

## Exportation des données

Gérée par :

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Formats d'exportation :

- `.xlsx`, `.csv`, `.txt`, `.json`, `.xml`, `.db`

Remarque :

- La génération `.xlsx` est réalisée en pur C++ via la structure OpenXML et `QXmlStreamWriter`, sans composant Python externe.

## Filtres et boîte à outils

Le sous-système `Filtros` fournit :

- Filtrage par opération ;
- Expressions multicritères combinées ;
- Compteur de correspondances en temps réel ;
- Synchronisation avec les menus de l'application ;
- Persistance des sélections.

## Évolution de l'architecture : composants retirés et ajoutés

Certains modules anciens ne sont plus présents :

- `source/gui/GerenciadorEstruturaDiretoriosWidget/` ;
- `source/gui/GerenciadorDesempenho/` ;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*`.

En contrepartie, de nouveaux piliers ont été intégrés :

- `source/data/Charts/` (moteur graphique C++/Qt du projet) ;
- `mocks/` (projet CMake pour `find_VersionEditor`) ;
- `repo/`, `.vscode/` et `.github/workflows/` (empaquetage, IDE et intégration continue).

## Internationalisation (i18n)

Langues gérées et vérifiées :

- `pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT`, `de_DE`

Principes :

- `LocalizadorQt` pilote la langue courante et le chargement des catalogues ;
- Préférence sauvegardée dans `language_config.json` ;
- Retraduction intégrale de l'interface, des types d'opérations, des métadonnées, du tableau et des manuels.

## Documents et ressources intégrées

Le dossier `source/assets/` regroupe :

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `LISTAGEM`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE`, `icones`.

Le manuel d'utilisation intégré est généré depuis `source/ui/Manual/`.

## Persistance locale et fichiers de configuration

Dossier persistant :

- `%LOCALAPPDATA%\LinceuLighthouse`

Fichiers de données :

- `monitoramento.db`, `language_config.json`, `colunas_config.json`, `colunas_coloridas.json`, `cores_config.json`, `trial_info.json`.

Journalisation :

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`.

## Flux principal d'exécution

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. Création de `InterfaceMonitor`
5. Initialisation des langues, icônes et composants visuels
6. Sélection du dossier et reprise de session
7. Instanciation de `Observador`
8. Analyse initiale dans `QThread`
9. Surveillance continue dans `std::thread`
10. Réconciliation et journalisation
11. Persistance SQLite
12. Actualisation du tableau
13. Filtrage, export et statistiques sur les données locales

## Points techniques pour la maintenance

1. La nomenclature modulaire reflète l'origine du projet (`ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_`, etc.).
2. Le fichier `README.md` comportait d'anciennes références techniques corrigées par cette observation.
3. L'application est nativement orientée Windows (`ReadDirectoryChangesW`, `CreateFileW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `wintrust`, `version`, `crypt32`).
4. La transition vers l'architecture model/view est progressive (`EventTableModel` et `QTableView`).
5. La base SQLite locale est le cœur opérationnel du produit.
6. L'utilitaire `find_VersionEditor` est isolé sous `mocks/` pour assurer la cohérence des versions dans la documentation.

## Conclusion

Le **Linceu Lighthouse** de ce référentiel est une application de bureau native pour Windows en **C++17/Qt6**, hautement modulaire, articulée autour de :

- La surveillance et l'intégrité des fichiers ;
- La persistance locale sous SQLite ;
- L'extraction native de métadonnées ;
- La génération statistique vectorielle par graphiques natifs ;
- L'internationalisation complète en six langues ;
- La modernisation progressive vers le modèle model/view.

</details>

---

<h2 id="itit">🇮🇹 Italiano (IT)</h2>

<details>
<summary>Fai clic per espandere l'Osservazione Tecnica in italiano</summary>

# LINCEU LIGHTHOUSE - OSSERVAZIONE TECNICA E POSIZIONAMENTO STRATEGICO

## Stato attuale del progetto e Posizionamento

Il repository attuale di **Linceu Lighthouse** consolida una piattaforma desktop **nativa in C++17 con Qt6** compilata tramite CMake. Il prodotto supera la classificazione limitata di semplice monitor di file o FIM aziendale convenzionale, posizionandosi come una **Piattaforma Locale di Intelligence, Cronologia e Audit delle Attività sui File** (*Local File Activity Intelligence, History & Audit Platform*).

La sua proposta di valore fondamentale risponde in modo diretto alla domanda più frequente di qualsiasi utente davanti al computer:
> **"Scopri esattamente cosa è successo ai tuoi file — localmente, visivamente e con cronologia consultabile."**
> *(“Know exactly what happened to your files — locally, visually, deterministically.”)*

Il prodotto opera come soluzione sovrana **100% Local-First** (senza cloud, senza telemetria, senza account e senza IA esterna), articolata in 3 livelli integrati (Monitoraggio Continuo con rilevamento corretto di Move/Rename, Cronologia Consultabile con 17 grafici nativi e Sicurezza/Audit Locale).

Versione consolidata presente negli asset e nella documentazione integrata:
- `v2026.8.25.0`
Autore identificato nel progetto:
- `Fernando Nillsson Cidade`

Licenza del repository:
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Aggiornamento funzionale osservato nella versione 2026.7.4.0

Oltre alla solida base C++/Qt6 già consolidata, questa versione introduce miglioramenti direttamente legati all'esperienza d'uso della tabella principale:

- apertura dei file riportati nella tabella con supporto a diversi formati, sfruttando le associazioni native di Windows e una finestra di scelta in caso di molteplici applicazioni disponibili;
- colonna `MIME` salvata, visualizzata e tradotta dinamicamente, preservando l'identificativo originale per finalità di audit;
- popolamento dei campi `data_criacao`, `data_modificacao` e `data_acesso` per file, cartelle, sottocartelle e percorsi secondari durante la scansione;
- ritraduzione dei metadati della tabella a partire dal valore grezzo originale, evitando che il cambio di lingua riutilizzi testo già tradotto;
- aggiornamento dei manuali utente integrati per illustrare queste funzionalità in tutte le sei lingue supportate.

## Rettifica principale di questa osservazione

Il documento precedente descriveva un'architettura basata su:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- un runtime dipendente da moduli Python

Tale descrizione è completamente superata rispetto al codice attuale. L'implementazione corrente:

- è compilata nativamente in `C++17`;
- adotta `Qt6` come stack applicativo primario;
- impiega `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` e `Qt Qml`;
- esegue il rendering di grafici e diagrammi tramite un motore proprietario in `source/data/Charts`, basato su `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` e Qt Quick Scene Graph;
- utilizza le API native di Windows per l'osservazione delle cartelle e l'estrazione dei metadati degli eseguibili;
- mantiene, come eredità storica, la convenzione modulare del design originale.

In sintesi: l'architettura funzionale è stata integralmente preservata, mentre la piattaforma di esecuzione è stata completamente modernizzata in C++ nativo ad alte prestazioni.

## Riepilogo strutturale consolidato

Conteggio dei file validato direttamente nell'albero `source/`:

| Livello | `.cpp` | `.h` | Note |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Interfaccia principale, bootstrap visivo, notifiche e integrazione UI |
| `source/services` | 162 | 164 | Monitoraggio, database, metadati, eventi e tabella |
| `source/ui` | 126 | 126 | Menu, pulsanti, finestre di dialogo, manuale, statistiche e localizzazione |
| `source/data` | 26 | 27 | Facciata statistiche + infrastruttura `Charts` + 17 generatori |
| `source/tools` | 23 | 24 | Filtri e controlli amministrativi dei filtri |
| `source/utils` | 5 | 5 | Percorsi, log, icone, applicazioni associate e lettura asset |
| `source/locale` | 0 | 0 | Cataloghi di traduzione `.ts` e `.qm` |
| `source/security` | 24 | 24 | Strumenti di sicurezza forense |
| radice di `source/` | 2 | 2 | Inizializzazione principale e splash screen |
| **Totale in `source/`** | **376** | **380** | Base di codice C++ attuale |

Directory principali sotto `source/`:

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Compilazione, toolchain e dipendenze effettive

Dati verificati in `CMakeLists.txt` e `mocks/CMakeLists.txt`:

- `cmake_minimum_required(VERSION 4.1)` nel build principale
- `cmake_minimum_required(VERSION 3.24)` nel build ausiliario di `mocks/`
- Standard di linguaggio: `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` nel build principale

Componenti Qt richiesti dal build principale:

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

Librerie native Windows aggiuntive:

- `wintrust`
- `version`
- `crypt32`

Nel build ausiliario di `mocks/find_VersionEditor`, i componenti Qt richiesti sono:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Eseguibili definiti in CMake:

- `Linceu_Lighthouse_cpp` nel `CMakeLists.txt` principale
- `find_VersionEditor` in `mocks/CMakeLists.txt`

Note:

- `CMakeLists.txt` individua automaticamente installazioni Qt6 in `C:/Qt/*/mingw_64/...` in ambiente Windows.
- L'utility `find_VersionEditor` è isolata in `mocks/`, con proprio `CMakePresets.json` e output in `mocks/output`.
- L'applicazione attuale non richiede alcun interprete Python.
- Lo strumento da riga di comando `source/locale/compile_translations.cpp` compila i file `.ts`/`.qm`, ma non costituisce il punto di ingresso dell'applicazione.

## Directory ausiliarie esterne a `source/`

Oltre al codice applicativo sotto `source/`, il repository comprende:

- `repo/`, con `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1` e script di supporto per CMake;
- `mocks/`, con `find_VersionEditor.cpp/.h`, `CMakeLists.txt` e `CMakePresets.json` per l'aggiornamento automatico delle versioni;
- `.vscode/`, con `c_cpp_properties.json`, `settings.json` e `tasks.json` per la configurazione dell'IDE;
- `.github/workflows/`, con `update-readme-version.yml` per l'automazione della documentazione.

Queste cartelle non appartengono all'albero di esecuzione del programma, ma supportano la manutenzione, il build e il rilascio.

## Punto di ingresso e bootstrap

Flusso di esecuzione verificato nel codice sorgente:

1. `main.cpp`
   - Inoltra immediatamente l'esecuzione a `source::iniciar_aplicacao(argc, argv)`.

2. `source/src_01_InicializadorMain.cpp`
   - Imposta la directory di lavoro corrente;
   - Assegna l'`AppUserModelID` di Windows;
   - Tenta l'avvio tramite `SplashAppStarter`;
   - In modalità fallback, crea `QApplication`, configura icone e traduzioni, istanzia `InterfaceMonitor` e avvia `DatabaseWriter` in modalità immediata.

3. `source/src_02_SplashAppStarter.cpp`
   - Crea `QApplication`;
   - Carica l'icona dell'applicazione;
   - Inizializza il sistema di localizzazione;
   - Costruisce lo splash screen con:
     - Immagine grafica;
     - Barra di avanzamento;
     - Area di output del log;
   - Istanzia `InterfaceMonitor`;
   - Inizializza `DatabaseWriter`;
   - Chiude lo splash screen e consegna il controllo alla finestra principale.

Questo flusso di bootstrap è strutturato per garantire:

- Diagnostica rapida all'avvio;
- Procedure di ripristino affidabili;
- Caricamento sicuro degli asset;
- Piena compatibilità con la distribuzione su Windows.

## Struttura per livelli applicativi

### `source/gui`

File principali validati:

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Sottodirectory:

- Nessuna sottodirectory attiva sotto `source/gui` nel codice corrente;
- I vecchi moduli `GerenciadorDesempenho/` e `GerenciadorEstruturaDiretoriosWidget/` non sono più presenti.

Responsabilità:

- Finestra principale;
- Composizione grafica dei widget;
- Aggiornamento visivo;
- Notifiche e barre di avanzamento;
- Internazionalizzazione della UI;
- Coordinamento della tabella e avvio visivo.

### `source/services`

File principali validati:

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

Sottodirectory convalidate:

- `GerenciamentoBaseEvento/` con `21` moduli
- `GerenciamentoDiretorioScanner/` con `9` moduli
- `GerenciamentoEventoModificado/` con `7` moduli
- `GerenciamentoEventoMovido/` con `9` moduli
- `GerenciamentoMetadados/` con `47` moduli
- `GerenciamentoObservador/` con `13` moduli
- `GerenciamentoTabela/` con `47` moduli

Responsabilità:

- Monitoraggio del file system;
- Scansione ricorsiva iniziale;
- Smistamento delle tipologie di evento;
- Persistenza SQLite locale;
- Estrazione dei metadati;
- Aggiornamento della tabella;
- Evidenziazione cromatica, temi e ritraduzione.

### `source/ui`

File principali validati:

- `AppChooserDialog.cpp/.h`
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

Sottodirectory convalidate:

- `GerenciamentoBotoes/` con `6` moduli
- `GerenciamentoDialogoCores/` con `8` moduli
- `GerenciamentoEstatisticasUI/` con `31` moduli
- `GerenciamentoEventosUI/` con `11` moduli
- `GerenciamentoLocalizadorQt/` con `14` moduli
- `GerenciamentoMenusUI/` con `24` moduli
- `GerenciamentoTradutorMetadadosQt/` con `9` moduli
- `Manual/` con `7` moduli

Responsabilità:

- Barre dei menu e pulsanti di comando;
- Esportazione dati multiformato;
- Finestra dedicata alle statistiche;
- Manuale e guida integrati;
- Finestra di dialogo per la gestione dei colori;
- Selettore di applicazioni per l'apertura dei file;
- Cambio della lingua a runtime;
- Dialoghi Informazioni, licenze, note legali, privacy e note di rilascio.

### `source/data`

Struttura validata:

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` con `8` moduli per l'infrastruttura grafica nativa
- `GeradorEstatisticas/` con `18` moduli (`1` base comune + `17` generatori dedicati)

Generatori di grafici confermati:

- Grafico a torta (pie)
- Grafico a barre
- Linea temporale (timeline)
- Treemap
- Istogramma
- Curva di Pareto
- Grafico a linee
- Boxplot
- Radar degli eventi
- Mappa termica (heatmap)
- Grafico di dispersione (scatter)
- Diagramma di Sankey
- Radar multidimensionale
- Dotplot
- Sankey evento-percorso
- Sankey tipo-percorso
- Albero delle directory

Nota:

- Il livello `source/data` comprende `27` moduli in totale: `1` facciata + `8` moduli in `Charts/` + `18` moduli in `GeradorEstatisticas/`.

### `source/tools`

File principali:

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Sottodirectory:

- `GerenciamentoAdministradorFiltros/` con `10` moduli
- `GerenciamentoFiltros/` con `10` moduli

Responsabilità:

- Filtri multicriterio avanzati;
- Integrazione con il calendario;
- Sincronizzazione con il menu principale;
- Conteggio degli elementi corrispondenti in tempo reale;
- Persistenza dello stato delle caselle di controllo.

### `source/utils`

Moduli validati:

- `ApplicationPathUtils.cpp/.h`
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Responsabilità:

- Risoluzione dei percorsi degli asset;
- Cartelle persistenti standard;
- Logging centralizzato thread-safe;
- Caricamento di icone High-DPI;
- Rilevamento delle applicazioni associate in Windows.

### `source/locale`

File di localizzazione validati:

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Responsabilità:

- File sorgente delle traduzioni (`.ts`);
- Cataloghi binari compilati (`.qm`);
- Supporto alla traduzione dell'interfaccia, dei metadati, dei tipi MIME e dei manuali.

## Architettura funzionale corrente

### 1. Monitoraggio nativo del file system

Il modulo `source/services/ob_01_Observador.*` costituisce il motore del monitoraggio.

Elementi chiave:

- Cartella radice monitorata;
- Callback verso l'interfaccia grafica;
- Buffer circolari e hash per la deduplicazione;
- Controlli di pausa e ripresa;
- Contatori di eventi in tempo reale;
- `thread_scan` per la scansione iniziale;
- `std::thread` per l'ascolto continuo delle API di sistema;
- `std::thread` per l'elaborazione dei buffer in lotti.

Chiamate native Windows impiegate:

- `CreateFileW`
- `ReadDirectoryChangesW`

Flag di notifica registrati:

- Nome del file;
- Nome della directory;
- Attributi;
- Dimensione;
- Data di ultima scrittura;
- Descrittori di sicurezza;
- Data di creazione.

Operazioni gestite:

- Aggiunto
- Eliminato
- Modificato
- Ridenominato
- Spostato
- Scansionato

### 2. Separazione tra baseline e flusso in tempo reale

Il design architetturale separa:

- La scansione ricorsiva iniziale della cartella;
- L'acquisizione continua tramite API nativa;
- L'elaborazione multithread del buffer;
- La persistenza asincrona nel database;
- L'aggiornamento della visualizzazione della tabella.

Questo disaccoppiamento garantisce l'assenza di blocchi anche durante picchi intensi di attività.

### 3. Persistenza locale con SQLite

Database primario:

- `monitoramento.db`

Percorso standard:

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

Parametri SQLite convalidati in `gbank_02_criar_banco_de_dados.cpp`:

- Driver `QSQLITE`
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- Creazione automatica degli indici sulle tabelle di audit e snapshot

Scrittore asincrono dedicato:

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

Caratteristiche di `DatabaseWriter`:

- Pattern Singleton;
- Coda interna thread-safe;
- Thread dedicato `std::thread`;
- Scritture in transazioni batch;
- Cicli di svuotamento con timeout;
- Chiusura controllata e sicura.

### 4. Correlazione di eventi complessi

Il sistema adotta logiche dedicate per:

- Spostamento di file tra percorsi monitorati;
- Eliminazioni temporanee;
- Ridenominazioni;
- Modifiche ad alta frequenza;
- Operazioni massive.

Livelli coinvolti:

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

Ciò assicura che le notifiche del sistema operativo vengano aggregate in operazioni semanticamente significative.

## Tabella, streaming e compatibilità dell'interfaccia

L'architettura presenta un approccio ibrido:

- Piena compatibilità con `QTableWidget`;
- Evoluzione moderna basata su `QTableView + QAbstractTableModel`.

Modello di dati nativo:

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Funzionalità del modello:

- `rowCount`, `columnCount`, `headerData`, `data`;
- `prepend_event`, `append_events`, `replace_events`;
- Cache interne per etichette e stili cromatici;
- Invalidazione selettiva della cache colori.

Caratteristiche recenti:

- Colonna `MIME` persistita e tradotta dinamicamente;
- Date di creazione, modifica e accesso valorizzate per file e cartelle;
- Apertura file con associazione nativa Windows e dialogo di scelta ausiliario.

## Colonne, colori, temi e ritraduzione

`GerenciamentoTabela/` include moduli per:

- Configurazione della tabella;
- Dimensionamento di colonne e intestazioni;
- A capo automatico nei titoli delle colonne;
- Aggiornamento dei dati in streaming;
- Evidenziazione della riga più recente;
- Gestione della visibilità delle colonne;
- Profili cromatici per operazione e colonna;
- Salvataggio delle impostazioni dei colori;
- Rilevamento del tema chiaro/scuro di Windows (`QSettings`);
- Ritraduzione immediata delle righe caricate nella tabella.

## Estrazione dei metadati

`GerenciamentoMetadados/` raggruppa `47` moduli specializzati per:

- Codice sorgente, immagini, audio, video, documenti, fogli di calcolo, presentazioni, database, file eseguibili, file temporanei, archivi compressi, backup, file di log, configurazioni, file OLE e dati strutturati.

Tecnologie adottate:

- `QImageReader` per dimensioni e attributi delle immagini;
- Parsing ZIP/OpenXML per formati Office;
- `QSqlDatabase` per l'ispezione delle tabelle SQLite;
- `QMimeDatabase` per l'identificazione MIME basata sui contenuti;
- `QFileInfo` per le date del file system;
- `WinVerifyTrust`, `GetFileVersionInfoW` e `VerQueryValueW` per gli eseguibili Windows.

## Statistiche e rendering dei grafici

L'infrastruttura di analisi include:

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Caratteristiche:

- Generazione multi-grafico in finestra dedicata;
- Pannello di selezione interattivo;
- Ritraduzione dinamica dei grafici;
- Esportazione di immagini ad alta risoluzione;
- Calcolo geometrico tramite `ChartLayoutEngine` e `LgplChartModel`;
- Canvas vettoriale ad alta definizione con `RasterChartView` e `QPainter` nella finestra Statistiche;
- Backend alternativi con `WidgetChartView` (`QOpenGLWidget`) e `QuickChartItem` (Qt Quick Scene Graph).

Totale disponibile:

- `17` diagrammi visuali

## Esportazione dei dati

Gestita da:

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Formati supportati:

- `.xlsx`, `.csv`, `.txt`, `.json`, `.xml`, `.db`

Nota tecnica:

- La generazione `.xlsx` è realizzata in C++ nativo tramite architettura OpenXML e `QXmlStreamWriter`, senza alcuna dipendenza da Python.

## Filtri e strumenti

Il modulo `Filtros` offre:

- Filtro per tipo di operazione;
- Espressioni combinate multicriterio;
- Conteggio dei risultati in tempo reale;
- Sincronizzazione con la barra dei menu;
- Persistenza dello stato delle selezioni.

## Evoluzione dell'architettura: componenti rimossi e aggiunti

Rispetto a precedenti note tecniche, alcuni moduli sono stati rimossi:

- `source/gui/GerenciadorEstruturaDiretoriosWidget/`;
- `source/gui/GerenciadorDesempenho/`;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*`.

Sono stati invece introdotti:

- `source/data/Charts/` (motore grafico nativo C++/Qt del progetto);
- `mocks/` (progetto CMake per `find_VersionEditor`);
- `repo/`, `.vscode/` e `.github/workflows/` (packaging, configurazione IDE e CI).

## Internazionalizzazione (i18n)

Lingue supportate e verificate:

- `pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT`, `de_DE`

Caratteristiche:

- `LocalizadorQt` gestisce lingua attiva, cataloghi di traduzione e persistenza;
- Preferenza memorizzata in `language_config.json`;
- Ritraduzione completa di interfaccia, tipi di operazione, metadati, tabella e manuali.

## Risorse integrate e documentazione

La directory `source/assets/` contiene:

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `LISTAGEM`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE`, `icones`.

Il manuale utente integrato è generato da `source/ui/Manual/`.

## Persistenza locale e file di configurazione

Directory persistente:

- `%LOCALAPPDATA%\LinceuLighthouse`

File principali:

- `monitoramento.db`, `language_config.json`, `colunas_config.json`, `colunas_coloridas.json`, `cores_config.json`, `trial_info.json`.

Log:

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`.

## Flusso principale di esecuzione

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. Creazione di `InterfaceMonitor`
5. Inizializzazione di lingue, icone e componenti grafici
6. Scelta della cartella e ripristino sessione
7. Creazione di `Observador`
8. Scansione iniziale in `QThread`
9. Monitoraggio continuo in `std::thread`
10. Riconciliazione e registrazione eventi
11. Persistenza SQLite
12. Aggiornamento tabella
13. Filtri, esportazione e statistiche sui dati locali

## Punti tecnici per la manutenzione

1. La nomenclatura conserva l'origine modulare (`ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_`, ecc.).
2. Il file `README.md` conteneva vecchi riferimenti tecnici corretti da questa osservazione.
3. L'applicazione è nativamente integrata con Windows (`ReadDirectoryChangesW`, `CreateFileW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `wintrust`, `version`, `crypt32`).
4. L'evoluzione verso il modello model/view è progressiva (`EventTableModel` e `QTableView`).
5. Il database SQLite locale è il nucleo operativo dell'applicazione.
6. L'utility `find_VersionEditor` è isolata in `mocks/` per la sincronizzazione delle versioni nella documentazione.

## Conclusione

Il **Linceu Lighthouse** presente in questo repository è un'applicazione desktop nativa per Windows in **C++17/Qt6**, con un'architettura modulare avanzata e incentrata su:

- Monitoraggio e integrità del file system;
- Persistenza locale con SQLite;
- Estrazione nativa dei metadati;
- Generazione statistica con grafici nativi;
- Internazionalizzazione completa in sei lingue;
- Compatibilità progressiva con il modello model/view moderno.

</details>

---

<h2 id="dede">🇩🇪 Deutsch (DE)</h2>

<details>
<summary>Klicken Sie hier, um die Technische Beobachtung auf Deutsch zu erweitern</summary>

# LINCEU LIGHTHOUSE - TECHNISCHE BEOBACHTUNG UND STRATEGISCHE POSITIONIERUNG

## Aktueller Projektstatus und Positionierung

Das aktuelle Repository von **Linceu Lighthouse** konsolidiert eine native Desktop-Plattform in **C++17 mit Qt6**, kompiliert über CMake. Das Produkt geht weit über die eingeschränkte Kategorie eines einfachen Datei-Monitors oder traditionellen FIM hinaus und positioniert sich als **Lokale Plattform für Datei-Intelligenz, Verlauf und Dateiaktivitäts-Audit** (*Local File Activity Intelligence, History & Audit Platform*).

Sein zentrales Wertversprechen beantwortet direkt die häufigste Frage jedes Benutzers vor dem Computer:
> **„Erfahren Sie genau, was mit Ihren Dateien geschehen ist — lokal, visuell und mit durchsuchbarem Verlauf.“**
> *(“Know exactly what happened to your files — locally, visually, deterministically.”)*

Das Produkt fungiert als souveräne **100% Local-First**-Lösung (ohne Cloud, ohne Telemetrie, ohne Konten und ohne externe KI), gegliedert in 3 integrierte Schichten (Kontinuierliche Überwachung mit präziser Move/Rename-Erkennung, Durchsuchbarer Verlauf mit 17 nativen Diagrammen und Lokale Sicherheit/Audit).

Konsolidierte Version gemäß Assets und integrierter Dokumentation:
- `v2026.8.25.0`
Identifizierter Autor im Projekt:
- `Fernando Nillsson Cidade`

Lizenz des Repositorys:
- `GNU Lesser General Public License v3.0 (LGPL-3.0)`

## Funktionale Aktualisierungen der Version 2026.7.4.0

Über das etablierte C++/Qt6-Fundament hinaus führt diese Version Verbesserungen ein, die direkt das Benutzererlebnis der Haupttabelle betreffen:

- Öffnen von in der Tabelle gelisteten Dateien mit Unterstützung verschiedener Dateiformate unter Nutzung von Windows-Dateizuordnungen und einem Auswahl-Dialog bei mehreren Alternativen;
- Gespeicherte, angezeigte und dynamisch übersetzte `MIME`-Spalte unter Beibehaltung der Roh-Kennung für Audit-Zwecke;
- Befüllung von `data_criacao`, `data_modificacao` und `data_acesso` für Dateien, Ordner, Unterordner und untergeordnete Pfade während des Suchlaufs;
- Neuübersetzung von Tabellen-Metadaten anhand der gespeicherten Rohwerte, wodurch verhindert wird, dass bei einem Sprachwechsel bereits übersetzte Texte wiederverwendet werden;
- Aktualisierung der integrierten Benutzerhandbücher zur Dokumentation dieser Funktionen in allen sechs unterstützten Sprachen.

## Hauptkorrektur dieser Beobachtung

Die vorherige Dokumentation beschrieb eine Architektur basierend auf:

- Python
- PySide6
- pandas / numpy / matplotlib / seaborn / plotly
- einer von Python-Bibliotheken abhängigen Laufzeitumgebung

Diese Beschreibung ist für den aktuellen Codebestand vollständig veraltet. Die vorliegende Implementierung:

- ist in nativem `C++17` kompiliert;
- nutzt `Qt6` als zentralen Software-Stack;
- verwendet `Qt Widgets`, `Qt SQL`, `Qt XML`, `Qt Concurrent`, `Qt OpenGL/OpenGLWidgets`, `Qt Quick` und `Qt Qml`;
- rendert Diagramme und Grafiken über ein proprietäres Subsystem in `source/data/Charts`, basierend auf `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` und Qt Quick Scene Graph;
- nutzt native Windows-APIs für Verzeichnisüberwachung und Metadaten-Extraktion aus ausführbaren Dateien;
- bewahrt aus historischer Kontinuität die modulare Nomenklatur des ursprünglichen Entwurfs.

Zusammenfassend: Die funktionale Architektur wurde beibehalten, während die Ausführungsplattform vollständig auf natives Hochleistungs-C++ umgestellt wurde.

## Konsolidierte Strukturübersicht

Datei-Zählung direkt im `source/`-Verzeichnisbaum validiert:

| Ebene | `.cpp` | `.h` | Anmerkungen |
|---|---:|---:|---|
| `source/gui` | 8 | 8 | Hauptoberfläche, visueller Bootstrap, Benachrichtigungen und UI-Integration |
| `source/services` | 162 | 164 | Überwachung, Datenbank, Metadaten, Ereignisse und Tabelle |
| `source/ui` | 126 | 126 | Menüs, Schaltflächen, Dialoge, Handbuch, Statistiken und Lokalisierung |
| `source/data` | 26 | 27 | Statistik-Fassade + `Charts`-Infrastruktur + 17 Generatoren |
| `source/tools` | 23 | 24 | Filter und administrative Filtersteuerung |
| `source/utils` | 5 | 5 | Pfade, Protokolle, Symbole, verknüpfte Apps und Asset-Verwaltung |
| `source/locale` | 0 | 0 | Übersetzungskataloge (`.ts` und `.qm`) |
| `source/security` | 24 | 24 | Forensische Sicherheitswerkzeuge |
| `source/`-Wurzel | 2 | 2 | Hauptinitialisierung und Ladebildschirm (Splash) |
| **Gesamt in `source/`** | **376** | **380** | Aktuelle C++-Codebasis |

Hauptverzeichnisse unter `source/`:

- `assets`
- `data`
- `gui`
- `locale`
- `security`
- `services`
- `tools`
- `ui`
- `utils`

## Build, Toolchain und tatsächliche Abhängigkeiten

Validierte Daten aus `CMakeLists.txt` und `mocks/CMakeLists.txt`:

- `cmake_minimum_required(VERSION 4.1)` im Haupt-Build
- `cmake_minimum_required(VERSION 3.24)` im Hilfs-Build unter `mocks/`
- Sprachstandard: `C++17`
- `CMAKE_AUTOMOC ON`
- `CMAKE_AUTORCC ON` im Haupt-Build

Vom Haupt-Build benötigte Qt-Module:

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

Zusätzliche native Windows-Bibliotheken:

- `wintrust`
- `version`
- `crypt32`

Im Hilfs-Build von `mocks/find_VersionEditor` benötigte Qt-Komponenten:

- `Qt::Core`
- `Qt::Gui`
- `Qt::Widgets`

Über CMake definierte ausführbare Dateien:

- `Linceu_Lighthouse_cpp` in der Wurzel-`CMakeLists.txt`
- `find_VersionEditor` in `mocks/CMakeLists.txt`

Anmerkungen:

- `CMakeLists.txt` erkennt automatisch Qt6-Installationen unter `C:/Qt/*/mingw_64/...` auf Windows-Systemen.
- Das Hilfsprogramm `find_VersionEditor` wurde unter `mocks/` isoliert, mit eigener `CMakePresets.json` und Ausgaben in `mocks/output`.
- Das Projekt benötigt keine Python-Laufzeitumgebung.
- Das Konsolenprogramm `source/locale/compile_translations.cpp` dient der Übersetzungskompilierung (`.ts`/`.qm`), ist jedoch nicht der Haupteinstiegspunkt.

## Externe Hilfsverzeichnisse außerhalb von `source/`

Neben dem Quellcode unter `source/` enthält das Repository:

- `repo/`, mit `Linceu_Lighthouse.iss`, `limpar_build_qt6_regeneravel.ps1` und CMake-Skripten;
- `mocks/`, mit `find_VersionEditor.cpp/.h`, `CMakeLists.txt` und `CMakePresets.json` zur Versionspflege;
- `.vscode/`, mit `c_cpp_properties.json`, `settings.json` und `tasks.json` für die IDE-Konfiguration;
- `.github/workflows/`, mit `update-readme-version.yml` für Dokumentations-Workflows.

Diese Verzeichnisse gehören nicht zur Laufzeitumgebung der Anwendung, sondern dienen der Entwicklung, Automatisierung und Bereitstellung.

## Einstiegspunkt und Initialisierungsablauf

Verifizierter Ablauf im Quellcode:

1. `main.cpp`
   - Leitet die Ausführung unmittelbar an `source::iniciar_aplicacao(argc, argv)` weiter.

2. `source/src_01_InicializadorMain.cpp`
   - Setzt das aktuelle Arbeitsverzeichnis;
   - Weist die Windows-`AppUserModelID` zu;
   - Versucht den Start über `SplashAppStarter`;
   - Erstellt im Fallback-Modus `QApplication`, richtet Symbole und Übersetzungen ein, instanziiert `InterfaceMonitor` und startet `DatabaseWriter` im Sofortmodus.

3. `source/src_02_SplashAppStarter.cpp`
   - Erstellt `QApplication`;
   - Lädt das Anwendungssymbol;
   - Initialisiert das Übersetzungssystem;
   - Baut den Ladebildschirm auf mit:
     - Bildanzeige;
     - Fortschrittsbalken;
     - Protokollausgabebereich;
   - Instanziiert `InterfaceMonitor`;
   - Initialisiert `DatabaseWriter`;
   - Schließt den Ladebildschirm und übergibt die Kontrolle an das Hauptfenster.

Diese Startsequenz gewährleistet:

- Schnelle Startdiagnose;
- Zuverlässige Fallback-Mechanismen;
- Robustes Laden von Assets;
- Volle Kompatibilität mit Windows-Binärpaketen.

## Schichtenarchitektur

### `source/gui`

Validierte Hauptdateien:

- `ic_01_InterfaceMonitor.cpp/.h`
- `ic_02_Inicializador.cpp/.h`
- `ic_03_Configurador.cpp/.h`
- `ic_04_Atualizador.cpp/.h`
- `ic_05_GerenciadorProgresso.cpp/.h`
- `ic_06_GerenciadorMensagens.cpp/.h`
- `ic_07_ManipuladorTabela.cpp/.h`
- `ic_08_Internacionalizador.cpp/.h`

Unterverzeichnisse:

- Keine aktiven Unterordner unter `source/gui` in dieser Struktur;
- Die früheren Module `GerenciadorDesempenho/` und `GerenciadorEstruturaDiretoriosWidget/` wurden entfernt.

Aufgaben:

- Hauptanwendungsfenster;
- Widget-Zusammensetzung;
- Grafische Aktualisierungen;
- Fortschritts- und Benachrichtigungsverwaltung;
- Internationalisierung der GUI;
- Tabellen-Koordination und visueller Startablauf.

### `source/services`

Validierte Hauptdateien:

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

Validierte Unterordner:

- `GerenciamentoBaseEvento/` mit `21` Modulen
- `GerenciamentoDiretorioScanner/` mit `9` Modulen
- `GerenciamentoEventoModificado/` mit `7` Modulen
- `GerenciamentoEventoMovido/` mit `9` Modulen
- `GerenciamentoMetadados/` mit `47` Modulen
- `GerenciamentoObservador/` mit `13` Modulen
- `GerenciamentoTabela/` mit `47` Modulen

Aufgaben:

- Dateisystemüberwachung;
- Rekursiver Initial-Suchlauf;
- Ereignis-Kategorisierung;
- Lokale SQLite-Persistenz;
- Metadatenextraktion;
- Tabellenaktualisierung;
- Farbcodierung, Theming und Neuübersetzung.

### `source/ui`

Validierte Hauptdateien:

- `AppChooserDialog.cpp/.h`
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

Validierte Unterordner:

- `GerenciamentoBotoes/` mit `6` Modulen
- `GerenciamentoDialogoCores/` mit `8` Modulen
- `GerenciamentoEstatisticasUI/` mit `31` Modulen
- `GerenciamentoEventosUI/` mit `11` Modulen
- `GerenciamentoLocalizadorQt/` mit `14` Modulen
- `GerenciamentoMenusUI/` mit `24` Modulen
- `GerenciamentoTradutorMetadadosQt/` mit `9` Modulen
- `Manual/` mit `7` Modulen

Aufgaben:

- Menüs und Symbolleisten;
- Multiformat-Datenexport;
- Statistikfenster;
- Benutzerhandbuch und Hilfe;
- Farbauswahldialog;
- App-Auswahl zum Öffnen von Dateien;
- Sprachumschaltung zur Laufzeit;
- Dialoge für Info, Lizenzen, Hinweise, Datenschutz und Versionshinweise.

### `source/data`

Struktur:

- `st_01_GeradorEstatisticas.cpp/.h`
- `Charts/` mit `8` Grafik-Infrastrukturmodulen
- `GeradorEstatisticas/` mit `18` Modulen (`1` Basisklasse + `17` spezialisierte Generatoren)

Bestätigte Diagrammgeneratoren:

- Kreisdiagramm (Pie)
- Balkendiagramm
- Zeitstrahl (Timeline)
- Treemap
- Histogramm
- Pareto-Diagramm
- Liniendiagramm
- Boxplot
- Ereignis-Radar
- Heatmap
- Streudiagramm (Scatter)
- Sankey-Diagramm
- Mehrdimensionales Radar
- Dotplot
- Sankey Ereignis-Pfad
- Sankey Typ-Pfad
- Verzeichnisbaum

Anmerkung:

- Die Ebene `source/data` umfasst insgesamt `27` Module: `1` Fassade + `8` Module in `Charts/` + `18` Module in `GeradorEstatisticas/`.

### `source/tools`

Hauptdateien:

- `fil_01_Filtros.cpp/.h`
- `fil_02_AdministradorCalendario.cpp/.h`
- `fil_03_AdministradorFiltros.cpp/.h`

Unterverzeichnisse:

- `GerenciamentoAdministradorFiltros/` mit `10` Modulen
- `GerenciamentoFiltros/` mit `10` Modulen

Aufgaben:

- Erweiterte Filterkriterien;
- Kalenderintegration;
- Menüsynchronisation;
- Trefferzählung in Echtzeit;
- Speicherung des Auswahlstatus von Kontrollkästchen.

### `source/utils`

Validierte Hilfsmodule:

- `ApplicationPathUtils.cpp/.h`
- `AssociatedApps.cpp/.h`
- `CaminhoPersistenteUtils.cpp/.h`
- `IconUtils.cpp/.h`
- `LogManager.cpp/.h`

Aufgaben:

- Asset-Pfadauflösung;
- Standardisierte persistente Pfade;
- Zentralisierte threadsichere Protokollierung;
- High-DPI-Symbolverwaltung;
- Erkennung von Windows-Dateizuordnungen.

### `source/locale`

Validierte Lokalisierungsdateien:

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

Aufgaben:

- XML-Übersetzungsquellen (`.ts`);
- Kompilierte Binärkataloge (`.qm`);
- Übersetzung von UI, Metadaten, MIME-Typen und Handbüchern.

## Funktionale Architektur

### 1. Native Dateisystemüberwachung

`source/services/ob_01_Observador.*` bildet das Herzstück der Überwachung.

Zentrale Merkmale:

- Überwachter Wurzelpfad;
- Callbacks zur Benutzeroberfläche;
- Ringpuffer und Hashes zur Deduplizierung;
- Pausen- und Fortsetzungssteuerung;
- Echtzeit-Ereigniszähler;
- `thread_scan` für den Initial-Suchlauf;
- `std::thread` für kontinuierliches API-Polling;
- `std::thread` für die Stapelverarbeitung des Puffers.

Genutzte Windows-APIs:

- `CreateFileW`
- `ReadDirectoryChangesW`

Überwachte Benachrichtigungs-Flags:

- Dateiname;
- Verzeichnisname;
- Attribute;
- Größe;
- Änderungsdatum;
- Sicherheitsdeskriptoren;
- Erstellungsdatum.

Unterstützte Operationen:

- Hinzugefügt
- Gelöscht
- Geändert
- Umbenannt
- Verschoben
- Gescannt

### 2. Entkopplung von Initial-Scan und Echtzeit-Strom

Das Design trennt:

- Rekursive Initial-Ordneranalyse;
- Kontinuierliche Erfassung via nativer API;
- Multithread-Pufferung;
- Asynchrone Datenbankpersistenz;
- Anzeige in der Tabellenansicht.

Diese Entkopplung verhindert UI-Blockaden selbst bei massiven Dateiaktivitätsspitzen.

### 3. Lokale Persistenz mit SQLite

Hauptdatenbank:

- `monitoramento.db`

Standardspeicherort:

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db`

SQLite-Konfigurationen in `gbank_02_criar_banco_de_dados.cpp`:

- `QSQLITE`-Treiber
- `PRAGMA journal_mode = WAL`
- `PRAGMA synchronous = NORMAL`
- `PRAGMA temp_store = MEMORY`
- `PRAGMA busy_timeout = 30000`
- Automatische Indexerstellung auf Audit- und Snapshot-Tabellen

Dedizierter asynchroner Writer:

- `source/services/GerenciamentoBaseEvento/gbank_20_db_writer.*`

Eigenschaften des `DatabaseWriter`:

- Singleton-Muster;
- Threadsichere interne Queue;
- Dedizierter `std::thread`-Worker;
- Batch-Transaktionen;
- Timeout-gesteuerte Flush-Zyklen;
- Kontrolliertes sicheres Herunterfahren.

### 4. Korrelation komplexer Ereignisse

Das System enthält dedizierte Algorithmen für:

- Datei-Verschiebungen über Verzeichnisse hinweg;
- Temporäre Löschungen;
- Umbenennungen;
- Hochfrequente Änderungsspitzen;
- Massenoperationen.

Beteiligte Schichten:

- `GerenciamentoEventoMovido/`
- `GerenciamentoEventoModificado/`
- `GerenciamentoBaseEvento/`

Hierdurch werden rohe Betriebssystemsignale zu semantisch klaren Ereignissen konsolidiert.

## Tabelle, Streaming und Schnittstellen-Kompatibilität

Die Benutzeroberfläche nutzt einen hybriden Ansatz:

- Abwärtskompatibilität mit `QTableWidget`;
- Moderne Architektur auf Basis von `QTableView + QAbstractTableModel`.

Natives Tabellenmodell:

- `source/services/GerenciamentoTabela/gtab_47_EventTableModel.*`

Fähigkeiten des Modells:

- `rowCount`, `columnCount`, `headerData`, `data`;
- `prepend_event`, `append_events`, `replace_events`;
- Interne Caches für Texte und Farbprofile;
- Dynamische Farb-Cache-Invalidierung.

Aktuelle Features:

- `MIME`-Spalte wird dynamisch übersetzt und persistiert;
- Erstellungs-, Änderungs- und Zugriffsdaten werden für Dateien und Ordner befüllt;
- Datei-Öffnen-Aktionen nutzen Windows-Zuordnungen mit Auswahldialog.

## Spalten, Farben, Themes und Neuübersetzung

`GerenciamentoTabela/` steuert:

- Tabellenkonfiguration;
- Spalten- und Kopfzeilenbreiten;
- Zeilenumbruch in Spaltenüberschriften;
- Datenaktualisierung im Stream;
- Hervorhebung der neuesten Zeile;
- Spaltensichtbarkeit;
- Farbprofile nach Operationsart;
- Erkennung des Windows-Themes (`QSettings`);
- Sofortige Neuübersetzung vorhandener Zeilen.

## Metadatenextraktion

`GerenciamentoMetadados/` umfasst `47` spezialisierte Module für:

- Quellcode, Bilder, Audio, Video, Dokumente, Tabellenkalkulationen, Präsentationen, Datenbanken, Binärdateien, temporäre Dateien, Archive, Backups, Protokolle, Konfigurationen, OLE-Strukturen und strukturierte Daten.

Technische Grundlagen:

- `QImageReader` für Bildabmessungen;
- ZIP/OpenXML-Parsing für Office-Dateien;
- `QSqlDatabase` für SQLite-Strukturanalysen;
- `QMimeDatabase` für inhaltliche MIME-Erkennung;
- `QFileInfo` für Dateisystemzeitstempel;
- `WinVerifyTrust`, `GetFileVersionInfoW` und `VerQueryValueW` für Windows-PE-Binaries.

## Statistik und Diagramm-Rendering

Die Analyse-Infrastruktur umfasst:

- `source/data/st_01_GeradorEstatisticas.*`
- `source/data/GeradorEstatisticas/*`
- `source/ui/ui_06_GerenciadorEstatisticasUI.*`
- `source/ui/GerenciamentoEstatisticasUI/*`

Funktionen:

- Multi-Diagramm-Generierung in eigenem Dialog;
- Interaktives Auswahlpanel;
- Live-Neuübersetzung der Diagramme;
- Hochauflösender Bildexport;
- Geometrische Berechnung mit `ChartLayoutEngine` und `LgplChartModel`;
- Hochpräzise Vektordarstellung über `RasterChartView` mit `QPainter` im Statistik-Dialog;
- Alternative Backends via `WidgetChartView` (`QOpenGLWidget`) und `QuickChartItem` (Qt Quick Scene Graph).

Gesamtumfang:

- `17` Diagrammtypen

## Datenexport

Gesteuert über:

- `source/ui/ui_07_GerenciadorDados.*`
- `source/ui/GerenciamentoBotoes/geb_03_exportar_dados.*`
- `source/ui/GerenciamentoBotoes/geb_04_exportar_para_sqlite.*`

Exportformate:

- `.xlsx`, `.csv`, `.txt`, `.json`, `.xml`, `.db`

Anmerkung:

- Der `.xlsx`-Export wird nativ über OpenXML und `QXmlStreamWriter` in C++ erzeugt, ohne externe Python-Abhängigkeiten.

## Filter und Werkzeuge

Das Subsystem `Filtros` bietet:

- Filterung nach Operationsart;
- Kombinierte Multikriterien-Ausdrücke;
- Trefferzählung in Echtzeit;
- Synchronisation mit der Menüleiste;
- Sitzungsübergreifende Speicherung des Auswahlstatus.

## Architekturevolution: Entfernte und Hinzugefügte Komponenten

Mehrere frühere Module wurden entfernt:

- `source/gui/GerenciadorEstruturaDiretoriosWidget/`;
- `source/gui/GerenciadorDesempenho/`;
- `source/ui/GerenciamentoMenusUI/gmui_23_toggle_desempenho.*`.

Eingeführte neue Kernbereiche:

- `source/data/Charts/` (natives C++/Qt-Grafik-Rendering);
- `mocks/` (isoliertes CMake-Projekt für `find_VersionEditor`);
- `repo/`, `.vscode/` und `.github/workflows/` (Packaging, IDE-Setup und CI).

## Internationalisierung (i18n)

Unterstützte Sprachen:

- `pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT`, `de_DE`

Merkmale:

- `LocalizadorQt` verwaltet Sprachauswahl, Kataloge und Speicherung;
- Einstellung gespeichert in `language_config.json`;
- Vollständige Übersetzung von UI, Operationen, Metadaten, Tabelleninhalten und Handbüchern.

## Integrierte Dokumentation und Assets

Der Ordner `source/assets/` enthält:

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `LISTAGEM`, `NOTICES`, `PRIVACY_POLICY`, `RELEASE`, `icones`.

Das integrierte Handbuch wird aus `source/ui/Manual/` kompiliert.

## Lokale Persistenz und Konfigurationsdateien

Speicherordner:

- `%LOCALAPPDATA%\LinceuLighthouse`

Dateien:

- `monitoramento.db`, `language_config.json`, `colunas_config.json`, `colunas_coloridas.json`, `cores_config.json`, `trial_info.json`.

Protokollierung:

- `%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log`.

## Hauptausführungsablauf

1. `main.cpp`
2. `src_01_InicializadorMain.cpp`
3. `src_02_SplashAppStarter.cpp`
4. Erzeugung von `InterfaceMonitor`
5. Initialisierung von Lokalisierung, Symbolen und Benutzeroberfläche
6. Verzeichnisauswahl und Sitzungswiederherstellung
7. Instanziierung von `Observador`
8. Initial-Scan in `QThread`
9. Kontinuierliche Überwachung in `std::thread`
10. Ereignisabgleich und Protokollierung
11. SQLite-Persistenz
12. Tabellenaktualisierung
13. Filter, Export und Statistiken auf Basis der lokalen Datenbank

## Technische Hinweise für die Wartung

1. Die modulare Nomenklatur spiegelt den Ursprung des Projekts wider (`ob_`, `gtab_`, `gmet_`, `gmui_`, `geui_` etc.).
2. Die Datei `README.md` enthielt frühere Implementierungsdetails, die durch diese Beobachtung korrigiert wurden.
3. Die Anwendung ist tief in Windows integriert (`ReadDirectoryChangesW`, `CreateFileW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `wintrust`, `version`, `crypt32`).
4. Der Übergang zum Model/View-Muster erfolgt schrittweise (`EventTableModel` und `QTableView`).
5. Die lokale SQLite-Datenbank ist der operative Kern des Produkts.
6. Das Hilfswerkzeug `find_VersionEditor` ist unter `mocks/` isoliert, um Versionen und Daten in der Dokumentation konsistent zu halten.

## Fazit

Das **Linceu Lighthouse** in diesem Workspace ist eine native Windows-Desktop-Anwendung in **C++17/Qt6** mit einer ausgeprägten modularen Architektur und Schwerpunkten auf:

- Dateisystemüberwachung und Dateiintegrität;
- Lokaler Persistenz mit SQLite;
- Nativer Metadatenextraktion;
- Visueller Statistik mit nativen Vektordiagrammen;
- Vollständiger Internationalisierung in sechs Sprachen;
- Stufenweiser Modernisierung hin zum Model/View-Muster.

</details>

---

<h2 id="tree">🌳 Estrutura da Árvore de Diretórios e Arquivos / Directory and File Tree Structure</h2>

<details>
<summary>Clique para expandir a Árvore de Arquivos / Click to expand File Tree</summary>

# LINCEU LIGHTHOUSE: source
**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Linceu_Lighthouse_cpp\source`

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
│       │   ├── 📄 linceu_com_fundo_1080_1080.ico
│       │   ├── 📄 linceu_com_fundo_1440_2160.ico
│       │   ├── 📄 linceu_com_fundo_150_150.ico
│       │   ├── 📄 linceu_com_fundo_1920_1080.ico
│       │   ├── 📄 linceu_com_fundo_2160_2160.ico
│       │   ├── 📄 linceu_com_fundo_300_300.ico
│       │   ├── 📄 linceu_com_fundo_3840_2160.ico
│       │   ├── 📄 linceu_com_fundo_584_800.ico
│       │   ├── 📄 linceu_com_fundo_71_71.ico
│       │   ├── 📄 linceu_sem_fundo.ico
│       │   ├── 📄 linceu_sem_fundo_1080_1080.ico
│       │   ├── 📄 linceu_sem_fundo_1440_2160.ico
│       │   ├── 📄 linceu_sem_fundo_150_150.ico
│       │   ├── 📄 linceu_sem_fundo_1920_1080.ico
│       │   ├── 📄 linceu_sem_fundo_2160_2160.ico
│       │   ├── 📄 linceu_sem_fundo_300_300.ico
│       │   ├── 📄 linceu_sem_fundo_3840_2160.ico
│       │   ├── 📄 linceu_sem_fundo_584_800.ico
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
│           ├── 🖼️ linceu_com_fundo_1080_1080.png
│           ├── 🖼️ linceu_com_fundo_1440_2160.png
│           ├── 🖼️ linceu_com_fundo_150_150.png
│           ├── 🖼️ linceu_com_fundo_1920_1080.png
│           ├── 🖼️ linceu_com_fundo_2160_2160.png
│           ├── 🖼️ linceu_com_fundo_300_300.png
│           ├── 🖼️ linceu_com_fundo_3840_2160.png
│           ├── 🖼️ linceu_com_fundo_584_800.png
│           ├── 🖼️ linceu_com_fundo_71_71.png
│           ├── 🖼️ linceu_sem_fundo.png
│           ├── 🖼️ linceu_sem_fundo_1080_1080.png
│           ├── 🖼️ linceu_sem_fundo_1440_2160.png
│           ├── 🖼️ linceu_sem_fundo_150_150.png
│           ├── 🖼️ linceu_sem_fundo_1920_1080.png
│           ├── 🖼️ linceu_sem_fundo_2160_2160.png
│           ├── 🖼️ linceu_sem_fundo_300_300.png
│           ├── 🖼️ linceu_sem_fundo_3840_2160.png
│           ├── 🖼️ linceu_sem_fundo_584_800.png
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
│   │   ├── ⚡ gevmov_09_adicionar_evento.h
│   │   └── ⚡ gevmov_10_PendenciasMovimentacao.h
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
│   ├── ⚡ AppChooserDialog.cpp
│   ├── ⚡ AppChooserDialog.h
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
│   ├── ⚡ AssociatedApps.cpp
│   ├── ⚡ AssociatedApps.h
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
# LINCEU LIGHTHOUSE: .vscode

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Linceu_Lighthouse_cpp\.vscode`

```
├── ⚙️ c_cpp_properties.json
├── ⚙️ cmake-kits.json
├── ⚙️ settings.json
└── ⚙️ tasks.json
```
# LINCEU LIGHTHOUSE: .github

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Linceu_Lighthouse_cpp\.github`

```
└── 📁 workflows
    └── ⚙️ update-readme-version.yml
```
# LINCEU LIGHTHOUSE: mocks

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Linceu_Lighthouse_cpp\mocks`

```
├── 📄 CMakeLists.txt
├── ⚙️ CMakePresets.json
├── ⚡ compile_translations.cpp
├── ⚡ compile_translations.h
├── ⚡ find_VersionEditor.cpp
├── ⚡ find_VersionEditor.h
└── 📄 limpar_build_qt6_regeneravel.ps1
```
# LINCEU LIGHTHOUSE: repo

**Root Path:** `c:\Users\ferna\APLICATIVOS\CPP\Linceu_Lighthouse_cpp\repo`

```
├── 📁 LISTAGEM
│   ├── 📁 de_DE
│   │   ├── 🖼️ Captura de tela 2026-08-25 131219.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131236.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131253.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131314.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131333.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131352.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131411.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131430.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131450.png
│   │   └── 🖼️ Captura de tela 2026-08-25 131517.png
│   ├── 📁 en_US
│   │   ├── 🖼️ Captura de tela 2026-08-25 125717.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125736.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125753.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125809.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125827.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125846.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125904.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125923.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 125941.png
│   │   └── 🖼️ Captura de tela 2026-08-25 130002.png
│   ├── 📁 es_ES
│   │   ├── 🖼️ Captura de tela 2026-08-25 130104.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130127.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130143.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130201.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130224.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130242.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130302.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130319.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130340.png
│   │   └── 🖼️ Captura de tela 2026-08-25 130400.png
│   ├── 📁 fr_FR
│   │   ├── 🖼️ Captura de tela 2026-08-25 130451.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130510.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130527.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130546.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130603.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130620.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130641.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130659.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130719.png
│   │   └── 🖼️ Captura de tela 2026-08-25 130740.png
│   ├── 📁 it_IT
│   │   ├── 🖼️ Captura de tela 2026-08-25 130824.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130842.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130859.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130917.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 130953.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131018.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131036.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131055.png
│   │   ├── 🖼️ Captura de tela 2026-08-25 131115.png
│   │   └── 🖼️ Captura de tela 2026-08-25 131136.png
│   └── 📁 pt_BR
│       ├── 🖼️ Captura de tela 2026-08-25 123724.png
│       ├── 🖼️ Captura de tela 2026-08-25 123745.png
│       ├── 🖼️ Captura de tela 2026-08-25 123803.png
│       ├── 🖼️ Captura de tela 2026-08-25 123821.png
│       ├── 🖼️ Captura de tela 2026-08-25 123839.png
│       ├── 🖼️ Captura de tela 2026-08-25 123859.png
│       ├── 🖼️ Captura de tela 2026-08-25 123916.png
│       ├── 🖼️ Captura de tela 2026-08-25 123933.png
│       ├── 🖼️ Captura de tela 2026-08-25 123952.png
│       └── 🖼️ Captura de tela 2026-08-25 124030.png
├── 📄 Linceu_Lighthouse.iss
├── 📄 Linceu_Lighthouse_mingw.iss
├── 📄 Linceu_Lighthouse_msvc.iss
└── 📄 Para preservar o cmake --build sem rodar cmake --preset.txt
```
