# Arquitetura do Sistema / System Architecture — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher a arquitetura em português</summary><br>

# Linceu Lighthouse — Arquitetura de Sistema e Especificações Técnicas

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Padrão:** C++17 | Qt 6.x | Win32 API (`ReadDirectoryChangesW`, `wintrust`, `version`, `crypt32`) | SQLite3  
**Plataforma Alvo:** Windows 11 / Windows 10 / Windows Server 2022+ (x64)  
**Autor:** Fernando Nillsson Cidade  

---

## 1. Visão Geral e Filosofia Arquitetural

O **Linceu Lighthouse** é uma plataforma desktop nativa para Windows desenvolvida em C++17 com Qt 6, projetada para fornecer visibilidade absoluta, histórico cronológico estruturado, inteligência de arquivos sem IA e auditoria de segurança local.

A arquitetura é fundamentada no princípio da **Privacidade Soberana (Local-First / Sovereign Privacy)**: todos os eventos, metadados enriquecidos, bancos de dados SQLite, caches em memória e laudos de evidência são processados e armazenados estritamente na máquina local do usuário, sem qualquer conexão externa de rede, sem telemetria, sem computação em nuvem e **sem dependência de modelos de linguagem (LLMs) ou IA externa**. Toda a análise, classificação e pontuação de risco utilizam lógica algorítmica e matemática determinística pura.

Em termos funcionais e estruturais, o sistema opera como uma **pipeline de dados local dividida em 7 domínios cooperantes**:

```text
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                          PIPELINE DE DADOS - LINCEU LIGHTHOUSE                          │
└─────────────────────────────────────────────────────────────────────────────────────────┘
                                             │
 1. DOMÍNIO DE MONITORAMENTO (APIs Nativas Windows / ReadDirectoryChangesW)
                                             ↓
 2. DOMÍNIO DE EVENTOS E CORRELAÇÃO (Normalização / Detecção Real de Move & Rename)
                                             ↓
 3. DOMÍNIO DE METADADOS (30+ Categorias, Magic Numbers, Hashes Cripto)
                                             ↓
 4. DOMÍNIO DE PERSISTÊNCIA (SQLite Local Assíncrono / Batch Writers em WAL)
                                             ↓
 5. DOMÍNIO DE ANALYTICS & ESTATÍSTICAS (17 Gráficos Nativos / Determinismo sem IA)
                                             ↓
 6. DOMÍNIO DE SEGURANÇA E AUDITORIA (Baseline Criptográfico, Regras, Risco, Evidências)
                                             ↓
 7. DOMÍNIO DE APRESENTAÇÃO (Interface Qt6 Widgets, DWM Mica/Dark, Internacionalização)
```

### 1.1. Diagrama Geral das Camadas do Sistema

```
+-----------------------------------------------------------------------------------------+
|                                 CAMADA DE APRESENTAÇÃO                                  |
|                 (Qt 6.x Widgets, DWM Mica/Dark, Subsistema Gráfico OpenGL)              |
|                                                                                         |
|   InterfaceMonitor  |  SplashAppStarter  |  MultiDirectoryDialog  |  AlertCenterDialog  |
|   SessionCenter     |  RecoveryDialog    |  EvidenceDialog        |  NotificationPopup  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                           SUBSISTEMA GRÁFICO & ESTATÍSTICAS                             |
|                             (source/data/Charts - 17 Gráficos)                          |
|                                                                                         |
|    ChartLayoutEngine   |   RasterChartView   |   QuickChartItem   |   LegacyAdapter     |
|    - Linha do Tempo    |   - Mapas de Calor  |   - Fluxo Sankey   |   - Pareto/Treemap  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                    NÚCLEO DE SERVIÇOS                                   |
|                                (source/services/ - C++17)                               |
|                                                                                         |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Observador & Scanner       |  |  Correlação e Semântica de Eventos               |  |
|  |  - ReadDirectoryChangesW    |  |  - EventoModificado / EventoAdicionado / Excluido|  |
|  |  - Fila Assíncrona Buffer   |  |  - Detecção real de Move e Rename                |  |
|  |  - DiretorioScanner         |  |  - Eliminação de pares espúrios Add/Delete       |  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Enriquecimento de Metadados|  |  Persistência Local SQLite3                      |  |
|  |  - 30+ Categorias           |  |  - DatabaseWriter / SecurityDbWriter             |  |
|  |  - Assinaturas Magic Numbers|  |  - Gravação Assíncrona em Lote (Batch WAL)       |  |
|  |  - Tipos MIME Traduzidos    |  |  - Persistência em %LOCALAPPDATA%\LinceuLighthouse|  |
|  +-----------------------------+  +--------------------------------------------------+  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                  SUBSISTEMA DE SEGURANÇA                                |
|                                 (source/security/ - C++17)                              |
|                                                                                         |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Baseline & Integridade     |  |  Quarentena e Restauração Segura                 |  |
|  |  - Baseline Criptográfico   |  |  - Isolamento de Binários Suspeitos              |  |
|  |  - Assinatura Hash SHA-256  |  |  - Validação Criptográfica pré-restauração       |  |
|  |  - Detecção de Alterações   |  |  - Prevenção de substituição acidental de arquivos| |
|  +-----------------------------+  +--------------------------------------------------+  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Motor de Regras e Risco    |  |  Exportação de Evidências                        |  |
|  |  - Score de Risco (0-100)   |  |  - Pacotes de Auditoria Forense (.xlsx, .json)   |  |
|  |  - Alertas em Tempo Real    |  |  - Cadeia de Custódia Imutável                   |  |
|  +-----------------------------+  +--------------------------------------------------+  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                 APIS NATIVAS DO WINDOWS                                 |
|                                                                                         |
|      kernel32.dll (ReadDirectoryChangesW) |  crypt32.dll (CNG SHA-256)                  |
|      wintrust.dll (Validação de Assinatura) |  version.dll (Metadados de Executáveis)     |
|      user32.dll                           |  shell32.dll                                |
+-----------------------------------------------------------------------------------------+
```

---

## 2. Dependências Efetivas e Ambiente de Compilação

