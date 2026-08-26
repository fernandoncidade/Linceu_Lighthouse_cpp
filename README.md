<!-- Multilanguage README.md for Linceu_Lighthouse -->

<p align="center">
  <b>Selecione o idioma / Select language / Seleccione el idioma / Sélectionnez la langue / Seleziona la lingua / Sprache wählen</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> |
  <a href="#enus">🇺🇸 English (US)</a>
</p>

---

<h2 id="ptbr">🇧🇷 Português (BR)</h2>

> **Observação:** Este repositório refere-se à versão **v2026.4.27.0** do Projeto Linceu Lighthouse. Apoie o projeto e adquira a versão paga através do link: [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Clique para expandir o README em português</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015127.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015146.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015204.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015222.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015241.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015300.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015319.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015345.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015410.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-04-27%20015902.png" alt="Linceu Lighthouse - Português" width="320">
</p>

Projeto: LINCEU LIGHTHOUSE  
Versão: v2026.4.27.0<br>
Autor: Fernando Nillsson Cidade  

Descrição: Visibilidade e controle sobre seus arquivos.

O **Linceu Lighthouse** é uma aplicação desktop nativa em **C++17 com Qt6** para monitoramento de integridade de arquivos. O projeto registra, persiste, enriquece e apresenta eventos de criação, modificação, exclusão, renomeação, movimentação e varredura inicial em diretórios escolhidos pelo usuário.

A implementação atual não usa mais a antiga base Python. O runtime principal é compilado com CMake, Qt Widgets, Qt SQL, Qt XML, Qt Concurrent, Qt OpenGL/OpenGLWidgets, Qt Quick/QML, APIs nativas do Windows e uma camada própria de geração de gráficos.

Principais casos de uso:

- Auditoria de atividades em pastas compartilhadas, projetos e repositórios locais.
- Registro cronológico de operações de arquivos com persistência local em SQLite.
- Investigação de eventos específicos, como remoção, renomeação ou movimentação inesperada.
- Análise de padrões de atividade com filtros, estatísticas e gráficos.
- Exportação de evidências e relatórios para formatos tabulares, estruturados e banco de dados.
- Apoio a fluxos de segurança, quarentena, sessões em tempo real e pacotes de evidência.

Como o Linceu Lighthouse funciona:

1. Ao selecionar um diretório, a aplicação cria uma linha de base por varredura recursiva.
2. Em seguida, o monitoramento contínuo observa alterações usando APIs nativas do Windows.
3. Eventos brutos são normalizados em tipos de operação e enriquecidos com metadados.
4. A camada de persistência grava os dados em SQLite com escrita assíncrona em lote.
5. A interface Qt exibe, traduz, filtra, colore, ordena, exporta e resume os eventos.

Ferramentas disponíveis:

- Filtros rápidos e avançados por operação, texto, extensão, data e seleção.
- Tabela com colunas configuráveis, cores por operação e suporte progressivo a model/view.
- Estatísticas visuais com gráficos nativos renderizados por Qt.
- Exportação para `.xlsx`, `.csv`, `.txt`, `.json`, `.xml` e `.db`.
- Diálogos de segurança para multi-diretório, alertas, sessões, quarentena, restauração e evidência.
- Internacionalização em `pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` e `de_DE`.

---

MANUAL DE UTILIZAÇÃO RÁPIDO:

1. Execute o aplicativo para abrir a interface gráfica.
2. Selecione um diretório para monitorar.
3. Inicie o monitoramento para executar a varredura inicial e acompanhar alterações.
4. Use os menus e botões laterais para filtrar, exportar, visualizar estatísticas e consultar segurança.
5. Salve relatórios ou bancos exportados quando precisar arquivar resultados.

MANUAL DE UTILIZAÇÃO DETALHADO:

Requisitos do sistema:

- Windows 10 ou superior.
- Permissão de leitura no diretório monitorado.
- Permissão de gravação em `%LOCALAPPDATA%\LinceuLighthouse` para banco, configurações e logs.

Instalação:

- Instale o executável distribuído pelo instalador do projeto ou gere o binário pelo fluxo CMake descrito neste README.
- Em builds locais, use o preset compatível com sua toolchain Qt6: MinGW ou MSVC.

Configuração inicial:

- A interface inicia com a tabela de eventos vazia.
- O idioma é carregado pelo sistema de localização e pode ser alterado pelo menu de opções.
- A aplicação cria diretórios persistentes quando necessário.
- Logs são gravados em `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Monitoramento de diretórios:

1. Selecionar um diretório:
   - Clique em `Selecionar Diretório` ou use `Arquivo -> Selecionar Diretório`.
   - O caminho selecionado passa a ser usado pelo observador e pelo fluxo de varredura.

2. Iniciar ou parar monitoramento:
   - Clique em `Iniciar/Parar` ou use o menu correspondente.
   - A varredura inicial popula o snapshot e registra eventos `escaneado`.
   - Depois da varredura, o monitoramento contínuo passa a registrar alterações em tempo real.

3. Pausar monitoramento ou escaneamento:
   - O projeto possui controles separados para interromper monitoramento, scan ou fluxos ativos.
   - O estado visual é atualizado pela camada de progresso e mensagens.

Interface principal:

- Barra de menu: arquivo, configurações, opções, exportação, ajuda, segurança e idioma.
- Barra lateral: ações frequentes como selecionar, iniciar/parar, pausar, limpar, exportar e estatísticas.
- Rótulo de diretório: caminho monitorado.
- Rótulo de resultado: mensagens operacionais e feedback de segurança.
- Tabela de dados: eventos e metadados.
- Barra de progresso: varreduras e tarefas longas.
- Diálogos auxiliares: filtros, cores, colunas, estatísticas, manual, ajuda e sobre.

Campos de evento mais importantes:

- Tipo de operação.
- Nome.
- Diretório anterior.
- Diretório atual.
- Datas de criação, modificação e acesso.
- Tipo/extensão.
- Tamanhos em múltiplas unidades.
- Atributos, autor, dimensões, duração, taxa de bits e proteção.
- Metadados específicos para documentos, planilhas, apresentações, bancos, arquivos compactados e executáveis.

Filtros e pesquisa:

- Filtros rápidos por tipo de operação no menu.
- Filtros avançados por operação, extensão, texto, intervalo de data e combinação de critérios.
- Sincronização com o menu principal.
- Atualização de contagem após alteração de filtro ou idioma.
- Persistência de estado dos checkboxes de filtros.

Visualização de estatísticas:

- Acesse pelo botão de estatísticas ou pelo menu.
- Os gráficos são gerados por módulos C++ em `source/data/GeradorEstatisticas`.
- A janela permite seleção de gráficos, regeneração, salvamento e atualização após troca de idioma.

Gráficos disponíveis:

- Pizza.
- Barras.
- Timeline.
- Treemap.
- Histograma.
- Pareto.
- Linha.
- Boxplot.
- Radar de eventos.
- Heatmap.
- Scatter.
- Sankey.
- Radar.
- Dotplot.
- Sankey evento-caminho.
- Sankey tipo-caminho.
- Árvore de diretórios.

Exportação de dados:

- `Excel (.xlsx)`: pacote OpenXML montado pelo próprio código C++.
- `CSV (.csv)`: delimitado por vírgula.
- `Texto (.txt)`: delimitado por tabulação.
- `JSON (.json)`: estrutura serializada com cabeçalhos e linhas.
- `XML (.xml)`: saída hierárquica via `QXmlStreamWriter`.
- `SQLite (.db)`: banco gerado pela camada de exportação.

Opções de exportação:

- Apenas colunas ativas.
- Apenas filtros ativos.
- Apenas seleção atual.
- Compatibilidade com `QTableWidget` e `QTableView`.

Configuração de colunas e cores:

- O menu de colunas permite ativar, ocultar e restaurar campos.
- O menu de cores permite colorir por operação ou por coluna.
- Configurações persistem em arquivos JSON locais.
- A tabela reage à troca de tema do Windows e invalida caches visuais quando necessário.

Internacionalização:

- Idiomas suportados: português, inglês, espanhol, francês, italiano e alemão.
- Arquivos `.ts` e `.qm` ficam em `source/locale`.
- O `LocalizadorQt` carrega traduções, salva preferência e fornece fallback.
- Operações, metadados, menus, manual, ajuda, sobre e textos de segurança são traduzidos.

Resolução de problemas:

1. Monitoramento não inicia:
   - Confirme permissões do diretório.
   - Evite diretórios protegidos do sistema.
   - Verifique se o banco local pode ser criado.

2. Eventos não aparecem:
   - Confira se o monitoramento está ativo.
   - Limpe filtros.
   - Teste criando, renomeando ou excluindo um arquivo simples no diretório monitorado.

3. Alto consumo de recursos:
   - Evite monitorar árvores muito grandes sem necessidade.
   - Prefira subdiretórios específicos.
   - Exporte ou limpe dados antigos quando a base crescer demais.

4. Estatísticas vazias:
   - Confirme que existem eventos carregados.
   - Remova filtros que possam ocultar linhas.
   - Verifique se `monitoramento.db` está acessível.

## 📋 Documentação Técnica

### Visão Geral do Sistema

**Linceu Lighthouse** é uma aplicação desktop nativa para Windows, escrita em **C++17** e baseada em **Qt6**. A arquitetura atual substitui a antiga cadeia Python/PySide por módulos `.cpp` e `.h`, com build CMake, persistência SQLite via Qt SQL, renderização Qt Widgets/OpenGL/Quick e integração com APIs nativas do Windows.

A aplicação é organizada em camadas:

- `source/gui`: janela principal, composição visual e integração de alto nível.
- `source/services`: observação de diretórios, eventos, banco, metadados e tabela.
- `source/ui`: menus, botões, estatísticas, filtros, manual, ajuda, sobre e localização.
- `source/data`: fachada estatística, geradores e infraestrutura de gráficos.
- `source/tools`: filtros e administração de critérios.
- `source/security`: recursos de segurança, sessões, quarentena, evidência e backend de armazenamento.
- `source/utils`: caminhos, ícones e logging.
- `source/assets` e `source/locale`: conteúdo embarcado, licenças, textos e traduções.

#### Tecnologias Principais

- **Linguagem:** C++17.
- **Build:** CMake, com `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- **Framework:** Qt6.
- **UI:** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, menus e ações.
- **Banco:** SQLite via `QSqlDatabase` e `QSqlQuery`.
- **Concorrência:** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` e Qt Concurrent.
- **Gráficos:** infraestrutura própria com `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` e modelo tipado `LgplChartModel`.
- **Exportação:** escrita própria de CSV, TXT, JSON, XML, SQLite e pacote `.xlsx` OpenXML.
- **Internacionalização:** Qt Linguist (`.ts` e `.qm`) com seis idiomas.
- **Windows:** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` para tema e bibliotecas `wintrust`, `version` e `crypt32`.
- **Segurança:** camada C++ própria com baseline criptográfico, regras, score de risco, quarentena, evidências e cache local SQLite para backend de segurança.

O aplicativo principal não depende de runtime Python, `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` ou bibliotecas equivalentes. Qualquer menção a Python em arquivos de licença reflete documentação de terceiros embarcada, não a stack de execução principal.

---

### 📁 Estrutura de Diretórios e Módulos

Resumo validado na árvore `source/`:

| Camada | `.cpp` | `.h` | Responsabilidade |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Inicialização principal e splash |
| `source/assets` | 0 | 0 | Textos, licenças, imagens, ícones e conteúdo embarcado |
| `source/data` | 26 | 27 | Estatísticas, gráficos e renderização |
| `source/gui` | 8 | 8 | Janela principal e composição visual |
| `source/locale` | 0 | 0 | Arquivos `.ts` e `.qm` de tradução |
| `source/security` | 24 | 24 | Segurança, evidência, quarentena e sessões |
| `source/services` | 162 | 163 | Observação, eventos, banco, metadados e tabela |
| `source/tools` | 23 | 24 | Filtros e administração de filtros |
| `source/ui` | 125 | 125 | Menus, botões, diálogos, manual, estatísticas e localização |
| `source/utils` | 4 | 4 | Caminhos, ícones e logs |
| **Total em `source/`** | **374** | **377** | Base C++ atual |

<details>
<summary><b>📂 source-root - Bootstrap da aplicação</b></summary>

#### Módulos Principais

- `src_01_InicializadorMain.cpp/.h` - Entrada real da aplicação após `main.cpp`, ajuste de diretório corrente, identificação Windows e fallback de inicialização.
- `src_02_SplashAppStarter.cpp/.h` - Inicialização com splash, progresso, ícone, traduções, criação da `InterfaceMonitor` e partida antecipada do writer.

</details>

<details>
<summary><b>📂 source/gui/ - Camada de Interface Gráfica</b></summary>

#### Módulos Principais

- `ic_01_InterfaceMonitor.cpp/.h` - Janela principal, propriedades compartilhadas, container de tabelas e integração de subsistemas.
- `ic_02_Inicializador.cpp/.h` - Inicialização de widgets e objetos base.
- `ic_03_Configurador.cpp/.h` - Layouts, componentes e configuração visual.
- `ic_04_Atualizador.cpp/.h` - Atualização de interface para tabela, status e progresso.
- `ic_05_GerenciadorProgresso.cpp/.h` - Barras e feedback de progresso.
- `ic_06_GerenciadorMensagens.cpp/.h` - Mensagens, status e comunicação com usuário.
- `ic_07_ManipuladorTabela.cpp/.h` - Operações de tabela e adaptação para a UI.
- `ic_08_Internacionalizador.cpp/.h` - Integração da interface com o sistema de localização.

Não existem mais os antigos subdiretórios `GerenciadorDesempenho/` e `GerenciadorEstruturaDiretoriosWidget/` nesta árvore. A documentação atual não deve tratá-los como subsistemas ativos.

</details>

<details>
<summary><b>📂 source/services/ - Núcleo de Monitoramento, Eventos e Dados</b></summary>

#### Módulos Principais

- `ob_01_Observador.cpp/.h` - Observador de diretório, threads, buffers e ciclo de monitoramento.
- `ob_02_BaseEvento.cpp/.h` - Base comum para eventos e integração com SQLite.
- `ob_03_DiretorioScanner.cpp/.h` - Varredura inicial e construção de snapshot.
- `ob_04_EventoAdicionado.cpp/.h` - Registro de arquivos criados.
- `ob_05_EventoExcluido.cpp/.h` - Registro de exclusões e exclusões temporárias.
- `ob_06_EventoModificado.cpp/.h` - Consolidação de modificações.
- `ob_07_EventoRenomeado.cpp/.h` - Tratamento de renomeações.
- `ob_08_EventoMovido.cpp/.h` - Detecção e reconciliação de movimentações.
- `ob_09_GerenciadorColunas.cpp/.h` - Colunas disponíveis e persistência de visibilidade.
- `ob_10_GerenciadorTabela.cpp/.h` - Ponte entre eventos, tabela e interface.

#### Subdiretórios Especializados

- `GerenciamentoBaseEvento/` - Criação de banco, PRAGMAs, índices, registros, snapshot, remoções, movimentações e `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Workers de scan, fila de processamento, lotes, progresso e finalização.
- `GerenciamentoEventoModificado/` - Intervalos de deduplicação, cache, arquivos grandes e processamento normal/massivo.
- `GerenciamentoEventoMovido/` - Worker de movimentação, exclusões pendentes, atualização de linhas e tabela.
- `GerenciamentoMetadados/` - Extração nativa de metadados para código, imagem, áudio, vídeo, documentos, planilhas, apresentações, executáveis, compactados, logs e dados estruturados.
- `GerenciamentoObservador/` - Início, parada, monitoramento, buffer, detecção de operação massiva e pausa.
- `GerenciamentoTabela/` - Tema, cabeçalhos, larguras, cores, seleção, retradução e `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menus, Diálogos, Ações e Localização</b></summary>

#### Módulos Principais

- `ui_01_GerenciadorBotoes.cpp/.h` - Fachada de botões e ações rápidas.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Construção visual dos botões.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Menus principais e ações persistentes.
- `ui_04_GerenciadorEventosUI.cpp/.h` - Eventos de UI.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Feedback visual de operações longas.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Janela e fluxo de estatísticas.
- `ui_07_GerenciadorDados.cpp/.h` - Exportação de dados.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Integração entre eventos e UI.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Controle de monitoramento.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Limpeza de dados.
- `ui_11_DialogoCores.cpp/.h` - Seleção e persistência de cores.
- `ui_12_LocalizadorQt.cpp/.h` - Localização e preferência de idioma.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Tradução de metadados.
- `ui_14_OpcoesSobre.cpp/.h` - Conteúdo de sobre, licença, avisos, privacidade e release notes.
- `ui_15_Manual.cpp/.h` - Manual embarcado.

#### Subdiretórios

- `GerenciamentoBotoes/` - Seleção de diretório, alternância de análise, exportação, SQLite, limpeza e pausa.
- `GerenciamentoDialogoCores/` - UI de seleção de cores, grade, seletor avançado e traduções.
- `GerenciamentoEstatisticasUI/` - Painel, checkboxes, workers, regeneração, abas, layout e salvamento de gráficos.
- `GerenciamentoEventosUI/` - Filtros, colunas, cores, idioma, limpeza e reinício.
- `GerenciamentoLocalizadorQt/` - Carregamento de tradutores, fallback, arquivos `.ts/.qm` e tradução de operações.
- `GerenciamentoMenusUI/` - Menus, submenus, exportação, idiomas, sobre, manual e ajuda.
- `GerenciamentoTradutorMetadadosQt/` - Traduções de tipos de arquivo, atributos, autor, proteção e dimensões.
- `Manual/` - Documentação interna em seis idiomas.

</details>

<details>
<summary><b>📂 source/data/ - Estatísticas e Gráficos</b></summary>

#### Módulos Principais

- `st_01_GeradorEstatisticas.cpp/.h` - Fachada para geração de estatísticas.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Cálculo geométrico de layout.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Fábrica de widgets de gráfico.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Adaptação de payloads legados.
- `Charts/ch_04_QmlRegistration.cpp/.h` - Registro QML.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Item Qt Quick.
- `Charts/ch_06_WidgetChartView.cpp/.h` - Visualização via `QOpenGLWidget`.
- `Charts/ch_07_LgplChartModel.h` - Modelo tipado de gráfico.
- `Charts/ch_08_RasterChartView.cpp/.h` - Renderização QWidget com `QPainter`.

#### 17 Geradores de Gráficos Disponíveis

- `gst_02_GraficoPizza`
- `gst_03_GraficoBarras`
- `gst_04_GraficoTimeline`
- `gst_05_GraficoTreemap`
- `gst_06_GraficoHistograma`
- `gst_07_GraficoPareto`
- `gst_08_GraficoLinha`
- `gst_09_GraficoBoxplot`
- `gst_10_GraficoRadarEventos`
- `gst_11_GraficoHeatmap`
- `gst_12_GraficoScatter`
- `gst_13_GraficoSankey`
- `gst_14_GraficoRadar`
- `gst_15_GraficoDotplot`
- `gst_16_GraficoSankeyEventoCaminho`
- `gst_17_GraficoSankeyTipoCaminho`
- `gst_18_GraficoArvoreDiretorios`

</details>

<details>
<summary><b>📂 source/tools/ - Filtros e Administração</b></summary>

#### Módulos de Filtros

- `fil_01_Filtros.cpp/.h` - Widget e lógica principal de filtros.
- `fil_02_AdministradorCalendario.cpp/.h` - Seleção de datas.
- `fil_03_AdministradorFiltros.cpp/.h` - Aplicação combinada de filtros.

#### Subdiretórios

- `GerenciamentoAdministradorFiltros/` - Aplicação, limpeza, sincronização com menu, contagens, idioma e persistência.
- `GerenciamentoFiltros/` - UI, alteração de filtro, sincronização, verificação de operação e limpeza.

</details>

<details>
<summary><b>📂 source/security/ - Segurança, Evidência e Quarentena</b></summary>

#### Módulos Principais

- `secore_02_SecurityManager.cpp/.h` - Orquestração da camada de segurança.
- `secore_01_postgres_utils.cpp/.h` - Backend Qt SQL, PostgreSQL configurável e cache local SQLite.
- `secore_05_SecurityDbWriter.cpp/.h` - Writer assíncrono de segurança.
- `base_01_baseline_criptografico.cpp/.h` - Baseline criptográfico.
- `exec_01_inteligencia_executaveis.cpp/.h` - Inteligência para executáveis.
- `sess_01_correlacao_sessoes.cpp/.h` - Correlação de sessões.
- `sdiff_01_diff_conteudo.cpp/.h` - Diferenças de conteúdo.
- `attr_01_atribuicao_forense.cpp/.h` - Atribuição forense.
- `agre_01_motor_regras.cpp/.h` - Motor de regras.
- `risk_01_score_risco.cpp/.h` - Score de risco.
- `quar_01_quarentena.cpp/.h` - Quarentena e restauração.
- `evid_01_exportacao_evidencia.cpp/.h` - Pacotes de evidência.
- `secui_*` - Diálogos e controladores de UI para segurança.

#### Recursos

- Perfis e multi-diretório.
- Central de alertas.
- Sessões em tempo real.
- Quarentena e restauração.
- Pacote de evidência.
- Notificações e configuração de pop-ups.
- Armazenamento local em `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ e source/locale/ - Conteúdo Embarcado</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` e `RELEASE`.
- Ícones `.ico` e imagens `.png`.
- Textos de sobre, licença, avisos, política de privacidade, histórico e notas de lançamento em múltiplos idiomas.

#### Arquivos de Tradução

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Diretórios Auxiliares</b></summary>

- `mocks/` - Utilitários auxiliares, incluindo `find_VersionEditor` e `compile_translations`.
- `repo/` - Scripts e arquivos de empacotamento, incluindo instaladores Inno Setup.
- `.vscode/` - Configuração local de IDE.
- `.github/workflows/` - Automação de atualização documental.
- `CMakePresets.json` - Presets principais MinGW e MSVC.
- `mocks/CMakePresets.json` - Presets dos utilitários auxiliares.

</details>

---

### 🏗️ Arquitetura e Padrões de Design

#### Princípios SOLID Aplicados

- Responsabilidade única por módulos numerados e subcamadas especializadas.
- Fachadas para subsistemas complexos, como estatísticas, botões, menus, eventos e segurança.
- Separação entre observação, persistência, metadados, visualização, exportação e tradução.
- Baixo acoplamento por uso de `QObject` properties, sinais/slots e interfaces de serviço.
- Extensão por novos módulos `.cpp/.h` sem reescrever o núcleo.

#### Padrões de Design Implementados

- **Facade:** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer/Event loop:** sinais/slots Qt, callbacks de eventos e notificações de monitoramento.
- **Worker/Producer-Consumer:** filas de eventos, `DatabaseWriter`, workers de gráficos e scanner.
- **Model/View:** `EventTableModel` com `QTableView`, mantendo compatibilidade com `QTableWidget`.
- **Factory:** criação de widgets de gráficos por `ChartWidgetFactory`.
- **Adapter:** `LegacyVariantChartAdapter` converte payloads antigos para modelos atuais.
- **Repository/DAO local:** acesso SQLite por conexões Qt SQL com nomes únicos.
- **Strategy informal:** geradores separados para cada tipo de gráfico e extratores separados por tipo de arquivo.

#### Multithreading e Performance

- Varredura inicial em worker Qt.
- Monitoramento contínuo em `std::thread`.
- Processamento de buffer de eventos em thread dedicada.
- Escrita de eventos com `DatabaseWriter`, fila interna e persistência em lote.
- Extração de metadados em lote com `QThreadPool`.
- Geração de gráficos com `GraficoWorker`.
- Cache de rótulos, brushes e traduções em `EventTableModel`.
- Atualização visual assíncrona para reduzir bloqueios da UI.

#### Otimizações de Banco de Dados

- SQLite em modo WAL.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` e limite de journal no writer.
- Tabelas por tipo de evento e tabela consolidada `monitoramento`.
- Tabela `snapshot` para baseline.
- Índices por nome, diretórios, timestamp e chaves frequentes.
- Inserção em lote com `execBatch()` e transações.

#### Arquitetura de Eventos

1. `Observador` detecta alterações no diretório.
2. Eventos brutos são classificados como adicionado, excluído, modificado, renomeado, movido ou escaneado.
3. Submódulos especializados deduplicam ruído e conciliam operações complexas.
4. Metadados são extraídos quando aplicável.
5. Eventos são enviados para a UI e para o writer.
6. O banco local permite recarregar, filtrar, exportar e gerar estatísticas.

#### Extração de Metadados

- `QImageReader` para imagens.
- Inspeção de ZIP/OpenXML para documentos, planilhas e apresentações.
- `QSqlDatabase` para SQLite e bancos suportados pelo Qt SQL.
- Leitura de headers para identificação de formatos.
- APIs Windows para executáveis, assinatura e versão.
- Cálculos de tamanho em B, KB, MB, GB e TB.
- Metadados especializados para logs, backups, compactados, OLE, dados estruturados e código-fonte.

#### Segurança

A camada `source/security` amplia o monitoramento com recursos de resposta e investigação:

- Perfis de diretórios e multi-monitoramento.
- Alertas derivados de regras.
- Correlação de sessões.
- Baseline criptográfico.
- Quarentena e restauração de itens.
- Pacotes de evidência.
- Score de risco.
- Backend configurável via Qt SQL, com cache local SQLite quando necessário.

---

### 🔄 Fluxo de Execução Principal

1. `main.cpp` chama `source::iniciar_aplicacao(argc, argv)`.
2. `src_01_InicializadorMain` ajusta diretório corrente, identidade Windows e fallback de inicialização.
3. `src_02_SplashAppStarter` cria `QApplication`, carrega ícone, traduções e splash.
4. `InterfaceMonitor` monta a janela principal e registra propriedades compartilhadas.
5. Gerenciadores de botões, menus, eventos, progresso, dados e estatísticas são criados.
6. A camada de localização define idioma e tradutores.
7. A camada de segurança inicia seu manager e writer.
8. O usuário seleciona um diretório.
9. `Observador` e `DiretorioScanner` executam a varredura inicial.
10. O snapshot e eventos `escaneado` são persistidos.
11. O monitoramento contínuo usa `CreateFileW` e `ReadDirectoryChangesW`.
12. Eventos são classificados, enriquecidos e enfileirados.
13. `DatabaseWriter` persiste em lote.
14. Tabela, filtros, contadores e cores são atualizados.
15. Estatísticas, exportações e diálogos consultam o estado atual.
16. No encerramento, threads e conexões são finalizadas de forma ordenada.

---

### 📦 Requisitos e Dependências

#### Bibliotecas Principais

- Qt6 Core.
- Qt6 Gui.
- Qt6 Widgets.
- Qt6 Sql.
- Qt6 Xml.
- Qt6 Concurrent.
- Qt6 OpenGL.
- Qt6 OpenGLWidgets.
- Qt6 Quick.
- Qt6 Qml.
- Bibliotecas Windows: `wintrust`, `version`, `crypt32`.

#### Requisitos de Build

- CMake 4.1 ou superior para o build principal.
- CMake 3.24 ou superior para os utilitários em `mocks/`.
- Qt 6.11.0 nos presets atuais.
- MinGW 13.1.0 ou Visual Studio 2022 Build Tools, conforme preset.
- Windows SDK compatível com a toolchain escolhida.

#### Presets CMake

Configurar e compilar com MinGW:

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Configurar e compilar com MSVC Debug:

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Configurar e compilar com MSVC Release:

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Compilar utilitário de edição de versões:

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Deploy

No Windows, o `CMakeLists.txt` tenta localizar `windeployqt` e copia:

- executável final com nome `Linceu_Lighthouse`;
- dependências Qt;
- `source/assets`;
- arquivos `.qm` da aplicação;
- traduções Qt para `locale/qt`.

---

### ✨ Funcionalidades Principais

<details>
<summary><b>Monitoramento de Arquivos</b></summary>

- Varredura inicial recursiva.
- Snapshot persistente.
- Monitoramento em tempo real por API nativa do Windows.
- Eventos: adicionado, excluído, modificado, renomeado, movido e escaneado.
- Deduplicação e reconciliação de eventos complexos.
- Detecção de operações massivas.
- Pausa e parada de scan/monitoramento.

</details>

<details>
<summary><b>Banco e Persistência</b></summary>

- `monitoramento.db` em `%LOCALAPPDATA%\LinceuLighthouse`.
- Tabelas por tipo de evento.
- Tabela consolidada para consulta e exportação.
- Índices otimizados.
- Escrita assíncrona em lote.
- Configurações locais em JSON e INI.
- Logs timestampados.

</details>

<details>
<summary><b>Metadados</b></summary>

- Tipo de arquivo.
- Tamanho em múltiplas unidades.
- Atributos.
- Autor.
- Dimensões.
- Duração.
- Taxa de bits.
- Proteção.
- Páginas, linhas, palavras, slides, planilhas, tabelas e registros quando aplicável.
- Assinatura e versão de executáveis no Windows.

</details>

<details>
<summary><b>Visualização e Tabela</b></summary>

- `QTableView` com `EventTableModel`.
- Compatibilidade transitória com `QTableWidget`.
- Ordenação.
- Visibilidade de colunas.
- Cores por operação e coluna.
- Tema claro/escuro do Windows.
- Retradução de dados existentes.
- Atualização incremental de eventos.

</details>

<details>
<summary><b>Estatísticas e Gráficos</b></summary>

- 17 geradores especializados.
- Renderização raster com `QPainter`.
- Backend OpenGL por `QOpenGLWidget`.
- Integração Qt Quick/QML.
- Painel de seleção.
- Regeneração após mudança de idioma.
- Salvamento de gráficos.

</details>

<details>
<summary><b>Filtros Avançados</b></summary>

- Operação.
- Extensão.
- Texto.
- Intervalo de data.
- Combinação de filtros.
- Sincronização com menus.
- Contagem atualizada.
- Persistência de estado.

</details>

<details>
<summary><b>Exportação de Dados</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Texto `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Exportação de seleção, colunas ativas e filtros ativos.
- Suporte a dados vindos de model/view.

</details>

<details>
<summary><b>Internacionalização</b></summary>

- Seis idiomas.
- Tradução de interface, operações e metadados.
- Fallback textual.
- Preferência persistida.
- Manual e conteúdo institucional embarcado.
- Diálogo Sobre reativo à troca de idioma.

</details>

<details>
<summary><b>Segurança</b></summary>

- Multi-diretório e perfis.
- Central de alertas.
- Sessões em tempo real.
- Quarentena.
- Restauração.
- Pacotes de evidência.
- Notificações.
- Score de risco e regras.

</details>

---

### 🎯 Boas Práticas Implementadas

#### Código Limpo

- Organização por camadas e módulos numerados.
- Classes e funções com responsabilidade delimitada.
- Uso consistente de namespaces.
- Cabeçalhos `.h` separados de implementações `.cpp`.
- Fallbacks para falhas de UI, logging e inicialização.
- Comentários pontuais em áreas complexas.

#### Performance

- Monitoramento nativo em vez de polling.
- Escrita SQLite em lote.
- Índices em campos consultados com frequência.
- Workers e threads para tarefas pesadas.
- Caches de tradução, rótulos e cores.
- Atualização incremental da tabela.
- Evita dependências pesadas de análise em runtime.

#### Manutenibilidade

- Camadas bem separadas.
- Módulos pequenos para fluxos específicos.
- Build declarativo em CMake.
- Presets para toolchains.
- Assets e traduções versionados.
- Utilitários isolados em `mocks/`.
- Documentos técnicos no repositório.

#### Segurança e Robustez

- Uso de diretório persistente local do usuário.
- Conexões SQLite com nomes únicos.
- Encerramento ordenado de threads.
- Logs de erro e diagnóstico.
- Tratamento defensivo com `try/catch` nas bordas da UI.
- Validação de ponteiros antes de uso em integrações Qt.

---

### 🚀 Para Adicionar Novos Recursos

- **Novo tipo de gráfico:** crie um novo gerador em `source/data/GeradorEstatisticas/`, seguindo `gst_01_BaseGerador` e registrando a opção na UI de estatísticas.
- **Novo tipo de evento:** adicione classe ou função especializada em `source/services`, atualize colunas se necessário e registre persistência no fluxo de `BaseEvento`.
- **Nova coluna:** atualize `ob_09_GerenciadorColunas`, o schema em `gbank_02_criar_banco_de_dados`, exportação e traduções.
- **Novo metadado:** implemente extrator em `GerenciamentoMetadados`, adicione coluna, tradução e lógica de exibição.
- **Novo idioma:** crie `linceu_XX.ts`, compile para `.qm`, adicione fallback no `LocalizadorQt` e traduza assets quando aplicável.
- **Novo filtro:** implemente em `source/tools/GerenciamentoAdministradorFiltros` e sincronize com menu e contagem.
- **Nova ação de segurança:** adicione serviço em `source/security`, diálogo em `secui_*`, strings traduzidas e persistência no backend.
- **Nova exportação:** estenda `geb_03_exportar_dados.cpp` ou adicione módulo dedicado, mantendo suporte a `QTableView` e `QTableWidget`.

---

### 📝 Notas Importantes

#### Versões e Toolchain

- O build principal exige C++17.
- Os presets atuais apontam para Qt 6.11.0.
- O preset MinGW usa `C:/Qt/Tools/mingw1310_64`.
- O preset MSVC usa Visual Studio 2022 Build Tools.
- O projeto é orientado a Windows por depender de APIs nativas de monitoramento e metadados.

#### Arquivos Críticos

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Banco principal de eventos.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Cache local da camada de segurança quando usado.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Preferência de idioma.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Visibilidade de colunas.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Colunas coloridas.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Cores configuradas.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Preferências da UI de segurança.

#### Logs

Logs são gravados com timestamp em:

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Utilitários Auxiliares

- `mocks/find_VersionEditor.cpp` atualiza versões e datas em assets, `README.md`, `OBSERVACAO.md` e trechos do diálogo Sobre.
- `mocks/compile_translations.cpp` auxilia a compilação/validação de traduções.
- Os utilitários usam CMake próprio em `mocks/`.

---

### 📄 Licença e Autoria

**Autores:** Fernando Nillsson Cidade  
**Versão:** v2026.4.27.0<br>
**Licença:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Para mais informações, consulte também `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, os assets embarcados e a documentação específica nos módulos do código-fonte.

---

</details>

---

<h2 id="enus">🇺🇸 English (US)</h2>

> **Note:** This repository refers to the **v2026.4.27.0** version of the Linceu Lighthouse Project. Support the project and purchase the paid version through the link: [Install via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Click to expand the README in English</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-26%20235704.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002013.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002037.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002109.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002222.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002242.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002305.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002332.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20002353.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-04-27%20015939.png" alt="Linceu Lighthouse - English" width="320">
</p>

Project: LINCEU LIGHTHOUSE  
Version: v2026.4.27.0<br>
Authors: Fernando Nillsson Cidade  

Description: Visibility and control over your files.

**Linceu Lighthouse** is a native desktop application written in **C++17 with Qt6** for File Integrity Monitoring. It records, persists, enriches and presents file creation, modification, deletion, renaming, moving and initial scan events for directories selected by the user.

The current implementation no longer uses the legacy Python codebase. The main runtime is compiled with CMake, Qt Widgets, Qt SQL, Qt XML, Qt Concurrent, Qt OpenGL/OpenGLWidgets, Qt Quick/QML, native Windows APIs and an in-project charting layer.

Main use cases:

- Auditing shared folders, project directories and local repositories.
- Chronological event logging with local SQLite persistence.
- Investigating deletion, renaming or moving of important files.
- Analyzing activity patterns with filters, statistics and charts.
- Exporting reports and evidence to tabular, structured and database formats.
- Supporting security workflows, quarantine, real-time sessions and evidence packages.

How Linceu Lighthouse works:

1. When a directory is selected, the application builds a recursive baseline.
2. Continuous monitoring then observes file changes with native Windows APIs.
3. Raw events are normalized into operation types and enriched with metadata.
4. The persistence layer writes events to SQLite asynchronously in batches.
5. The Qt interface displays, translates, filters, colors, sorts, exports and summarizes events.

Available tools:

- Quick and advanced filters by operation, text, extension, date and selection.
- Configurable table columns, operation colors and progressive model/view support.
- Native Qt statistics charts.
- Export to `.xlsx`, `.csv`, `.txt`, `.json`, `.xml` and `.db`.
- Security dialogs for multi-directory profiles, alerts, sessions, quarantine, recovery and evidence.
- Internationalization for `pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` and `de_DE`.

---

QUICK USE MANUAL:

1. Run the application to open the graphical interface.
2. Select a directory to monitor.
3. Start monitoring to run the initial scan and track changes.
4. Use menus and side buttons to filter, export, view statistics and inspect security data.
5. Save reports or exported databases when you need to archive results.

DETAILED USER MANUAL:

System requirements:

- Windows 10 or higher.
- Read permission for the monitored directory.
- Write permission in `%LOCALAPPDATA%\LinceuLighthouse` for databases, settings and logs.

Installation:

- Install the distributed executable or build locally using the CMake flow described below.
- For local builds, use the Qt6 preset that matches your toolchain: MinGW or MSVC.

Initial setup:

- The interface starts with an empty event table.
- The language is loaded by the localization system and can be changed from the options menu.
- The application creates persistent directories when needed.
- Logs are written to `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Directory monitoring:

1. Select a directory:
   - Click `Select Directory` or use `File -> Select Directory`.
   - The selected path becomes the active observer target.

2. Start or stop monitoring:
   - Click `Start/Stop` or use the matching menu item.
   - The initial scan populates the snapshot and records `scanned` events.
   - After scanning, continuous monitoring records real-time changes.

3. Pause monitoring or scanning:
   - The project has controls for stopping monitoring, scans and active flows separately.
   - Visual state is updated by the progress and message layers.

Main interface:

- Menu bar: file, settings, options, export, help, security and language.
- Sidebar: frequent actions such as select, start/stop, pause, clear, export and statistics.
- Directory label: monitored path.
- Result label: operational and security feedback.
- Data table: events and metadata.
- Progress bar: scans and long-running tasks.
- Auxiliary dialogs: filters, colors, columns, statistics, manual, help and about.

Important event fields:

- Operation type.
- Name.
- Previous directory.
- Current directory.
- Creation, modification and access dates.
- Type/extension.
- Sizes in multiple units.
- Attributes, author, dimensions, duration, bit rate and protection.
- Specific metadata for documents, spreadsheets, presentations, databases, archives and executables.

Filters and search:

- Quick filters by operation type.
- Advanced filters by operation, extension, text, date range and combined criteria.
- Main menu synchronization.
- Count updates after filter or language changes.
- Checkbox state persistence.

Statistics visualization:

- Open from the statistics button or menu.
- Charts are generated by C++ modules in `source/data/GeradorEstatisticas`.
- The window supports chart selection, regeneration, saving and language-aware updates.

Available charts:

- Pie.
- Bar.
- Timeline.
- Treemap.
- Histogram.
- Pareto.
- Line.
- Boxplot.
- Event radar.
- Heatmap.
- Scatter.
- Sankey.
- Radar.
- Dotplot.
- Event-path Sankey.
- Type-path Sankey.
- Directory tree.

Data export:

- `Excel (.xlsx)`: OpenXML package written by the C++ code.
- `CSV (.csv)`: comma-delimited.
- `Text (.txt)`: tab-delimited.
- `JSON (.json)`: structured rows and headers.
- `XML (.xml)`: hierarchical output through `QXmlStreamWriter`.
- `SQLite (.db)`: database generated by the export layer.

Export options:

- Active columns only.
- Active filters only.
- Current selection only.
- Compatibility with `QTableWidget` and `QTableView`.

Column and color configuration:

- The column menu enables hiding, showing and restoring fields.
- The color menu supports operation and column coloring.
- Settings persist in local JSON files.
- The table reacts to Windows theme changes and invalidates visual caches when needed.

Internationalization:

- Supported languages: Portuguese, English, Spanish, French, Italian and German.
- `.ts` and `.qm` files live in `source/locale`.
- `LocalizadorQt` loads translations, saves preferences and provides fallback text.
- Operations, metadata, menus, manual, help, about and security texts are translated.

Troubleshooting:

1. Monitoring does not start:
   - Confirm directory permissions.
   - Avoid protected system directories.
   - Check whether the local database can be created.

2. Events do not appear:
   - Confirm monitoring is active.
   - Clear filters.
   - Test by creating, renaming or deleting a simple file in the monitored directory.

3. High resource consumption:
   - Avoid unnecessarily monitoring very large trees.
   - Prefer specific subdirectories.
   - Export or clear old data when the database grows too much.

4. Empty statistics:
   - Confirm that events are loaded.
   - Remove filters that may hide rows.
   - Check whether `monitoramento.db` is accessible.

## 📋 Technical Documentation

### System Overview

**Linceu Lighthouse** is a native Windows desktop application written in **C++17** and built on **Qt6**. The current architecture replaces the legacy Python/PySide stack with `.cpp` and `.h` modules, a CMake build, SQLite persistence through Qt SQL, Qt Widgets/OpenGL/Quick rendering and native Windows API integration.

The application is organized into layers:

- `source/gui`: main window, visual composition and high-level integration.
- `source/services`: directory observation, events, database, metadata and table handling.
- `source/ui`: menus, buttons, statistics, filters, manual, help, about and localization.
- `source/data`: statistics facade, chart generators and chart infrastructure.
- `source/tools`: filters and filter administration.
- `source/security`: security, sessions, quarantine, evidence and storage backend.
- `source/utils`: paths, icons and logging.
- `source/assets` and `source/locale`: embedded content, licenses, texts and translations.

#### Main Technologies

- **Language:** C++17.
- **Build:** CMake with `CMAKE_AUTOMOC` and `CMAKE_AUTORCC`.
- **Framework:** Qt6.
- **UI:** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, menus and actions.
- **Database:** SQLite through `QSqlDatabase` and `QSqlQuery`.
- **Concurrency:** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` and Qt Concurrent.
- **Charts:** internal infrastructure with `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` and typed `LgplChartModel`.
- **Export:** custom CSV, TXT, JSON, XML, SQLite and `.xlsx` OpenXML writers.
- **Internationalization:** Qt Linguist (`.ts` and `.qm`) with six languages.
- **Windows:** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` for theme detection and `wintrust`, `version`, `crypt32`.
- **Security:** in-project C++ layer with cryptographic baseline, rule engine, risk score, quarantine, evidence and local SQLite cache for the security backend.

The main application runtime does not depend on Python, `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` or equivalent analysis libraries. Any Python-related license file is third-party documentation carried as an asset, not the current runtime stack.

---

### 📁 Directory and Module Structure

Validated summary from the `source/` tree:

| Layer | `.cpp` | `.h` | Responsibility |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Application initialization and splash |
| `source/assets` | 0 | 0 | Texts, licenses, images, icons and embedded content |
| `source/data` | 26 | 27 | Statistics, charts and rendering |
| `source/gui` | 8 | 8 | Main window and visual composition |
| `source/locale` | 0 | 0 | `.ts` and `.qm` translation files |
| `source/security` | 24 | 24 | Security, evidence, quarantine and sessions |
| `source/services` | 162 | 163 | Observation, events, database, metadata and table |
| `source/tools` | 23 | 24 | Filters and filter administration |
| `source/ui` | 125 | 125 | Menus, buttons, dialogs, manual, statistics and localization |
| `source/utils` | 4 | 4 | Paths, icons and logs |
| **Total in `source/`** | **374** | **377** | Current C++ codebase |

<details>
<summary><b>📂 source-root - Application Bootstrap</b></summary>

#### Main Modules

- `src_01_InicializadorMain.cpp/.h` - Real entry after `main.cpp`, current directory handling, Windows identity and fallback initialization.
- `src_02_SplashAppStarter.cpp/.h` - Splash startup, progress, icon, translations, `InterfaceMonitor` creation and early writer startup.

</details>

<details>
<summary><b>📂 source/gui/ - Graphical Interface Layer</b></summary>

#### Main Modules

- `ic_01_InterfaceMonitor.cpp/.h` - Main window, shared properties, table container and subsystem integration.
- `ic_02_Inicializador.cpp/.h` - Base widget and object initialization.
- `ic_03_Configurador.cpp/.h` - Layouts, components and visual configuration.
- `ic_04_Atualizador.cpp/.h` - Interface updates for table, status and progress.
- `ic_05_GerenciadorProgresso.cpp/.h` - Progress bars and feedback.
- `ic_06_GerenciadorMensagens.cpp/.h` - Messages, status and user communication.
- `ic_07_ManipuladorTabela.cpp/.h` - Table operations and UI adaptation.
- `ic_08_Internacionalizador.cpp/.h` - Interface integration with localization.

The legacy `GerenciadorDesempenho/` and `GerenciadorEstruturaDiretoriosWidget/` subdirectories are no longer active in this tree and should not be documented as current subsystems.

</details>

<details>
<summary><b>📂 source/services/ - Monitoring, Events and Data Core</b></summary>

#### Main Modules

- `ob_01_Observador.cpp/.h` - Directory observer, threads, buffers and monitoring lifecycle.
- `ob_02_BaseEvento.cpp/.h` - Shared event base and SQLite integration.
- `ob_03_DiretorioScanner.cpp/.h` - Initial scan and snapshot creation.
- `ob_04_EventoAdicionado.cpp/.h` - Created file handling.
- `ob_05_EventoExcluido.cpp/.h` - Deletion and temporary deletion handling.
- `ob_06_EventoModificado.cpp/.h` - Modification consolidation.
- `ob_07_EventoRenomeado.cpp/.h` - Rename handling.
- `ob_08_EventoMovido.cpp/.h` - Move detection and reconciliation.
- `ob_09_GerenciadorColunas.cpp/.h` - Available columns and visibility persistence.
- `ob_10_GerenciadorTabela.cpp/.h` - Event/table/interface bridge.

#### Specialized Subdirectories

- `GerenciamentoBaseEvento/` - Database creation, PRAGMAs, indexes, records, snapshot, removals, moves and `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Scan workers, processing queue, batches, progress and finalization.
- `GerenciamentoEventoModificado/` - Deduplication intervals, cache, large files and normal/massive processing.
- `GerenciamentoEventoMovido/` - Move worker, pending deletions, row updates and table updates.
- `GerenciamentoMetadados/` - Native metadata extraction for source code, images, audio, video, documents, spreadsheets, presentations, executables, archives, logs and structured data.
- `GerenciamentoObservador/` - Start, stop, monitor, buffer, mass-operation detection and pause.
- `GerenciamentoTabela/` - Theme, headers, widths, colors, selection, retranslation and `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menus, Dialogs, Actions and Localization</b></summary>

#### Main Modules

- `ui_01_GerenciadorBotoes.cpp/.h` - Button and quick-action facade.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Button UI construction.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Main menus and persistent actions.
- `ui_04_GerenciadorEventosUI.cpp/.h` - UI events.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Long-operation visual feedback.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Statistics window and flow.
- `ui_07_GerenciadorDados.cpp/.h` - Data export.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Event/UI integration.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Monitoring controls.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Data cleanup.
- `ui_11_DialogoCores.cpp/.h` - Color selection and persistence.
- `ui_12_LocalizadorQt.cpp/.h` - Localization and language preference.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Metadata translation.
- `ui_14_OpcoesSobre.cpp/.h` - About, license, notices, privacy and release notes content.
- `ui_15_Manual.cpp/.h` - Embedded manual.

#### Subdirectories

- `GerenciamentoBotoes/` - Directory selection, analysis toggle, export, SQLite export, cleanup and pause.
- `GerenciamentoDialogoCores/` - Color selector UI, grid, advanced picker and translations.
- `GerenciamentoEstatisticasUI/` - Panel, checkboxes, workers, regeneration, tabs, layout and chart saving.
- `GerenciamentoEventosUI/` - Filters, columns, colors, language, cleanup and restart.
- `GerenciamentoLocalizadorQt/` - Translator loading, fallback, `.ts/.qm` files and operation translation.
- `GerenciamentoMenusUI/` - Menus, submenus, export, languages, about, manual and help.
- `GerenciamentoTradutorMetadadosQt/` - File type, attribute, author, protection and dimension translations.
- `Manual/` - Internal documentation in six languages.

</details>

<details>
<summary><b>📂 source/data/ - Statistics and Charts</b></summary>

#### Main Modules

- `st_01_GeradorEstatisticas.cpp/.h` - Statistics generation facade.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Chart geometry and layout computation.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Chart widget factory.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Legacy payload adapter.
- `Charts/ch_04_QmlRegistration.cpp/.h` - QML registration.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Qt Quick item.
- `Charts/ch_06_WidgetChartView.cpp/.h` - `QOpenGLWidget` view.
- `Charts/ch_07_LgplChartModel.h` - Typed chart model.
- `Charts/ch_08_RasterChartView.cpp/.h` - `QPainter` QWidget renderer.

#### 17 Available Chart Generators

- `gst_02_GraficoPizza`
- `gst_03_GraficoBarras`
- `gst_04_GraficoTimeline`
- `gst_05_GraficoTreemap`
- `gst_06_GraficoHistograma`
- `gst_07_GraficoPareto`
- `gst_08_GraficoLinha`
- `gst_09_GraficoBoxplot`
- `gst_10_GraficoRadarEventos`
- `gst_11_GraficoHeatmap`
- `gst_12_GraficoScatter`
- `gst_13_GraficoSankey`
- `gst_14_GraficoRadar`
- `gst_15_GraficoDotplot`
- `gst_16_GraficoSankeyEventoCaminho`
- `gst_17_GraficoSankeyTipoCaminho`
- `gst_18_GraficoArvoreDiretorios`

</details>

<details>
<summary><b>📂 source/tools/ - Filters and Administration</b></summary>

#### Filter Modules

- `fil_01_Filtros.cpp/.h` - Main filter widget and logic.
- `fil_02_AdministradorCalendario.cpp/.h` - Date selection.
- `fil_03_AdministradorFiltros.cpp/.h` - Combined filter application.

#### Subdirectories

- `GerenciamentoAdministradorFiltros/` - Apply, clear, menu synchronization, counts, language and persistence.
- `GerenciamentoFiltros/` - UI, filter changes, synchronization, operation checks and clearing.

</details>

<details>
<summary><b>📂 source/security/ - Security, Evidence and Quarantine</b></summary>

#### Main Modules

- `secore_02_SecurityManager.cpp/.h` - Security layer orchestration.
- `secore_01_postgres_utils.cpp/.h` - Qt SQL backend, configurable PostgreSQL and local SQLite cache.
- `secore_05_SecurityDbWriter.cpp/.h` - Asynchronous security writer.
- `base_01_baseline_criptografico.cpp/.h` - Cryptographic baseline.
- `exec_01_inteligencia_executaveis.cpp/.h` - Executable intelligence.
- `sess_01_correlacao_sessoes.cpp/.h` - Session correlation.
- `sdiff_01_diff_conteudo.cpp/.h` - Content differences.
- `attr_01_atribuicao_forense.cpp/.h` - Forensic attribution.
- `agre_01_motor_regras.cpp/.h` - Rule engine.
- `risk_01_score_risco.cpp/.h` - Risk score.
- `quar_01_quarentena.cpp/.h` - Quarantine and recovery.
- `evid_01_exportacao_evidencia.cpp/.h` - Evidence packages.
- `secui_*` - Security UI dialogs and controllers.

#### Features

- Profiles and multi-directory handling.
- Alert center.
- Real-time sessions.
- Quarantine and recovery.
- Evidence packages.
- Notifications and popup configuration.
- Storage under `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ and source/locale/ - Embedded Content</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` and `RELEASE`.
- `.ico` icons and `.png` images.
- About, license, notices, privacy policy, history and release notes in multiple languages.

#### Translation Files

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Auxiliary Directories</b></summary>

- `mocks/` - Auxiliary utilities, including `find_VersionEditor` and `compile_translations`.
- `repo/` - Packaging scripts and files, including Inno Setup installers.
- `.vscode/` - Local IDE configuration.
- `.github/workflows/` - Documentation update automation.
- `CMakePresets.json` - Main MinGW and MSVC presets.
- `mocks/CMakePresets.json` - Auxiliary utility presets.

</details>

---

### 🏗️ Architecture and Design Patterns

#### Applied SOLID Principles

- Single responsibility through numbered modules and specialized sublayers.
- Facades for complex subsystems such as statistics, buttons, menus, events and security.
- Separation between observation, persistence, metadata, visualization, export and translation.
- Low coupling through `QObject` properties, signals/slots and service interfaces.
- Extension through new `.cpp/.h` modules without rewriting the core.

#### Implemented Design Patterns

- **Facade:** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer/Event loop:** Qt signals/slots, event callbacks and monitoring notifications.
- **Worker/Producer-Consumer:** event queues, `DatabaseWriter`, chart workers and scanner workers.
- **Model/View:** `EventTableModel` with `QTableView`, while preserving `QTableWidget` compatibility.
- **Factory:** chart widget creation through `ChartWidgetFactory`.
- **Adapter:** `LegacyVariantChartAdapter` converts old payloads into current models.
- **Local Repository/DAO:** SQLite access through Qt SQL connections with unique names.
- **Informal Strategy:** separate chart generators and metadata extractors by domain.

#### Multithreading and Performance

- Initial scan in Qt worker flow.
- Continuous monitoring in `std::thread`.
- Event buffer processing in a dedicated thread.
- Event writes through `DatabaseWriter`, internal queue and batch persistence.
- Batch metadata extraction with `QThreadPool`.
- Chart generation with `GraficoWorker`.
- Label, brush and translation caches in `EventTableModel`.
- Asynchronous visual updates to reduce UI blocking.

#### Database Optimizations

- SQLite WAL mode.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` and journal size limit in the writer.
- Tables per event type and consolidated `monitoramento` table.
- `snapshot` table for baseline.
- Indexes by name, directories, timestamp and frequent keys.
- Batch insert with `execBatch()` and transactions.

#### Event Architecture

1. `Observador` detects directory changes.
2. Raw events are classified as added, deleted, modified, renamed, moved or scanned.
3. Specialized submodules deduplicate noise and reconcile complex operations.
4. Metadata is extracted when applicable.
5. Events are sent to the UI and writer.
6. The local database supports reload, filtering, export and statistics.

#### Metadata Extraction

- `QImageReader` for images.
- ZIP/OpenXML inspection for documents, spreadsheets and presentations.
- `QSqlDatabase` for SQLite and databases supported by Qt SQL.
- Header reading for format identification.
- Windows APIs for executables, signatures and versions.
- Size calculations in B, KB, MB, GB and TB.
- Specialized metadata for logs, backups, archives, OLE, structured data and source code.

#### Security

The `source/security` layer extends monitoring with response and investigation features:

- Directory profiles and multi-monitoring.
- Rule-derived alerts.
- Session correlation.
- Cryptographic baseline.
- Item quarantine and recovery.
- Evidence packages.
- Risk scoring.
- Configurable Qt SQL backend with local SQLite cache when needed.

---

### 🔄 Main Execution Flow

1. `main.cpp` calls `source::iniciar_aplicacao(argc, argv)`.
2. `src_01_InicializadorMain` handles current directory, Windows identity and fallback initialization.
3. `src_02_SplashAppStarter` creates `QApplication`, icon, translations and splash.
4. `InterfaceMonitor` builds the main window and registers shared properties.
5. Button, menu, event, progress, data and statistics managers are created.
6. The localization layer sets language and translators.
7. The security layer starts its manager and writer.
8. The user selects a directory.
9. `Observador` and `DiretorioScanner` run the initial scan.
10. The snapshot and `scanned` events are persisted.
11. Continuous monitoring uses `CreateFileW` and `ReadDirectoryChangesW`.
12. Events are classified, enriched and queued.
13. `DatabaseWriter` persists batches.
14. Table, filters, counters and colors are updated.
15. Statistics, exports and dialogs query the current state.
16. On shutdown, threads and connections are finalized in an orderly way.

---

### 📦 Requirements and Dependencies

#### Main Libraries

- Qt6 Core.
- Qt6 Gui.
- Qt6 Widgets.
- Qt6 Sql.
- Qt6 Xml.
- Qt6 Concurrent.
- Qt6 OpenGL.
- Qt6 OpenGLWidgets.
- Qt6 Quick.
- Qt6 Qml.
- Windows libraries: `wintrust`, `version`, `crypt32`.

#### Build Requirements

- CMake 4.1 or higher for the main build.
- CMake 3.24 or higher for utilities in `mocks/`.
- Qt 6.11.0 in the current presets.
- MinGW 13.1.0 or Visual Studio 2022 Build Tools, depending on preset.
- Windows SDK compatible with the selected toolchain.

#### CMake Presets

Configure and build with MinGW:

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Configure and build with MSVC Debug:

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Configure and build with MSVC Release:

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Build the version editor utility:

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Deployment

On Windows, `CMakeLists.txt` tries to locate `windeployqt` and copies:

- final executable named `Linceu_Lighthouse`;
- Qt runtime dependencies;
- `source/assets`;
- application `.qm` files;
- Qt translations into `locale/qt`.

---

### ✨ Main Features

<details>
<summary><b>File Monitoring</b></summary>

- Recursive initial scan.
- Persistent snapshot.
- Real-time monitoring through native Windows API.
- Events: added, deleted, modified, renamed, moved and scanned.
- Deduplication and complex event reconciliation.
- Mass-operation detection.
- Scan and monitoring pause/stop.

</details>

<details>
<summary><b>Database and Persistence</b></summary>

- `monitoramento.db` in `%LOCALAPPDATA%\LinceuLighthouse`.
- Tables by event type.
- Consolidated table for query and export.
- Optimized indexes.
- Asynchronous batch writing.
- Local JSON and INI settings.
- Timestamped logs.

</details>

<details>
<summary><b>Metadata</b></summary>

- File type.
- Size in multiple units.
- Attributes.
- Author.
- Dimensions.
- Duration.
- Bit rate.
- Protection.
- Pages, lines, words, slides, spreadsheets, tables and records when applicable.
- Executable signature and version on Windows.

</details>

<details>
<summary><b>Visualization and Table</b></summary>

- `QTableView` with `EventTableModel`.
- Transitional compatibility with `QTableWidget`.
- Sorting.
- Column visibility.
- Operation and column colors.
- Windows light/dark theme.
- Existing data retranslation.
- Incremental event updates.

</details>

<details>
<summary><b>Statistics and Charts</b></summary>

- 17 specialized generators.
- Raster rendering with `QPainter`.
- OpenGL backend through `QOpenGLWidget`.
- Qt Quick/QML integration.
- Selection panel.
- Regeneration after language changes.
- Chart saving.

</details>

<details>
<summary><b>Advanced Filters</b></summary>

- Operation.
- Extension.
- Text.
- Date range.
- Filter combination.
- Menu synchronization.
- Updated counts.
- State persistence.

</details>

<details>
<summary><b>Data Export</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Text `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Selection, active-column and active-filter export.
- Model/view data support.

</details>

<details>
<summary><b>Internationalization</b></summary>

- Six languages.
- Interface, operation and metadata translation.
- Text fallback.
- Persisted preference.
- Embedded manual and institutional content.
- Language-aware About dialog.

</details>

<details>
<summary><b>Security</b></summary>

- Multi-directory profiles.
- Alert center.
- Real-time sessions.
- Quarantine.
- Recovery.
- Evidence packages.
- Notifications.
- Risk score and rules.

</details>

---

### 🎯 Implemented Best Practices

#### Clean Code

- Layered organization and numbered modules.
- Classes and functions with bounded responsibilities.
- Consistent namespace usage.
- `.h` headers separated from `.cpp` implementations.
- Fallbacks for UI, logging and startup failures.
- Focused comments in complex areas.

#### Performance

- Native monitoring instead of polling.
- Batch SQLite writes.
- Indexes for frequently queried fields.
- Workers and threads for heavy tasks.
- Translation, label and color caches.
- Incremental table updates.
- No heavy analysis-library dependency at runtime.

#### Maintainability

- Clearly separated layers.
- Small modules for specific flows.
- Declarative CMake build.
- Toolchain presets.
- Versioned assets and translations.
- Utilities isolated under `mocks/`.
- Technical documents in the repository.

#### Security and Robustness

- User-local persistent directory.
- SQLite connections with unique names.
- Ordered thread shutdown.
- Error and diagnostic logs.
- Defensive `try/catch` at UI boundaries.
- Pointer validation before Qt integrations.

---

### 🚀 To Add New Features

- **New chart type:** create a generator in `source/data/GeradorEstatisticas/`, follow `gst_01_BaseGerador` and register it in the statistics UI.
- **New event type:** add a specialized class or function in `source/services`, update columns if needed and register persistence in the `BaseEvento` flow.
- **New column:** update `ob_09_GerenciadorColunas`, the schema in `gbank_02_criar_banco_de_dados`, export logic and translations.
- **New metadata:** implement an extractor in `GerenciamentoMetadados`, add the column, translation and display logic.
- **New language:** create `linceu_XX.ts`, compile to `.qm`, add fallback in `LocalizadorQt` and translate assets when applicable.
- **New filter:** implement it in `source/tools/GerenciamentoAdministradorFiltros` and synchronize with menu and counts.
- **New security action:** add a service under `source/security`, a dialog in `secui_*`, translated strings and backend persistence.
- **New export format:** extend `geb_03_exportar_dados.cpp` or add a dedicated module while keeping `QTableView` and `QTableWidget` support.

---

### 📝 Important Notes

#### Versions and Toolchain

- The main build requires C++17.
- Current presets point to Qt 6.11.0.
- The MinGW preset uses `C:/Qt/Tools/mingw1310_64`.
- The MSVC preset uses Visual Studio 2022 Build Tools.
- The project is Windows-oriented because it depends on native monitoring and metadata APIs.

#### Critical Files

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Main event database.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Local security cache when used.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Language preference.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Column visibility.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Colored columns.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Configured colors.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Security UI preferences.

#### Logs

Logs are written with timestamps to:

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Auxiliary Utilities

- `mocks/find_VersionEditor.cpp` updates versions and dates in assets, `README.md`, `OBSERVACAO.md` and About dialog snippets.
- `mocks/compile_translations.cpp` helps compile and validate translations.
- The utilities use their own CMake setup under `mocks/`.

---

### 📄 License and Authorship

**Authors:** Fernando Nillsson Cidade  
**Version:** v2026.4.27.0<br>
**License:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

For more information, also consult `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, embedded assets and module-specific source documentation.

---

</details>
