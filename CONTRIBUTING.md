# Guia de Contribuição / Contributing Guide — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher as instruções em português</summary><br>

# Contribuindo com o Linceu Lighthouse

Antes de tudo, agradecemos seu interesse em contribuir com o **Linceu Lighthouse**! Sua colaboração nos ajuda a manter esta plataforma local de inteligência, histórico e auditoria de arquivos cada vez mais estável, precisa e segura para todos.

> [!NOTE]
> **Sobre o Desenvolvimento, Suporte e Contribuições Externas:**  
> Este projeto é concebido, desenvolvido e mantido primariamente como uma iniciativa pessoal e educacional pelo autor, que está em processo contínuo de aprendizado e aprimoramento em engenharia de software e C++.  
> O envio de sugestões, abertura de *Issues* e submissão de *Pull Requests (PRs)* são sempre muito bem-vindos! No entanto, ressalta-se que a análise de código, interação em tópicos técnicos e incorporação de contribuições são realizadas no próprio ritmo do autor e de forma estritamente voluntária. Caso um chamado ou PR demore para ser respondido ou não venha a ser incorporado à branch principal, por favor não reaja com alarde ou frustração. Conforme estabelecido pela licença MIT, você possui total liberdade para criar um *Fork* independente deste repositório e evoluir suas próprias ideias, compilações e versões.

---

## 1. Código de Conduta

Ao participar deste projeto, você concorda em cumprir nosso [Código de Conduta](CODE_OF_CONDUCT.md). Trate todos os colaboradores com respeito, cortesia e profissionalismo técnico.

---

## 2. Formas de Contribuir

Você pode colaborar com o Linceu Lighthouse de várias maneiras:
- **Relatar Bugs:** Submeta relatórios de bugs claros, reproduzíveis e com logs de diagnóstico quando disponíveis.
- **Sugerir Funcionalidades:** Proponha novas ideias de visualização estatística, regras de risco para arquivos ou melhorias nos fluxos de auditoria.
- **Melhorar a Documentação:** Esclareça instruções de compilação, aprimore o manual do usuário ou adicione cenários práticos de investigação forense.
- **Enviar Pull Requests:** Implemente correções de concorrência multithread, novas heurísticas de correlação de eventos (`Move`/`Rename`), otimizações de banco SQLite ou novos geradores de gráficos.
- **Traduções (i18n):** O Linceu Lighthouse suporta nativamente 6 idiomas (Português, Inglês, Espanhol, Francês, Italiano e Alemão). Contribua revisando as traduções existentes em `source/locale/` ou adicionando novos idiomas (como Japonês ou Chinês).

---

## 3. Configuração do Ambiente de Desenvolvimento

### 3.1. Pré-requisitos
- **Sistema Operacional:** Windows 10 / 11 x64 ou Windows Server 2022+
- **Compilador:**
  - Microsoft Visual C++ (MSVC) do Visual Studio 2022 (toolset v143 ou superior) ou Build Tools 2022
  - Ou MinGW GCC 13+ (x86_64-w64-mingw32)
- **CMake:** Versão 3.24 ou superior
- **Qt:** Qt 6.x (Core, Gui, Widgets, Sql, Xml, Concurrent, OpenGL, OpenGLWidgets, Quick, Qml)
- **Ferramenta de Build:** Ninja ou MinGW Makefiles

### 3.2. Compilando com Presets do CMake
Disponibilizamos presets oficiais pré-configurados no arquivo `CMakePresets.json`:

```powershell
# Clonar o repositório
git clone https://github.com/fernandoncidade/Linceu_Lighthouse_cpp.git
cd Linceu_Lighthouse_cpp

# Opção A: Compilar com MinGW (Recomendado para Debug/Release ágil)
cmake --preset qt6-mingw-release
cmake --build --preset qt6-mingw-release

# Executar o deploy automático com as dependências do Qt
cmake --build --preset linceu-qt6-mingw-release
```

Ou com MSVC (Visual Studio 2022):

```powershell
# Opção B: Compilar com MSVC
cmake --preset qt6-msvc-release
cmake --build --preset qt6-msvc-release

# Executar deploy com MSVC
cmake --build --preset linceu-qt6-msvc-release
```

### 3.3. Compilação Manual (sem Presets)
Caso o Qt ou Ninja estejam instalados em um diretório personalizado, aponte a variável `CMAKE_PREFIX_PATH`:

```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64/lib/cmake"
cmake --build build --config Release
```

---

## 4. Padrões de Código e Arquitetura

- **Padrão da Linguagem:** C++17 (`set(CMAKE_CXX_STANDARD 17)`). Utilize recursos modernos de C++ (RAII, smart pointers, structured binding, lambdas, gerenciamento seguro de recursos).
- **Integridade da Arquitetura e Privacidade Soberana:**
  - **100% Local-First & Offline:** É estritamente proibido adicionar chamadas de rede externas, telemetria, rastreamento ou dependências de serviços de nuvem.
  - **Determinismo sem IA Externa:** A classificação de eventos, pontuações de risco e estatísticas devem basear-se em lógica algorítmica pura e matemática determinística, sem dependência de modelos de linguagem (LLMs) ou APIs externas.
  - **Separação de Camadas:**
    - Núcleo de observação (`source/services/`) deve manter desacoplamento da interface gráfica, concentrando a captura assíncrona de `ReadDirectoryChangesW`, normalização de eventos e escrita em lote no SQLite via `DatabaseWriter`.
    - Módulos de segurança (`source/security/`) devem priorizar integridade criptográfica (CNG SHA-256) e rotinas de quarentena com validação prévia à restauração.
    - Motor de gráficos (`source/data/Charts/`) deve permanecer autônomo, utilizando a infraestrutura interna (`ChartLayoutEngine`, `RasterChartView`, `QuickChartItem`, `QPainter` e `QOpenGLWidget`).
- **Codificação de Arquivos:** UTF-8 sem BOM (exceto onde arquivos específicos do Windows ou MSVC exijam), com indentação consistente de 4 espaços.
- **Convenções de Nomenclatura:**
  - Classes e Tipos: `PascalCase` (ex: `InterfaceMonitor`, `DatabaseWriter`, `SecurityManager`).
  - Métodos e Funções: `camelCase` ou `snake_case` consistente com o módulo específico.
  - Variáveis e Membros: `camelCase` ou `snake_case` com clareza semântica.

---

## 5. Como Submeter um Pull Request

1. Faça um **Fork** do repositório e crie uma branch descritiva a partir de `main`:
   ```bash
   git checkout -b feature/minha-melhoria-linceu
   ```
2. Realize seus **Commits** com mensagens claras e semânticas:
   - `feat: adiciona novo gráfico de correlação temporal em Charts`
   - `fix: corrige detecção de renomeação em volumes compartilhados`
   - `docs: atualiza guia de compilação com MinGW 13`
   - `i18n: adiciona traduções completas para novos diálogos de quarentena`
3. Valide a compilação nos presets suportados e certifique-se de que não há regressões ou travamentos na interface.
4. Envie a branch para o seu fork e abra um **Pull Request** apontando para a branch `main`.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the instructions in English</summary><br>

# Contributing to Linceu Lighthouse

First off, thank you for considering contributing to **Linceu Lighthouse**! Your support helps make this local file activity intelligence, history, and audit platform more reliable, precise, and secure for everyone.

> [!NOTE]
> **About Project Development, Support, and External Contributions:**  
> This project is designed, developed, and maintained primarily as a personal and educational initiative by the author, who is in an ongoing learning journey in software engineering and modern C++.  
> Feedback, suggestions, *Issues*, and *Pull Requests (PRs)* are always warmly welcomed from the community! However, please be aware that reviewing technical inquiries, code submissions, and upstream merges is carried out strictly at the author's own pace on a voluntary basis. If a ticket or PR is not answered promptly or is not merged into the mainline branch, please do not react with alarm or frustration. Under the terms of the MIT License, you have complete freedom to create an independent *Fork* of this repository to evolve your own ideas, builds, and custom releases.

---

## 1. Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md). Please treat all contributors with respect, courtesy, and technical professionalism.

---

## 2. Ways to Contribute

You can contribute to Linceu Lighthouse in many ways:
- **Reporting Bugs:** Submit clear, reproducible issue reports with reproduction steps and logs when applicable.
- **Requesting Features:** Propose ideas for new statistical visualizations, file risk detection heuristics, or audit workflow enhancements.
- **Improving Documentation:** Clarify build instructions, enhance the user operations manual, or document real-world digital forensics and audit use cases.
- **Submitting Pull Requests:** Implement bug fixes, multithreaded buffering improvements, event correlation logic (`Move`/`Rename`), SQLite batch writer optimizations, or new chart generators.
- **Translations (i18n):** Linceu Lighthouse natively supports 6 languages (Portuguese, English, Spanish, French, Italian, and German). Help review existing translation catalogs under `source/locale/` or add new locales (such as Japanese or Chinese).