### 2.1. Linguagem, Build e Plataforma
- **Padrão C++:** C++17 definido estritamente em `CMakeLists.txt` (`set(CMAKE_CXX_STANDARD 17)`).
- **CMake:** Configurado com `CMAKE_AUTOMOC` e `CMAKE_AUTORCC`.
- **Compiladores Homologados:** MinGW GCC 13+ (x86_64-w64-mingw32) e Microsoft Visual C++ (MSVC toolset v143 / Visual Studio 2022).
- **Alvo Windows:** Aplicação Windows nativa criada como alvo `WIN32`.
- **Recurso Nativo:** `source/Linceu_Lighthouse_cpp.rc` com ícones e metadados PE.
- **Deploy Pós-Build:** Orquestrado com `windeployqt`, empacotamento de `source/assets/` e compilação dos catálogos de tradução `.qm`.

### 2.2. Módulos Qt Vinculados
O binário vincula dinamicamente os módulos fundamentais do Qt 6:
- `Qt::Core`, `Qt::Gui`, `Qt::Widgets`
- `Qt::Sql` (Conexão e pooling para SQLite3 local)
- `Qt::Xml` (Geração de relatórios estruturados e parsing de esquemas)
- `Qt::Concurrent` (Paralelismo assíncrono para extração de metadados em lote)
- `Qt::OpenGL`, `Qt::OpenGLWidgets` (Aceleração por hardware do subsistema de visualização)
- `Qt::Quick`, `Qt::Qml` (Composição de cena vetorial via Qt Quick Scene Graph)

Não há dependência direta de módulos opcionais como servidores HTTP embutidos, WebSockets remotos, navegadores baseados em Chromium ou autorizadores de rede externa.

### 2.3. Bibliotecas Nativas do Windows
- `kernel32.dll`: Observação de baixo nível de diretórios via `ReadDirectoryChangesW`.
- `crypt32.dll`: Hash criptográfico SHA-256 via CNG (Cryptography API: Next Generation).
- `wintrust.dll`: Validação de certificados Authenticode e integridade de binários executáveis.
- `version.dll`: Extração de metadados de recursos de executáveis e DLLs do Windows.

### 2.4. Variáveis Operacionais de Ambiente
- `LINCEU_OPENGL_MODE=auto` *(Padrão)*: Seleciona automaticamente o melhor perfil de aceleração.
- `LINCEU_OPENGL_MODE=desktop`: Força OpenGL desktop nativo (GPUs dedicadas NVIDIA/AMD).
- `LINCEU_OPENGL_MODE=angle`: Força tradução via Direct3D 11 via ANGLE.
- `LINCEU_OPENGL_MODE=software`: Força rasterização por CPU (máquinas virtuais e sessões RDP).

---

## 3. Mapa de Diretórios e Componentes

| Caminho | Papel Arquitetural |
|---|---|
| `main.cpp` | Ponto de entrada mínimo do processo. Encaminha para `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Bootstrap principal, backend gráfico, registro QML, tradução, splash e instanciação do writer do banco. |
| `source/src_02_SplashAppStarter.*` | Fluxo de inicialização com tela splash, log de carregamento e barra de progresso. |
| `source/gui/` | Janela principal (`InterfaceMonitor`), bootstrap visual, mensagens e internacionalizador. |
| `source/ui/` | Gerenciadores especializados da UI (botões, menus, estatísticas, progresso, dados, diálogos e manual). |
| `source/services/` | Observação de diretórios, scanner recursivo, correlação semântica de eventos, persistência SQLite e metadados. |
| `source/security/` | Motor de regras locais, score de risco, baseline criptográfico, quarentena, restauração assistida e evidências. |
| `source/data/` | Geração estatística e motor gráfico interno (`source/data/Charts/` com 17 geradores). |
| `source/tools/` | Filtros multidimensionais, seletores de calendário e administração de consultas. |
| `source/locale/` | Catálogos de internacionalização (`.ts`, `.qm`) para 6 idiomas. |
| `source/assets/` | Documentação regulatória da Microsoft Store, ícones, release notes, EULA, licenças e notices. |
| `source/utils/` | Gerenciamento de caminhos persistentes, resolução de ícones, logger central e utilitários de aplicação. |
| `docs/` | Documentação técnica, arquitetura, conformidade, guia de uso, segurança e licenciamento. |

---

## 4. Fluxo de Inicialização e Ciclo de Vida

```text
main.cpp
  │
  ▼
source::iniciar_aplicacao(argc, argv)
  │
  ├── ajustar_cwd() (Garante diretório de trabalho consistente para assets e banco)
  │
  ├── Tentar inicialização via SplashAppStarter
  │     │
  │     ├── criar QApplication
  │     ├── configurar_backend_grafico() (OpenGL/QSurfaceFormat com double buffer e antialiasing)
  │     ├── inicializar_sistema_traducao()
  │     ├── exibir Splash com progresso
  │     ├── criar InterfaceMonitor
  │     └── iniciar DatabaseWriter de forma eager se houver db_path
  │
  └── Fallback padrão (caso splash não seja utilizado)
        │
        ├── configurar_backend_grafico()
        ├── QApplication(argc, argv)
        ├── register_qml_types() (Expõe QuickChartItem ao QML)
        ├── security::pg::prepare_runtime() (Prepara runtime de persistência)
        ├── SetCurrentProcessExplicitAppUserModelID(L"LinceuLighthouse.App")
        ├── Definir ícone da aplicação (linceu_sem_fundo.ico)
        ├── Internacionalizador::inicializar_sistema_traducao(&app)
        ├── window = new InterfaceMonitor()
        ├── DatabaseWriter::get_instance(dbPath)
        ├── window->show()
        └── app.exec()
