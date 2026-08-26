<!-- Multilanguage README.md for Linceu_Lighthouse -->

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

> **Observação:** Este repositório refere-se à versão **v2026.8.25.0** do Projeto Linceu Lighthouse. Apoie o projeto e adquira a versão paga através do link: [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Clique para expandir o README em português</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123724.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123745.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123803.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123821.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123839.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123859.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123916.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123933.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20123952.png" alt="Linceu Lighthouse - Português" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/pt_BR/Captura%20de%20tela%202026-08-25%20124030.png" alt="Linceu Lighthouse - Português" width="320">
</p>

Projeto: LINCEU LIGHTHOUSE  
Versão: v2026.8.25.0<br>
Autor: Fernando Nillsson Cidade<br>

Descrição: Plataforma Local de Inteligência, Histórico e Auditoria de Atividades de Arquivos
> **"Saiba exatamente o que aconteceu com seus arquivos — localmente, visualmente e com histórico pesquisável."**

O **Linceu Lighthouse** é uma aplicação desktop nativa em **C++17 com Qt6** desenvolvida para fornecer visibilidade absoluta, rastreamento histórico e auditoria determinística sobre o sistema de arquivos do Windows. Em vez de atuar como um mero monitor efêmero ou uma ferramenta restrita de segurança, o Linceu Lighthouse unifica três pilares fundamentais em uma experiência ágil, intuitiva e de alta performance:

1. **Monitoramento Contínuo e Semântico de Atividades:** Acompanhamento em tempo real de eventos (criação, modificação, exclusão, renomeação, movimentação e varredura inicial), com algoritmos específicos para correlação de eventos complexos — identificando operações reais de `RENAMED` e `MOVED` sem gerar pares falsos de exclusão e criação.
2. **Histórico Pesquisável e Inteligência de Arquivos sem IA:** Construção automática de um banco de dados relacional SQLite local, enriquecido com mais de 30 categorias de metadados antes e depois de cada modificação, acompanhado por 17 visualizações gráficas nativas e relatórios estatísticos determinísticos.
3. **Segurança Local, Auditoria e Evidências:** Linha de base criptográfica (baseline), cálculo determinístico de score de risco, motor de regras locais, quarentena, restauração e exportação de relatórios auditáveis em múltiplos formatos de mercado.

### 🛡️ Privacidade Soberana (Local-First)
* **100% Local e Offline:** Seus arquivos, metadados, históricos e relatórios residem exclusivamente na sua máquina (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Zero Nuvem, Zero Telemetria, Zero Contas:** Nenhuma informação pessoal ou operacional é transmitida para a rede.
* **Inteligência Determinística sem IA:** Toda a análise, classificação e pontuação de risco utilizam lógica algorítmica e estatística pura — sem modelos de linguagem (LLMs), sem RAG e sem dependências externas.

---

### 🧭 Os 5 Modos de Operação

| Modo | Pergunta / Foco Central | Benefício Principal |
|---|---|---|
| **Modo Monitoramento** | *O que está acontecendo agora?* | Visualização em tempo real de alterações em diretórios selecionados com alertas visuais imediatos por cor. |
| **Modo Histórico** | *O que aconteceu com meu arquivo?* | Consulta cronológica reversa (*File History* / *What Changed?*), mostrando a genealogia completa de edições e renomeações. |
| **Modo Analytics (Sem IA)** | *Quais são os padrões de uso?* | Mapas de calor (Heatmaps), horários de pico, tipos de arquivo mais afetados e fluxos de diretório (Sankey). |
| **Modo Segurança** | *Houve alguma alteração suspeita?* | Verificação contra linha de base criptográfica, hashes, regras de conformidade e integridade estrutural. |
| **Modo Evidência** | *Como comprovar as alterações?* | Exportação de relatórios imutáveis e pacotes de auditoria para conformidade interna ou investigação técnica. |

---

### 🎯 Públicos-Alvo e Casos de Uso

* **Desenvolvedores e Engenheiros de Software:** Rastrear com exatidão o que compilações, IDEs, scripts de build, gerenciadores de pacotes e ferramentas Git criam, modificam ou deletam nos projetos.
* **Administradores de TI, Sysadmins e Suporte:** Diagnosticar arquivos desaparecidos, renomeações acidentais em pastas compartilhadas e reconstruir a linha do tempo de incidentes operacionais.
* **Setores com Dados Sensíveis (Jurídico, Saúde, Finanças, Engenharia):** Manter controle auditável estrito de pastas com dados confidenciais com garantia de sigilo total e isolamento de rede.
* **Criadores de Conteúdo (Designers, Editores de Vídeo, Fotógrafos):** Acompanhar movimentações de bibliotecas pesadas de assets, processos de renderização e fluxos complexos de exportação.
* **Usuários em Geral:** Proteger diretórios essenciais (Documentos, Downloads, unidades de nuvem sincronizadas) contra exclusões acidentais e modificações não autorizadas.

---

### ⚙️ Como o Linceu Lighthouse Funciona

```text
Sistema de Arquivos (Windows)
      ↓
APIs Nativas Assíncronas (ReadDirectoryChangesW)
      ↓
Fila de Eventos Thread-Safe & Buffers de Pico
      ↓
Normalização e Correlação Semântica (Detecção Real de Move/Rename)
      ↓
Enriquecimento com Metadados (30+ categorias, Magic Numbers, Hashes)
      ↓
Persistência Assíncrona em Lote no SQLite Local
      ↓
Visualização Interativa Qt6 | 17 Gráficos Nativos | Exportação Multiformato
```

---

### 🛠️ Recursos e Ferramentas Disponíveis

- **Filtros Multidimensionais:** Filtragem rápida por operação e filtros avançados com suporte a expressões, extensões, intervalos de data e seleção na tabela.
- **Tabela de Auditoria Enriquecida:** Colunas configuráveis, persistência de preferências de cor por operação, identificação MIME real traduzida dinamicamente e metadados temporais completos para pastas e subpastas.
- **17 Visualizações Gráficas Nativas:** Pizza, Barras, Linha do Tempo (Timeline), Mapa de Árvore (Treemap), Histograma, Curva de Pareto, Linhas, Boxplot, Radar, Heatmap Temporal, Scatter, Diagramas de Sankey, Dotplot e Árvore Hierárquica.
- **Exportação Flexível de Evidências:** Geração de relatórios em `.xlsx` (OpenXML nativo), `.csv`, `.txt`, `.json`, `.xml` e banco `.db`.
- **Internacionalização Completa:** Interface e documentação técnica integradas nativamente em 6 idiomas (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` e `de_DE`).

---

MANUAL DE UTILIZAÇÃO RÁPIDO:

1. Execute o aplicativo para abrir a interface gráfica.
2. Selecione um diretório para monitorar.
3. Inicie o monitoramento para executar a varredura inicial e acompanhar alterações.
4. Use os menus e botões laterais para filtrar, exportar, visualizar estatísticas e consultar segurança.
5. Use duplo clique ou a ação Abrir para abrir arquivos reportados na tabela com o aplicativo adequado.
6. Salve relatórios ou bancos exportados quando precisar arquivar resultados.

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
- Datas completas para arquivos, pastas, subpastas e diretórios filhos.
- Tipo/extensão.
- MIME traduzido e identificador MIME original.
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
- Colunas como MIME, Data de Criação, Data de Modificação e Data de Acesso podem ser habilitadas para auditoria mais detalhada.
- O menu de cores permite colorir por operação ou por coluna.
- Configurações persistem em arquivos JSON locais.
- A tabela reage à troca de tema do Windows e invalida caches visuais quando necessário.

Internacionalização:

- Idiomas suportados: português, inglês, espanhol, francês, italiano e alemão.
- Arquivos `.ts` e `.qm` ficam em `source/locale`.
- O `LocalizadorQt` carrega traduções, salva preferência e fornece fallback.
- Operações, metadados, MIME, menus, manual, ajuda, sobre e textos de segurança são traduzidos.

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

5. Arquivo não abre:
   - Confirme se o arquivo ainda existe no caminho exibido.
   - Verifique se o Windows possui um aplicativo associado ao tipo de arquivo.
   - Quando a janela de escolha aparecer, selecione outro aplicativo compatível.

6. Datas de pastas não aparecem:
   - Habilite as colunas Data de Criação, Data de Modificação e Data de Acesso.
   - Execute uma nova varredura para atualizar pastas, subpastas e diretórios filhos.

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

**Autores:** Fernando Nillsson Cidade<br>
**Versão:** v2026.8.25.0<br>
**Licença:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Para mais informações, consulte também `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, os assets embarcados e a documentação específica nos módulos do código-fonte.

---

</details>

---

<h2 id="enus">🇺🇸 English (US)</h2>

> **Note:** This repository refers to version **v2026.8.25.0** of the Linceu Lighthouse Project. Support the project and get the store release via: [Install via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Click to expand the README in English</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125717.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125736.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125753.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125809.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125827.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125846.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125904.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125923.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20125941.png" alt="Linceu Lighthouse - English" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/en_US/Captura%20de%20tela%202026-08-25%20130002.png" alt="Linceu Lighthouse - English" width="320">
</p>

Project: LINCEU LIGHTHOUSE  
Version: v2026.8.25.0<br>
Author: Fernando Nillsson Cidade<br>

Description: Local File Activity Intelligence, History & Audit Platform
> **"Know exactly what happened to your files — locally, visually, deterministically."**

**Linceu Lighthouse** is a native desktop application in **C++17 with Qt6** engineered to provide absolute visibility, historical tracking, and deterministic auditing over the Windows file system. Rather than operating as a mere ephemeral monitor or a restrictive security tool, Linceu Lighthouse unifies three fundamental pillars into an agile, intuitive, and high-performance experience:

1. **Continuous & Semantic Activity Monitoring:** Real-time tracking of events (creation, modification, deletion, renaming, moving, and initial scan), with specific algorithms for complex event correlation — identifying real `RENAMED` and `MOVED` operations without producing false deletion and creation pairs.
2. **Searchable History & File Intelligence without AI:** Automatic construction of a local SQLite relational database, enriched with over 30 categories of metadata before and after each modification, accompanied by 17 native graphical visualizations and deterministic statistical reports.
3. **Local Security, Audit & Evidence:** Cryptographic baseline, deterministic risk score calculation, local rules engine, quarantine, restoration, and export of auditable reports in multiple industry-standard formats.

### 🛡️ Sovereign Privacy (Local-First)
* **100% Local and Offline:** Your files, metadata, history, and reports reside exclusively on your machine (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Zero Cloud, Zero Telemetry, Zero Accounts:** No personal or operational information is ever transmitted across the network.
* **Deterministic Intelligence without AI:** All analysis, classification, and risk scoring utilize pure algorithmic and statistical logic — no language models (LLMs), no RAG, and no external dependencies.

---

### 🧭 The 5 Operating Modes

| Mode | Central Question / Focus | Key Benefit |
|---|---|---|
| **Monitoring Mode** | *What is happening right now?* | Real-time visualization of changes in selected directories with instant visual color-coded alerts. |
| **History Mode** | *What happened to my file?* | Reverse chronological inquiry (*File History* / *What Changed?*), showing the complete genealogy of edits, renames, and moves. |
| **Analytics Mode (No AI)** | *What are the usage patterns?* | Heatmaps, peak activity hours, most affected file types, and directory flows (Sankey). |
| **Security Mode** | *Were there any suspicious changes?* | Verification against cryptographic baseline, hashes, compliance rules, and structural integrity. |
| **Evidence Mode** | *How to prove changes?* | Export of immutable reports and audit packages for internal compliance or technical investigation. |

---

### 🎯 Target Audiences and Use Cases

* **Software Developers and Engineers:** Track with precision what builds, IDEs, build scripts, package managers, and Git tools create, modify, or delete across projects.
* **IT Administrators, Sysadmins, and Support:** Diagnose missing files, accidental renames in shared folders, and reconstruct operational incident timelines.
* **Sectors with Sensitive Data (Legal, Healthcare, Finance, Engineering):** Maintain strict auditable control of confidential data folders with guaranteed total confidentiality and network isolation.
* **Content Creators (Designers, Video Editors, Photographers):** Monitor movements in heavy asset libraries, rendering processes, and complex export workflows.
* **General Users:** Safeguard essential directories (Documents, Downloads, synced cloud drives) against accidental deletions and unauthorized modifications.

---

### ⚙️ How Linceu Lighthouse Works

```text
Windows File System
      ↓
Asynchronous Native APIs (ReadDirectoryChangesW)
      ↓
Thread-Safe Event Queue & Spike Buffers
      ↓
Normalization & Semantic Correlation (True Move/Rename Detection)
      ↓
Metadata Enrichment (30+ categories, Magic Numbers, Hashes)
      ↓
Asynchronous Batch Persistence in Local SQLite
      ↓
Interactive Qt6 Presentation | 17 Native Charts | Multiformat Export
```

---

### 🛠️ Available Features and Tools

- **Multidimensional Filters:** Quick filtering by operation and advanced filters with support for regex expressions, extensions, date ranges, and direct table selection.
- **Enriched Audit Table:** Configurable columns, per-operation color preference persistence, real MIME identification dynamically translated, and complete temporal metadata for folders and subfolders.
- **17 Native Graphical Visualizations:** Pie, Bar, Timeline, Treemap, Histogram, Pareto Curve, Line, Boxplot, Radar, Temporal Heatmap, Scatter, Sankey Diagrams, Dotplot, and Hierarchical Directory Tree.
- **Flexible Evidence Export:** Report generation in `.xlsx` (native OpenXML), `.csv`, `.txt`, `.json`, `.xml`, and `.db` database.
- **Complete Internationalization:** Interface and technical documentation natively integrated in 6 languages (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT`, and `de_DE`).

---

QUICK USER MANUAL:

1. Run the application to open the graphical interface.
2. Select a directory to monitor.
3. Start monitoring to execute the initial scan and track changes.
4. Use the menus and side buttons to filter, export, view statistics, and inspect security data.
5. Use double-click or Open to open reported files with the appropriate application.
6. Save reports or exported databases when you need to archive results.

DETAILED USER MANUAL:

System requirements:

- Windows 10 or higher.
- Read permission for the monitored directory.
- Write permission in `%LOCALAPPDATA%\LinceuLighthouse` for databases, settings, and logs.

Installation:

- Install the distributed executable via the project installer or build the binary using the CMake workflow described in this README.
- For local builds, use the Qt6 preset matching your toolchain: MinGW or MSVC.

Initial setup:

- The interface starts with an empty event table.
- The language is loaded by the localization system and can be changed from the options menu.
- The application creates persistent directories when needed.
- Logs are written to `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Directory monitoring:

1. Select a directory:
   - Click `Select Directory` or use `File -> Select Directory`.
   - The selected path becomes active for the observer and scan workflow.

2. Start or stop monitoring:
   - Click `Start/Stop` or use the matching menu item.
   - The initial scan populates the snapshot and records `scanned` events.
   - After scanning, continuous monitoring records real-time changes.

3. Pause monitoring or scanning:
   - The project has separate controls for stopping monitoring, scans, and active flows.
   - Visual state is updated by the progress and message layer.

Main interface:

- Menu bar: file, settings, options, export, help, security, and language.
- Sidebar: frequent actions such as select, start/stop, pause, clear, export, and statistics.
- Directory label: monitored path.
- Result label: operational and security feedback.
- Data table: events and metadata.
- Progress bar: scans and long-running tasks.
- Auxiliary dialogs: filters, colors, columns, statistics, manual, help, and about.

Important event fields:

- Operation type.
- Name.
- Previous directory.
- Current directory.
- Creation, modification, and access dates.
- Complete dates for files, folders, subfolders, and child directories.
- Type/extension.
- Translated MIME and original MIME identifier.
- Sizes in multiple units.
- Attributes, author, dimensions, duration, bit rate, and protection.
- Specific metadata for documents, spreadsheets, presentations, databases, compressed archives, and executables.

Filters and search:

- Quick filters by operation type in the menu.
- Advanced filters by operation, extension, text, date range, and combined criteria.
- Synchronization with the main menu.
- Count updates after filter or language changes.
- Checkbox state persistence.

Statistics visualization:

- Open from the statistics button or menu.
- Charts are generated by C++ modules in `source/data/GeradorEstatisticas`.
- The window supports chart selection, regeneration, saving, and language-aware updates.

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

- `Excel (.xlsx)`: OpenXML package assembled by native C++ code.
- `CSV (.csv)`: comma-delimited.
- `Text (.txt)`: tab-delimited.
- `JSON (.json)`: serialized structure with headers and rows.
- `XML (.xml)`: hierarchical output via `QXmlStreamWriter`.
- `SQLite (.db)`: database generated by the export layer.

Export options:

- Active columns only.
- Active filters only.
- Current selection only.
- Compatibility with `QTableWidget` and `QTableView`.

Column and color configuration:

- The column menu allows enabling, hiding, and restoring fields.
- Columns such as MIME, Creation Date, Modification Date, and Access Date can be enabled for deeper audits.
- The color menu supports coloring by operation or by column.
- Settings persist in local JSON files.
- The table reacts to Windows theme changes and invalidates visual caches when needed.

Internationalization:

- Supported languages: Portuguese, English, Spanish, French, Italian, and German.
- `.ts` and `.qm` files reside in `source/locale`.
- `LocalizadorQt` loads translations, saves preferences, and provides fallback text.
- Operations, metadata, MIME, menus, manual, help, about, and security texts are translated.

Troubleshooting:

1. Monitoring does not start:
   - Confirm directory permissions.
   - Avoid protected system directories.
   - Check whether the local database can be created.

2. Events do not appear:
   - Confirm monitoring is active.
   - Clear filters.
   - Test by creating, renaming, or deleting a simple file in the monitored directory.

3. High resource consumption:
   - Avoid unnecessarily monitoring very large trees.
   - Prefer specific subdirectories.
   - Export or clear old data when the database grows too large.

4. Empty statistics:
   - Confirm that events are loaded.
   - Remove filters that may hide rows.
   - Check whether `monitoramento.db` is accessible.

5. File does not open:
   - Confirm the file still exists at the displayed path.
   - Check whether Windows has an application associated with the file type.
   - When the chooser appears, select another compatible application.

6. Folder dates do not appear:
   - Enable Creation Date, Modification Date, and Access Date columns.
   - Run a new scan to refresh folders, subfolders, and child directories.


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

**Authors:** Fernando Nillsson Cidade<br>
**Version:** v2026.8.25.0<br>
**License:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

For more information, also consult `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, embedded assets and module-specific source documentation.

---

</details>

---

<h2 id="eses">🇪🇸 Español (ES)</h2>

> **Nota:** Este repositorio se refiere a la versión **v2026.8.25.0** del Proyecto Linceu Lighthouse. Apoye el proyecto y adquiera la versión de pago a través del enlace: [Instalar vía Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Haga clic para expandir el README en español</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130104.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130127.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130143.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130201.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130224.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130242.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130302.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130319.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130340.png" alt="Linceu Lighthouse - Español" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/es_ES/Captura%20de%20tela%202026-08-25%20130400.png" alt="Linceu Lighthouse - Español" width="320">
</p>

Proyecto: LINCEU LIGHTHOUSE  
Versión: v2026.8.25.0<br>
Autor: Fernando Nillsson Cidade<br>

Descripción: Plataforma Local de Inteligencia, Historial y Auditoría de Actividades de Archivos
> **"Sepa exactamente qué ocurrió con sus archivos — localmente, visualmente y con historial consultable."**

El **Linceu Lighthouse** es una aplicación de escritorio nativa en **C++17 con Qt6** desarrollada para proporcionar visibilidad absoluta, rastreo histórico y auditoría determinista sobre el sistema de archivos de Windows. En lugar de actuar como un mero monitor efímero o una herramienta restringida de seguridad, Linceu Lighthouse unifica tres pilares fundamentales en una experiencia ágil, intuitiva y de alto rendimiento:

1. **Monitorización Continua y Semántica de Actividades:** Seguimiento en tiempo real de eventos (creación, modificación, eliminación, renombrado, movimiento y escaneo inicial), con algoritmos específicos para correlación de eventos complejos — identificando operaciones reales de `RENAMED` y `MOVED` sin generar pares falsos de eliminación y creación.
2. **Historial Consultable e Inteligencia de Archivos sin IA:** Construcción automática de una base de datos relacional SQLite local, enriquecida con más de 30 categorías de metadatos antes y después de cada modificación, acompañada por 17 visualizaciones gráficas nativas e informes estadísticos deterministas.
3. **Seguridad Local, Auditoría y Evidencias:** Línea de base criptográfica (baseline), cálculo determinista de puntuación de riesgo, motor de reglas locales, cuarentena, restauración y exportación de informes auditables en múltiples formatos del mercado.

### 🛡️ Privacidad Soberana (Local-First)
* **100% Local y Offline:** Sus archivos, metadados, historiales e informes residen exclusivamente en su máquina (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Cero Nube, Cero Telemetría, Cero Cuentas:** Ninguna información personal u operativa se transmite a la red.
* **Inteligencia Determinista sin IA:** Todo el análisis, clasificación y puntuación de riesgo utilizan lógica algorítmica y estadística pura — sin modelos de lenguaje (LLM), sin RAG y sin dependencias externas.

---

### 🧭 Los 5 Modos de Operación

| Modo | Pregunta / Enfoque Central | Beneficio Principal |
|---|---|---|
| **Modo Monitorización** | *¿Qué está sucediendo ahora?* | Visualización en tiempo real de cambios en directorios seleccionados con alertas visuales inmediatas por color. |
| **Modo Historial** | *¿Qué ocurrió con mi archivo?* | Consulta cronológica inversa (*File History* / *What Changed?*), mostrando la genealogía completa de ediciones y renombres. |
| **Modo Analytics (Sin IA)** | *¿Cuáles son los patrones de uso?* | Mapas de calor (Heatmaps), horarios pico, tipos de archivo más afectados y flujos de directorios (Sankey). |
| **Modo Seguridad** | *¿Hubo alguna alteración sospechosa?* | Verificación contra línea de base criptográfica, hashes, reglas de conformidad e integridad estructural. |
| **Modo Evidencia** | *¿Cómo comprobar las alteraciones?* | Exportación de informes inmutables y paquetes de auditoría para conformidad interna o investigación técnica. |

---

### 🎯 Públicos Objetivo y Casos de Uso

* **Desarrolladores e Ingenieros de Software:** Rastrear con exactitud qué crean, modifican o eliminan las compilaciones, IDEs, scripts de build, gestores de paquetes y herramientas Git en los proyectos.
* **Administradores de TI, Sysadmins y Soporte:** Diagnosticar archivos desaparecidos, renombres accidentales en carpetas compartidas y reconstruir la línea de tiempo de incidentes operativos.
* **Sectores con Datos Sensibles (Jurídico, Salud, Finanzas, Ingeniería):** Mantener un control auditable estricto de carpetas con datos confidenciales con garantía de confidencialidad total y aislamiento de red.
* **Creadores de Contenido (Diseñadores, Editores de Vídeo, Fotógrafos):** Acompañar movimientos de bibliotecas pesadas de assets, procesos de renderizado y flujos complejos de exportación.
* **Usuarios en General:** Proteger directorios esenciales (Documentos, Descargas, unidades de nube sincronizadas) contra eliminaciones accidentales y modificaciones no autorizadas.

---

### ⚙️ Cómo Funciona Linceu Lighthouse

```text
Sistema de Archivos (Windows)
      ↓
APIs Nativas Asíncronas (ReadDirectoryChangesW)
      ↓
Cola de Eventos Thread-Safe y Búferes de Pico
      ↓
Normalización y Correlación Semántica (Detección Real de Move/Rename)
      ↓
Enriquecimiento con Metadatos (30+ categorías, Magic Numbers, Hashes)
      ↓
Persistencia Asíncrona por Lotes en SQLite Local
      ↓
Visualización Interactiva Qt6 | 17 Gráficos Nativos | Exportación Multiformato
```

---

### 🛠️ Recursos y Herramientas Disponibles

- **Filtros Multidimensionales:** Filtrado rápido por operación y filtros avanzados con soporte para expresiones, extensiones, rangos de fecha y selección en la tabla.
- **Tabla de Auditoría Enriquecida:** Columnas configurables, persistencia de preferencias de color por operación, identificación MIME real traducida dinámicamente y metadados temporales completos para carpetas y subcarpetas.
- **17 Visualizaciones Gráficas Nativas:** Tarta, Barras, Línea de Tiempo (Timeline), Mapa de Árbol (Treemap), Histograma, Curva de Pareto, Líneas, Boxplot, Radar, Heatmap Temporal, Dispersión (Scatter), Diagramas de Sankey, Dotplot y Árbol Jerárquico.
- **Exportación Flexible de Evidencias:** Generación de informes en `.xlsx` (OpenXML nativo), `.csv`, `.txt`, `.json`, `.xml` y base de datos `.db`.
- **Internacionalización Completa:** Interfaz y documentación técnica integradas nativamente en 6 idiomas (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` y `de_DE`).

---

MANUAL DE USO RÁPIDO:

1. Ejecute la aplicación para abrir la interfaz gráfica.
2. Seleccione un directorio para monitorizar.
3. Inicie la monitorización para ejecutar el escaneo inicial y seguir las alteraciones.
4. Use los menús y botones laterales para filtrar, exportar, visualizar estadísticas y consultar seguridad.
5. Use doble clic o la acción Abrir para abrir los archivos reportados en la tabla con la aplicación adecuada.
6. Guarde informes o bases exportadas cuando necesite archivar resultados.

MANUAL DE USO DETALLADO:

Requisitos del sistema:

- Windows 10 o superior.
- Permiso de lectura en el directorio monitorizado.
- Permiso de escritura en `%LOCALAPPDATA%\LinceuLighthouse` para base de datos, configuraciones y registros (logs).

Instalación:

- Instale el ejecutable distribuido por el instalador del proyecto o genere el binario mediante el flujo CMake descrito en este README.
- En compilaciones locales, use el preset compatible con su toolchain Qt6: MinGW o MSVC.

Configuración inicial:

- La interfaz se inicia con la tabla de eventos vacía.
- El idioma se carga a través del sistema de localización y se puede cambiar desde el menú de opciones.
- La aplicación crea directorios persistentes cuando es necesario.
- Los registros (logs) se guardan en `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Monitorización de directorios:

1. Seleccionar un directorio:
   - Haga clic en `Seleccionar Directorio` o use `Archivo -> Seleccionar Directorio`.
   - La ruta seleccionada pasa a ser utilizada por el observador y por el flujo de escaneo.

2. Iniciar o detener la monitorización:
   - Haga clic en `Iniciar/Parar` o use el menú correspondiente.
   - El escaneo inicial llena el snapshot y registra eventos `escaneado`.
   - Tras el escaneo, la monitorización continua pasa a registrar alteraciones en tiempo real.

3. Pausar monitorización o escaneo:
   - El proyecto cuenta con controles separados para interrumpir monitorización, escaneo o flujos activos.
   - El estado visual se actualiza mediante la capa de progreso y mensajes.

Interfaz principal:

- Barra de menú: archivo, configuraciones, opciones, exportación, ayuda, seguridad e idioma.
- Barra lateral: acciones frecuentes como seleccionar, iniciar/detener, pausar, limpiar, exportar y estadísticas.
- Etiqueta de directorio: ruta monitorizada.
- Etiqueta de resultado: mensajes operativos y comentarios de seguridad.
- Tabla de datos: eventos y metadados.
- Barra de progreso: escaneos y tareas prolongadas.
- Diálogos auxiliares: filtros, colores, columnas, estadísticas, manual, ayuda y acerca de.

Campos de eventos más importantes:

- Tipo de operación.
- Nombre.
- Directorio anterior.
- Directorio actual.
- Fechas de creación, modificación y acceso.
- Fechas completas para archivos, carpetas, subcarpetas y directorios secundarios.
- Tipo/extensión.
- MIME traducido e identificador MIME original.
- Tamaños en múltiples unidades.
- Atributos, autor, dimensiones, duración, tasa de bits y protección.
- Metadados específicos para documentos, hojas de cálculo, presentaciones, bases de datos, archivos comprimidos y ejecutables.

Filtros y búsqueda:

- Filtros rápidos por tipo de operación en el menú.
- Filtros avanzados por operación, extensión, texto, rango de fechas y combinación de criterios.
- Sincronización con el menú principal.
- Actualización del recuento tras cambiar de filtro o idioma.
- Persistencia del estado de las casillas de verificación de filtros.

Visualización de estadísticas:

- Acceda mediante el botón de estadísticas o desde el menú.
- Los gráficos son generados por módulos C++ en `source/data/GeradorEstatisticas`.
- La ventana permite selección de gráficos, regeneración, guardado y actualización tras cambiar de idioma.

Gráficos disponibles:

- Tarta (Pizza).
- Barras.
- Línea de Tiempo (Timeline).
- Treemap (Mapa de Árbol).
- Histograma.
- Pareto.
- Línea.
- Boxplot.
- Radar de eventos.
- Heatmap.
- Scatter (Dispersión).
- Sankey.
- Radar.
- Dotplot.
- Sankey evento-ruta.
- Sankey tipo-ruta.
- Árbol de directorios.

Exportación de datos:

- `Excel (.xlsx)`: paquete OpenXML generado por el propio código C++.
- `CSV (.csv)`: delimitado por comas.
- `Texto (.txt)`: delimitado por tabulaciones.
- `JSON (.json)`: estructura serializada con encabezados y filas.
- `XML (.xml)`: salida jerárquica mediante `QXmlStreamWriter`.
- `SQLite (.db)`: base de datos generada por la capa de exportación.

Opciones de exportación:

- Solo columnas activas.
- Solo filtros activos.
- Solo selección actual.
- Compatibilidad con `QTableWidget` y `QTableView`.

Configuración de columnas y colores:

- El menú de columnas permite activar, ocultar y restaurar campos.
- Columnas como MIME, Fecha de Creación, Fecha de Modificación y Fecha de Acceso pueden habilitarse para auditorías más detalladas.
- El menú de colores permite colorear por operación o por columna.
- Las configuraciones persisten en archivos JSON locales.
- La tabla reacciona a los cambios de tema de Windows e invalida las cachés visuales cuando es necesario.

Internacionalización:

- Idiomas compatibles: portugués, inglés, español, francés, italiano y alemán.
- Los archivos `.ts` y `.qm` se encuentran en `source/locale`.
- El `LocalizadorQt` carga traducciones, guarda preferencias y proporciona fallback.
- Operaciones, metadados, MIME, menús, manual, ayuda, acerca de y textos de seguridad están traducidos.

Resolución de problemas:

1. La monitorización no se inicia:
   - Confirme los permisos del directorio.
   - Evite directorios protegidos del sistema.
   - Verifique si la base de datos local puede crearse.

2. Los eventos no aparecen:
   - Verifique que la monitorización esté activa.
   - Limpie los filtros.
   - Pruebe creando, renombrando o eliminando un archivo simple en el directorio monitorizado.

3. Alto consumo de recursos:
   - Evite monitorizar árboles de directorios excesivamente grandes sin necesidad.
   - Prefiera subdirectorios específicos.
   - Exporte o limpie datos antiguos cuando la base de datos crezca demasiado.

4. Estadísticas vacías:
   - Confirme que existen eventos cargados.
   - Elimine filtros que puedan ocultar filas.
   - Verifique si `monitoramento.db` es accesible.

5. El archivo no se abre:
   - Confirme que el archivo todavía existe en la ruta mostrada.
   - Verifique si Windows tiene una aplicación asociada al tipo de archivo.
   - Cuando aparezca la ventana de selección, elija otra aplicación compatible.

6. Las fechas de las carpetas no aparecen:
   - Habilite las columnas Fecha de Creación, Fecha de Modificación y Fecha de Acceso.
   - Ejecute un nuevo escaneo para actualizar carpetas, subcarpetas y directorios secundarios.

## 📋 Documentación Técnica

### Visión General del Sistema

**Linceu Lighthouse** es una aplicación de escritorio nativa para Windows, escrita en **C++17** y basada en **Qt6**. La arquitectura actual reemplaza la antigua cadena Python/PySide por módulos `.cpp` y `.h`, con compilación CMake, persistencia SQLite mediante Qt SQL, renderizado Qt Widgets/OpenGL/Quick e integración con APIs nativas de Windows.

La aplicación está organizada en capas:

- `source/gui`: ventana principal, composición visual e integración de alto nivel.
- `source/services`: observación de directorios, eventos, base de datos, metadados y tabla.
- `source/ui`: menús, botones, estadísticas, filtros, manual, ayuda, acerca de y localización.
- `source/data`: fachada estadística, generadores e infraestructura de gráficos.
- `source/tools`: filtros y administración de criterios.
- `source/security`: recursos de seguridad, sesiones, cuarentena, evidencia y backend de almacenamiento.
- `source/utils`: rutas, iconos y logging.
- `source/assets` y `source/locale`: contenido integrado, licencias, textos y traducciones.

#### Tecnologías Principales

- **Lenguaje:** C++17.
- **Build:** CMake, con `CMAKE_AUTOMOC` y `CMAKE_AUTORCC`.
- **Framework:** Qt6.
- **UI:** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, menús y acciones.
- **Base de Datos:** SQLite mediante `QSqlDatabase` y `QSqlQuery`.
- **Concurrencia:** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` y Qt Concurrent.
- **Gráficos:** infraestructura propia con `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` y modelo tipado `LgplChartModel`.
- **Exportación:** escritura propia de CSV, TXT, JSON, XML, SQLite y paquete `.xlsx` OpenXML.
- **Internacionalización:** Qt Linguist (`.ts` y `.qm`) con seis idiomas.
- **Windows:** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` para tema y bibliotecas `wintrust`, `version` y `crypt32`.
- **Seguridad:** capa C++ propia con línea de base criptográfica, reglas, puntuación de riesgo, cuarentena, evidencias y caché local SQLite para el backend de seguridad.

La aplicación principal no depende del entorno de ejecución Python, `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` ni de bibliotecas equivalentes. Cualquier mención a Python en archivos de licencia refleja documentación de terceros integrada, no la pila de ejecución principal.

---

### 📁 Estructura de Directorios y Módulos

Resumen validado en el árbol `source/`:

| Capa | `.cpp` | `.h` | Responsabilidad |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Inicialización principal y splash |
| `source/assets` | 0 | 0 | Textos, licencias, imágenes, iconos y contenido integrado |
| `source/data` | 26 | 27 | Estadísticas, gráficos y renderizado |
| `source/gui` | 8 | 8 | Ventana principal y composición visual |
| `source/locale` | 0 | 0 | Archivos `.ts` y `.qm` de traducción |
| `source/security` | 24 | 24 | Seguridad, evidencia, cuarentena y sesiones |
| `source/services` | 162 | 163 | Observación, eventos, base de datos, metadados y tabla |
| `source/tools` | 23 | 24 | Filtros y administración de filtros |
| `source/ui` | 125 | 125 | Menús, botones, diálogos, manual, estadísticas y localización |
| `source/utils` | 4 | 4 | Rutas, iconos y logs |
| **Total en `source/`** | **374** | **377** | Base C++ actual |

<details>
<summary><b>📂 source-root - Bootstrap de la aplicación</b></summary>

#### Módulos Principales

- `src_01_InicializadorMain.cpp/.h` - Entrada real de la aplicación tras `main.cpp`, ajuste de directorio de trabajo, identificación de Windows y fallback de inicialización.
- `src_02_SplashAppStarter.cpp/.h` - Inicialización con splash, progreso, icono, traducciones, creación de `InterfaceMonitor` e inicio anticipado del writer.

</details>

<details>
<summary><b>📂 source/gui/ - Capa de Interfaz Gráfica</b></summary>

#### Módulos Principales

- `ic_01_InterfaceMonitor.cpp/.h` - Ventana principal, propiedades compartidas, contenedor de tablas e integración de subsistemas.
- `ic_02_Inicializador.cpp/.h` - Inicialización de widgets y objetos base.
- `ic_03_Configurador.cpp/.h` - Layouts, componentes y configuración visual.
- `ic_04_Atualizador.cpp/.h` - Actualización de la interfaz para tabla, estado y progreso.
- `ic_05_GerenciadorProgresso.cpp/.h` - Barras y retroalimentación de progreso.
- `ic_06_GerenciadorMensagens.cpp/.h` - Mensajes, estado y comunicación con el usuario.
- `ic_07_ManipuladorTabela.cpp/.h` - Operaciones de tabla y adaptación para la UI.
- `ic_08_Internacionalizador.cpp/.h` - Integración de la interfaz con el sistema de localización.

Ya no existen los antiguos subdirectorios `GerenciadorDesempenho/` y `GerenciadorEstruturaDiretoriosWidget/` en este árbol. La documentación actual no debe tratarlos como subsistemas activos.

</details>

<details>
<summary><b>📂 source/services/ - Núcleo de Monitorización, Eventos y Datos</b></summary>

#### Módulos Principales

- `ob_01_Observador.cpp/.h` - Observador de directorio, subprocesos (threads), búferes y ciclo de monitorización.
- `ob_02_BaseEvento.cpp/.h` - Base común para eventos e integración con SQLite.
- `ob_03_DiretorioScanner.cpp/.h` - Escaneo inicial y construcción del snapshot.
- `ob_04_EventoAdicionado.cpp/.h` - Registro de archivos creados.
- `ob_05_EventoExcluido.cpp/.h` - Registro de eliminaciones y eliminaciones temporales.
- `ob_06_EventoModificado.cpp/.h` - Consolidación de modificaciones.
- `ob_07_EventoRenomeado.cpp/.h` - Procesamiento de renombres.
- `ob_08_EventoMovido.cpp/.h` - Detección y reconciliación de movimientos.
- `ob_09_GerenciadorColunas.cpp/.h` - Columnas disponibles y persistencia de visibilidad.
- `ob_10_GerenciadorTabela.cpp/.h` - Puente entre eventos, tabla e interfaz.

#### Subdirectorios Especializados

- `GerenciamentoBaseEvento/` - Creación de base de datos, PRAGMAs, índices, registros, snapshot, eliminaciones, movimientos y `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Workers de escaneo, cola de procesamiento, lotes, progreso y finalización.
- `GerenciamentoEventoModificado/` - Intervalos de deduplicación, caché, archivos grandes y procesamiento normal/masivo.
- `GerenciamentoEventoMovido/` - Worker de movimiento, eliminaciones pendientes, actualización de filas y tabla.
- `GerenciamentoMetadados/` - Extracción nativa de metadados para código, imágenes, audio, vídeo, documentos, hojas de cálculo, presentaciones, ejecutables, comprimidos, logs y datos estructurados.
- `GerenciamentoObservador/` - Inicio, parada, monitorización, búfer, detección de operaciones masivas y pausa.
- `GerenciamentoTabela/` - Tema, encabezados, anchos, colores, selección, retraducción y `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menús, Diálogos, Acciones y Localización</b></summary>

#### Módulos Principales

- `ui_01_GerenciadorBotoes.cpp/.h` - Fachada de botones y acciones rápidas.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Construcción visual de los botones.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Menús principales y acciones persistentes.
- `ui_04_GerenciadorEventosUI.cpp/.h` - Eventos de UI.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Retroalimentación visual de operaciones prolongadas.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Ventana y flujo de estadísticas.
- `ui_07_GerenciadorDados.cpp/.h` - Exportación de datos.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Integración entre eventos y UI.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Control de monitorización.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Limpieza de datos.
- `ui_11_DialogoCores.cpp/.h` - Selección y persistencia de colores.
- `ui_12_LocalizadorQt.cpp/.h` - Localización y preferencia de idioma.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Traducción de metadados.
- `ui_14_OpcoesSobre.cpp/.h` - Contenido de acerca de, licencia, avisos, privacidad y notas de lanzamiento.
- `ui_15_Manual.cpp/.h` - Manual integrado.

#### Subdirectorios

- `GerenciamentoBotoes/` - Selección de directorio, alternancia de análisis, exportación, SQLite, limpieza y pausa.
- `GerenciamentoDialogoCores/` - UI de selección de colores, cuadrícula, selector avanzado y traducciones.
- `GerenciamentoEstatisticasUI/` - Panel, casillas de verificación, workers, regeneración, pestañas, diseño y guardado de gráficos.
- `GerenciamentoEventosUI/` - Filtros, columnas, colores, idioma, limpieza y reinicio.
- `GerenciamentoLocalizadorQt/` - Carga de traductores, fallback, archivos `.ts/.qm` y traducción de operaciones.
- `GerenciamentoMenusUI/` - Menús, submenús, exportación, idiomas, acerca de, manual y ayuda.
- `GerenciamentoTradutorMetadadosQt/` - Traducciones de tipos de archivo, atributos, autor, protección y dimensiones.
- `Manual/` - Documentación interna en seis idiomas.

</details>

<details>
<summary><b>📂 source/data/ - Estadísticas y Gráficos</b></summary>

#### Módulos Principales

- `st_01_GeradorEstatisticas.cpp/.h` - Fachada para generación de estadísticas.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Cálculo geométrico de diseño.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Fábrica de widgets de gráficos.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Adaptación de payloads heredados.
- `Charts/ch_04_QmlRegistration.cpp/.h` - Registro QML.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Elemento Qt Quick.
- `Charts/ch_06_WidgetChartView.cpp/.h` - Visualización mediante `QOpenGLWidget`.
- `Charts/ch_07_LgplChartModel.h` - Modelo tipado de gráfico.
- `Charts/ch_08_RasterChartView.cpp/.h` - Renderizado QWidget con `QPainter`.

#### 17 Generadores de Gráficos Disponibles

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
<summary><b>📂 source/tools/ - Filtros y Administración</b></summary>

#### Módulos de Filtros

- `fil_01_Filtros.cpp/.h` - Widget y lógica principal de filtros.
- `fil_02_AdministradorCalendario.cpp/.h` - Selección de fechas.
- `fil_03_AdministradorFiltros.cpp/.h` - Aplicación combinada de filtros.

#### Subdirectorios

- `GerenciamentoAdministradorFiltros/` - Aplicación, limpieza, sincronización con menú, recuentos, idioma y persistencia.
- `GerenciamentoFiltros/` - UI, cambio de filtro, sincronización, verificación de operación y limpieza.

</details>

<details>
<summary><b>📂 source/security/ - Seguridad, Evidencia y Cuarentena</b></summary>

#### Módulos Principales

- `secore_02_SecurityManager.cpp/.h` - Orquestación de la capa de seguridad.
- `secore_01_postgres_utils.cpp/.h` - Backend Qt SQL, PostgreSQL configurable y caché local SQLite.
- `secore_05_SecurityDbWriter.cpp/.h` - Writer asíncrono de seguridad.
- `base_01_baseline_criptografico.cpp/.h` - Línea de base criptográfica.
- `exec_01_inteligencia_executaveis.cpp/.h` - Inteligencia para ejecutables.
- `sess_01_correlacao_sessoes.cpp/.h` - Correlación de sesiones.
- `sdiff_01_diff_conteudo.cpp/.h` - Diferencias de contenido.
- `attr_01_atribuicao_forense.cpp/.h` - Atribución forense.
- `agre_01_motor_regras.cpp/.h` - Motor de reglas.
- `risk_01_score_risco.cpp/.h` - Puntuación de riesgo.
- `quar_01_quarentena.cpp/.h` - Cuarentena y restauración.
- `evid_01_exportacao_evidencia.cpp/.h` - Paquetes de evidencia.
- `secui_*` - Diálogos y controladores de UI para seguridad.

#### Recursos

- Perfiles y multi-directorio.
- Central de alertas.
- Sesiones en tiempo real.
- Cuarentena y restauración.
- Paquete de evidencia.
- Notificaciones y configuración de ventanas emergentes.
- Almacenamiento local en `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ y source/locale/ - Contenido Integrado</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` y `RELEASE`.
- Iconos `.ico` e imágenes `.png`.
- Textos de acerca de, licencia, avisos, política de privacidad, historial y notas de lanzamiento en múltiples idiomas.

#### Archivos de Traducción

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Directorios Auxiliares</b></summary>

- `mocks/` - Utilidades auxiliares, incluyendo `find_VersionEditor` y `compile_translations`.
- `repo/` - Scripts y archivos de empaquetado, incluyendo instaladores Inno Setup.
- `.vscode/` - Configuración local de IDE.
- `.github/workflows/` - Automatización de actualización documental.
- `CMakePresets.json` - Presets principales MinGW y MSVC.
- `mocks/CMakePresets.json` - Presets de utilidades auxiliares.

</details>

---

### 🏗️ Arquitectura y Patrones de Diseño

#### Principios SOLID Aplicados

- Responsabilidad única por módulos numerados y subcapas especializadas.
- Fachadas para subsistemas complejos, como estadísticas, botones, menús, eventos y seguridad.
- Separación entre observación, persistencia, metadados, visualización, exportación y traducción.
- Bajo acoplamiento mediante el uso de propiedades de `QObject`, señales/ranuras (signals/slots) e interfaces de servicio.
- Extensión mediante nuevos módulos `.cpp/.h` sin reescribir el núcleo.

#### Patrones de Diseño Implementados

- **Facade:** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer/Event loop:** señales/ranuras Qt, callbacks de eventos y notificaciones de monitorización.
- **Worker/Producer-Consumer:** colas de eventos, `DatabaseWriter`, workers de gráficos y escáner.
- **Model/View:** `EventTableModel` con `QTableView`, manteniendo compatibilidad con `QTableWidget`.
- **Factory:** creación de widgets de gráficos mediante `ChartWidgetFactory`.
- **Adapter:** `LegacyVariantChartAdapter` convierte payloads antiguos a modelos actuales.
- **Repository/DAO local:** acceso SQLite mediante conexiones Qt SQL con nombres únicos.
- **Strategy informal:** generadores separados para cada tipo de gráfico y extractores separados por tipo de archivo.

#### Multithreading y Rendimiento

- Escaneo inicial en worker Qt.
- Monitorización continua en `std::thread`.
- Procesamiento del búfer de eventos en subproceso dedicado.
- Escritura de eventos con `DatabaseWriter`, cola interna y persistencia por lotes.
- Extracción de metadados por lotes con `QThreadPool`.
- Generación de gráficos con `GraficoWorker`.
- Caché de etiquetas, pinceles (brushes) y traducciones en `EventTableModel`.
- Actualización visual asíncrona para evitar bloqueos en la UI.

#### Optimizaciones de Base de Datos

- SQLite en modo WAL.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` y límite de journal en el writer.
- Tablas por tipo de evento y tabla consolidada `monitoramento`.
- Tabla `snapshot` para línea de base.
- Índices por nombre, directorios, marca temporal (timestamp) y claves frecuentes.
- Inserción por lotes con `execBatch()` y transacciones.

#### Arquitectura de Eventos

1. `Observador` detecta cambios en el directorio.
2. Los eventos brutos se clasifican como añadido, eliminado, modificado, renombrado, movido o escaneado.
3. Submódulos especializados eliminan el ruido duplicado y concilian operaciones complejas.
4. Los metadados se extraen cuando corresponde.
5. Los eventos se envían a la UI y al writer.
6. La base de datos local permite recargar, filtrar, exportar y generar estadísticas.

#### Extracción de Metadados

- `QImageReader` para imágenes.
- Inspección de ZIP/OpenXML para documentos, hojas de cálculo y presentaciones.
- `QSqlDatabase` para SQLite y bases de datos compatibles con Qt SQL.
- Lectura de encabezados para identificación de formatos.
- APIs de Windows para ejecutables, firma y versión.
- Cálculos de tamaño en B, KB, MB, GB y TB.
- Metadados especializados para logs, copias de seguridad, comprimidos, OLE, datos estructurados y código fuente.

#### Seguridad

La capa `source/security` amplía la monitorización con funciones de respuesta e investigación:

- Perfiles de directorios y monitorización múltiple.
- Alertas derivadas de reglas.
- Correlación de sesiones.
- Línea de base criptográfica.
- Cuarentena y restauración de elementos.
- Paquetes de evidencia.
- Puntuación de riesgo.
- Backend configurable mediante Qt SQL, con caché local SQLite cuando es necesario.

---

### 🔄 Flujo de Ejecución Principal

1. `main.cpp` llama a `source::iniciar_aplicacao(argc, argv)`.
2. `src_01_InicializadorMain` ajusta el directorio de trabajo, la identidad de Windows y el fallback de inicialización.
3. `src_02_SplashAppStarter` crea `QApplication`, carga el icono, las traducciones y el splash.
4. `InterfaceMonitor` monta la ventana principal y registra las propiedades compartidas.
5. Se crean los gestores de botones, menús, eventos, progreso, datos y estadísticas.
6. La capa de localización define el idioma y los traductores.
7. La capa de seguridad inicia su manager y writer.
8. El usuario selecciona un directorio.
9. `Observador` e `DiretorioScanner` ejecutan el escaneo inicial.
10. Se persisten el snapshot y los eventos `escaneado`.
11. La monitorización continua utiliza `CreateFileW` y `ReadDirectoryChangesW`.
12. Los eventos se clasifican, se enriquecen y se encolan.
13. `DatabaseWriter` persiste por lotes.
14. Se actualizan la tabla, los filtros, los contadores y los colores.
15. Las estadísticas, exportaciones y diálogos consultan el estado actual.
16. Al cerrar la aplicación, los subprocesos y las conexiones finalizan de manera ordenada.

---

### 📦 Requisitos y Dependencias

#### Bibliotecas Principales

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
- Bibliotecas de Windows: `wintrust`, `version`, `crypt32`.

#### Requisitos de Compilación

- CMake 4.1 o superior para la compilación principal.
- CMake 3.24 o superior para las utilidades en `mocks/`.
- Qt 6.11.0 en los presets actuales.
- MinGW 13.1.0 o Visual Studio 2022 Build Tools, según el preset.
- Windows SDK compatible con la toolchain seleccionada.

#### Presets CMake

Configurar y compilar con MinGW:

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Configurar y compilar con MSVC Debug:

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Configurar y compilar con MSVC Release:

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Compilar utilidad de edición de versiones:

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Despliegue (Deploy)

En Windows, `CMakeLists.txt` intenta localizar `windeployqt` y copia:

- ejecutable final con el nombre `Linceu_Lighthouse`;
- dependencias de Qt;
- `source/assets`;
- archivos `.qm` de la aplicación;
- traducciones de Qt en `locale/qt`.

---

### ✨ Funcionalidades Principales

<details>
<summary><b>Monitorización de Archivos</b></summary>

- Escaneo inicial recursivo.
- Snapshot persistente.
- Monitorización en tiempo real mediante API nativa de Windows.
- Eventos: añadido, eliminado, modificado, renombrado, movido y escaneado.
- Deduplicación y reconciliación de eventos complejos.
- Detección de operaciones masivas.
- Pausa y detención de escaneo/monitorización.

</details>

<details>
<summary><b>Base de Datos y Persistencia</b></summary>

- `monitoramento.db` en `%LOCALAPPDATA%\LinceuLighthouse`.
- Tablas por tipo de evento.
- Tabla consolidada para consultas y exportaciones.
- Índices optimizados.
- Escritura asíncrona por lotes.
- Configuraciones locales en JSON e INI.
- Registros con marca temporal (timestamp).

</details>

<details>
<summary><b>Metadatos</b></summary>

- Tipo de archivo.
- Tamaño en múltiples unidades.
- Atributos.
- Autor.
- Dimensiones.
- Duración.
- Tasa de bits.
- Protección.
- Páginas, líneas, palabras, diapositivas, hojas de cálculo, tablas y registros cuando corresponda.
- Firma y versión de ejecutables en Windows.

</details>

<details>
<summary><b>Visualización y Tabla</b></summary>

- `QTableView` con `EventTableModel`.
- Compatibilidad transitoria con `QTableWidget`.
- Ordenación.
- Visibilidad de columnas.
- Colores por operación y por columna.
- Tema claro/oscuro de Windows.
- Retraducción de datos existentes.
- Actualización incremental de eventos.

</details>

<details>
<summary><b>Estadísticas y Gráficos</b></summary>

- 17 generadores especializados.
- Renderizado ráster con `QPainter`.
- Backend OpenGL mediante `QOpenGLWidget`.
- Integración Qt Quick/QML.
- Panel de selección.
- Regeneración tras cambio de idioma.
- Guardado de gráficos.

</details>

<details>
<summary><b>Filtros Avanzados</b></summary>

- Operación.
- Extensión.
- Texto.
- Rango de fechas.
- Combinación de filtros.
- Sincronización con menús.
- Recuento actualizado.
- Persistencia de estado.

</details>

<details>
<summary><b>Exportación de Datos</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Texto `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Exportación de selección, columnas activas y filtros activos.
- Compatibilidad con datos procedentes de model/view.

</details>

<details>
<summary><b>Internacionalización</b></summary>

- Seis idiomas.
- Traducción de interfaz, operaciones y metadados.
- Fallback textual.
- Preferencia persistida.
- Manual y contenido institucional integrado.
- Diálogo Acerca de reactivo al cambio de idioma.

</details>

<details>
<summary><b>Seguridad</b></summary>

- Multi-directorio y perfiles.
- Central de alertas.
- Sesiones en tiempo real.
- Cuarentena.
- Restauración.
- Paquetes de evidencia.
- Notificaciones.
- Puntuación de riesgo y reglas.

</details>

---

### 🎯 Buenas Prácticas Implementadas

#### Código Limpio

- Organización por capas y módulos numerados.
- Clases y funciones con responsabilidades delimitadas.
- Uso coherente de namespaces.
- Encabezados `.h` separados de las implementaciones `.cpp`.
- Fallbacks ante fallos de UI, logging e inicialización.
- Comentarios puntuales en áreas complejas.

#### Rendimiento

- Monitorización nativa en lugar de polling.
- Escritura SQLite por lotes.
- Índices en campos consultados frecuentemente.
- Workers y subprocesos para tareas pesadas.
- Cachés de traducción, etiquetas y colores.
- Actualización incremental de la tabla.
- Evita dependencias pesadas de análisis en tiempo de ejecución.

#### Mantenibilidad

- Capas bien separadas.
- Módulos pequeños para flujos específicos.
- Compilación declarativa en CMake.
- Presets para toolchains.
- Assets y traducciones versionados.
- Utilidades aisladas en `mocks/`.
- Documentación técnica en el repositorio.

#### Seguridad y Robustez

- Uso de directorio persistente local del usuario.
- Conexiones SQLite con nombres únicos.
- Finalización ordenada de subprocesos.
- Registros de error y diagnóstico.
- Tratamiento defensivo con `try/catch` en los bordes de la UI.
- Validación de punteros antes de su uso en integraciones Qt.

---

### 🚀 Para Añadir Nuevas Funcionalidades

- **Nuevo tipo de gráfico:** cree un nuevo generador en `source/data/GeradorEstatisticas/`, siguiendo `gst_01_BaseGerador` y registrando la opción en la UI de estadísticas.
- **Nuevo tipo de evento:** añada una clase o función especializada en `source/services`, actualice las columnas si es necesario y registre la persistencia en el flujo de `BaseEvento`.
- **Nueva columna:** actualice `ob_09_GerenciadorColunas`, el esquema en `gbank_02_criar_banco_de_dados`, exportación y traducciones.
- **Nuevo metadato:** implemente un extractor en `GerenciamentoMetadados`, añada columna, traducción y lógica de visualización.
- **Nuevo idioma:** cree `linceu_XX.ts`, compile a `.qm`, añada fallback en `LocalizadorQt` y traduzca assets cuando corresponda.
- **Nuevo filtro:** implemente en `source/tools/GerenciamentoAdministradorFiltros` y sincronice con el menú y el contador.
- **Nueva acción de seguridad:** añada un servicio en `source/security`, diálogo en `secui_*`, cadenas traducidas y persistencia en el backend.
- **Nueva exportación:** amplíe `geb_03_exportar_dados.cpp` o añada un módulo dedicado, manteniendo compatibilidad con `QTableView` y `QTableWidget`.

---

### 📝 Notas Importantes

#### Versiones y Toolchain

- La compilación principal requiere C++17.
- Los presets actuales apuntan a Qt 6.11.0.
- El preset MinGW utiliza `C:/Qt/Tools/mingw1310_64`.
- El preset MSVC utiliza Visual Studio 2022 Build Tools.
- El proyecto está orientado a Windows debido a su dependencia de APIs nativas de monitorización y metadados.

#### Archivos Críticos

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Base de datos principal de eventos.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Caché local de la capa de seguridad cuando se utiliza.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Preferencia de idioma.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Visibilidad de columnas.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Columnas coloreadas.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Colores configurados.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Preferencias de la UI de seguridad.

#### Registros (Logs)

Los registros se guardan con marca temporal en:

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Utilidades Auxiliares

- `mocks/find_VersionEditor.cpp` actualiza versiones y fechas en assets, `README.md`, `OBSERVACAO.md` y fragmentos del diálogo Acerca de.
- `mocks/compile_translations.cpp` ayuda en la compilación/validación de traducciones.
- Las utilidades utilizan su propio CMake en `mocks/`.

---

### 📄 Licencia y Autoría

**Autor:** Fernando Nillsson Cidade<br>
**Versión:** v2026.8.25.0<br>
**Licencia:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Para más información, consulte también `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, los assets integrados y la documentación específica en los módulos del código fuente.

---

</details>

---

<h2 id="frfr">🇫🇷 Français (FR)</h2>

> **Remarque :** Ce dépôt fait référence à la version **v2026.8.25.0** du Projet Linceu Lighthouse. Soutenez le projet et procurez-vous la version payante via le lien : [Installer via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Cliquez pour développer le README en français</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130451.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130510.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130527.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130546.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130603.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130620.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130641.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130659.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130719.png" alt="Linceu Lighthouse - Français" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/fr_FR/Captura%20de%20tela%202026-08-25%20130740.png" alt="Linceu Lighthouse - Français" width="320">
</p>

Projet : LINCEU LIGHTHOUSE  
Version : v2026.8.25.0<br>
Auteur : Fernando Nillsson Cidade<br>

Description : Plateforme Locale d'Intelligence, d'Historique et d'Audit d'Activités de Fichiers
> **« Sachez exactement ce qui est arrivé à vos fichiers — localement, visuellement et avec un historique consultable. »**

**Linceu Lighthouse** est une application de bureau native en **C++17 avec Qt6** développée pour offrir une visibilité absolue, une traçabilité historique et un audit déterministe sur le système de fichiers Windows. Au lieu d'agir comme un simple moniteur éphémère ou un outil restreint de sécurité, Linceu Lighthouse unifie trois piliers fondamentaux au sein d'une expérience fluide, intuitive et à haute performance :

1. **Surveillance Continue et Sémantique des Activités :** Suivi en temps réel des événements (création, modification, suppression, renommage, déplacement et analyse initiale), avec des algorithmes spécifiques de corrélation d'événements complexes — identifiant les opérations réelles de `RENAMED` et `MOVED` sans générer de fausses paires de suppression et de création.
2. **Historique Consultable et Intelligence des Fichiers sans IA :** Construction automatique d'une base de données relationnelle SQLite locale, enrichie de plus de 30 catégories de métadonnées avant et après chaque modification, accompagnée de 17 visualisations graphiques natives et de rapports statistiques déterministes.
3. **Sécurité Locale, Audit et Preuves :** Ligne de base cryptographique (baseline), calcul déterministe du score de risque, moteur de règles locales, quarantaine, restauration et exportation de rapports auditables sous de multiples formats standards du marché.

### 🛡️ Confidentialité Souveraine (Local-First)
* **100% Local et Hors Ligne :** Vos fichiers, métadonnées, historiques et rapports résident exclusivement sur votre machine (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Zéro Cloud, Zéro Télémétrie, Zéro Compte :** Aucune information personnelle ou opérationnelle n'est transmise sur le réseau.
* **Intelligence Déterministe sans IA :** L'ensemble des analyses, classifications et évaluations de risques repose sur une logique algorithmique et statistique pure — sans modèles de langage (LLM), sans RAG et sans dépendances externes.

---

### 🧭 Les 5 Modes de Fonctionnement

| Mode | Question / Focus Central | Bénéfice Principal |
|---|---|---|
| **Mode Surveillance** | *Que se passe-t-il actuellement ?* | Visualisation en temps réel des modifications dans les répertoires sélectionnés avec alertes visuelles instantanées par couleur. |
| **Mode Historique** | *Qu'est-il arrivé à mon fichier ?* | Consultation chronologique inverse (*File History* / *What Changed?*), retraçant la généalogie complète des éditions et renommages. |
| **Mode Analytics (Sans IA)** | *Quels sont les modèles d'usage ?* | Cartes thermiques (Heatmaps), heures de pointe, types de fichiers les plus affectés et flux de répertoires (Sankey). |
| **Mode Sécurité** | *Y a-t-il eu une modification suspecte ?* | Vérification par rapport à la ligne de base cryptographique, hachages, règles de conformité et intégrité structurelle. |
| **Mode Preuve** | *Comment prouver les modifications ?* | Exportation de rapports immuables et de packages d'audit pour la conformité interne ou l'investigation technique. |

---

### 🎯 Publics Cibles et Cas d'Utilisation

* **Développeurs et Ingénieurs Logiciels :** Suivre avec exactitude ce que les compilations, IDE, scripts de build, gestionnaires de paquets et outils Git créent, modifient ou suppriment dans les projets.
* **Administrateurs Système, Sysadmins et Support :** Diagnostiquer les fichiers disparus, les renommages accidentels dans les dossiers partagés et reconstruire la chronologie des incidents opérationnels.
* **Secteurs gérant des Données Sensibles (Juridique, Santé, Finance, Ingénierie) :** Maintenir un contrôle d'audit strict sur les dossiers contenant des données confidentielles, avec garantie de secret absolu et isolation réseau.
* **Créateurs de Contenu (Designers, Monteurs Vidéo, Photographes) :** Suivre les déplacements de lourdes bibliothèques d'assets, les processus de rendu et les flux complexes d'exportation.
* **Utilisateurs Généraux :** Protéger les répertoires essentiels (Documents, Téléchargements, disques cloud synchronisés) contre les suppressions accidentelles et les modifications non autorisées.

---

### ⚙️ Comment Fonctionne Linceu Lighthouse

```text
Système de Fichiers (Windows)
      ↓
APIs Natives Asynchrones (ReadDirectoryChangesW)
      ↓
File d'Événements Thread-Safe & Tampons de Pics
      ↓
Normalisation et Corrélation Sémantique (Détection Réelle de Move/Rename)
      ↓
Enrichissement par Métadonnées (30+ catégories, Magic Numbers, Hashes)
      ↓
Persistance Asynchrone par Lots dans SQLite Local
      ↓
Visualisation Interactive Qt6 | 17 Graphiques Nactifs | Exportation Multiformat
```

---

### 🛠️ Ressources et Outils Disponibles

- **Filtres Multidimensionnels :** Filtrage rapide par opération et filtres avancés prenant en charge les expressions, extensions, plages de dates et sélection dans le tableau.
- **Tableau d'Audit Enrichi :** Colonnes configurables, persistance des préférences de couleur par opération, identification MIME réelle traduite dynamiquement et métadonnées temporelles complètes pour dossiers et sous-dossiers.
- **17 Visualisations Graphiques Natives :** Camembert, Barres, Chronologie (Timeline), Carte d'Arbre (Treemap), Histogramme, Courbe de Pareto, Lignes, Boîte à Moustaches (Boxplot), Radar, Heatmap Temporel, Nuage de Points (Scatter), Diagrammes de Sankey, Dotplot et Arborescence Hiérarchique.
- **Exportation Souple des Preuves :** Génération de rapports en `.xlsx` (OpenXML natif), `.csv`, `.txt`, `.json`, `.xml` et base de données `.db`.
- **Internationalisation Complète :** Interface et documentation technique intégrées nativement en 6 langues (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` et `de_DE`).

---

MANUEL D'UTILISATION RAPIDE :

1. Lancez l'application pour ouvrir l'interface graphique.
2. Sélectionnez un répertoire à surveiller.
3. Démarrez la surveillance pour exécuter l'analyse initiale et suivre les modifications.
4. Utilisez les menus et boutons latéraux pour filtrer, exporter, visualiser les statistiques et consulter la sécurité.
5. Utilisez le double-clic ou l'action Ouvrir pour ouvrir les fichiers signalés dans le tableau avec l'application appropriée.
6. Enregistrez les rapports ou bases exportées lorsque vous devez archiver les résultats.

MANUEL D'UTILISATION DÉTAILLÉ :

Configuration requise :

- Windows 10 ou supérieur.
- Autorisation de lecture sur le répertoire surveillé.
- Autorisation d'écriture dans `%LOCALAPPDATA%\LinceuLighthouse` pour la base de données, les configurations et les journaux (logs).

Installation :

- Installez l'exécutable distribué par l'installateur du projet ou générez le binaire via le flux CMake décrit dans ce README.
- Pour les compilations locales, utilisez le preset compatible avec votre chaîne d'outils Qt6 : MinGW ou MSVC.

Configuration initiale :

- L'interface s'ouvre avec un tableau d'événements vide.
- La langue est chargée par le système de localisation et peut être modifiée depuis le menu des options.
- L'application crée les répertoires persistants lorsque nécessaire.
- Les journaux (logs) sont enregistrés dans `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Surveillance des répertoires :

1. Sélectionner un répertoire :
   - Cliquez sur `Sélectionner un Répertoire` ou utilisez `Fichier -> Sélectionner un Répertoire`.
   - Le chemin sélectionné est alors utilisé par l'observateur et par le flux d'analyse.

2. Démarrer ou arrêter la surveillance :
   - Cliquez sur `Démarrer/Arrêter` ou utilisez le menu correspondant.
   - L'analyse initiale peuple le snapshot et enregistre les événements `escaneado`.
   - Après l'analyse, la surveillance continue enregistre les modifications en temps réel.

3. Suspendre la surveillance ou l'analyse :
   - Le projet dispose de commandes séparées pour interrompre la surveillance, le scan ou les flux actifs.
   - L'état visuel est mis à jour par la couche de progression et de messages.

Interface principale :

- Barre de menus : fichier, paramètres, options, exportation, aide, sécurité et langue.
- Barre latérale : actions fréquentes telles que sélectionner, démarrer/arrêter, pause, effacer, exporter et statistiques.
- Libellé du répertoire : chemin surveillé.
- Libellé de résultat : messages opérationnels et retours de sécurité.
- Tableau de données : événements et métadonnées.
- Barre de progression : analyses et tâches longues.
- Boîtes de dialogue auxiliaires : filtres, couleurs, colonnes, statistiques, manuel, aide et à propos.

Champs d'événements les plus importants :

- Type d'opération.
- Nom.
- Répertoire précédent.
- Répertoire actuel.
- Dates de création, modification et accès.
- Dates complètes pour fichiers, dossiers, sous-dossiers et répertoires enfants.
- Type/extension.
- MIME traduit et identifiant MIME d'origine.
- Tailles en multiples unités.
- Attributs, auteur, dimensions, durée, débit binaire et protection.
- Métadonnées spécifiques pour documents, feuilles de calcul, présentations, bases de données, archives compressées et exécutables.

Filtres et recherche :

- Filtres rapides par type d'opération dans le menu.
- Filtres avancés par opération, extension, texte, plage de dates et combinaison de critères.
- Synchronisation avec le menu principal.
- Mise à jour du décompte après modification de filtre ou de langue.
- Persistance de l'état des cases à cocher des filtres.

Visualisation des statistiques :

- Accédez-y via le bouton de statistiques ou depuis le menu.
- Les graphiques sont générés par des modules C++ dans `source/data/GeradorEstatisticas`.
- La fenêtre permet la sélection des graphiques, la régénération, l'enregistrement et la mise à jour après un changement de langue.

Graphiques disponibles :

- Camembert (Pizza).
- Barres.
- Chronologie (Timeline).
- Treemap (Carte d'Arbre).
- Histogramme.
- Pareto.
- Ligne.
- Boxplot.
- Radar d'événements.
- Heatmap.
- Scatter (Nuage de points).
- Sankey.
- Radar.
- Dotplot.
- Sankey événement-chemin.
- Sankey type-chemin.
- Arborescence de répertoires.

Exportation de données :

- `Excel (.xlsx)` : package OpenXML assemblé directement par le code C++.
- `CSV (.csv)` : délimité par des virgules.
- `Texte (.txt)` : délimité par des tabulations.
- `JSON (.json)` : structure sérialisée avec en-têtes et lignes.
- `XML (.xml)` : sortie hiérarchique via `QXmlStreamWriter`.
- `SQLite (.db)` : base de données générée par la couche d'exportation.

Options d'exportation :

- Uniquement les colonnes actives.
- Uniquement les filtres actifs.
- Uniquement la sélection actuelle.
- Compatibilité avec `QTableWidget` et `QTableView`.

Configuration des colonnes et couleurs :

- Le menu des colonnes permet d'activer, masquer et restaurer des champs.
- Des colonnes comme MIME, Date de Création, Date de Modification et Date d'Accès peuvent être activées pour un audit plus détaillé.
- Le menu des couleurs permet de colorer par opération ou par colonne.
- Les configurations persistent dans des fichiers JSON locaux.
- Le tableau s'adapte aux changements de thème de Windows et invalide les caches visuels lorsque nécessaire.

Internationalisation :

- Langues prises en charge : portugais, anglais, espagnol, français, italien et allemand.
- Les fichiers `.ts` et `.qm` se trouvent dans `source/locale`.
- `LocalizadorQt` charge les traductions, enregistre les préférences et fournit un repli (fallback).
- Les opérations, métadonnées, MIME, menus, manuel, aide, à propos et textes de sécurité sont traduits.

Dépannage / Résolution des problèmes :

1. La surveillance ne démarre pas :
   - Vérifiez les autorisations du répertoire.
   - Évitez les répertoires système protégés.
   - Vérifiez si la base de données locale peut être créée.

2. Les événements n'apparaissent pas :
   - Vérifiez que la surveillance est active.
   - Effacez les filtres.
   - Testez en créant, renommant ou supprimant un simple fichier dans le répertoire surveillé.

3. Consommation élevée de ressources :
   - Évitez de surveiller des arborescences de répertoires excessivement volumineuses sans nécessité.
   - Privilégiez des sous-répertoires ciblés.
   - Exportez ou purgez les données anciennes lorsque la base de données devient trop volumineuse.

4. Statistiques vides :
   - Vérifiez que des événements sont bien chargés.
   - Supprimez les filtres susceptibles de masquer des lignes.
   - Vérifiez que `monitoramento.db` est accessible.

5. Le fichier ne s'ouvre pas :
   - Confirmez que le fichier existe toujours au chemin indiqué.
   - Vérifiez que Windows dispose d'une application associée à ce type de fichier.
   - Lorsque la boîte de dialogue de choix s'affiche, sélectionnez une autre application compatible.

6. Les dates des dossiers n'apparaissent pas :
   - Activez les colonnes Date de Création, Date de Modification et Date d'Accès.
   - Lancez une nouvelle analyse pour actualiser les dossiers, sous-dossiers et répertoires enfants.

## 📋 Documentation Technique

### Vue d'Ensemble du Système

**Linceu Lighthouse** est une application de bureau native pour Windows, écrite en **C++17** et basée sur **Qt6**. L'architecture actuelle remplace l'ancienne chaîne Python/PySide par des modules `.cpp` et `.h`, avec une compilation CMake, une persistance SQLite via Qt SQL, un rendu Qt Widgets/OpenGL/Quick et une intégration avec les APIs natives de Windows.

L'application est structurée en couches :

- `source/gui` : fenêtre principale, composition visuelle et intégration de haut niveau.
- `source/services` : observation des répertoires, événements, base de données, métadonnées et tableau.
- `source/ui` : menus, boutons, statistiques, filtres, manuel, aide, à propos et localisation.
- `source/data` : façade statistique, générateurs et infrastructure de graphiques.
- `source/tools` : filtres et administration des critères.
- `source/security` : fonctionnalités de sécurité, sessions, quarantaine, preuves et backend de stockage.
- `source/utils` : chemins, icônes et journalisation (logging).
- `source/assets` et `source/locale` : contenu intégré, licences, textes et traductions.

#### Technologies Principales

- **Langage :** C++17.
- **Build :** CMake, avec `CMAKE_AUTOMOC` et `CMAKE_AUTORCC`.
- **Framework :** Qt6.
- **UI :** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, menus et actions.
- **Base de Données :** SQLite via `QSqlDatabase` et `QSqlQuery`.
- **Concurrence :** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` et Qt Concurrent.
- **Graphiques :** infrastructure interne avec `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` et modèle typé `LgplChartModel`.
- **Exportation :** écriture interne de CSV, TXT, JSON, XML, SQLite et package `.xlsx` OpenXML.
- **Internationalisation :** Qt Linguist (`.ts` et `.qm`) avec six langues.
- **Windows :** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` pour le thème et bibliothèques `wintrust`, `version` et `crypt32`.
- **Sécurité :** couche C++ interne avec ligne de base cryptographique, règles, score de risque, quarantaine, preuves et cache local SQLite pour le backend de sécurité.

L'application principale ne dépend pas de l'environnement d'exécution Python, `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` ou de bibliothèques équivalentes. Toute mention de Python dans les fichiers de licence reflète une documentation tierce intégrée et non la pile d'exécution principale.

---

### 📁 Structure des Répertoires et Modules

Résumé validé dans l'arborescence `source/` :

| Couche | `.cpp` | `.h` | Responsabilité |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Initialisation principale et splash |
| `source/assets` | 0 | 0 | Textes, licences, images, icônes et contenu intégré |
| `source/data` | 26 | 27 | Statistiques, graphiques et rendu |
| `source/gui` | 8 | 8 | Fenêtre principale et composition visuelle |
| `source/locale` | 0 | 0 | Fichiers de traduction `.ts` et `.qm` |
| `source/security` | 24 | 24 | Sécurité, preuves, quarantaine et sessions |
| `source/services` | 162 | 163 | Observation, événements, base de données, métadonnées et tableau |
| `source/tools` | 23 | 24 | Filtres et gestion des filtres |
| `source/ui` | 125 | 125 | Menus, boutons, dialogues, manuel, statistiques et localisation |
| `source/utils` | 4 | 4 | Chemins, icônes et journaux (logs) |
| **Total dans `source/`** | **374** | **377** | Base C++ actuelle |

<details>
<summary><b>📂 source-root - Bootstrap de l'application</b></summary>

#### Modules Principaux

- `src_01_InicializadorMain.cpp/.h` - Point d'entrée réel de l'application après `main.cpp`, ajustement du répertoire de travail, identification Windows et repli d'initialisation.
- `src_02_SplashAppStarter.cpp/.h` - Initialisation avec écran de démarrage (splash), progression, icône, traductions, création d'`InterfaceMonitor` et démarrage anticipé du writer.

</details>

<details>
<summary><b>📂 source/gui/ - Couche d'Interface Graphique</b></summary>

#### Modules Principaux

- `ic_01_InterfaceMonitor.cpp/.h` - Fenêtre principale, propriétés partagées, conteneur de tableaux et intégration des sous-systèmes.
- `ic_02_Inicializador.cpp/.h` - Initialisation des widgets et objets de base.
- `ic_03_Configurador.cpp/.h` - Dispositions (layouts), composants et configuration visuelle.
- `ic_04_Atualizador.cpp/.h` - Mise à jour de l'interface pour le tableau, l'état et la progression.
- `ic_05_GerenciadorProgresso.cpp/.h` - Barres et retours de progression.
- `ic_06_GerenciadorMensagens.cpp/.h` - Messages, statut et communication avec l'utilisateur.
- `ic_07_ManipuladorTabela.cpp/.h` - Opérations sur le tableau et adaptation pour l'interface utilisateur.
- `ic_08_Internacionalizador.cpp/.h` - Intégration de l'interface avec le système de localisation.

Les anciens sous-répertoires `GerenciadorDesempenho/` et `GerenciadorEstruturaDiretoriosWidget/` n'existent plus dans cette arborescence. La documentation actuelle ne doit pas les traiter comme des sous-systèmes actifs.

</details>

<details>
<summary><b>📂 source/services/ - Cœur de Surveillance, Événements et Données</b></summary>

#### Modules Principaux

- `ob_01_Observador.cpp/.h` - Observateur de répertoire, threads, tampons et cycle de surveillance.
- `ob_02_BaseEvento.cpp/.h` - Base commune pour les événements et intégration avec SQLite.
- `ob_03_DiretorioScanner.cpp/.h` - Analyse initiale et construction du snapshot.
- `ob_04_EventoAdicionado.cpp/.h` - Enregistrement des fichiers créés.
- `ob_05_EventoExcluido.cpp/.h` - Enregistrement des suppressions et suppressions temporaires.
- `ob_06_EventoModificado.cpp/.h` - Consolidation des modifications.
- `ob_07_EventoRenomeado.cpp/.h` - Traitement des renommages.
- `ob_08_EventoMovido.cpp/.h` - Détection et réconciliation des déplacements.
- `ob_09_GerenciadorColunas.cpp/.h` - Colonnes disponibles et persistance de visibilité.
- `ob_10_GerenciadorTabela.cpp/.h` - Pont entre événements, tableau et interface utilisateur.

#### Sous-répertoires Spécialisés

- `GerenciamentoBaseEvento/` - Création de base de données, PRAGMAs, index, enregistrements, snapshot, suppressions, déplacements et `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Workers d'analyse, file de traitement, lots, progression et finalisation.
- `GerenciamentoEventoModificado/` - Intervalles de déduplication, cache, fichiers volumineux et traitement normal/massif.
- `GerenciamentoEventoMovido/` - Worker de déplacement, suppressions en attente, mise à jour des lignes et du tableau.
- `GerenciamentoMetadados/` - Extraction native de métadonnées pour code, images, audio, vidéo, documents, feuilles de calcul, présentations, exécutables, archives compressées, journaux et données structurées.
- `GerenciamentoObservador/` - Démarrage, arrêt, surveillance, tampon, détection d'opérations massives et pause.
- `GerenciamentoTabela/` - Thème, en-têtes, largeurs, couleurs, sélection, retraduction et `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menus, Dialogues, Actions et Localisation</b></summary>

#### Modules Principaux

- `ui_01_GerenciadorBotoes.cpp/.h` - Façade de boutons et actions rapides.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Construction visuelle des boutons.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Menus principaux et actions persistantes.
- `ui_04_GerenciadorEventosUI.cpp/.h` - Événements de l'interface utilisateur.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Retours visuels des opérations longues.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Fenêtre et flux de statistiques.
- `ui_07_GerenciadorDados.cpp/.h` - Exportation de données.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Intégration entre événements et interface.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Contrôle de la surveillance.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Nettoyage des données.
- `ui_11_DialogoCores.cpp/.h` - Sélection et persistance des couleurs.
- `ui_12_LocalizadorQt.cpp/.h` - Localisation et préférence linguistique.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Traduction des métadonnées.
- `ui_14_OpcoesSobre.cpp/.h` - Contenu d'à propos, licences, avis, politique de confidentialité et notes de version.
- `ui_15_Manual.cpp/.h` - Manuel intégré.

#### Sous-répertoires

- `GerenciamentoBotoes/` - Sélection de répertoire, bascule d'analyse, exportation, SQLite, nettoyage et pause.
- `GerenciamentoDialogoCores/` - Interface de sélection de couleurs, grille, sélecteur avancé et traductions.
- `GerenciamentoEstatisticasUI/` - Panneau, cases à cocher, workers, régénération, onglets, disposition et enregistrement de graphiques.
- `GerenciamentoEventosUI/` - Filtres, colonnes, couleurs, langue, nettoyage et réinitialisation.
- `GerenciamentoLocalizadorQt/` - Chargement des traducteurs, repli, fichiers `.ts/.qm` et traduction des opérations.
- `GerenciamentoMenusUI/` - Menus, sous-menus, exportation, langues, à propos, manuel et aide.
- `GerenciamentoTradutorMetadadosQt/` - Traductions des types de fichiers, attributs, auteur, protection et dimensions.
- `Manual/` - Documentation interne en six langues.

</details>

<details>
<summary><b>📂 source/data/ - Statistiques et Graphiques</b></summary>

#### Modules Principaux

- `st_01_GeradorEstatisticas.cpp/.h` - Façade pour la génération de statistiques.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Calcul géométrique de disposition.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Fabrique de widgets de graphiques.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Adaptation des payloads hérités.
- `Charts/ch_04_QmlRegistration.cpp/.h` - Enregistrement QML.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Élément Qt Quick.
- `Charts/ch_06_WidgetChartView.cpp/.h` - Visualisation via `QOpenGLWidget`.
- `Charts/ch_07_LgplChartModel.h` - Modèle typé de graphique.
- `Charts/ch_08_RasterChartView.cpp/.h` - Rendu QWidget avec `QPainter`.

#### 17 Générateurs de Graphiques Disponibles

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
<summary><b>📂 source/tools/ - Filtres et Administration</b></summary>

#### Modules de Filtres

- `fil_01_Filtros.cpp/.h` - Widget et logique principale de filtrage.
- `fil_02_AdministradorCalendario.cpp/.h` - Sélection de dates.
- `fil_03_AdministradorFiltros.cpp/.h` - Application combinée des filtres.

#### Sous-répertoires

- `GerenciamentoAdministradorFiltros/` - Application, effacement, synchronisation avec le menu, décomptes, langue et persistance.
- `GerenciamentoFiltros/` - Interface, modification de filtre, synchronisation, vérification d'opération et effacement.

</details>

<details>
<summary><b>📂 source/security/ - Sécurité, Preuves et Quarantaine</b></summary>

#### Modules Principaux

- `secore_02_SecurityManager.cpp/.h` - Orchestration de la couche de sécurité.
- `secore_01_postgres_utils.cpp/.h` - Backend Qt SQL, PostgreSQL configurable et cache local SQLite.
- `secore_05_SecurityDbWriter.cpp/.h` - Writer asynchrone de sécurité.
- `base_01_baseline_criptografico.cpp/.h` - Ligne de base cryptographique.
- `exec_01_inteligencia_executaveis.cpp/.h` - Intelligence pour exécutables.
- `sess_01_correlacao_sessoes.cpp/.h` - Corrélation des sessions.
- `sdiff_01_diff_conteudo.cpp/.h` - Différences de contenu.
- `attr_01_atribuicao_forense.cpp/.h` - Attribution médico-légale (forensique).
- `agre_01_motor_regras.cpp/.h` - Moteur de règles.
- `risk_01_score_risco.cpp/.h` - Score de risque.
- `quar_01_quarentena.cpp/.h` - Quarantaine et restauration.
- `evid_01_exportacao_evidencia.cpp/.h` - Packages de preuves.
- `secui_*` - Boîtes de dialogue et contrôleurs d'interface pour la sécurité.

#### Ressources

- Profils et multi-répertoires.
- Centre d'alertes.
- Sessions en temps réel.
- Quarantaine et restauration.
- Package de preuves.
- Notifications et configuration des pop-ups.
- Stockage local dans `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ et source/locale/ - Contenu Intégré</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` et `RELEASE`.
- Icônes `.ico` et images `.png`.
- Textes d'à propos, licences, avis, politique de confidentialité, historique et notes de version en plusieurs langues.

#### Fichiers de Traduction

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Répertoires Auxiliaires</b></summary>

- `mocks/` - Utilitaires auxiliaires, incluant `find_VersionEditor` et `compile_translations`.
- `repo/` - Scripts et fichiers d'empaquetage, incluant les installateurs Inno Setup.
- `.vscode/` - Configuration locale de l'EDI.
- `.github/workflows/` - Automatisation des mises à jour documentaires.
- `CMakePresets.json` - Presets principaux MinGW et MSVC.
- `mocks/CMakePresets.json` - Presets des utilitaires auxiliaires.

</details>

---

### 🏗️ Architecture et Patrons de Conception

#### Principes SOLID Appliqués

- Responsabilité unique via des modules numérotés et des sous-couches spécialisées.
- Façades pour les sous-systèmes complexes, tels que les statistiques, boutons, menus, événements et sécurité.
- Séparation nette entre observation, persistance, métadonnées, visualisation, exportation et traduction.
- Faible couplage grâce à l'utilisation des propriétés `QObject`, signaux/slots et interfaces de service.
- Extensibilité par de nouveaux modules `.cpp/.h` sans réécriture du noyau.

#### Patrons de Conception Implémentés

- **Facade :** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer / Boucle d'événements :** signaux/slots Qt, callbacks d'événements et notifications de surveillance.
- **Worker / Producer-Consumer :** files d'événements, `DatabaseWriter`, workers de graphiques et scanner.
- **Model / View :** `EventTableModel` avec `QTableView`, maintenant la compatibilité avec `QTableWidget`.
- **Factory :** création de widgets graphiques par `ChartWidgetFactory`.
- **Adapter :** `LegacyVariantChartAdapter` convertit les anciens formats de données vers les modèles actuels.
- **Repository / DAO local :** accès SQLite via des connexions Qt SQL aux noms uniques.
- **Strategy informelle :** générateurs dédiés pour chaque type de graphique et extracteurs séparés par type de fichier.

#### Multithreading et Performance

- Analyse initiale dans un worker Qt.
- Surveillance continue dans un `std::thread`.
- Traitement du tampon d'événements dans un thread dédié.
- Écriture des événements avec `DatabaseWriter`, file interne et persistance par lots.
- Extraction de métadonnées par lots avec `QThreadPool`.
- Génération de graphiques avec `GraficoWorker`.
- Cache d'étiquettes, de pinceaux (brushes) et de traductions dans `EventTableModel`.
- Mise à jour visuelle asynchrone pour éviter les blocages de l'interface utilisateur.

#### Optimisations de Base de Données

- SQLite en mode WAL.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` et limite de journal dans le writer.
- Tables par type d'événement et table consolidée `monitoramento`.
- Table `snapshot` pour la baseline.
- Index par nom, répertoires, horodatage (timestamp) et clés fréquentes.
- Insertion par lots avec `execBatch()` et transactions.

#### Architecture des Événements

1. `Observador` détecte les modifications dans le répertoire.
2. Les événements bruts sont classés en : ajouté, supprimé, modifié, renommé, déplacé ou scanné.
3. Des sous-modules spécialisés dédupliquent le bruit et réconcilient les opérations complexes.
4. Les métadonnées sont extraites le cas échéant.
5. Les événements sont transmis à l'interface graphique et au writer.
6. La base de données locale permet de recharger, filtrer, exporter et générer des statistiques.

#### Extraction de Métadonnées

- `QImageReader` pour les images.
- Inspection de fichiers ZIP/OpenXML pour documents, feuilles de calcul et présentations.
- `QSqlDatabase` pour SQLite et bases de données supportées par Qt SQL.
- Lecture des en-têtes pour l'identification des formats.
- APIs Windows pour les exécutables, signatures et versions.
- Calculs de taille en B, Ko, Mo, Go et To.
- Métadonnées spécialisées pour journaux, sauvegardes, archives compressées, OLE, données structurées et code source.

#### Sécurité

La couche `source/security` étend la surveillance avec des capacités de réponse et d'investigation :

- Profils de répertoires et surveillance multiple.
- Alertes dérivées de règles.
- Corrélation de sessions.
- Ligne de base cryptographique.
- Quarantaine et restauration d'éléments.
- Packages de preuves.
- Score de risque.
- Backend configurable via Qt SQL, avec cache local SQLite lorsque nécessaire.

---

### 🔄 Flux d'Exécution Principal

1. `main.cpp` appelle `source::iniciar_aplicacao(argc, argv)`.
2. `src_01_InicializadorMain` ajuste le répertoire de travail, l'identité Windows et le repli d'initialisation.
3. `src_02_SplashAppStarter` crée `QApplication`, charge l'icône, les traductions et l'écran de démarrage.
4. `InterfaceMonitor` assemble la fenêtre principale et enregistre les propriétés partagées.
5. Les gestionnaires de boutons, menus, événements, progression, données et statistiques sont créés.
6. La couche de localisation définit la langue et les traducteurs.
7. La couche de sécurité démarre son manager et son writer.
8. L'utilisateur sélectionne un répertoire.
9. `Observador` e `DiretorioScanner` exécutent l'analyse initiale.
10. Le snapshot et les événements `escaneado` sont persistés.
11. La surveillance continue utilise `CreateFileW` et `ReadDirectoryChangesW`.
12. Les événements sont classés, enrichis et mis en file d'attente.
13. `DatabaseWriter` persiste par lots.
14. Le tableau, les filtres, les compteurs et les couleurs sont mis à jour.
15. Les statistiques, exportations et dialogues consultent l'état actuel.
16. À la fermeture, les threads et les connexions sont arrêtés de manière ordonnée.

---

### 📦 Prérequis et Dépendances

#### Bibliothèques Principales

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
- Bibliothèques Windows : `wintrust`, `version`, `crypt32`.

#### Prérequis de Compilation (Build)

- CMake 4.1 ou supérieur pour la compilation principale.
- CMake 3.24 ou supérieur pour les utilitaires dans `mocks/`.
- Qt 6.11.0 dans les presets actuels.
- MinGW 13.1.0 ou Visual Studio 2022 Build Tools, selon le preset.
- Windows SDK compatible avec la chaîne d'outils sélectionnée.

#### Presets CMake

Configurer et compiler avec MinGW :

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Configurer et compiler avec MSVC Debug :

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Configurer et compiler avec MSVC Release :

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Compiler l'utilitaire d'édition de version :

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Déploiement (Deploy)

Sous Windows, `CMakeLists.txt` tente de localiser `windeployqt` et copie :

- l'exécutable final nommé `Linceu_Lighthouse` ;
- les dépendances Qt ;
- `source/assets` ;
- les fichiers `.qm` de l'application ;
- les traductions de Qt dans `locale/qt`.

---

### ✨ Fonctionnalités Principales

<details>
<summary><b>Surveillance de Fichiers</b></summary>

- Analyse initiale récursive.
- Snapshot persistant.
- Surveillance en temps réel via l'API native de Windows.
- Événements : ajouté, supprimé, modifié, renommé, déplacé et scanné.
- Déduplication et réconciliation des événements complexes.
- Détection des opérations massives.
- Pause et arrêt de l'analyse / surveillance.

</details>

<details>
<summary><b>Base de Données et Persistance</b></summary>

- `monitoramento.db` dans `%LOCALAPPDATA%\LinceuLighthouse`.
- Tables par type d'événement.
- Table consolidée pour consultation et exportation.
- Index optimisés.
- Écriture asynchrone par lots.
- Configurations locales en JSON et INI.
- Journaux horodatés (timestamped).

</details>

<details>
<summary><b>Métadonnées</b></summary>

- Type de fichier.
- Taille en plusieurs unités.
- Attributs.
- Auteur.
- Dimensions.
- Durée.
- Débit binaire (bitrate).
- Protection.
- Pages, lignes, mots, diapositives, feuilles de calcul, tables et enregistrements lorsque applicable.
- Signature et version des exécutables sous Windows.

</details>

<details>
<summary><b>Visualisation et Tableau</b></summary>

- `QTableView` avec `EventTableModel`.
- Compatibilité transitoire avec `QTableWidget`.
- Tri.
- Visibilité des colonnes.
- Couleurs par opération et par colonne.
- Thème clair/sombre de Windows.
- Retraduction des données existantes.
- Mise à jour incrémentielle des événements.

</details>

<details>
<summary><b>Statistiques et Graphiques</b></summary>

- 17 générateurs spécialisés.
- Rendu matriciel (raster) avec `QPainter`.
- Backend OpenGL via `QOpenGLWidget`.
- Intégration Qt Quick/QML.
- Panneau de sélection.
- Régénération après changement de langue.
- Sauvegarde de graphiques.

</details>

<details>
<summary><b>Filtres Avancés</b></summary>

- Opération.
- Extension.
- Texte.
- Plage de dates.
- Combinaison de filtres.
- Synchronisation avec les menus.
- Décompte actualisé.
- Persistance de l'état.

</details>

<details>
<summary><b>Exportation de Données</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Texte `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Exportation de la sélection, des colonnes actives et des filtres actifs.
- Prise en charge des données provenant du model/view.

</details>

<details>
<summary><b>Internationalisation</b></summary>

- Six langues.
- Traduction de l'interface, des opérations et des métadonnées.
- Repli textuel (fallback).
- Préférence persistée.
- Manuel et contenu institutionnel intégrés.
- Boîte de dialogue À propos réactive au changement de langue.

</details>

<details>
<summary><b>Sécurité</b></summary>

- Multi-répertoires et profils.
- Centre d'alertes.
- Sessions en temps réel.
- Quarantaine.
- Restauration.
- Packages de preuves.
- Notifications.
- Score de risque et règles.

</details>

---

### 🎯 Bonnes Pratiques Implémentées

#### Code Propre

- Organisation par couches et modules numérotés.
- Classes et fonctions aux responsabilités délimitées.
- Utilisation cohérente des espaces de noms (namespaces).
- En-têtes `.h` séparés des implémentations `.cpp`.
- Replis prévus en cas de défaillance d'interface, de logging ou d'initialisation.
- Commentaires ciblés dans les zones complexes.

#### Performance

- Surveillance native au lieu du polling.
- Écriture SQLite par lots.
- Index sur les champs fréquemment interrogés.
- Workers et threads pour les tâches lourdes.
- Caches de traduction, libellés et couleurs.
- Mise à jour incrémentielle du tableau.
- Évite les dépendances d'analyse lourdes à l'exécution.

#### Maintenabilité

- Couches clairement séparées.
- Petits modules pour les flux spécifiques.
- Compilation déclarative avec CMake.
- Presets pour les chaînes d'outils.
- Assets et traductions versionnés.
- Utilitaires isolés dans `mocks/`.
- Documents techniques dans le dépôt.

#### Sécurité et Robustesse

- Utilisation du répertoire persistant local de l'utilisateur.
- Connexions SQLite aux noms uniques.
- Fermeture ordonnée des threads.
- Journaux d'erreurs et de diagnostic.
- Traitement défensif avec `try/catch` aux limites de l'interface graphique.
- Validation des pointeurs avant utilisation dans les intégrations Qt.

---

### 🚀 Pour Ajouter de Nouvelles Fonctionnalités

- **Nouveau type de graphique :** créez un nouveau générateur dans `source/data/GeradorEstatisticas/`, en héritant de `gst_01_BaseGerador` et en enregistrant l'option dans l'interface de statistiques.
- **Nouveau type d'événement :** ajoutez une classe ou fonction spécialisée dans `source/services`, mettez à jour les colonnes si nécessaire et enregistrez la persistance dans le flux de `BaseEvento`.
- **Nouvelle colonne :** mettez à jour `ob_09_GerenciadorColunas`, le schéma dans `gbank_02_criar_banco_de_dados`, l'exportation et les traductions.
- **Nouvelle métadonnée :** implémentez un extracteur dans `GerenciamentoMetadados`, ajoutez la colonne, la traduction et la logique d'affichage.
- **Nouvelle langue :** créez `linceu_XX.ts`, compilez en `.qm`, ajoutez le repli dans `LocalizadorQt` et traduisez les assets si nécessaire.
- **Nouveau filtre :** implémentez dans `source/tools/GerenciamentoAdministradorFiltros` et synchronisez avec le menu et le décompte.
- **Nouvelle action de sécurité :** ajoutez un service dans `source/security`, un dialogue dans `secui_*`, les chaînes traduites et la persistance dans le backend.
- **Nouvelle exportation :** étendez `geb_03_exportar_dados.cpp` ou ajoutez un module dédié, en conservant la compatibilité avec `QTableView` et `QTableWidget`.

---

### 📝 Remarques Importantes

#### Versions et Toolchain

- La compilation principale requiert C++17.
- Les presets actuels ciblent Qt 6.11.0.
- Le preset MinGW utilise `C:/Qt/Tools/mingw1310_64`.
- Le preset MSVC utilise Visual Studio 2022 Build Tools.
- Le projet est orienté Windows en raison de sa dépendance aux APIs natives de surveillance et de métadonnées.

#### Fichiers Critiques

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Base de données principale des événements.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Cache local de la couche de sécurité lorsqu'elle est utilisée.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Préférence linguistique.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Visibilité des colonnes.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Colonnes colorées.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Couleurs configurées.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Préférences de l'interface de sécurité.

#### Journaux (Logs)

Les journaux sont enregistrés avec horodatage dans :

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Utilitaires Auxiliaires

- `mocks/find_VersionEditor.cpp` met à jour les versions et dates dans les assets, `README.md`, `OBSERVACAO.md` et certains passages de la boîte À propos.
- `mocks/compile_translations.cpp` facilite la compilation et la validation des traductions.
- Les utilitaires utilisent leur propre CMake dans `mocks/`.

---

### 📄 Licence et Auteurs

**Auteur :** Fernando Nillsson Cidade<br>
**Version :** v2026.8.25.0<br>
**Licence :** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Pour plus d'informations, consultez également `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, les assets intégrés et la documentation spécifique dans les modules du code source.

---

</details>

---

<h2 id="itit">🇮🇹 Italiano (IT)</h2>

> **Nota:** Questo repository fa riferimento alla versione **v2026.8.25.0** del Progetto Linceu Lighthouse. Sostieni il progetto e acquista la versione a pagamento tramite il link: [Installa via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Fai clic per espandere il README in italiano</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20130824.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20130842.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20130859.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20130917.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20130953.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20131018.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20131036.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20131055.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20131115.png" alt="Linceu Lighthouse - Italiano" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/it_IT/Captura%20de%20tela%202026-08-25%20131136.png" alt="Linceu Lighthouse - Italiano" width="320">
</p>

Progetto: LINCEU LIGHTHOUSE  
Versione: v2026.8.25.0<br>
Autore: Fernando Nillsson Cidade<br>

Descrizione: Piattaforma Locale di Intelligence, Cronologia e Audit delle Attività sui File
> **"Scopri esattamente cosa è successo ai tuoi file — localmente, visivamente e con cronologia consultabile."**

**Linceu Lighthouse** è un'applicazione desktop nativa in **C++17 con Qt6** progettata per offrire visibilità assoluta, tracciamento storico e audit deterministico sul file system di Windows. Invece di operare come un semplice monitor effimero o un limitato strumento di sicurezza, Linceu Lighthouse unisce tre pilastri fondamentali in un'esperienza agile, intuitiva e ad alte prestazioni:

1. **Monitoraggio Continuo e Semantico delle Attività:** Tracciamento in tempo reale degli eventi (creazione, modifica, eliminazione, ridenominazione, spostamento e scansione iniziale), con algoritmi specifici per la correlazione di eventi complessi — identificando operazioni reali di `RENAMED` e `MOVED` senza generare false coppie di eliminazione e creazione.
2. **Cronologia Consultabile e Intelligence dei File senza IA:** Costruzione automatica di un database relazionale SQLite locale, arricchito con oltre 30 categorie di metadati prima e dopo ciascuna modifica, accompagnato da 17 visualizzazioni grafiche native e report statistici deterministici.
3. **Sicurezza Locale, Audit e Prove Forensi:** Baseline crittografica, calcolo deterministico del punteggio di rischio, motore di regole locali, quarantena, ripristino ed esportazione di report verificabili in molteplici formati standard di settore.

### 🛡️ Privacy Sovrana (Local-First)
* **100% Locale e Offline:** I tuoi file, metadati, cronologie e report risiedono esclusivamente sul tuo computer (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Zero Cloud, Zero Telemetria, Zero Account:** Nessuna informazione personale o operativa viene mai trasmessa in rete.
* **Intelligence Deterministica senza IA:** Tutte le analisi, classificazioni e valutazioni di rischio utilizzano pura logica algoritmica e statistica — senza modelli linguistici (LLM), senza RAG e senza dipendenze esterne.

---

### 🧭 Le 5 Modalità Operative

| Modalità | Domanda / Focus Principale | Beneficio Principale |
|---|---|---|
| **Modo Monitoraggio** | *Cosa sta accadendo adesso?* | Visualizzazione in tempo reale delle modifiche nelle directory selezionate con segnalazione cromatica istantanea. |
| **Modo Cronologia** | *Cosa è successo al mio file?* | Consultazione cronologica inversa (*File History* / *What Changed?*), mostrando l'albero genealogico completo di modifiche e ridenominazioni. |
| **Modo Analytics (Senza IA)** | *Quali sono i pattern di utilizzo?* | Mappe termiche (Heatmap), orari di picco, tipi di file più modificati e flussi di directory (Sankey). |
| **Modo Sicurezza** | *C'è stata qualche modifica sospetta?* | Verifica rispetto alla baseline crittografica, hash, regole di conformità e integrità strutturale. |
| **Modo Prove** | *Come certificare le modifiche?* | Esportazione di report immutabili e pacchetti di audit per conformità interna o indagini tecniche. |

---

### 🎯 Destinatari e Casi d'Uso

* **Sviluppatori e Ingegneri del Software:** Tracciare con esattezza cosa creano, modificano o eliminano build, IDE, script, gestori di pacchetti e comandi Git all'interno dei progetti.
* **Amministratori di Sistema, Sysadmin e Supporto:** Diagnosticare file scomparsi, ridenominazioni accidentali in cartelle condivise e ricostruire la cronologia degli incidenti operativi.
* **Settori con Dati Riservati (Legale, Sanitario, Finanziario, Ingegneria):** Mantenere un controllo di audit rigoroso su cartelle riservate con garanzia di riservatezza totale e isolamento di rete.
* **Creator di Contenuti (Designer, Video Editor, Fotografi):** Monitorare gli spostamenti di librerie di asset pesanti, processi di rendering e flussi complessi di esportazione.
* **Utenti Generici:** Proteggere directory essenziali (Documenti, Download, unità cloud sincronizzate) contro eliminazioni accidentali e modifiche non autorizzate.

---

### ⚙️ Come Funziona Linceu Lighthouse

```text
File System (Windows)
      ↓
API Native Asincrone (ReadDirectoryChangesW)
      ↓
Coda di Eventi Thread-Safe e Buffer di Picco
      ↓
Normalizzazione e Correlazione Semantica (Rilevamento Reale di Move/Rename)
      ↓
Arricchimento con Metadati (30+ categorie, Magic Numbers, Hash)
      ↓
Persistenza Asincrona Batch in SQLite Locale
      ↓
Visualizzazione Interattiva Qt6 | 17 Grafici Nativi | Esportazione Multiformato
```

---

### 🛠️ Strumenti e Risorse Disponibili

- **Filtri Multidimensionali:** Filtraggio rapido per operazione e filtri avanzati con supporto per espressioni, estensioni, intervalli di date e selezione nella tabella.
- **Tabella di Audit Arricchita:** Colonne configurabili, persistenza delle preferenze di colore per operazione, identificazione MIME reale tradotta dinamicamente e metadati temporali completi per cartelle e sottocartelle.
- **17 Visualizzazioni Grafiche Native:** Torta, Barre, Cronologia (Timeline), Mappa ad Albero (Treemap), Istogramma, Curva di Pareto, Linee, Boxplot, Radar, Heatmap Temporale, Dispersione (Scatter), Diagrammi di Sankey, Dotplot e Albero Gerarchico.
- **Esportazione Flessibile delle Prove:** Generazione di report in `.xlsx` (OpenXML nativo), `.csv`, `.txt`, `.json`, `.xml` e database `.db`.
- **Internazionalizzazione Completa:** Interfaccia e documentazione tecnica integrate nativamente in 6 lingue (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` e `de_DE`).

---

MANUALE DI UTILIZZO RAPIDO:

1. Esegui l'applicazione per aprire l'interfaccia grafica.
2. Seleziona una directory da monitorare.
3. Avvia il monitoraggio per eseguire la scansione iniziale e seguire le modifiche.
4. Usa i menu e i pulsanti laterali per filtrare, esportare, visualizzare statistiche e consultare la sicurezza.
5. Fai doppio clic o usa l'azione Apri per aprire i file segnalati nella tabella con l'applicazione corretta.
6. Salva i report o i database esportati quando è necessario archiviare i risultati.

MANUALE DI UTILIZZO DETTAGLIATO:

Requisiti di sistema:

- Windows 10 o superiore.
- Permesso di lettura nella directory monitorata.
- Permesso di scrittura in `%LOCALAPPDATA%\LinceuLighthouse` per database, configurazioni e log.

Installazione:

- Installa l'eseguibile distribuito tramite il programma di installazione o genera il binario con la procedura CMake descritta in questo README.
- Nelle compilazioni locali, usa il preset compatibile con la tua toolchain Qt6: MinGW o MSVC.

Configurazione iniziale:

- L'interfaccia si avvia con la tabella degli eventi vuota.
- La lingua viene caricata dal sistema di localizzazione e può essere modificata dal menu opzioni.
- L'applicazione crea directory persistenti quando necessario.
- I log vengono registrati in `%LOCALAPPDATA%\LinceuLighthouse\logs`.

Monitoraggio delle directory:

1. Selezionare una directory:
   - Fai clic su `Seleziona Directory` o usa `File -> Seleziona Directory`.
   - Il percorso selezionato verrà utilizzato dall'osservatore e dal flusso di scansione.

2. Avviare o arrestare il monitoraggio:
   - Fai clic su `Avvia/Arresta` o usa il menu corrispondente.
   - La scansione iniziale popola lo snapshot e registra eventi `escaneado`.
   - Dopo la scansione, il monitoraggio continuo inizia a registrare le modifiche in tempo reale.

3. Sospendere il monitoraggio o la scansione:
   - Il progetto dispone di controlli separati per interrompere monitoraggio, scansione o flussi attivi.
   - Lo stato visivo viene aggiornato dal livello di avanzamento e messaggi.

Interfaccia principale:

- Barra dei menu: file, impostazioni, opzioni, esportazione, guida, sicurezza e lingua.
- Barra laterale: azioni frequenti come selezionare, avviare/arrestare, pausa, cancella, esportare e statistiche.
- Etichetta directory: percorso monitorato.
- Etichetta risultato: messaggi operativi e feedback di sicurezza.
- Tabella dati: eventi e metadati.
- Barra di avanzamento: scansioni e operazioni lunghe.
- Finestre di dialogo ausiliarie: filtri, colori, colonne, statistiche, manuale, guida e informazioni.

Campi degli eventi più importanti:

- Tipo di operazione.
- Nome.
- Directory precedente.
- Directory corrente.
- Date di creazione, modifica e accesso.
- Date complete per file, cartelle, sottocartelle e directory figlie.
- Tipo/estensione.
- MIME tradotto e identificatore MIME originale.
- Dimensioni in molteplici unità.
- Attributi, autore, dimensioni, durata, bitrate e protezione.
- Metadati specifici per documenti, fogli di calcolo, presentazioni, database, archivi compressi ed eseguibili.

Filtri e ricerca:

- Filtri rapidi per tipo di operazione nel menu.
- Filtri avanzati per operazione, estensione, testo, intervallo di date e combinazione di criteri.
- Sincronizzazione con il menu principale.
- Aggiornamento del conteggio dopo la modifica di filtri o lingua.
- Persistenza dello stato delle caselle di controllo dei filtri.

Visualizzazione delle statistiche:

- Accedi tramite il pulsante statistiche o dal menu.
- I grafici sono generati da moduli C++ in `source/data/GeradorEstatisticas`.
- La finestra consente la selezione dei grafici, la rigenerazione, il salvataggio e l'aggiornamento dopo il cambio di lingua.

Grafici disponibili:

- Torta (Pizza).
- Barre.
- Cronologia (Timeline).
- Treemap (Mappa ad Albero).
- Istogramma.
- Pareto.
- Linea.
- Boxplot.
- Radar degli eventi.
- Heatmap.
- Scatter (Dispersione).
- Sankey.
- Radar.
- Dotplot.
- Sankey evento-percorso.
- Sankey tipo-percorso.
- Albero delle directory.

Esportazione dei dati:

- `Excel (.xlsx)`: pacchetto OpenXML generato direttamente dal codice C++.
- `CSV (.csv)`: delimitato da virgole.
- `Testo (.txt)`: delimitato da tabulazioni.
- `JSON (.json)`: struttura serializzata con intestazioni e righe.
- `XML (.xml)`: output gerarchico tramite `QXmlStreamWriter`.
- `SQLite (.db)`: database generato dal layer di esportazione.

Opzioni di esportazione:

- Solo colonne attive.
- Solo filtri attivi.
- Solo selezione corrente.
- Compatibilità con `QTableWidget` e `QTableView`.

Configurazione di colonne e colori:

- Il menu delle colonne consente di attivare, nascondere e ripristinare i campi.
- Colonne come MIME, Data di Creazione, Data di Modifica e Data di Accesso possono essere abilitate per un audit più dettagliato.
- Il menu dei colori consente di colorare per operazione o per colonna.
- Le impostazioni persistono in file JSON locali.
- La tabella reagisce al cambio di tema di Windows e invalida le cache visive quando necessario.

Internazionalizzazione:

- Lingue supportate: portoghese, inglese, spagnolo, francese, italiano e tedesco.
- I file `.ts` e `.qm` si trovano in `source/locale`.
- `LocalizadorQt` carica le traduzioni, salva le preferenze e fornisce fallback.
- Operazioni, metadati, MIME, menu, manuale, guida, informazioni e testi di sicurezza sono tradotti.

Risoluzione dei problemi:

1. Il monitoraggio non si avvia:
   - Verifica i permessi della directory.
   - Evita directory protette di sistema.
   - Verifica che il database locale possa essere creato.

2. Gli eventi non compaiono:
   - Verifica che il monitoraggio sia attivo.
   - Cancella i filtri.
   - Prova a creare, rinominare o eliminare un semplice file nella directory monitorata.

3. Elevato consumo di risorse:
   - Evita di monitorare alberi di directory eccessivamente ampi senza necessità.
   - Preferisci sottodirectory mirate.
   - Esporta o ripulisci i dati vecchi quando il database cresce troppo.

4. Statistiche vuote:
   - Verifica che siano presenti eventi caricati.
   - Rimuovi i filtri che potrebbero nascondere le righe.
   - Verifica che `monitoramento.db` sia accessibile.

5. Il file non si apre:
   - Verifica che il file esista ancora nel percorso visualizzato.
   - Controlla che Windows abbia un'applicazione associata a quel tipo di file.
   - Quando compare la finestra di dialogo, seleziona un'altra applicazione compatibile.

6. Le date delle cartelle non compaiono:
   - Abilita le colonne Data di Creazione, Data di Modifica e Data di Accesso.
   - Esegui una nuova scansione per aggiornare cartelle, sottocartelle e directory figlie.

## 📋 Documentazione Tecnica

### Panoramica del Sistema

**Linceu Lighthouse** è un'applicazione desktop nativa per Windows, scritta in **C++17** e basata su **Qt6**. L'architettura attuale sostituisce la vecchia catena Python/PySide con moduli `.cpp` e `.h`, con compilazione CMake, persistenza SQLite tramite Qt SQL, rendering Qt Widgets/OpenGL/Quick e integrazione con le API native di Windows.

L'applicazione è strutturata a livelli:

- `source/gui`: finestra principale, composizione visiva e integrazione di alto livello.
- `source/services`: osservazione delle directory, eventi, database, metadati e tabella.
- `source/ui`: menu, pulsanti, statistiche, filtri, manuale, guida, informazioni e localizzazione.
- `source/data`: facciata statistica, generatori e infrastruttura dei grafici.
- `source/tools`: filtri e gestione dei criteri.
- `source/security`: funzionalità di sicurezza, sessioni, quarantena, prove e backend di archiviazione.
- `source/utils`: percorsi, icone e logging.
- `source/assets` e `source/locale`: contenuti integrati, licenze, testi e traduzioni.

#### Tecnologie Principali

- **Linguaggio:** C++17.
- **Build:** CMake, con `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- **Framework:** Qt6.
- **UI:** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, menu e azioni.
- **Database:** SQLite tramite `QSqlDatabase` e `QSqlQuery`.
- **Concorrenza:** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` e Qt Concurrent.
- **Grafici:** infrastruttura proprietaria con `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` e modello tipizzato `LgplChartModel`.
- **Esportazione:** scrittura nativa di CSV, TXT, JSON, XML, SQLite e pacchetto `.xlsx` OpenXML.
- **Internazionalizzazione:** Qt Linguist (`.ts` e `.qm`) con sei lingue.
- **Windows:** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` per il tema e librerie `wintrust`, `version` e `crypt32`.
- **Sicurezza:** livello C++ proprietario con baseline crittografica, regole, punteggio di rischio, quarantena, prove e cache locale SQLite per il backend di sicurezza.

L'applicazione principale non dipende dal runtime Python, `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` o librerie analoghe. Qualsiasi menzione a Python nei file di licenza riflette documentazione di terze parti integrata e non lo stack di esecuzione principale.

---

### 📁 Struttura delle Directory e Moduli

Riepilogo verificato nell'albero `source/`:

| Livello | `.cpp` | `.h` | Responsabilità |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Inizializzazione principale e splash |
| `source/assets` | 0 | 0 | Testi, licenze, immagini, icone e contenuti integrati |
| `source/data` | 26 | 27 | Statistiche, grafici e rendering |
| `source/gui` | 8 | 8 | Finestra principale e composizione visiva |
| `source/locale` | 0 | 0 | File `.ts` e `.qm` di traduzione |
| `source/security` | 24 | 24 | Sicurezza, prove, quarantena e sessioni |
| `source/services` | 162 | 163 | Osservazione, eventi, database, metadati e tabella |
| `source/tools` | 23 | 24 | Filtri e gestione filtri |
| `source/ui` | 125 | 125 | Menu, pulsanti, dialoghi, manuale, statistiche e localizzazione |
| `source/utils` | 4 | 4 | Percorsi, icone e log |
| **Totale in `source/`** | **374** | **377** | Base C++ attuale |

<details>
<summary><b>📂 source-root - Bootstrap dell'applicazione</b></summary>

#### Moduli Principali

- `src_01_InicializadorMain.cpp/.h` - Punto di ingresso reale dell'applicazione dopo `main.cpp`, correzione della directory di lavoro, identificazione Windows e fallback di inizializzazione.
- `src_02_SplashAppStarter.cpp/.h` - Inizializzazione con splash screen, avanzamento, icona, traduzioni, creazione di `InterfaceMonitor` e avvio anticipato del writer.

</details>

<details>
<summary><b>📂 source/gui/ - Livello Interfaccia Grafica</b></summary>

#### Moduli Principali

- `ic_01_InterfaceMonitor.cpp/.h` - Finestra principale, proprietà condivise, contenitore di tabelle e integrazione dei sottosistemi.
- `ic_02_Inicializador.cpp/.h` - Inizializzazione dei widget e degli oggetti di base.
- `ic_03_Configurador.cpp/.h` - Layout, componenti e configurazione visiva.
- `ic_04_Atualizador.cpp/.h` - Aggiornamento dell'interfaccia per tabella, stato e avanzamento.
- `ic_05_GerenciadorProgresso.cpp/.h` - Barre e feedback di avanzamento.
- `ic_06_GerenciadorMensagens.cpp/.h` - Messaggi, stato e comunicazione con l'utente.
- `ic_07_ManipuladorTabela.cpp/.h` - Operazioni sulla tabella e adattamento per l'interfaccia.
- `ic_08_Internacionalizador.cpp/.h` - Integrazione dell'interfaccia con il sistema di localizzazione.

Le vecchie sottodirectory `GerenciadorDesempenho/` e `GerenciadorEstruturaDiretoriosWidget/` non esistono più in questo albero. La documentazione corrente non deve trattarle come sottosistemi attivi.

</details>

<details>
<summary><b>📂 source/services/ - Cuore di Monitoraggio, Eventi e Dati</b></summary>

#### Moduli Principali

- `ob_01_Observador.cpp/.h` - Osservatore di directory, thread, buffer e ciclo di monitoraggio.
- `ob_02_BaseEvento.cpp/.h` - Base comune per gli eventi e integrazione con SQLite.
- `ob_03_DiretorioScanner.cpp/.h` - Scansione iniziale e costruzione dello snapshot.
- `ob_04_EventoAdicionado.cpp/.h` - Registrazione dei file creati.
- `ob_05_EventoExcluido.cpp/.h` - Registrazione delle eliminazioni ed eliminazioni temporanee.
- `ob_06_EventoModificado.cpp/.h` - Consolidamento delle modifiche.
- `ob_07_EventoRenomeado.cpp/.h` - Gestione delle ridenominazioni.
- `ob_08_EventoMovido.cpp/.h` - Rilevamento e riconciliazione degli spostamenti.
- `ob_09_GerenciadorColunas.cpp/.h` - Colonne disponibili e persistenza della visibilità.
- `ob_10_GerenciadorTabela.cpp/.h` - Ponte tra eventi, tabella e interfaccia utente.

#### Sottodirectory Specializzate

- `GerenciamentoBaseEvento/` - Creazione del database, PRAGMA, indici, record, snapshot, eliminazioni, spostamenti e `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Worker di scansione, coda di elaborazione, batch, avanzamento e finalizzazione.
- `GerenciamentoEventoModificado/` - Intervalli di deduplicazione, cache, file grandi ed elaborazione normale/massiva.
- `GerenciamentoEventoMovido/` - Worker di spostamento, eliminazioni in sospeso, aggiornamento di righe e tabella.
- `GerenciamentoMetadados/` - Estrazione nativa dei metadati per codice, immagini, audio, video, documenti, fogli di calcolo, presentazioni, eseguibili, compressi, log e dati strutturati.
- `GerenciamentoObservador/` - Avvio, arresto, monitoraggio, buffer, rilevamento operazioni massive e pausa.
- `GerenciamentoTabela/` - Tema, intestazioni, larghezze, colori, selezione, ritraduzione ed `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menu, Dialoghi, Azioni e Localizzazione</b></summary>

#### Moduli Principali

- `ui_01_GerenciadorBotoes.cpp/.h` - Facciata dei pulsanti e azioni rapide.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Costruzione visiva dei pulsanti.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Menu principali e azioni persistenti.
- `ui_04_GerenciadorEventosUI.cpp/.h` - Eventi di interfaccia utente.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Feedback visivo di operazioni prolungate.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Finestra e flusso delle statistiche.
- `ui_07_GerenciadorDados.cpp/.h` - Esportazione dei dati.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Integrazione tra eventi e interfaccia.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Controllo del monitoraggio.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Pulizia dei dati.
- `ui_11_DialogoCores.cpp/.h` - Selezione e persistenza dei colori.
- `ui_12_LocalizadorQt.cpp/.h` - Localizzazione e preferenza linguistica.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Traduzione dei metadati.
- `ui_14_OpcoesSobre.cpp/.h` - Contenuto di informazioni, licenze, avvisi, privacy e note di rilascio.
- `ui_15_Manual.cpp/.h` - Manuale integrato.

#### Sottodirectory

- `GerenciamentoBotoes/` - Selezione directory, alternanza analisi, esportazione, SQLite, pulizia e pausa.
- `GerenciamentoDialogoCores/` - Interfaccia di selezione colori, griglia, selettore avanzato e traduzioni.
- `GerenciamentoEstatisticasUI/` - Pannello, caselle di controllo, worker, rigenerazione, schede, layout e salvataggio grafici.
- `GerenciamentoEventosUI/` - Filtri, colonne, colori, lingua, pulizia e riavvio.
- `GerenciamentoLocalizadorQt/` - Caricamento traduttori, fallback, file `.ts/.qm` e traduzione operazioni.
- `GerenciamentoMenusUI/` - Menu, sottomenu, esportazione, lingue, informazioni, manuale e guida.
- `GerenciamentoTradutorMetadadosQt/` - Traduzioni di tipi di file, attributi, autore, protezione e dimensioni.
- `Manual/` - Documentazione interna in sei lingue.

</details>

<details>
<summary><b>📂 source/data/ - Statistiche e Grafici</b></summary>

#### Moduli Principali

- `st_01_GeradorEstatisticas.cpp/.h` - Facciata per la generazione delle statistiche.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Calcolo geometrico del layout.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Fabbrica di widget grafici.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Adattamento dei payload legacy.
- `Charts/ch_04_QmlRegistration.cpp/.h` - Registrazione QML.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Elemento Qt Quick.
- `Charts/ch_06_WidgetChartView.cpp/.h` - Visualizzazione tramite `QOpenGLWidget`.
- `Charts/ch_07_LgplChartModel.h` - Modello tipizzato di grafico.
- `Charts/ch_08_RasterChartView.cpp/.h` - Rendering QWidget con `QPainter`.

#### 17 Generatori di Grafici Disponibili

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
<summary><b>📂 source/tools/ - Filtri e Amministrazione</b></summary>

#### Moduli dei Filtri

- `fil_01_Filtros.cpp/.h` - Widget e logica principale di filtraggio.
- `fil_02_AdministradorCalendario.cpp/.h` - Selezione delle date.
- `fil_03_AdministradorFiltros.cpp/.h` - Applicazione combinata dei filtri.

#### Sottodirectory

- `GerenciamentoAdministradorFiltros/` - Applicazione, pulizia, sincronizzazione con menu, conteggi, lingua e persistenza.
- `GerenciamentoFiltros/` - Interfaccia, modifica filtro, sincronizzazione, verifica operazione e pulizia.

</details>

<details>
<summary><b>📂 source/security/ - Sicurezza, Prove e Quarantena</b></summary>

#### Moduli Principali

- `secore_02_SecurityManager.cpp/.h` - Orchestrazione del livello di sicurezza.
- `secore_01_postgres_utils.cpp/.h` - Backend Qt SQL, PostgreSQL configurabile e cache locale SQLite.
- `secore_05_SecurityDbWriter.cpp/.h` - Writer asincrono di sicurezza.
- `base_01_baseline_criptografico.cpp/.h` - Baseline crittografica.
- `exec_01_inteligencia_executaveis.cpp/.h` - Intelligence per eseguibili.
- `sess_01_correlacao_sessoes.cpp/.h` - Correlazione delle sessioni.
- `sdiff_01_diff_conteudo.cpp/.h` - Differenze di contenuto.
- `attr_01_atribuicao_forense.cpp/.h` - Attribuzione forense.
- `agre_01_motor_regras.cpp/.h` - Motore di regole.
- `risk_01_score_risco.cpp/.h` - Punteggio di rischio.
- `quar_01_quarentena.cpp/.h` - Quarantena e ripristino.
- `evid_01_exportacao_evidencia.cpp/.h` - Pacchetti di prove.
- `secui_*` - Finestre di dialogo e controller di interfaccia per la sicurezza.

#### Risorse

- Profili e multi-directory.
- Centro avvisi.
- Sessioni in tempo reale.
- Quarantena e ripristino.
- Pacchetto di prove.
- Notifiche e configurazione popup.
- Archiviazione locale in `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ e source/locale/ - Contenuto Integrato</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` e `RELEASE`.
- Icone `.ico` e immagini `.png`.
- Testi di informazioni, licenze, avvisi, informativa sulla privacy, cronologia e note di rilascio in più lingue.

#### File di Traduzione

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Directory Ausiliarie</b></summary>

- `mocks/` - Utility ausiliarie, inclusi `find_VersionEditor` e `compile_translations`.
- `repo/` - Script e file di pacchettizzazione, inclusi i programmi di installazione Inno Setup.
- `.vscode/` - Configurazione locale dell'IDE.
- `.github/workflows/` - Automazione per aggiornamenti documentali.
- `CMakePresets.json` - Preset principali MinGW e MSVC.
- `mocks/CMakePresets.json` - Preset delle utility ausiliarie.

</details>

---

### 🏗️ Architettura e Design Pattern

#### Principi SOLID Applicati

- Singola responsabilità garantita da moduli numerati e sottolivelli specializzati.
- Facciate per sottosistemi complessi, come statistiche, pulsanti, menu, eventi e sicurezza.
- Netta separazione tra osservazione, persistenza, metadati, visualizzazione, esportazione e traduzione.
- Basso accoppiamento mediante l'uso di proprietà `QObject`, segnali/slot e interfacce di servizio.
- Estendibilità tramite nuovi moduli `.cpp/.h` senza dover riscrivere il core.

#### Design Pattern Implementati

- **Facade:** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer / Event Loop:** segnali/slot Qt, callback di eventi e notifiche di monitoraggio.
- **Worker / Producer-Consumer:** code di eventi, `DatabaseWriter`, worker di grafici e scanner.
- **Model / View:** `EventTableModel` con `QTableView`, mantenendo la compatibilità con `QTableWidget`.
- **Factory:** creazione di widget grafici tramite `ChartWidgetFactory`.
- **Adapter:** `LegacyVariantChartAdapter` converte i vecchi formati di payload nei modelli attuali.
- **Repository / DAO locale:** accesso a SQLite tramite connessioni Qt SQL con nomi univoci.
- **Strategy informale:** generatori dedicati per ogni tipo di grafico ed estrattori separati per tipo di file.

#### Multithreading e Prestazioni

- Scansione iniziale in un worker Qt.
- Monitoraggio continuo in `std::thread`.
- Elaborazione del buffer di eventi in un thread dedicato.
- Scrittura degli eventi con `DatabaseWriter`, coda interna e persistenza batch.
- Estrazione di metadati batch con `QThreadPool`.
- Generazione di grafici con `GraficoWorker`.
- Cache di etichette, pennelli (brushes) e traduzioni in `EventTableModel`.
- Aggiornamento visivo asincrono per evitare blocchi dell'interfaccia utente.

#### Ottimizzazioni del Database

- SQLite in modalità WAL.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` e limite di journal nel writer.
- Tabelle per tipo di evento e tabella consolidata `monitoramento`.
- Tabella `snapshot` per la baseline.
- Indici per nome, directory, timestamp e chiavi frequenti.
- Inserimento batch con `execBatch()` e transazioni.

#### Architettura degli Eventi

1. `Observador` rileva le modifiche nella directory.
2. Gli eventi grezzi vengono classificati in: aggiunto, eliminato, modificato, rinominato, spostato o scansionato.
3. Sottomoduli specializzati deduplicano il rumore e riconciliano le operazioni complesse.
4. I metadati vengono estratti ove applicabile.
5. Gli eventi vengono inviati all'interfaccia utente e al writer.
6. Il database locale consente di ricaricare, filtrare, esportare e generare statistiche.

#### Estrazione dei Metadati

- `QImageReader` per le immagini.
- Ispezione di file ZIP/OpenXML per documenti, fogli di calcolo e presentazioni.
- `QSqlDatabase` per SQLite e database supportati da Qt SQL.
- Lettura degli header per l'identificazione dei formati.
- API Windows per eseguibili, firme e versioni.
- Calcoli di dimensione in B, KB, MB, GB e TB.
- Metadati specializzati per log, backup, archivi compressi, OLE, dati strutturati e codice sorgente.

#### Sicurezza

Il livello `source/security` espande il monitoraggio con funzionalità di risposta e indagine:

- Profili di directory e monitoraggio multiplo.
- Avvisi generati da regole.
- Correlazione delle sessioni.
- Baseline crittografica.
- Quarantena e ripristino di elementi.
- Pacchetti di prove.
- Punteggio di rischio.
- Backend configurabile tramite Qt SQL, con cache locale SQLite quando necessario.

---

### 🔄 Flusso di Esecuzione Principale

1. `main.cpp` chiama `source::iniciar_aplicacao(argc, argv)`.
2. `src_01_InicializadorMain` imposta la directory di lavoro, l'identità Windows e il fallback di inizializzazione.
3. `src_02_SplashAppStarter` crea `QApplication`, carica icona, traduzioni e splash screen.
4. `InterfaceMonitor` compone la finestra principale e registra le proprietà condivise.
5. Vengono creati i gestori di pulsanti, menu, eventi, avanzamento, dati e statistiche.
6. Il livello di localizzazione imposta la lingua e i traduttori.
7. Il livello di sicurezza avvia il proprio manager e writer.
8. L'utente seleziona una directory.
9. `Observador` e `DiretorioScanner` eseguono la scansione iniziale.
10. Lo snapshot e gli eventi `escaneado` vengono salvati.
11. Il monitoraggio continuo utilizza `CreateFileW` e `ReadDirectoryChangesW`.
12. Gli eventi vengono classificati, arricchiti e accodati.
13. `DatabaseWriter` esegue la persistenza batch.
14. Tabella, filtri, contatori e colori vengono aggiornati.
15. Statistiche, esportazioni e finestre di dialogo consultano lo stato corrente.
16. Alla chiusura, thread e connessioni vengono terminati in modo ordinato.

---

### 📦 Requisiti e Dipendenze

#### Librerie Principali

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
- Librerie Windows: `wintrust`, `version`, `crypt32`.

#### Requisiti di Compilazione (Build)

- CMake 4.1 o superiore per la build principale.
- CMake 3.24 o superiore per le utility in `mocks/`.
- Qt 6.11.0 nei preset attuali.
- MinGW 13.1.0 o Visual Studio 2022 Build Tools, in base al preset.
- Windows SDK compatibile con la toolchain scelta.

#### Preset CMake

Configurare e compilare con MinGW:

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Configurare e compilare con MSVC Debug:

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Configurare e compilare con MSVC Release:

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Compilare l'utility di modifica versione:

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Distribuzione (Deploy)

Su Windows, `CMakeLists.txt` tenta di individuare `windeployqt` e copia:

- l'eseguibile finale denominato `Linceu_Lighthouse`;
- le dipendenze Qt;
- `source/assets`;
- i file `.qm` dell'applicazione;
- le traduzioni Qt in `locale/qt`.

---

### ✨ Funzionalità Principali

<details>
<summary><b>Monitoraggio dei File</b></summary>

- Scansione iniziale ricorsiva.
- Snapshot persistente.
- Monitoraggio in tempo reale tramite API native di Windows.
- Eventi: aggiunto, eliminato, modificato, rinominato, spostato e scansionato.
- Deduplicazione e riconciliazione di eventi complessi.
- Rilevamento di operazioni massive.
- Pausa e arresto di scansione / monitoraggio.

</details>

<details>
<summary><b>Database e Persistenza</b></summary>

- `monitoramento.db` in `%LOCALAPPDATA%\LinceuLighthouse`.
- Tabelle per tipo di evento.
- Tabella consolidata per consultazione ed esportazione.
- Indici ottimizzati.
- Scrittura asincrona batch.
- Configurazioni locali in JSON e INI.
- Log con timestamp.

</details>

<details>
<summary><b>Metadati</b></summary>

- Tipo di file.
- Dimensione in molteplici unità.
- Attributi.
- Autore.
- Dimensioni.
- Durata.
- Bitrate.
- Protezione.
- Pagine, righe, parole, diapositive, fogli di calcolo, tabelle e record ove applicabile.
- Firma e versione degli eseguibili su Windows.

</details>

<details>
<summary><b>Visualizzazione e Tabella</b></summary>

- `QTableView` con `EventTableModel`.
- Compatibilità transitoria con `QTableWidget`.
- Ordinamento.
- Visibilità delle colonne.
- Colori per operazione e per colonna.
- Tema chiaro/scuro di Windows.
- Ritraduzione dei dati esistenti.
- Aggiornamento incrementale degli eventi.

</details>

<details>
<summary><b>Statistiche e Grafici</b></summary>

- 17 generatori specializzati.
- Rendering raster con `QPainter`.
- Backend OpenGL tramite `QOpenGLWidget`.
- Integrazione Qt Quick/QML.
- Pannello di selezione.
- Rigenerazione dopo il cambio di lingua.
- Salvataggio dei grafici.

</details>

<details>
<summary><b>Filtri Avanzati</b></summary>

- Operazione.
- Estensione.
- Testo.
- Intervallo di date.
- Combinazione di filtri.
- Sincronizzazione con i menu.
- Conteggio aggiornato.
- Persistenza dello stato.

</details>

<details>
<summary><b>Esportazione dei Dati</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Testo `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Esportazione della selezione, colonne attive e filtri attivi.
- Supporto per dati provenienti da model/view.

</details>

<details>
<summary><b>Internazionalizzazione</b></summary>

- Sei lingue.
- Traduzione di interfaccia, operazioni e metadati.
- Fallback testuale.
- Preferenza persistita.
- Manuale e contenuti istituzionali integrati.
- Finestra di dialogo Informazioni reattiva al cambio di lingua.

</details>

<details>
<summary><b>Sicurezza</b></summary>

- Multi-directory e profili.
- Centro avvisi.
- Sessioni in tempo reale.
- Quarantena.
- Ripristino.
- Pacchetti di prove.
- Notifiche.
- Punteggio di rischio e regole.

</details>

---

### 🎯 Buone Pratiche Implementate

#### Codice Pulito

- Organizzazione a livelli e moduli numerati.
- Classi e funzioni con responsabilità ben definite.
- Uso coerente dei namespace.
- Header `.h` separati dalle implementazioni `.cpp`.
- Fallback previsti in caso di errori di UI, logging o inizializzazione.
- Commenti mirati nelle aree complesse.

#### Prestazioni

- Monitoraggio nativo anziché polling.
- Scrittura SQLite batch.
- Indici sui campi interrogati frequentemente.
- Worker e thread per operazioni pesanti.
- Cache di traduzione, etichette e colori.
- Aggiornamento incrementale della tabella.
- Nessuna dipendenza pesante di analisi a runtime.

#### Manutenibilità

- Livelli chiaramente separati.
- Moduli compatti per flussi specifici.
- Build dichiarativa in CMake.
- Preset per le toolchain.
- Asset e traduzioni versionati.
- Utility isolate in `mocks/`.
- Documentazione tecnica nel repository.

#### Sicurezza e Robustezza

- Uso della directory persistente locale dell'utente.
- Connessioni SQLite con nomi univoci.
- Chiusura ordinata dei thread.
- Log di errore e diagnostica.
- Gestione difensiva con `try/catch` ai limiti dell'interfaccia.
- Validazione dei puntatori prima dell'uso nelle integrazioni Qt.

---

### 🚀 Per Aggiungere Nuove Funzionalità

- **Nuovo tipo di grafico:** crea un nuovo generatore in `source/data/GeradorEstatisticas/`, derivando da `gst_01_BaseGerador` e registrando l'opzione nell'interfaccia delle statistiche.
- **Nuovo tipo di evento:** aggiungi una classe o funzione specializzata in `source/services`, aggiorna le colonne se necessario e registra la persistenza nel flusso di `BaseEvento`.
- **Nuova colonna:** aggiorna `ob_09_GerenciadorColunas`, lo schema in `gbank_02_criar_banco_de_dados`, l'esportazione e le traduzioni.
- **Nuovo metadato:** implementa un estrattore in `GerenciamentoMetadados`, aggiungi colonna, traduzione e logica di visualizzazione.
- **Nuova lingua:** crea `linceu_XX.ts`, compila in `.qm`, aggiungi il fallback in `LocalizadorQt` e traduci gli asset ove necessario.
- **Nuovo filtro:** implementa in `source/tools/GerenciamentoAdministradorFiltros` e sincronizza con il menu e il conteggio.
- **Nuova azione di sicurezza:** aggiungi un servizio in `source/security`, un dialogo in `secui_*`, le stringhe tradotte e la persistenza nel backend.
- **Nuova esportazione:** estendi `geb_03_exportar_dados.cpp` o aggiungi un modulo dedicato, mantenendo la compatibilità con `QTableView` e `QTableWidget`.

---

### 📝 Note Importanti

#### Versioni e Toolchain

- La build principale richiede C++17.
- I preset attuali puntano a Qt 6.11.0.
- Il preset MinGW utilizza `C:/Qt/Tools/mingw1310_64`.
- Il preset MSVC utilizza Visual Studio 2022 Build Tools.
- Il progetto è orientato a Windows in quanto si basa su API native di monitoraggio e metadati.

#### File Critici

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Database principale degli eventi.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Cache locale del livello di sicurezza quando utilizzato.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Preferenza linguistica.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Visibilità delle colonne.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Colonne colorate.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Colori configurati.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Preferenze dell'interfaccia di sicurezza.

#### Log

I log vengono registrati con timestamp in:

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Utility Ausiliarie

- `mocks/find_VersionEditor.cpp` aggiorna versioni e date negli asset, `README.md`, `OBSERVACAO.md` e passaggi del dialogo Informazioni.
- `mocks/compile_translations.cpp` facilita la compilazione e validazione delle traduzioni.
- Le utility utilizzano un proprio CMake in `mocks/`.

---

### 📄 Licenza e Autori

**Autore:** Fernando Nillsson Cidade<br>
**Versione:** v2026.8.25.0<br>
**Licence:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Per ulteriori informazioni, consultare anche `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, gli asset integrati e la documentazione specifica nei moduli del codice sorgente.

---

</details>

---

<h2 id="dede">🇩🇪 Deutsch (DE)</h2>

> **Hinweis:** Dieses Repository bezieht sich auf die Version **v2026.8.25.0** des Projekts Linceu Lighthouse. Unterstützen Sie das Projekt und erwerben Sie die kostenpflichtige Version über den Link: [Über Microsoft Store installieren](https://apps.microsoft.com/detail/9NN8Z5Z700TM)

<details>
<summary>Klicken Sie hier, um die README auf Deutsch zu erweitern</summary>

<p align="center">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131219.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131236.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131253.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131314.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131333.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131352.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131411.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131430.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131450.png" alt="Linceu Lighthouse - Deutsch" width="320">
  <img src="https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/repo/LISTAGEM/de_DE/Captura%20de%20tela%202026-08-25%20131517.png" alt="Linceu Lighthouse - Deutsch" width="320">
</p>

Projekt: LINCEU LIGHTHOUSE  
Version: v2026.8.25.0<br>
Autor: Fernando Nillsson Cidade<br>

Beschreibung: Lokale Plattform für Datei-Intelligenz, Verlauf und Dateiaktivitäts-Audit
> **„Erfahren Sie genau, was mit Ihren Dateien geschehen ist — lokal, visuell und mit durchsuchbarem Verlauf.“**

**Linceu Lighthouse** ist eine native C++17-Desktopanwendung mit **Qt6**, die entwickelt wurde, um absolute Transparenz, lückenlose historische Nachverfolgung und deterministische Audits für das Windows-Dateisystem bereitzustellen. Anstatt als einfacher flüchtiger Monitor oder als eingeschränktes Sicherheitswerkzeug zu agieren, vereint Linceu Lighthouse drei wesentliche Säulen in einer agilen, intuitiven und hochperformanten Benutzererfahrung:

1. **Kontinuierliche und semantische Dateiaktivitätsüberwachung:** Echtzeit-Erfassung von Dateisystemereignissen (Erstellung, Bearbeitung, Löschung, Umbenennung, Verschiebung und Initialscan) mit spezifischen Algorithmen zur Korrelation komplexer Ereignisse — präzise Erkennung echter `RENAMED`- und `MOVED`-Operationen ohne fälschliche Paare aus Löschung und Neuerstellung.
2. **Durchsuchbarer Verlauf und Datei-Intelligenz ohne KI:** Automatischer Aufbau einer lokalen relationalen SQLite-Datenbank, angereichert mit über 30 Metadatenkategorien vor und nach jeder Änderung, begleitet von 17 nativen Diagrammvisualisierungen und deterministischen statistischen Berichten.
3. **Lokale Sicherheit, Audit und Beweissicherung:** Kryptografische Baseline, deterministische Berechnung der Risikobewertung, lokale Regel-Engine, Quarantäne, Wiederherstellung und Export revisionssicherer Berichte in gängigen Marktformaten.

### 🛡️ Absolute Datensouveränität (Local-First)
* **100% Lokal und Offline:** Ihre Dateien, Metadaten, Verläufe und Berichte verbleiben ausschließlich auf Ihrem Computer (`%LOCALAPPDATA%\LinceuLighthouse`).
* **Null Cloud, Null Telemetrie, Null Benutzerkonten:** Es werden keinerlei persönliche oder betriebliche Daten über das Netzwerk übertragen.
* **Deterministische Intelligenz ohne KI:** Sämtliche Analysen, Klassifizierungen und Risikobewertungen basieren rein auf algorithmischer und statistischer Logik — ohne Sprachmodelle (LLM), ohne RAG und ohne externe Abhängigkeiten.

---

### 🧭 Die 5 Betriebsmodi

| Modus | Leitfrage / Kernfokus | Hauptvorteil |
|---|---|---|
| **Überwachungsmodus** | *Was geschieht gerade?* | Echtzeit-Visualisierung von Änderungen in ausgewählten Verzeichnissen mit sofortiger farblicher Signalisierung. |
| **Verlaufsmodus** | *Was ist mit meiner Datei geschehen?* | Umgekehrte chronologische Abfrage (*File History* / *What Changed?*), welche die vollständige Genealogie von Bearbeitungen und Umbenennungen aufzeigt. |
| **Analytics-Modus (Ohne KI)** | *Welche Nutzungsmuster liegen vor?* | Heatmaps, Spitzenzeiten, am häufigsten betroffene Dateitypen und Verzeichnisflüsse (Sankey). |
| **Sicherheitsmodus** | *Gab es verdächtige Änderungen?* | Prüfung gegen kryptografische Baseline, Hashes, Compliance-Regeln und strukturelle Integrität. |
| **Beweissicherungsmodus** | *Wie lassen sich Änderungen belegen?* | Export unveränderlicher Berichte und Audit-Pakete für interne Compliance oder technische Untersuchungen. |

---

### 🎯 Zielgruppen und Anwendungsfälle

* **Softwareentwickler und Ingenieure:** Präzise nachverfolgen, was Builds, IDEs, Build-Skripte, Paketmanager und Git-Werkzeuge in Projekten erstellen, ändern oder löschen.
* **IT-Administratoren, Sysadmins und Support:** Verschwundene Dateien und versehentliche Umbenennungen in freigegebenen Ordnern diagnostizieren sowie die Zeitleiste von Betriebsvorfällen rekonstruieren.
* **Bereiche mit sensiblen Daten (Recht, Gesundheit, Finanzen, Ingenieurwesen):** Strikte auditierbare Kontrolle über vertrauliche Ordner mit absoluter Geheimhaltungsgarantie und Netzwerkisolation aufrechterhalten.
* **Content Creator (Designer, Video-Editoren, Fotografen):** Dateioperationen umfangreicher Asset-Bibliotheken, Rendering-Prozesse und komplexe Export-Pipelines überwachen.
* **Allgemeine Benutzer:** Wichtige Verzeichnisse (Dokumente, Downloads, synchronisierte Cloud-Laufwerke) vor versehentlichem Löschen und unbefugten Änderungen schützen.

---

### ⚙️ Wie Linceu Lighthouse Funktioniert

```text
Dateisystem (Windows)
      ↓
Asynchrone Native APIs (ReadDirectoryChangesW)
      ↓
Threadsichere Ereigniswarteschlange & Spitzenpuffer
      ↓
Semantische Normalisierung & Korrelation (Echte Move/Rename-Erkennung)
      ↓
Metadaten-Anreicherung (30+ Kategorien, Magic Numbers, Hashes)
      ↓
Asynchrone Batch-Persistenz in Lokaler SQLite-Datenbank
      ↓
Interaktive Qt6-Visualisierung | 17 Native Diagramme | Multiformat-Export
```

---

### 🛠️ Analysewerkzeuge und Verfügbare Funktionen

- **Multidimensionale Filter:** Schnelle Filterung nach Operation sowie erweiterte Filter mit Unterstützung für Ausdrücke, Dateiendungen, Datumsbereiche und Direktauswahl in der Tabelle.
- **Angereicherte Audit-Tabelle:** Konfigurierbare Spalten, Persistenz von Farbeinstellungen je Operation, dynamisch übersetzte echte MIME-Erkennung und vollständige Zeitstempel-Metadaten für Ordner und Unterordner.
- **17 Native Diagrammvisualisierungen:** Kreis (Pizza), Balken, Zeitachse (Timeline), Baumkarte (Treemap), Histogramm, Pareto-Kurve, Linien, Boxplot, Radar, Zeit-Heatmap, Streudiagramm (Scatter), Sankey-Diagramme, Punktdiagramm (Dotplot) und Hierarchischer Verzeichnisbaum.
- **Flexibler Beweisexport:** Berichtgenerierung in `.xlsx` (natives OpenXML), `.csv`, `.txt`, `.json`, `.xml` und `.db`-Datenbank.
- **Vollständige Mehrsprachigkeit:** Benutzeroberfläche und technische Dokumentation nativ in 6 Sprachen integriert (`pt_BR`, `en_US`, `es_ES`, `fr_FR`, `it_IT` und `de_DE`).

---

SCHNELLSTARTANLEITUNG:

1. Starten Sie die Anwendung, um die grafische Benutzeroberfläche zu öffnen.
2. Wählen Sie ein Verzeichnis zur Überwachung aus.
3. Starten Sie die Überwachung, um den Initialscan durchzuführen und Änderungen nachzuverfolgen.
4. Nutzen Sie die Menüs und seitlichen Schaltflächen zum Filtern, Exportieren, Anzeigen von Statistiken und Prüfen von Sicherheitsfunktionen.
5. Verwenden Sie einen Doppelklick oder die Aktion Öffnen, um in der Tabelle gemeldete Dateien mit der passenden Anwendung zu öffnen.
6. Speichern Sie exportierte Berichte oder Datenbanken, wenn Sie Ergebnisse archivieren möchten.

AUSFÜHRLICHES BENUTZERHANDBUCH:

Systemanforderungen:

- Windows 10 oder höher.
- Leseberechtigung für das überwachte Verzeichnis.
- Schreibberechtigung unter `%LOCALAPPDATA%\LinceuLighthouse` für Datenbank, Konfigurationen und Protokolle (Logs).

Installation:

- Installieren Sie die über das Projekt-Installationsprogramm verteilte ausführbare Datei oder erstellen Sie das Binärprogramm über den in dieser README beschriebenen CMake-Ablauf.
- Verwenden Sie bei lokalen Builds das Preset, das zu Ihrer Qt6-Toolchain passt: MinGW oder MSVC.

Ersteinrichtung:

- Die Benutzeroberfläche startet mit einer leeren Ereignistabelle.
- Die Sprache wird über das Lokalisierungssystem geladen und kann über das Optionsmenü geändert werden.
- Die Anwendung erstellt persistente Verzeichnisse bei Bedarf automatisch.
- Protokolle (Logs) werden unter `%LOCALAPPDATA%\LinceuLighthouse\logs` gespeichert.

Verzeichnisüberwachung:

1. Verzeichnis auswählen:
   - Klicken Sie auf `Verzeichnis auswählen` oder nutzen Sie `Datei -> Verzeichnis auswählen`.
   - Der ausgewählte Pfad wird fortan vom Beobachter und Scan-Ablauf verwendet.

2. Überwachung starten oder stoppen:
   - Klicken Sie auf `Starten/Stoppen` oder nutzen Sie das entsprechende Menü.
   - Der Initialscan befüllt den Snapshot und zeichnet `escaneado`-Ereignisse auf.
   - Nach dem Scan registriert die kontinuierliche Überwachung Änderungen in Echtzeit.

3. Überwachung oder Scan pausieren:
   - Das Projekt verfügt über separate Steuerungen zur Unterbrechung von Überwachung, Scan oder aktiven Abläufen.
   - Der visuelle Status wird über die Fortschritts- und Meldungsebene aktualisiert.

Hauptbenutzeroberfläche:

- Menüleiste: Datei, Einstellungen, Optionen, Export, Hilfe, Sicherheit und Sprache.
- Seitenleiste: Häufige Aktionen wie Auswählen, Starten/Stoppen, Pause, Bereinigen, Exportieren und Statistiken.
- Verzeichnisbezeichnung: Überwachter Pfad.
- Ergebnisbezeichnung: Betriebsmeldungen und Sicherheits-Feedback.
- Datentabelle: Ereignisse und Metadaten.
- Fortschrittsbalken: Scans und längere Vorgänge.
- Hilfsdialoge: Filter, Farben, Spalten, Statistiken, Handbuch, Hilfe und Über.

Wichtigste Ereignisfelder:

- Operationstyp.
- Name.
- Vorheriges Verzeichnis.
- Aktuelles Verzeichnis.
- Erstellungs-, Änderungs- und Zugriffsdaten.
- Vollständige Zeitstempel für Dateien, Ordner, Unterordner und untergeordnete Verzeichnisse.
- Typ/Dateiendung.
- Übersetzter MIME-Typ und ursprüngliche MIME-Kennung.
- Größen in mehreren Einheiten.
- Attribute, Autor, Dimensionen, Dauer, Bitrate und Schutz.
- Spezifische Metadaten für Dokumente, Tabellenkalkulationen, Präsentationen, Datenbanken, komprimierte Archive und ausführbare Dateien.

Filter und Suche:

- Schnellfilter nach Operationstyp im Menü.
- Erweiterte Filter nach Operation, Dateiendung, Text, Datumsbereich und Kriterienkombination.
- Synchronisation mit dem Hauptmenü.
- Aktualisierung der Zählerstände nach Filter- oder Sprachwechsel.
- Persistenz des Status der Filter-Kontrollkästchen.

Statistik-Visualisierung:

- Zugriff über die Statistik-Schaltfläche oder über das Menü.
- Diagramme werden von C++-Modulen unter `source/data/GeradorEstatisticas` generiert.
- Das Fenster ermöglicht Diagrammauswahl, Neuberechnung, Speichern und Aktualisierung nach einem Sprachwechsel.

Verfügbare Diagramme:

- Kreisdiagramm (Pizza).
- Balkendiagramm.
- Zeitachse (Timeline).
- Baumkarte (Treemap).
- Histogramm.
- Pareto-Diagramm.
- Liniendiagramm.
- Boxplot.
- Ereignis-Radar.
- Heatmap.
- Streudiagramm (Scatter).
- Sankey-Diagramm.
- Radar.
- Punktdiagramm (Dotplot).
- Sankey Ereignis-Pfad.
- Sankey Typ-Pfad.
- Verzeichnisbaum.

Datenexport:

- `Excel (.xlsx)`: OpenXML-Paket, direkt vom C++-Code generiert.
- `CSV (.csv)`: Kommagetrennt.
- `Text (.txt)`: Tabulatorgetrennt.
- `JSON (.json)`: Serialisierte Struktur mit Kopfzeilen und Zeilen.
- `XML (.xml)`: Hierarchische Ausgabe über `QXmlStreamWriter`.
- `SQLite (.db)`: Von der Exportschicht generierte Datenbank.

Exportoptionen:

- Nur aktive Spalten.
- Nur aktive Filter.
- Nur aktuelle Auswahl.
- Kompatibilität mit `QTableWidget` und `QTableView`.

Spalten- und Farbkonfiguration:

- Das Spaltenmenü ermöglicht das Aktivieren, Ausblenden und Wiederherstellen von Feldern.
- Spalten wie MIME, Erstellungsdatum, Änderungsdatum und Zugriffsdatum können für detailliertere Audits aktiviert werden.
- Das Farbmenü ermöglicht die Einfärbung nach Operation oder nach Spalte.
- Konfigurationen bleiben in lokalen JSON-Dateien persistent gespeichert.
- Die Tabelle reagiert auf Änderungen des Windows-Designs und invalidiert visuelle Caches bei Bedarf.

Internationalisierung:

- Unterstützte Sprachen: Portugiesisch, Englisch, Spanisch, Französisch, Italienisch und Deutsch.
- `.ts`- und `.qm`-Dateien befinden sich in `source/locale`.
- `LocalizadorQt` lädt Übersetzungen, speichert Einstellungen und stellt ein Fallback bereit.
- Operationen, Metadaten, MIME, Menüs, Handbuch, Hilfe, Über und Sicherheitstexte sind vollständig übersetzt.

Fehlerbehebung:

1. Überwachung startet nicht:
   - Überprüfen Sie die Verzeichnisberechtigungen.
   - Vermeiden Sie geschützte Systemverzeichnisse.
   - Prüfen Sie, ob die lokale Datenbank erstellt werden kann.

2. Ereignisse werden nicht angezeigt:
   - Prüfen Sie, ob die Überwachung aktiv ist.
   - Löschen Sie aktive Filter.
   - Testen Sie durch Erstellen, Umbenennen oder Löschen einer einfachen Datei im überwachten Verzeichnis.

3. Hoher Ressourcenverbrauch:
   - Vermeiden Sie die Überwachung übermäßig großer Verzeichnisbäume ohne Notwendigkeit.
   - Bevorzugen Sie gezielte Unterverzeichnisse.
   - Exportieren oder bereinigen Sie alte Daten, wenn die Datenbank zu groß wird.

4. Leere Statistiken:
   - Bestätigen Sie, dass geladene Ereignisse vorhanden sind.
   - Entfernen Sie Filter, die Zeilen ausblenden könnten.
   - Prüfen Sie, ob auf `monitoramento.db` zugegriffen werden kann.

5. Datei lässt sich nicht öffnen:
   - Vergewissern Sie sich, dass die Datei unter dem angezeigten Pfad noch existiert.
   - Prüfen Sie, ob Windows eine Anwendung mit diesem Dateityp verknüpft hat.
   - Wenn der Auswahldialog erscheint, wählen Sie eine andere kompatible Anwendung.

6. Ordnerdaten werden nicht angezeigt:
   - Aktivieren Sie die Spalten Erstellungsdatum, Änderungsdatum und Zugriffsdatum.
   - Führen Sie einen erneuten Scan durch, um Ordner, Unterordner und untergeordnete Verzeichnisse zu aktualisieren.

## 📋 Technische Dokumentation

### Systemübersicht

**Linceu Lighthouse** ist eine native Windows-Desktopanwendung, geschrieben in **C++17** und basierend auf **Qt6**. Die aktuelle Architektur ersetzt die frühere Python/PySide-Kette vollständig durch `.cpp`- und `.h`-Module, mit CMake-Build, SQLite-Persistenz über Qt SQL, Rendering über Qt Widgets/OpenGL/Quick und Integration mit nativen Windows-APIs.

Die Anwendung ist in Schichten gegliedert:

- `source/gui`: Hauptfenster, visuelle Komposition und High-Level-Integration.
- `source/services`: Verzeichnisbeobachtung, Ereignisse, Datenbank, Metadaten und Tabelle.
- `source/ui`: Menüs, Schaltflächen, Statistiken, Filter, Handbuch, Hilfe, Über und Lokalisierung.
- `source/data`: Statistik-Fassade, Generatoren und Diagramminfrastruktur.
- `source/tools`: Filter und Kriterienverwaltung.
- `source/security`: Sicherheitsfunktionen, Sitzungen, Quarantäne, Beweise und Speicher-Backend.
- `source/utils`: Pfade, Symbole und Protokollierung (Logging).
- `source/assets` und `source/locale`: Integrierte Inhalte, Lizenzen, Texte und Übersetzungen.

#### Kerntechnologien

- **Sprache:** C++17.
- **Build:** CMake, mit `CMAKE_AUTOMOC` und `CMAKE_AUTORCC`.
- **Framework:** Qt6.
- **UI:** Qt Widgets, `QDialog`, `QTableView`, `QTableWidget`, `QAbstractTableModel`, `QTextBrowser`, Menüs und Aktionen.
- **Datenbank:** SQLite über `QSqlDatabase` und `QSqlQuery`.
- **Nebenläufigkeit:** `std::thread`, `std::mutex`, `std::condition_variable`, `QThread`, `QThreadPool` und Qt Concurrent.
- **Diagramme:** Eigene Infrastruktur mit `ChartLayoutEngine`, `RasterChartView`, `QPainter`, `QOpenGLWidget`, `QuickChartItem` und typisiertem Modell `LgplChartModel`.
- **Export:** Eigene Implementierung für CSV, TXT, JSON, XML, SQLite und OpenXML-`.xlsx`-Pakete.
- **Internationalisierung:** Qt Linguist (`.ts` und `.qm`) mit sechs Sprachen.
- **Windows:** `CreateFileW`, `ReadDirectoryChangesW`, `WinVerifyTrust`, `GetFileVersionInfoW`, `VerQueryValueW`, `QSettings` für Designs sowie Bibliotheken `wintrust`, `version` und `crypt32`.
- **Sicherheit:** Eigene C++-Schicht mit kryptografischer Baseline, Regeln, Risikobewertung, Quarantäne, Beweisen und lokalem SQLite-Cache für das Sicherheits-Backend.

Die Hauptanwendung benötigt keine Python-Laufzeitumgebung, kein `pandas`, `numpy`, `matplotlib`, `seaborn`, `plotly` oder vergleichbare Bibliotheken. Jegliche Erwähnung von Python in Lizenzdateien spiegelt integrierte Drittanbieter-Dokumentation wider und nicht den primären Ausführungs-Stack.

---

### 📁 Verzeichnis- und Modulstruktur

Validierte Zusammenfassung im Verzeichnisbaum `source/`:

| Schicht | `.cpp` | `.h` | Verantwortung |
|---|---:|---:|---|
| `source-root` | 2 | 2 | Hauptinitialisierung und Splash |
| `source/assets` | 0 | 0 | Texte, Lizenzen, Bilder, Symbole und integrierte Inhalte |
| `source/data` | 26 | 27 | Statistiken, Diagramme und Rendering |
| `source/gui` | 8 | 8 | Hauptfenster und visuelle Komposition |
| `source/locale` | 0 | 0 | Übersetzungsdateien `.ts` und `.qm` |
| `source/security` | 24 | 24 | Sicherheit, Beweise, Quarantäne und Sitzungen |
| `source/services` | 162 | 163 | Beobachtung, Ereignisse, Datenbank, Metadaten und Tabelle |
| `source/tools` | 23 | 24 | Filter und Filterverwaltung |
| `source/ui` | 125 | 125 | Menüs, Schaltflächen, Dialoge, Handbuch, Statistiken und Lokalisierung |
| `source/utils` | 4 | 4 | Pfade, Symbole und Protokolle (Logs) |
| **Gesamt in `source/`** | **374** | **377** | Aktuelle C++-Basis |

<details>
<summary><b>📂 source-root - Anwendungs-Bootstrap</b></summary>

#### Hauptmodule

- `src_01_InicializadorMain.cpp/.h` - Reeller Einstiegspunkt der Anwendung nach `main.cpp`, Anpassung des Arbeitsverzeichnisses, Windows-Identifikation und Initialisierungs-Fallback.
- `src_02_SplashAppStarter.cpp/.h` - Initialisierung mit Splash-Screen, Fortschritt, Symbol, Übersetzungen, Erstellung von `InterfaceMonitor` und frühzeitigem Start des Writers.

</details>

<details>
<summary><b>📂 source/gui/ - Grafische Benutzeroberflächenschicht</b></summary>

#### Hauptmodule

- `ic_01_InterfaceMonitor.cpp/.h` - Hauptfenster, gemeinsam genutzte Eigenschaften, Tabellen-Container und Subsystemintegration.
- `ic_02_Inicializador.cpp/.h` - Initialisierung von Widgets und Basisobjekten.
- `ic_03_Configurador.cpp/.h` - Layouts, Komponenten und visuelle Konfiguration.
- `ic_04_Atualizador.cpp/.h` - Oberflächenaktualisierung für Tabelle, Status und Fortschritt.
- `ic_05_GerenciadorProgresso.cpp/.h` - Leisten und Fortschritts-Feedback.
- `ic_06_GerenciadorMensagens.cpp/.h` - Meldungen, Status und Benutzerkommunikation.
- `ic_07_ManipuladorTabela.cpp/.h` - Tabellenoperationen und Anpassung für die UI.
- `ic_08_Internacionalizador.cpp/.h` - Integration der Benutzeroberfläche mit dem Lokalisierungssystem.

Die früheren Unterverzeichnisse `GerenciadorDesempenho/` und `GerenciadorEstruturaDiretoriosWidget/` existieren in diesem Verzeichnisbaum nicht mehr. Die aktuelle Dokumentation darf sie nicht als aktive Subsysteme behandeln.

</details>

<details>
<summary><b>📂 source/services/ - Überwachungs-, Ereignis- und Datenkern</b></summary>

#### Hauptmodule

- `ob_01_Observador.cpp/.h` - Verzeichnisbeobachter, Threads, Puffer und Überwachungszyklus.
- `ob_02_BaseEvento.cpp/.h` - Gemeinsame Basis für Ereignisse und SQLite-Integration.
- `ob_03_DiretorioScanner.cpp/.h` - Initialscan und Snapshot-Erstellung.
- `ob_04_EventoAdicionado.cpp/.h` - Aufzeichnung erstellter Dateien.
- `ob_05_EventoExcluido.cpp/.h` - Aufzeichnung von Löschungen und temporären Löschungen.
- `ob_06_EventoModificado.cpp/.h` - Konsolidierung von Änderungen.
- `ob_07_EventoRenomeado.cpp/.h` - Behandlung von Umbenennungen.
- `ob_08_EventoMovido.cpp/.h` - Erkennung und Abgleich von Verschiebungen.
- `ob_09_GerenciadorColunas.cpp/.h` - Verfügbare Spalten und Persistenz der Sichtbarkeit.
- `ob_10_GerenciadorTabela.cpp/.h` - Brücke zwischen Ereignissen, Tabelle und Benutzeroberfläche.

#### Spezialisierte Unterverzeichnisse

- `GerenciamentoBaseEvento/` - Datenbankerstellung, PRAGMAs, Indizes, Datensätze, Snapshot, Löschungen, Verschiebungen und `DatabaseWriter`.
- `GerenciamentoDiretorioScanner/` - Scan-Worker, Verarbeitungswarteschlange, Batches, Fortschritt und Abschluss.
- `GerenciamentoEventoModificado/` - Deduplizierungsintervalle, Cache, große Dateien und normale/massive Verarbeitung.
- `GerenciamentoEventoMovido/` - Verschiebe-Worker, ausstehende Löschungen, Zeilen- und Tabellenaktualisierung.
- `GerenciamentoMetadados/` - Native Metadatenextraktion für Quellcode, Bilder, Audio, Video, Dokumente, Tabellenkalkulationen, Präsentationen, ausführbare Dateien, komprimierte Archive, Protokolle und strukturierte Daten.
- `GerenciamentoObservador/` - Start, Stopp, Überwachung, Puffer, Erkennung massiver Operationen und Pause.
- `GerenciamentoTabela/` - Design, Kopfzeilen, Breiten, Farben, Auswahl, Neuübersetzung und `EventTableModel`.

</details>

<details>
<summary><b>📂 source/ui/ - Menüs, Dialoge, Aktionen und Lokalisierung</b></summary>

#### Hauptmodule

- `ui_01_GerenciadorBotoes.cpp/.h` - Schaltflächen-Fassade und Schnellaktionen.
- `ui_02_GerenciadorBotoesUI.cpp/.h` - Visuelle Konstruktion der Schaltflächen.
- `ui_03_GerenciadorMenusUI.cpp/.h` - Hauptmenüs und persistente Aktionen.
- `ui_04_GerenciadorEventosUI.cpp/.h` - Benutzeroberflächen-Ereignisse.
- `ui_05_GerenciadorProgressoUI.cpp/.h` - Visuelles Feedback bei langen Operationen.
- `ui_06_GerenciadorEstatisticasUI.cpp/.h` - Statistikfenster und -ablauf.
- `ui_07_GerenciadorDados.cpp/.h` - Datenexport.
- `ui_08_GerenciadorEventosArquivo.cpp/.h` - Integration zwischen Ereignissen und Benutzeroberfläche.
- `ui_09_GerenciadorMonitoramento.cpp/.h` - Überwachungssteuerung.
- `ui_10_GerenciadorLimpeza.cpp/.h` - Datenbereinigung.
- `ui_11_DialogoCores.cpp/.h` - Farbauswahl und Farbpersistenz.
- `ui_12_LocalizadorQt.cpp/.h` - Lokalisierung und Spracheinstellung.
- `ui_13_TradutorMetadadosQt.cpp/.h` - Übersetzung von Metadaten.
- `ui_14_OpcoesSobre.cpp/.h` - Inhalte für Über, Lizenzen, Hinweise, Datenschutz und Versionshinweise.
- `ui_15_Manual.cpp/.h` - Integriertes Handbuch.

#### Unterverzeichnisse

- `GerenciamentoBotoes/` - Verzeichnisauswahl, Analyseumschaltung, Export, SQLite, Bereinigung und Pause.
- `GerenciamentoDialogoCores/` - Benutzeroberfläche zur Farbauswahl, Raster, erweiterte Auswahl und Übersetzungen.
- `GerenciamentoEstatisticasUI/` - Bedienfeld, Kontrollkästchen, Worker, Neuberechnung, Registerkarten, Layout und Diagrammspeicherung.
- `GerenciamentoEventosUI/` - Filter, Spalten, Farben, Sprache, Bereinigung und Neustart.
- `GerenciamentoLocalizadorQt/` - Laden von Übersetzern, Fallback, `.ts/.qm`-Dateien und Operationsübersetzungen.
- `GerenciamentoMenusUI/` - Menüs, Untermenüs, Export, Sprachen, Über, Handbuch und Hilfe.
- `GerenciamentoTradutorMetadadosQt/` - Übersetzungen von Dateitypen, Attributen, Autor, Schutz und Dimensionen.
- `Manual/` - Interne Dokumentation in sechs Sprachen.

</details>

<details>
<summary><b>📂 source/data/ - Statistiken und Diagramme</b></summary>

#### Hauptmodule

- `st_01_GeradorEstatisticas.cpp/.h` - Fassade zur Statistikgenerierung.
- `Charts/ch_01_ChartLayoutEngine.cpp/.h` - Geometrische Layout-Berechnung.
- `Charts/ch_02_ChartWidgetFactory.cpp/.h` - Fabrik für Diagramm-Widgets.
- `Charts/ch_03_LegacyVariantChartAdapter.cpp/.h` - Anpassung älterer Daten-Payloads.
- `Charts/ch_04_QmlRegistration.cpp/.h` - QML-Registrierung.
- `Charts/ch_05_QuickChartItem.cpp/.h` - Qt Quick-Element.
- `Charts/ch_06_WidgetChartView.cpp/.h` - Visualisierung über `QOpenGLWidget`.
- `Charts/ch_07_LgplChartModel.h` - Typisiertes Diagrammmodell.
- `Charts/ch_08_RasterChartView.cpp/.h` - QWidget-Rendering mit `QPainter`.

#### 17 Verfügbare Diagrammgeneratoren

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
<summary><b>📂 source/tools/ - Filter und Verwaltung</b></summary>

#### Filtermodule

- `fil_01_Filtros.cpp/.h` - Widget und primäre Filterlogik.
- `fil_02_AdministradorCalendario.cpp/.h` - Datumsauswahl.
- `fil_03_AdministradorFiltros.cpp/.h` - Kombinierte Filteranwendung.

#### Unterverzeichnisse

- `GerenciamentoAdministradorFiltros/` - Anwendung, Löschen, Menüsynchronisation, Zählungen, Sprache und Persistenz.
- `GerenciamentoFiltros/` - Benutzeroberfläche, Filteränderung, Synchronisation, Operationsprüfung und Löschen.

</details>

<details>
<summary><b>📂 source/security/ - Sicherheit, Beweise und Quarantäne</b></summary>

#### Hauptmodule

- `secore_02_SecurityManager.cpp/.h` - Orchestrierung der Sicherheitsschicht.
- `secore_01_postgres_utils.cpp/.h` - Qt SQL-Backend, konfigurierbares PostgreSQL und lokaler SQLite-Cache.
- `secore_05_SecurityDbWriter.cpp/.h` - Asynchroner Sicherheits-Writer.
- `base_01_baseline_criptografico.cpp/.h` - Kryptografische Baseline.
- `exec_01_inteligencia_executaveis.cpp/.h` - Intelligenz für ausführbare Dateien.
- `sess_01_correlacao_sessoes.cpp/.h` - Sitzungskorrelation.
- `sdiff_01_diff_conteudo.cpp/.h` - Inhaltsunterschiede.
- `attr_01_atribuicao_forense.cpp/.h` - Forensische Zuordnung.
- `agre_01_motor_regras.cpp/.h` - Regel-Engine.
- `risk_01_score_risco.cpp/.h` - Risikobewertung.
- `quar_01_quarentena.cpp/.h` - Quarantäne und Wiederherstellung.
- `evid_01_exportacao_evidencia.cpp/.h` - Beweispakete.
- `secui_*` - Dialoge und UI-Controller für Sicherheitsfunktionen.

#### Ressourcen

- Profile und Multi-Verzeichnis-Überwachung.
- Warnzentrale.
- Echtzeit-Sitzungen.
- Quarantäne und Wiederherstellung.
- Beweispaket.
- Benachrichtigungen und Popup-Konfiguration.
- Lokale Speicherung unter `%LOCALAPPDATA%\LinceuLighthouse\security`.

</details>

<details>
<summary><b>📂 source/assets/ und source/locale/ - Integrierte Inhalte</b></summary>

#### Assets

- `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` und `RELEASE`.
- `.ico`-Symbole und `.png`-Bilder.
- Texte für Über, Lizenzen, Hinweise, Datenschutzrichtlinie, Verlauf und Versionshinweise in mehreren Sprachen.

#### Übersetzungsdateien

- `linceu_pt_BR.ts/.qm`
- `linceu_en_US.ts/.qm`
- `linceu_es_ES.ts/.qm`
- `linceu_fr_FR.ts/.qm`
- `linceu_it_IT.ts/.qm`
- `linceu_de_DE.ts/.qm`

</details>

<details>
<summary><b>📂 Zusätzliche Verzeichnisse</b></summary>

- `mocks/` - Hilfsprogramme, einschließlich `find_VersionEditor` und `compile_translations`.
- `repo/` - Skripte und Paketierungsdateien, einschließlich Inno Setup-Installationsprogramme.
- `.vscode/` - Lokale IDE-Konfiguration.
- `.github/workflows/` - Automatisierung von Dokumentationsaktualisierungen.
- `CMakePresets.json` - Haupt-Presets für MinGW und MSVC.
- `mocks/CMakePresets.json` - Presets für Hilfsprogramme.

</details>

---

### 🏗️ Architektur und Entwurfsmuster

#### Angewandte SOLID-Prinzipien

- Einzelverantwortung durch nummerierte Module und spezialisierte Unterschichten.
- Fassaden für komplexe Subsysteme wie Statistiken, Schaltflächen, Menüs, Ereignisse und Sicherheit.
- Klare Trennung zwischen Beobachtung, Persistenz, Metadaten, Visualisierung, Export und Übersetzung.
- Geringe Kopplung durch Verwendung von `QObject`-Eigenschaften, Signalen/Slots und Serviceschnittstellen.
- Erweiterbarkeit durch neue `.cpp/.h`-Module ohne Umschreiben des Kerns.

#### Implementierte Entwurfsmuster

- **Facade:** `GerenciadorBotoes`, `GerenciadorMenusUI`, `GeradorEstatisticas`, `SecurityManager`.
- **Observer / Event-Loop:** Qt-Signale/Slots, Ereignis-Callbacks und Überwachungsbenachrichtigungen.
- **Worker / Producer-Consumer:** Ereigniswarteschlangen, `DatabaseWriter`, Diagramm-Worker und Scanner.
- **Model / View:** `EventTableModel` mit `QTableView`, unter Beibehaltung der Kompatibilität mit `QTableWidget`.
- **Factory:** Erstellung von Diagramm-Widgets über `ChartWidgetFactory`.
- **Adapter:** `LegacyVariantChartAdapter` wandelt ältere Payloads in aktuelle Modelle um.
- **Repository / Lokales DAO:** SQLite-Zugriff über Qt SQL-Verbindungen mit eindeutigen Namen.
- **Informelle Strategy:** Getrennte Generatoren für jeden Diagrammtyp und getrennte Extraktoren nach Dateityp.

#### Multithreading und Leistung

- Initialscan in einem Qt-Worker.
- Kontinuierliche Überwachung in einem `std::thread`.
- Verarbeitung des Ereignispuffers in einem dedizierten Thread.
- Ereignisspeicherung mit `DatabaseWriter`, interner Warteschlange und Batch-Persistenz.
- Batch-Extraktion von Metadaten mit `QThreadPool`.
- Diagrammgenerierung mit `GraficoWorker`.
- Caching von Beschriftungen, Pinseln (Brushes) und Übersetzungen in `EventTableModel`.
- Asynchrone visuelle Aktualisierung zur Vermeidung von UI-Blockaden.

#### Datenbankoptimierungen

- SQLite im WAL-Modus.
- `synchronous=NORMAL`.
- `temp_store=MEMORY`.
- `busy_timeout`.
- `mmap_size`, `cache_size`, `wal_autocheckpoint` und Journal-Begrenzung im Writer.
- Tabellen nach Ereignistyp und konsolidierte Tabelle `monitoramento`.
- Tabelle `snapshot` für die Baseline.
- Indizes nach Name, Verzeichnissen, Zeitstempel (Timestamp) und häufigen Schlüsseln.
- Batch-Einfügung mit `execBatch()` und Transaktionen.

#### Ereignisarchitektur

1. `Observador` erkennt Änderungen im Verzeichnis.
2. Rohereignisse werden klassifiziert als: hinzugefügt, gelöscht, geändert, umbenannt, verschoben oder gescannt.
3. Spezialisierte Submodule deduplizieren Rauschen und gleichen komplexe Operationen ab.
4. Metadaten werden extrahiert, sofern zutreffend.
5. Ereignisse werden an die Benutzeroberfläche und an den Writer übermittelt.
6. Die lokale Datenbank ermöglicht das Neuladen, Filtern, Exportieren und Generieren von Statistiken.

#### Metadaten-Extraktion

- `QImageReader` für Bilder.
- ZIP/OpenXML-Prüfung für Dokumente, Tabellenkalkulationen und Präsentationen.
- `QSqlDatabase` für SQLite und von Qt SQL unterstützte Datenbanken.
- Header-Analyse zur Formaterkennung.
- Windows-APIs für ausführbare Dateien, Signaturen und Versionen.
- Größenberechnungen in B, KB, MB, GB und TB.
- Spezialisierte Metadaten für Protokolle, Backups, komprimierte Archive, OLE, strukturierte Daten und Quellcode.

#### Sicherheit

Die Schicht `source/security` erweitert die Überwachung um Reaktions- und Untersuchungsfunktionen:

- Verzeichnisprofile und Multi-Überwachung.
- Regelbasierte Warnungen.
- Sitzungskorrelation.
- Kryptografische Baseline.
- Quarantäne und Wiederherstellung von Elementen.
- Beweispakete.
- Risikobewertung.
- Konfigurierbares Backend über Qt SQL, bei Bedarf mit lokalem SQLite-Cache.

---

### 🔄 Hauptausführungsfluss

1. `main.cpp` ruft `source::iniciar_aplicacao(argc, argv)` auf.
2. `src_01_InicializadorMain` passt das Arbeitsverzeichnis, die Windows-Identität und das Initialisierungs-Fallback an.
3. `src_02_SplashAppStarter` erstellt `QApplication`, lädt Symbol, Übersetzungen und Splash-Screen.
4. `InterfaceMonitor` baut das Hauptfenster auf und registriert gemeinsam genutzte Eigenschaften.
5. Manager für Schaltflächen, Menüs, Ereignisse, Fortschritt, Daten und Statistiken werden erstellt.
6. Die Lokalisierungsschicht legt Sprache und Übersetzer fest.
7. Die Sicherheitsschicht startet ihren Manager und Writer.
8. Der Benutzer wählt ein Verzeichnis aus.
9. `Observador` e `DiretorioScanner` führen den Initialscan durch.
10. Der Snapshot und die `escaneado`-Ereignisse werden persistent gespeichert.
11. Die kontinuierliche Überwachung verwendet `CreateFileW` und `ReadDirectoryChangesW`.
12. Ereignisse werden klassifiziert, angereichert und in die Warteschlange eingereiht.
13. `DatabaseWriter` führt die Batch-Persistenz durch.
14. Tabelle, Filter, Zähler und Farben werden aktualisiert.
15. Statistiken, Exporte und Dialoge rufen den aktuellen Zustand ab.
16. Beim Beenden werden Threads und Verbindungen ordnungsgemäß geschlossen.

---

### 📦 Anforderungen und Abhängigkeiten

#### Kernbibliotheken

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
- Windows-Bibliotheken: `wintrust`, `version`, `crypt32`.

#### Build-Anforderungen

- CMake 4.1 oder höher für den Hauptbuild.
- CMake 3.24 oder höher für die Hilfsprogramme in `mocks/`.
- Qt 6.11.0 in den aktuellen Presets.
- MinGW 13.1.0 oder Visual Studio 2022 Build Tools, je nach Preset.
- Windows SDK, passend zur gewählten Toolchain.

#### CMake-Presets

Mit MinGW konfigurieren und kompilieren:

```powershell
cmake --preset qt6-mingw-debug
cmake --build --preset linceu-qt6-mingw-debug
```

Mit MSVC Debug konfigurieren und kompilieren:

```powershell
cmake --preset qt6-msvc-debug
cmake --build --preset linceu-qt6-msvc-debug
```

Mit MSVC Release konfigurieren und kompilieren:

```powershell
cmake --preset qt6-msvc-release
cmake --build --preset linceu-qt6-msvc-release
```

Versionsbearbeitungs-Hilfsprogramm kompilieren:

```powershell
cd mocks
cmake --preset mock-qt6-mingw-debug
cmake --build --preset mock-qt6-mingw-debug
```

#### Bereitstellung (Deploy)

Unter Windows versucht `CMakeLists.txt`, `windeployqt` zu finden und kopiert:

- die fertige ausführbare Datei namens `Linceu_Lighthouse`;
- Qt-Abhängigkeiten;
- `source/assets`;
- `.qm`-Dateien der Anwendung;
- Qt-Übersetzungen nach `locale/qt`.

---

### ✨ Hauptfunktionen

<details>
<summary><b>Dateiüberwachung</b></summary>

- Rekursiver Initialscan.
- Persistenter Snapshot.
- Echtzeitüberwachung über native Windows-APIs.
- Ereignisse: hinzugefügt, gelöscht, geändert, umbenannt, verschoben und gescannt.
- Deduplizierung und Abgleich komplexer Ereignisse.
- Erkennung massiver Dateioperationen.
- Pausieren und Stoppen von Scan/Überwachung.

</details>

<details>
<summary><b>Datenbank und Persistenz</b></summary>

- `monitoramento.db` unter `%LOCALAPPDATA%\LinceuLighthouse`.
- Tabellen nach Ereignistyp.
- Konsolidierte Tabelle für Abfragen und Exporte.
- Optimierte Indizes.
- Asynchrones Batch-Schreiben.
- Lokale Konfigurationen in JSON und INI.
- Zeitstempelprotokolle (Timestamped Logs).

</details>

<details>
<summary><b>Metadaten</b></summary>

- Dateityp.
- Größe in mehreren Einheiten.
- Attribute.
- Autor.
- Dimensionen.
- Dauer.
- Bitrate.
- Schutz.
- Seiten, Zeilen, Wörter, Folien, Tabellenblätter, Tabellen und Datensätze, sofern zutreffend.
- Signatur und Version ausführbarer Dateien unter Windows.

</details>

<details>
<summary><b>Visualisierung und Tabelle</b></summary>

- `QTableView` mit `EventTableModel`.
- Übergangskompatibilität mit `QTableWidget`.
- Sortierung.
- Spaltensichtbarkeit.
- Farben nach Operation und Spalte.
- Helles/Dunkles Windows-Design.
- Neuübersetzung vorhandener Daten.
- Inkrementelle Ereignisaktualisierung.

</details>

<details>
<summary><b>Statistiken und Diagramme</b></summary>

- 17 spezialisierte Generatoren.
- Raster-Rendering mit `QPainter`.
- OpenGL-Backend über `QOpenGLWidget`.
- Qt Quick/QML-Integration.
- Auswahlbedienfeld.
- Neuberechnung nach Sprachwechsel.
- Diagrammspeicherung.

</details>

<details>
<summary><b>Erweiterte Filter</b></summary>

- Operation.
- Dateiendung.
- Text.
- Datumsbereich.
- Filterkombination.
- Synchronisation mit Menüs.
- Aktualisierter Zählerstand.
- Statuspersistenz.

</details>

<details>
<summary><b>Datenexport</b></summary>

- Excel `.xlsx`.
- CSV `.csv`.
- Text `.txt`.
- JSON `.json`.
- XML `.xml`.
- SQLite `.db`.
- Export der Auswahl, der aktiven Spalten und der aktiven Filter.
- Unterstützung für Daten aus Model/View.

</details>

<details>
<summary><b>Internationalisierung</b></summary>

- Sechs Sprachen.
- Übersetzung von Benutzeroberfläche, Operationen und Metadaten.
- Textuelles Fallback.
- Persistente Spracheinstellung.
- Integriertes Handbuch und institutionelle Inhalte.
- Über-Dialog reagiert direkt auf Sprachwechsel.

</details>

<details>
<summary><b>Sicherheit</b></summary>

- Multi-Verzeichnis und Profile.
- Warnzentrale.
- Echtzeit-Sitzungen.
- Quarantäne.
- Wiederherstellung.
- Beweispakete.
- Benachrichtigungen.
- Risikobewertung und Regeln.

</details>

---

### 🎯 Implementierte Best Practices

#### Sauberer Code

- Organisation nach Schichten und nummerierten Modulen.
- Klassen und Funktionen mit klar abgegrenzten Verantwortlichkeiten.
- Konsistente Verwendung von Namespaces.
- `.h`-Header getrennt von `.cpp`-Implementierungen.
- Vorgesehene Fallbacks bei Fehlern in UI, Logging oder Initialisierung.
- Gezielte Kommentare in komplexen Bereichen.

#### Leistung

- Native Überwachung anstelle von Polling.
- SQLite-Batch-Schreiben.
- Indizes auf häufig abgefragten Feldern.
- Worker und Threads für rechenintensive Aufgaben.
- Caches für Übersetzungen, Beschriftungen und Farben.
- Inkrementelle Tabellenaktualisierung.
- Vermeidung schwerer Analyseabhängigkeiten zur Laufzeit.

#### Wartbarkeit

- Sauber getrennte Schichten.
- Kompakte Module für spezifische Abläufe.
- Deklarativer CMake-Build.
- Presets für Toolchains.
- Versionierte Assets und Übersetzungen.
- Isolierte Hilfsprogramme in `mocks/`.
- Technische Dokumentation im Repository.

#### Sicherheit und Robustheit

- Nutzung des lokalen persistenten Benutzerverzeichnisses.
- SQLite-Verbindungen mit eindeutigen Namen.
- Geordnetes Beenden von Threads.
- Fehler- und Diagnoseprotokolle.
- Defensives Error-Handling mit `try/catch` an den Grenzen der Benutzeroberfläche.
- Zeigervalidierung vor der Verwendung in Qt-Integrationen.

---

### 🚀 Hinzufügen Neuer Funktionen

- **Neuer Diagrammtyp:** Erstellen Sie einen neuen Generator in `source/data/GeradorEstatisticas/`, basierend auf `gst_01_BaseGerador`, und registrieren Sie die Option in der Statistik-Benutzeroberfläche.
- **Neuer Ereignistyp:** Fügen Sie eine spezialisierte Klasse oder Funktion in `source/services` hinzu, aktualisieren Sie bei Bedarf die Spalten und registrieren Sie die Persistenz im `BaseEvento`-Ablauf.
- **Neue Spalte:** Aktualisieren Sie `ob_09_GerenciadorColunas`, das Schema in `gbank_02_criar_banco_de_dados`, den Export und die Übersetzungen.
- **Neues Metadatum:** Implementieren Sie einen Extraktor in `GerenciamentoMetadados`, fügen Sie die Spalte, Übersetzung und Anzeigelogik hinzu.
- **Neue Sprache:** Erstellen Sie `linceu_XX.ts`, kompilieren Sie nach `.qm`, fügen Sie das Fallback in `LocalizadorQt` ein und übersetzen Sie die Assets bei Bedarf.
- **Neuer Filter:** Implementieren Sie in `source/tools/GerenciamentoAdministradorFiltros` und synchronisieren Sie mit Menü und Zähler.
- **Neue Sicherheitsaktion:** Fügen Sie einen Dienst in `source/security` hinzu, einen Dialog in `secui_*`, die übersetzten Strings und die Persistenz im Backend.
- **Neuer Export:** Erweitern Sie `geb_03_exportar_dados.cpp` oder fügen Sie ein dediziertes Modul hinzu, unter Beibehaltung der Unterstützung für `QTableView` und `QTableWidget`.

---

### 📝 Wichtige Hinweise

#### Versionen und Toolchain

- Der Hauptbuild erfordert C++17.
- Die aktuellen Presets zielen auf Qt 6.11.0 ab.
- Das MinGW-Preset verwendet `C:/Qt/Tools/mingw1310_64`.
- Das MSVC-Preset verwendet Visual Studio 2022 Build Tools.
- Das Projekt ist auf Windows ausgerichtet, da es native Überwachungs- und Metadaten-APIs voraussetzt.

#### Kritische Dateien

- `%LOCALAPPDATA%\LinceuLighthouse\monitoramento.db` - Hauptdatenbank für Ereignisse.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security.db` - Lokaler Cache der Sicherheitsschicht bei Verwendung.
- `%LOCALAPPDATA%\LinceuLighthouse\language_config.json` - Spracheinstellung.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_config.json` - Spaltensichtbarkeit.
- `%LOCALAPPDATA%\LinceuLighthouse\colunas_coloridas.json` - Farbige Spalten.
- `%LOCALAPPDATA%\LinceuLighthouse\cores_config.json` - Konfigurierte Farben.
- `%LOCALAPPDATA%\LinceuLighthouse\security\security_ui.ini` - Einstellungen der Sicherheits-Benutzeroberfläche.

#### Protokolle (Logs)

Protokolle werden mit Zeitstempel gespeichert unter:

```text
%LOCALAPPDATA%\LinceuLighthouse\logs\file_monitor_YYYYMMDD_hhmmss.log
```

#### Hilfsprogramme

- `mocks/find_VersionEditor.cpp` aktualisiert Versionen und Daten in Assets, `README.md`, `OBSERVACAO.md` und Abschnitten des Über-Dialogs.
- `mocks/compile_translations.cpp` unterstützt die Kompilierung und Validierung von Übersetzungen.
- Die Hilfsprogramme verwenden ein eigenes CMake unter `mocks/`.

---

### 📄 Lizenz und Autoren

**Autor:** Fernando Nillsson Cidade<br>
**Version:** v2026.8.25.0<br>
**Lizenz:** [GNU Lesser General Public License v3.0 (LGPL-3.0)](https://github.com/fernandoncidade/Linceu_Lighthouse_cpp/blob/main/LICENSE)

Weitere Informationen finden Sie auch in `OBSERVACAO.md`, `ARCHITECTURE.md`, `MANUAL.md`, den integrierten Assets und der spezifischen Dokumentation in den Quellcodemodulen.

---

</details>