---

## 3. Development Environment Setup

### 3.1. Prerequisites
- **Operating System:** Windows 10 / 11 x64 or Windows Server 2022+
- **Compiler:**
  - Microsoft Visual C++ (MSVC) from Visual Studio 2022 (v143 toolset or higher) or Build Tools 2022
  - Or MinGW GCC 13+ (x86_64-w64-mingw32)
- **CMake:** Version 3.24 or higher
- **Qt:** Qt 6.x (Core, Gui, Widgets, Sql, Xml, Concurrent, OpenGL, OpenGLWidgets, Quick, Qml)
- **Build Tool:** Ninja or MinGW Makefiles

### 3.2. Building with CMake Presets
We provide pre-configured presets in `CMakePresets.json`:

```powershell
# Clone the repository
git clone https://github.com/fernandoncidade/Linceu_Lighthouse_cpp.git
cd Linceu_Lighthouse_cpp

# Option A: Build with MinGW (Recommended for fast Debug/Release cycles)
cmake --preset qt6-mingw-release
cmake --build --preset qt6-mingw-release

# Deploy with Qt runtime dependencies
cmake --build --preset linceu-qt6-mingw-release
```

Or with MSVC (Visual Studio 2022):

```powershell
# Option B: Build with MSVC
cmake --preset qt6-msvc-release
cmake --build --preset qt6-msvc-release

# Deploy with MSVC
cmake --build --preset linceu-qt6-msvc-release
```

### 3.3. Building without Presets (Manual Configuration)
If Qt or Ninja is installed in a custom location, point `CMAKE_PREFIX_PATH` to your Qt installation:

```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64/lib/cmake"
cmake --build build --config Release
```

---

## 4. Coding Standards & Architecture

- **Language Standard:** C++17 (`set(CMAKE_CXX_STANDARD 17)`). Use modern C++ practices (RAII, smart pointers, structured binding, lambdas, safe resource management).
- **Architecture Integrity & Sovereign Privacy:**
  - **100% Local-First & Offline:** It is strictly prohibited to add external network calls, telemetry, tracking, or cloud dependencies.
  - **Deterministic Intelligence without External AI:** Event classification, risk scores, and analytics must rely on pure algorithmic logic and deterministic mathematics, with no external LLMs or cloud AI APIs.
  - **Layer Decoupling:**
    - Observation services (`source/services/`) must remain decoupled from UI widgets, handling asynchronous `ReadDirectoryChangesW` capture, semantic normalization, and SQLite batch writes via `DatabaseWriter`.
    - Security modules (`source/security/`) must prioritize cryptographic integrity (CNG SHA-256) and quarantine routines with strict validation prior to restoration.
    - Chart engine (`source/data/Charts/`) must remain self-contained, using the internal rendering pipeline (`ChartLayoutEngine`, `RasterChartView`, `QuickChartItem`, `QPainter`, and `QOpenGLWidget`).
- **Formatting:** UTF-8 file encoding without BOM (except where Windows or MSVC tools specifically mandate it), consistent 4-space indentation.
- **Naming Conventions:**
  - Classes and Types: `PascalCase` (e.g., `InterfaceMonitor`, `DatabaseWriter`, `SecurityManager`).
  - Methods and Functions: `camelCase` or `snake_case` consistent with the given subsystem.
  - Variables: `camelCase` or `snake_case` with clear descriptive names.

---

## 5. Submitting a Pull Request

1. **Fork** the repository and create your branch from `main`:
   ```bash
   git checkout -b feature/my-cool-linceu-feature
   ```
2. **Commit** your changes with meaningful semantic messages:
   - `feat: add new temporal correlation chart generator`
   - `fix: resolve rename detection on network shared drives`
   - `docs: update MinGW 13 build instructions`
   - `i18n: complete translations for new quarantine dialogs`
3. Verify that the build succeeds and no regressions or UI freezes are introduced.
4. **Push** to your fork and submit a Pull Request targeting the `main` branch.

</details>