```

---

## 5. Janela Principal (`InterfaceMonitor`) e Hierarquia de Janelas

`source::gui::InterfaceMonitor` herda de `QMainWindow` e atua como o orquestrador central de todos os gerenciadores de alto nível:

### 5.1. Componentes Agregados
- `LocalizadorQt`: Instalação e propagação de catálogos dinâmicos de tradução.
- `GerenciadorTabela`: Controle da exibição, ordenação, seleção e temas da tabela de eventos.
- `GerenciadorColunas`: Gerenciamento de visibilidade, larguras e ordem das colunas.
- `GerenciadorEventosArquivo`: Integração e despacho de eventos do sistema de arquivos para a interface.
- `GerenciadorDados`: Exportação de dados e relatórios multiformato.
- `GerenciadorMonitoramento`: Controle de ciclo de vida da observação (iniciar, parar, pausar, retomar).
- `GerenciadorLimpeza`: Limpeza seletiva de histórico em memória e no banco.
- `GerenciadorBotoes` / `GerenciadorBotoesUI`: Ações da barra lateral e feedback visual.
- `GerenciadorMenusUI`: Estruturação da barra de menus superior.
- `GerenciadorEventosUI` / `GerenciadorProgressoUI`: Barra de progresso e eventos de janela.
- `GerenciadorEstatisticasUI`: Abertura e integração da janela de estatísticas.
- `Filtros`: Mecanismo de filtragem multidimensional por evento, extensão e datas.
- `BaseEvento`: Normalizador central de eventos de arquivos.
- `Observador`: Instância central do observador Win32.
- `EventTableModel`: Modelo virtualizado do Qt para a tabela central.
- `SecurityManager`: Fachada do subsistema de segurança e conformidade.

### 5.2. Gerenciamento e Hierarquia de Janelas
A aplicação adota uma hierarquia estruturada de diálogos associados via propriedades dinâmicas do Qt (`interface_principal`), incluindo janelas de manual, estatísticas, diálogos de cores e painéis de segurança:
- **`eventFilter`:** Filtra eventos de ativação, foco e visibilidade para evitar que janelas filhas fiquem presas em segundo plano.
- **Prevenção de Reentrância:** Sincronização agendada via `QTimer::singleShot` para evitar loops de ativação entre janelas.
- **Encerramento Coordenado:** O fechamento da janela principal fecha automaticamente todos os diálogos auxiliares vinculados.

---

## 6. Serviços de Observação e Correlação Semântica

### 6.1. Observador (`ob_01_Observador`)
Encapsula o monitoramento de baixo nível via `ReadDirectoryChangesW`:
- Operação em `std::thread` própria desacoplada da GUI;
- Buffer circular multithread thread-safe capaz de absorver picos de rajada (>50.000 eventos/s);
- Deduplicação inteligente de eventos repetitivos em frações de milissegundos;
- Detecção automática de sobrecarga e recuperação graciosa de buffer (*buffer overflow handling*).

### 6.2. Correlação Semântica de Eventos
O sistema diferencia com exatidão operações complexas no disco:
- **`EventoRenomeado` (`ob_07_EventoRenomeado`):** Identifica itens cujo caminho pai permanece inalterado e apenas o nome do arquivo foi modificado.
- **`EventoMovido` (`ob_08_EventoMovido`):** Algoritmo de correlação temporal que associa notificações consecutivas de remoção e criação em diretórios distintos, reconstruindo a genealogia real da movimentação e **eliminando pares falsos de criação e exclusão**.

### 6.3. Tabela de Submódulos de Serviços

| Submódulo | Responsabilidade Técnica |
|---|---|
| `GerenciamentoBaseEvento` | Criação do schema SQLite, registro, persistência assíncrona e writer em lote. |
| `GerenciamentoDiretorioScanner` | Varredura recursiva de diretórios, fila de arquivos para baseline e worker em `QThread`. |
| `GerenciamentoEventoModificado` | Agrupamento de eventos consecutivos, intervalo dinâmico e cache de atributos. |
| `GerenciamentoEventoMovido` | Correlação entre remoção e criação em pastas distintas para gerar eventos `MOVED`. |
| `GerenciamentoMetadados` | Extração de metadados por categoria de arquivo, Magic Numbers e identificação MIME. |
| `GerenciamentoObservador` | Controle de início, parada, pausa, reinício, buffers e recuperação de erros. |
| `GerenciamentoTabela` | Modelo de dados virtualizado, estilos cromáticos, cabeçalhos traduzidos e background worker. |

---

## 7. Persistência Local e Armazenamento SQLite3

- **Banco de Dados Local:** SQLite3 embarcado em modo **WAL (Write-Ahead Logging)**, operado via driver `QSQLITE` do Qt.
- **Localização dos Dados:** `%LOCALAPPDATA%\LinceuLighthouse` (ou caminho persistente personalizado).
- **Padrões de Persistência:**
  - `DatabaseWriter`: Writer assíncrono thread-safe baseado em fila FIFO e commits em lote (*batch commits*), evitando engasgos ou bloqueios no loop de eventos principal da interface.
  - `SecurityDbWriter`: Writer especializado para alertas, sessões temporais, inventários de baseline e histórico de quarentena.
  - Eventos serializados em `QVariantMap` mantendo total interoperabilidade entre o banco, a tabela virtualizada e os 17 geradores de estatísticas.

---

## 8. Subsistema de Metadados e Tipagem MIME Real

O módulo `GerenciamentoMetadados` inspeciona e cataloga mais de **30 categorias de atributos**:
- **Magic Numbers e Tipagem MIME:** Inspeção direta dos bytes iniciais do arquivo para identificação do tipo MIME real, independentemente da extensão nominal do arquivo (detectando camuflagens maliciosas).
- **Categorias Suportadas:** Código-fonte, Imagens, Áudio, Vídeo, Documentos de Escritório, Planilhas, Apresentações, Bancos de Dados, Binários Executáveis, Arquivos Compactados, Backups, Logs e Arquivos Estruturados.
- **Timestamps Completos:** Datas de Criação, Modificação e Último Acesso lidas e persistidas tanto para arquivos individuais quanto para pastas, subpastas e diretórios filhos.

---

## 9. Subsistema de Segurança, Quarentena e Forense

`source::security::SecurityManager` atua como a fachada central do subsistema de segurança:

```text
                                SecurityManager (Fachada)
                                           │
         ┌──────────────────┬──────────────┴─────┬──────────────────┐
         ▼                  ▼                    ▼                  ▼
base_01_baseline    agre_01_motor_regras    quar_01_quarentena   evid_01_exportacao
- Hash SHA-256      - Regras locais         - Isolamento         - Pacotes Forenses
- Inventário drift  - Score Risco (0-100)   - Restauração Segura - XLSX, JSON, CSV
```

### 9.1. Componentes Técnicos de Segurança
- **`base_01_baseline_criptografico`:** Gera e armazena assinaturas hash CNG SHA-256 para todos os arquivos da árvore, permitindo auditoria diferencial de alterações indevidas (*baseline drift*).
- **`risk_01_score_risco` & `agre_01_motor_regras`:** Motor analítico que avalia o comportamento de arquivos e calcula scores de risco (0 a 100), gerando alertas em 4 níveis determinísticos (Baixo, Médio, Alto e Crítico).
- **`quar_01_quarentena`:** Isola bytes de binários suspeitos com registro do hash SHA-256 original.
- **`RecoveryDialog`:** Diálogo de quarentena com atualização diferencial sem tremulação (`find_row_by_text`) e restauração com verificação criptográfica estrita prévia.
- **`AlertCenterDialog`:** Central de triagem de incidentes com correlação direta entre regras violadas e evidências.
- **`SessionCenterDialog`:** Carregamento assíncrono em segundo plano (`run_on_security_runtime_thread`) para análise temporal sem travamento da UI.
- **`MultiDirectoryDialog`:** Criação e persistência de perfis para auditoria concomitante de múltiplos diretórios.

---

## 10. Motor Gráfico e Visualização Estatística (17 Gráficos)

O subsistema em `source/data/Charts/` implementa uma arquitetura própria e autônoma de visualização gráfica:

```text
QVariantMap (Dataset Gerado)
         │
         ▼
LegacyVariantChartAdapter
         │
         ▼
LgplChartModel (Modelo Tipado de Cena)
         │
         ▼
ChartLayoutEngine (Cálculo de Geometria, Escalas, Eixos e Legendas)
         │
         ├────────────────────────┬────────────────────────┐
         ▼                        ▼                        ▼
RasterChartView          WidgetChartView          QuickChartItem
(QWidget + QPainter)     (QOpenGLWidget)          (QQuickItem Scene Graph)
Usado em Estatísticas    Overlay acelerado        Composição QML fluida
```

### 10.1. Gráficos Nativos Suportados
1. Pizza de Distribuição de Operações
2. Barras por Categoria e Tipo de Arquivo
3. Linha do Tempo Cronológica de Eventos
4. Treemap Hierárquico por Tipo de Arquivo
5. Histograma de Atividade por Horários do Dia
6. Gráfico de Pareto de Impacto Cumulativo
7. Gráficos de Linha / Cluster de Tendência
8. Boxplots de Dispersão e Tamanhos Físicos
9. Radar de Eventos Multivariado
10. Mapa de Calor (Heatmap) por Dia da Semana e Hora
11. Gráfico de Dispersão (Scatter Plot)
12. Fluxo Sankey de Diretórios para Tipos de Arquivo
13. Gráfico Radar de Risco e Anomalias
14. Gráfico Dotplot de Frequência
15. Fluxo Sankey por Evento e Caminho
16. Fluxo Sankey por Tipo e Caminho
17. Árvore de Ocupação e Navegação de Diretórios

**Decisão de Estabilidade:** O `RasterChartView` (baseado em `QWidget` + `QPainter` com antialiasing) é utilizado na janela principal de Estatísticas para garantir total estabilidade em ambientes com múltiplos monitores e placas gráficas heterogêneas, evitando problemas comuns de contexto OpenGL compartilhado.

---

## 11. Concorrência, Multithreading e Responsividade

Para suportar operações de alta intensidade sem comprometer a fluidez da interface:
- **`std::thread`:** Thread dedicada e prioritária do `Observador` para a leitura contínua de `ReadDirectoryChangesW`.
- **`QThread`:** Workers assíncronos para o `DiretorioScanner` e carregamento de sessões temporais no `SessionCenterDialog`.
- **`QtConcurrent`:** Processamento paralelo para extração de metadados em lote de arquivos recém-detectados.
- **`QTimer`:** Temporizadores de flush periódico de buffer, deduplicação e sincronização diferencial da interface gráfica.
- **Atomics e Mutexes:** Sincronização leve de estado entre threads de serviço e o thread principal do Qt (`GUI thread`).

---

## 12. Regras para Evolução e Extensão da Arquitetura

### 12.1. Como Adicionar um Novo Gráfico Estatístico
1. Criar a classe geradora especializada em `source/data/GeradorEstatisticas/` herdando de `BaseGerador`.
2. Registrar o gerador em `GeradorEstatisticas::inicializar_geradores()`.
3. Produzir payload consistente no formato `QVariantMap` / `QVariantList`.
4. Se o gráfico demandar geometria inédita, estender `ChartLayoutEngine`.
5. Expor a nova opção no diálogo de seleção em `GerenciadorEstatisticasUI`.
6. Atualizar os catálogos de tradução `.ts` e `.qm`.

### 12.2. Como Adicionar uma Nova Coluna na Tabela de Eventos
1. Adicionar o campo na estrutura de metadados em `GerenciamentoMetadados/`.
2. Registrar a chave e rótulo em `GerenciadorColunas`.
3. Adicionar as traduções correspondentes no `LocalizadorQt`.
4. Validar os fluxos de ordenação, persistência de preferências de largura e exportação (.xlsx, .csv, .json).

### 12.3. Como Adicionar uma Nova Regra de Segurança
1. Implementar a lógica heurística em `source/security/agre_01_motor_regras.cpp`.
2. Integrar a checagem no `SecurityManager`.
3. Atualizar o método `ensure_schema()` para garantir persistência adequada no SQLite.
4. Integrar o disparo com a Central de Alertas (`AlertCenterDialog`) e pacote de evidências.

### 12.4. Como Adicionar um Novo Idioma (i18n)
1. Criar os arquivos de catálogo `linceu_<locale>.ts` em `source/locale/`.
2. Traduzir strings da interface, menus, diálogos de quarentena e relatórios.
3. Registrar o novo idioma no enum e mapa do `source/gui/ic_08_Internacionalizador.*`.
4. Atualizar o menu de seleção de idiomas em `GerenciadorMenusUI`.

---

## 13. Referência Rápida de Arquivos do Projeto

| Domínio Funcional | Arquivos Centrais |
|---|---|
| **Ponto de Entrada** | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| **Janela Principal** | `source/gui/ic_01_InterfaceMonitor.*` |
| **Internacionalização** | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| **Observação de Arquivos** | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| **Normalização e Eventos** | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| **Tabela Virtualizada** | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| **Metadados e MIME** | `source/services/GerenciamentoMetadados/*` |
| **Segurança e Forense** | `source/security/*` |
| **Geração de Estatísticas** | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| **Motor Gráfico (Charts)** | `source/data/Charts/*` |
| **Gerenciadores de UI** | `source/ui/*` |
| **Filtros e Ferramentas** | `source/tools/*` |
| **Assets e Legal** | `source/assets/*` |
| **Utilitários Globais** | `source/utils/*` |

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the architecture in English</summary><br>

# Linceu Lighthouse — System Architecture & Technical Specifications

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Standard:** C++17 | Qt 6.x | Win32 API (`ReadDirectoryChangesW`, `wintrust`, `version`, `crypt32`) | SQLite3  
**Target Platform:** Windows 11 / Windows 10 / Windows Server 2022+ (x64)  
**Author:** Fernando Nillsson Cidade  

---

## 1. Architectural Overview & Vision

**Linceu Lighthouse** is a native Windows desktop platform engineered in C++17 with Qt 6, designed to provide absolute visibility, structured chronological history, non-AI file activity intelligence, and local forensic security auditing.

The architecture is built upon the **Sovereign Privacy (Local-First)** foundation: all filesystem events, enriched metadata, SQLite databases, memory caches, and forensic audit evidence are processed and stored strictly on the user's host machine. There are **zero external network calls, zero telemetry beacons, zero cloud dependencies, and zero reliance on external AI APIs or Large Language Models (LLMs)**. All classification, behavioral analysis, and risk scoring routines operate using pure algorithmic logic and deterministic mathematics.

Structurally, the application functions as a **local data pipeline organized into 7 cooperating domains**:

```text
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│                          DATA PIPELINE - LINCEU LIGHTHOUSE                              │
└─────────────────────────────────────────────────────────────────────────────────────────┘
                                             │
 1. MONITORING DOMAIN (Native Windows APIs / ReadDirectoryChangesW)
                                             ↓
 2. EVENTS & CORRELATION DOMAIN (Normalization / True Move & Rename Detection)
                                             ↓
 3. METADATA DOMAIN (30+ Categories, Magic Numbers, Cryptographic Hashes)
                                             ↓
 4. PERSISTENCE DOMAIN (Asynchronous Local SQLite / Batch Writers in WAL Mode)
                                             ↓
 5. ANALYTICS & STATISTICS DOMAIN (17 Native Charts / Deterministic Intelligence)
                                             ↓
 6. SECURITY & AUDIT DOMAIN (Cryptographic Baseline, Rules, Risk, Evidence Packages)
                                             ↓
 7. PRESENTATION DOMAIN (Qt6 Widgets, DWM Mica/Dark Styling, Dynamic Localization)
```

### 1.1. System Architecture Layer Diagram

```
+-----------------------------------------------------------------------------------------+
|                                    PRESENTATION LAYER                                   |
|                 (Qt 6.x Widgets, DWM Mica/Dark, OpenGL Graphics Subsystem)              |
|                                                                                         |
|   InterfaceMonitor  |  SplashAppStarter  |  MultiDirectoryDialog  |  AlertCenterDialog  |
|   SessionCenter     |  RecoveryDialog    |  EvidenceDialog        |  NotificationPopup  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                             CHART & ANALYTICS SUBSYSTEM                                 |
|                             (source/data/Charts - 17 Charts)                            |
|                                                                                         |
|    ChartLayoutEngine   |   RasterChartView   |   QuickChartItem   |   LegacyAdapter     |
|    - Timelines         |   - Heatmaps        |   - Sankey Flows   |   - Pareto/Treemap  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                    CORE SERVICES LAYER                                  |
|                                 (source/services/ - C++17)                              |
|                                                                                         |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Observer & Scanner         |  |  Semantic Event Correlation                      |  |
|  |  - ReadDirectoryChangesW    |  |  - Modified / Added / Deleted Events             |  |
|  |  - Thread-Safe Event Queue  |  |  - True Move and Rename Semantic Resolution      |  |
|  |  - DiretorioScanner         |  |  - Elimination of Spurious Add/Delete Pairs      |  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Metadata Enrichment Engine |  |  Local SQLite3 Persistence                       |  |
|  |  - 30+ Attribute Categories |  |  - DatabaseWriter / SecurityDbWriter             |  |
|  |  - Magic Number Signatures  |  |  - Asynchronous Batch Writes (WAL Mode)         |  |
|  |  - Localized MIME Types     |  |  - Storage in %LOCALAPPDATA%\LinceuLighthouse    |  |
|  +-----------------------------+  +--------------------------------------------------+  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                    SECURITY SUBSYSTEM                                   |
|                                 (source/security/ - C++17)                              |
|                                                                                         |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Baseline & Integrity       |  |  Quarantine & Secure Restoration                 |  |
|  |  - Cryptographic Baseline   |  |  - Malicious / Suspicious File Isolation         |  |
|  |  - SHA-256 Hash Fingerprint |  |  - Pre-restoration SHA-256 Hash Verification     |  |
|  |  - Baseline Drift Detection |  |  - Accidental File Overwrite Prevention          |  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  +-----------------------------+  +--------------------------------------------------+  |
|  |  Rule Engine & Risk Scoring |  |  Forensic Evidence Export                        |  |
|  |  - Risk Score (0-100)       |  |  - Forensic Audit Packages (.xlsx, .json, .csv)  |  |
|  |  - Real-time Alert Center   |  |  - Tamper-evident Chain of Custody               |  |
|  +-----------------------------+  +--------------------------------------------------+  |
+-----------------------------------------------------------------------------------------+
                                             |
                                             v
+-----------------------------------------------------------------------------------------+
|                                 NATIVE WINDOWS OS APIS                                  |
|                                                                                         |
|      kernel32.dll (ReadDirectoryChangesW) |  crypt32.dll (CNG SHA-256)                  |
|      wintrust.dll (Authenticode Checking) |  version.dll (Executable Metadata)          |
|      user32.dll                           |  shell32.dll                                |
+-----------------------------------------------------------------------------------------+
```

---

## 2. Dependencies & Build Environment

### 2.1. Language, Build, and Toolchain
- **Language Standard:** C++17 explicitly enforced in `CMakeLists.txt` (`set(CMAKE_CXX_STANDARD 17)`).
- **CMake:** Configured with `CMAKE_AUTOMOC` and `CMAKE_AUTORCC`.
- **Supported Compilers:** MinGW GCC 13+ (x86_64-w64-mingw32) and Microsoft Visual C++ (MSVC toolset v143 / Visual Studio 2022).
- **Platform Target:** Native Windows GUI application created as a `WIN32` executable.
- **Native Resource:** `source/Linceu_Lighthouse_cpp.rc` providing icons, application manifests, and PE metadata.
- **Post-Build Deployment:** Automates `windeployqt`, asset synchronization, and compiled translation (`.qm`) distribution.

### 2.2. Linked Qt Modules
The binary dynamically links against official Qt 6 shared libraries:
- `Qt::Core`, `Qt::Gui`, `Qt::Widgets`
- `Qt::Sql` (Connection pooling and transactional engine for local SQLite3)
- `Qt::Xml` (Schema parsing and structured XML report generation)
- `Qt::Concurrent` (Multi-core thread pooling for batch metadata extraction)
- `Qt::OpenGL`, `Qt::OpenGLWidgets` (Hardware-accelerated viewport rendering)
- `Qt::Quick`, `Qt::Qml` (Hardware scene graph composition via `QuickChartItem`)

The application has no runtime dependencies on embedded Chromium browsers, remote WebSockets, external HTTP servers, or external cloud telemetry endpoints.

### 2.3. Native Windows Libraries
- `kernel32.dll`: High-performance asynchronous filesystem observation via `ReadDirectoryChangesW`.
- `crypt32.dll`: Cryptographic SHA-256 hash calculations via Windows CNG.
- `wintrust.dll`: Digital signature and Authenticode certificate verification on binary executables.
- `version.dll`: Resource inspection for Windows executables and dynamic link libraries.

### 2.4. Runtime Environment Controls
- `LINCEU_OPENGL_MODE=auto` *(Default)*: Automatically selects optimal GPU acceleration context.
- `LINCEU_OPENGL_MODE=desktop`: Forces native desktop OpenGL context (NVIDIA / AMD dedicated GPUs).
- `LINCEU_OPENGL_MODE=angle`: Forces Direct3D 11 translation via ANGLE.
- `LINCEU_OPENGL_MODE=software`: Forces CPU rasterization fallback (virtual machines and RDP sessions).

---

## 3. Directory Map & Module Breakdown

| Path | Architectural Role |
|---|---|
| `main.cpp` | Minimal application entry point. Dispatches to `source::iniciar_aplicacao`. |
| `source/src_01_InicializadorMain.*` | Primary bootstrap, OpenGL configuration, QML type registration, localization setup, and database writer ignition. |
| `source/src_02_SplashAppStarter.*` | Splash screen flow with progressive loading logs and progress bar. |
| `source/gui/` | Main window (`InterfaceMonitor`), visual orchestration, message handling, and language coordination. |
| `source/ui/` | Specialized UI managers (buttons, menus, statistical views, progress, data export, dialogs, and manual). |
| `source/services/` | Filesystem observation, directory scanning, semantic event correlation, SQLite batch persistence, and metadata extraction. |
| `source/security/` | Local rule evaluation, risk scoring, cryptographic baselines, quarantine, assisted restoration, and forensic evidence generation. |
| `source/data/` | Statistical processing facade and internal chart rendering engine (`source/data/Charts/` with 17 chart generators). |
| `source/tools/` | Multi-dimensional filter engine, calendar selectors, and query administration. |
| `source/locale/` | Internationalization source and binary files (`.ts`, `.qm`) supporting 6 languages. |
| `source/assets/` | Regulatory marketplace documentation, icons, release notes, EULA, licenses, and notices. |
| `source/utils/` | Persistent path resolvers, icon finders, central logging, and system integration helpers. |
| `docs/` | Technical specifications, architecture, regulatory compliance, user manual, and licensing models. |

---

## 4. Bootstrap Flow & Lifecycle

```text
main.cpp
  │
  ▼
source::iniciar_aplicacao(argc, argv)
  │
  ├── ajustar_cwd() (Ensures predictable working directory for assets and SQLite storage)
  │
  ├── Attempt initialization via SplashAppStarter
  │     │
  │     ├── Initialize QApplication
  │     ├── configurar_backend_grafico() (OpenGL/QSurfaceFormat with double buffering and antialiasing)
  │     ├── inicializar_sistema_traducao()
  │     ├── Render splash screen with step-by-step progress
  │     ├── Instantiate InterfaceMonitor
  │     └── Warm up DatabaseWriter instance if db_path is established
  │
  └── Default Fallback Path (if splash is skipped)
        │
        ├── configurar_backend_grafico()
        ├── QApplication(argc, argv)
        ├── register_qml_types() (Exposes QuickChartItem to QML)
        ├── security::pg::prepare_runtime() (Initializes local security persistence)
        ├── SetCurrentProcessExplicitAppUserModelID(L"LinceuLighthouse.App")
        ├── Set application icon (linceu_sem_fundo.ico)
        ├── Internacionalizador::inicializar_sistema_traducao(&app)
        ├── window = new InterfaceMonitor()
        ├── DatabaseWriter::get_instance(dbPath)
        ├── window->show()
        └── app.exec()
```

---

## 5. Main Window (`InterfaceMonitor`) & Window Hierarchy

`source::gui::InterfaceMonitor` inherits from `QMainWindow` and serves as the central hub uniting UI managers, services, and background workers:

### 5.1. Aggregated Managers
- `LocalizadorQt`: Dynamic runtime language catalog loader and propagator.
- `GerenciadorTabela`: Manages table display, sorting, visual themes, and item selection.
- `GerenciadorColunas`: Manages column visibility, order, and width profiles.
- `GerenciadorEventosArquivo`: Dispatches incoming filesystem events to the UI thread.
- `GerenciadorDados`: Multi-format data manipulation and export coordinator.
- `GerenciadorMonitoramento`: Manages observation lifecycle (start, stop, pause, resume).
- `GerenciadorLimpeza`: Memory and database record pruning controller.
- `GerenciadorBotoes` / `GerenciadorBotoesUI`: Action triggers and visual toolbar states.
- `GerenciadorMenusUI`: Top-level application menu bar structure.
- `GerenciadorEventosUI` / `GerenciadorProgressoUI`: Window lifecycle events and progress tracking.
- `GerenciadorEstatisticasUI`: Statistics dashboard controller and chart regeneration coordinator.
- `Filtros`: Multi-dimensional filtering logic (event types, extensions, timestamp intervals).
- `BaseEvento`: Core event normalizer and metadata bridge.
- `Observador`: Win32 observation engine instance.
- `EventTableModel`: Virtualized Qt item model backing the central table.
- `SecurityManager`: Facade interfacing security, forensic, and quarantine subsystems.

### 5.2. Window Hierarchy & Focus Coordination
Auxiliary dialogs (User Manual, About, Statistics, Color Pickers, Security Dialogs) are associated via Qt dynamic properties (`interface_principal`):
- **`eventFilter`:** Filters activation, show/hide, and focus events, preventing child dialogs from becoming inaccessible behind the main window.
- **Reentrancy Protection:** Asynchronous event synchronization via `QTimer::singleShot` suppresses focus oscillation loops between active windows.
- **Clean Teardown:** Closing `InterfaceMonitor` automatically triggers graceful closing of all subordinate windows.

---

## 6. Observation Services & Semantic Event Correlation

### 6.1. Observer Core (`ob_01_Observador`)
Encapsulates low-level directory observation using `ReadDirectoryChangesW`:
- Runs in a dedicated background `std::thread` decoupled from the Qt GUI thread.
- Thread-safe ring buffer designed to withstand extreme event bursts (>50,000 notifications/sec).
- High-resolution temporal event deduplication to merge redundant notifications.
- Automatic buffer overflow detection and recovery.

### 6.2. Semantic Event Correlation
Accurately categorizes ambiguous filesystem activity:
- **`EventoRenomeado` (`ob_07_EventoRenomeado`):** Identifies items whose parent directory remains identical and only the nominal filename changed.
- **`EventoMovido` (`ob_08_EventoMovido`):** Temporal correlation algorithm matching consecutive delete and create notifications across different directory roots, reconstructing the true relocation path and **eliminating false-positive add/delete pairs**.

### 6.3. Service Submodule Breakdown

| Submodule | Technical Role |
|---|---|
| `GerenciamentoBaseEvento` | Database schema initialization, record serialization, and asynchronous batch writes. |
| `GerenciamentoDiretorioScanner` | Recursive directory scanning, file queues for baseline construction, and `QThread` workers. |
| `GerenciamentoEventoModificado` | Event batching, dynamic threshold evaluation, and metadata cache. |
| `GerenciamentoEventoMovido` | Correlation of inter-folder deletions and additions to synthesize `MOVED` events. |
| `GerenciamentoMetadados` | Attribute inspection by file category, binary Magic Numbers, and MIME resolution. |
| `GerenciamentoObservador` | Observer lifecycle, buffer queue management, error handling, and recovery. |
| `GerenciamentoTabela` | Virtualized table model, color styling, translated headers, and background processing. |

---

## 7. Local Persistence & Embedded SQLite Storage

- **Local Database:** Embedded SQLite3 configured in **WAL (Write-Ahead Logging)** mode via Qt's `QSQLITE` driver.
- **Storage Location:** `%LOCALAPPDATA%\LinceuLighthouse` (or user-defined storage path).
- **Persistence Patterns:**
  - `DatabaseWriter`: Non-blocking asynchronous writer utilizing a FIFO event queue and transactional batch commits, preventing UI stuttering.
  - `SecurityDbWriter`: Dedicated writer for security alerts, temporal sessions, baseline fingerprints, and quarantine ledgers.
  - Data contracts standardized around `QVariantMap` for transparent interoperability across SQLite, the virtualized table, and the chart generators.

---

## 8. Metadata Engine & Binary MIME Type Inspection

The `GerenciamentoMetadados` subsystem extracts over **30 attribute dimensions**:
- **Magic Numbers & True MIME Categorization:** Direct binary inspection of initial file header bytes to determine genuine content MIME types regardless of misleading nominal file extensions.
- **Supported Categories:** Source Code, Images, Audio, Video, Office Documents, Spreadsheets, Presentations, Databases, Executable Binaries, Compressed Archives, Backups, Logs, and Structured Data.
- **Full Timestamps:** Creation, Modification, and Last Access timestamps recorded for individual files, parent folders, and child directory trees.

---

## 9. Security, Forensic & Quarantine Subsystem

`source::security::SecurityManager` acts as the security facade:

```text
                                SecurityManager (Facade)
                                           │
         ┌──────────────────┬──────────────┴─────┬──────────────────┐
         ▼                  ▼                    ▼                  ▼
base_01_baseline    agre_01_motor_regras    quar_01_quarentena   evid_01_exportacao
- SHA-256 Hashes    - Local Rules Engine    - Isolation          - Forensic Packages
- Drift Auditing    - Risk Score (0-100)    - Safe Restoration   - XLSX, JSON, CSV
```

### 9.1. Technical Security Modules
- **`base_01_baseline_criptografico`:** Computes and stores Windows CNG SHA-256 cryptographic hashes for monitored folder structures, enabling tamper-evident baseline drift detection.
- **`risk_01_score_risco` & `agre_01_motor_regras`:** Algorithmic heuristic engine evaluating file behaviors, computing risk scores (0–100), and raising alerts across 4 deterministic tiers (Low, Medium, High, Critical).
- **`quar_01_quarentena`:** Safely isolates suspected files while recording original SHA-256 checksums in SQLite.
- **`RecoveryDialog`:** Quarantine recovery interface featuring flicker-free differential updating (`find_row_by_text`) and mandatory pre-restoration SHA-256 cryptographic validation.
- **`AlertCenterDialog`:** Incident triage console directly correlating triggered security rules with forensic evidence.
- **`SessionCenterDialog`:** Asynchronous background session loading (`run_on_security_runtime_thread`) preventing UI latency during historical reviews.
- **`MultiDirectoryDialog`:** Persistent SQLite profiles for concurrent multi-directory observation.

---

## 10. Native Charting Engine & Statistical Visualizations (17 Charts)

The `source/data/Charts/` subsystem replaces external charting libraries with a self-contained, high-performance rendering architecture:

```text
QVariantMap (Generated Dataset)
         │
         ▼
LegacyVariantChartAdapter
         │
         ▼
LgplChartModel (Typed Scene Model)
         │
         ▼
ChartLayoutEngine (Geometry, Scales, Axes, Labels, and Legends)
         │
         ├────────────────────────┬────────────────────────┐
         ▼                        ▼                        ▼
RasterChartView          WidgetChartView          QuickChartItem
(QWidget + QPainter)     (QOpenGLWidget)          (QQuickItem Scene Graph)
Used in Statistics       Accelerated Overlay      Fluid QML Composition
```

### 10.1. Supported Native Visualizations
1. Operation Type Distribution Pie Chart
2. File Category Bar Chart
3. Chronological Activity Timeline
4. Hierarchical File Type Treemap
5. Hourly Activity Histogram
6. Cumulative Impact Pareto Chart
7. Trend Line / Cluster Chart
8. Physical Size and Dispersion Boxplots
9. Multivariate Event Radar Chart
10. Hourly Day-of-Week Heatmap
11. Scatter Plot
12. Sankey Directory-to-Type Flow
13. Risk and Anomaly Radar Chart
14. Frequency Dotplot
15. Sankey Flow by Event and Path
16. Sankey Flow by Type and Path
17. Directory Space Occupancy Tree

**Stability Architecture:** `RasterChartView` (utilizing `QWidget` + `QPainter` with high-quality antialiasing) is the primary engine used in the Statistics window, delivering reliable multi-monitor performance without OpenGL context conflicts.

---

## 11. Concurrency, Multithreading & Responsiveness

To ensure seamless desktop responsiveness under extreme I/O workloads:
- **`std::thread`:** High-priority dedicated background thread executing `ReadDirectoryChangesW`.
- **`QThread`:** Asynchronous background workers for `DiretorioScanner` and `SessionCenterDialog`.
- **`QtConcurrent`:** Multi-threaded worker pool executing batch metadata inspection.
- **`QTimer`:** Scheduled non-blocking UI flushes, event deduplication, and differential table synchronization.
- **Atomics & Mutexes:** Lightweight thread synchronization between background workers and the Qt GUI thread.

---

## 12. Architectural Extension Guidelines

### 12.1. Adding a New Statistical Chart
1. Implement a generator in `source/data/GeradorEstatisticas/` inheriting from `BaseGerador`.
2. Register the generator inside `GeradorEstatisticas::inicializar_geradores()`.
3. Provide a structured `QVariantMap` / `QVariantList` dataset.
4. If unique geometry is required, extend `ChartLayoutEngine`.
5. Expose the chart in the selection panel of `GerenciadorEstatisticasUI`.
6. Update `.ts` and `.qm` translation catalogs.

### 12.2. Adding a New Table Column
1. Add the extracted field into the metadata structure in `GerenciamentoMetadados/`.
2. Register the column identifier and header label in `GerenciadorColunas`.
3. Provide localized translations in `LocalizadorQt`.
4. Validate column sorting, width preference persistence, and export fidelity (.xlsx, .csv, .json).

### 12.3. Adding a New Security Rule
1. Implement the heuristic detection logic in `source/security/agre_01_motor_regras.cpp`.
2. Integrate the rule check in `SecurityManager`.
3. Update `ensure_schema()` to guarantee proper persistence in SQLite.
4. Connect alerts to `AlertCenterDialog` and the forensic evidence exporter.

### 12.4. Adding a New Language (i18n)
1. Generate `linceu_<locale>.ts` translation catalogs under `source/locale/`.
2. Translate UI strings, menu labels, quarantine dialogs, and reports.
3. Register the locale in the enum and lookup map within `source/gui/ic_08_Internacionalizador.*`.
4. Add the language selector item in `GerenciadorMenusUI`.

---

## 13. Quick Codebase File Reference

| Domain | Key Source Files |
|---|---|
| **Entry Point** | `main.cpp`, `source/src_01_InicializadorMain.*`, `source/src_02_SplashAppStarter.*` |
| **Main Window** | `source/gui/ic_01_InterfaceMonitor.*` |
| **Localization** | `source/gui/ic_08_Internacionalizador.*`, `source/ui/ui_12_LocalizadorQt.*`, `source/locale/*` |
| **Observation** | `source/services/ob_01_Observador.*`, `source/services/GerenciamentoObservador/*` |
| **Event Normalization** | `source/services/ob_02_BaseEvento.*`, `source/services/GerenciamentoBaseEvento/*` |
| **Virtualized Table** | `source/services/ob_10_GerenciadorTabela.*`, `source/services/GerenciamentoTabela/*` |
| **Metadata & MIME** | `source/services/GerenciamentoMetadados/*` |
| **Security & Forensics** | `source/security/*` |
| **Statistics Facade** | `source/data/st_01_GeradorEstatisticas.*`, `source/data/GeradorEstatisticas/*` |
| **Native Chart Engine** | `source/data/Charts/*` |
| **UI Managers** | `source/ui/*` |
| **Filters & Tools** | `source/tools/*` |
| **Assets & Legal** | `source/assets/*` |
| **Global Utilities** | `source/utils/*` |

</details>
