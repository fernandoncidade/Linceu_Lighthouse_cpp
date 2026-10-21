# Sugestões de Issues Iniciais / Starter Issues — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher as sugestões de issues em português</summary><br>

# Issues Recomendadas para Engajamento da Comunidade

Abaixo estão 7 templates de issues prontas para você publicar no GitHub em [Issues -> New Issue](../../issues/new), facilitando a colaboração de desenvolvedores, pesquisadores de segurança e administradores de sistemas Windows:

---

### Issue 1: Suporte a Compartilhamentos de Rede SMB / Caminhos UNC com Reconexão Automática
- **Título:** `[FEATURE] Support remote SMB / UNC network share paths with automatic reconnect`
- **Labels:** `enhancement`, `help wanted`, `networking`
- **Conteúdo:**
```markdown
### Resumo
Atualmente, o `ob_01_Observador` opera com alta performance em volumes locais do Windows através da API `ReadDirectoryChangesW`.

### Melhoria Proposta
Adicionar suporte aprimorado para caminhos de rede UNC (ex: `\\servidor\compartilhamento`), incluindo detecção de perda de conexão de rede e algoritmo de reconexão automática com retentativas exponenciais.

### Critérios de Aceite
- [ ] Monitoramento contínuo de caminhos UNC sem interrupção de threads.
- [ ] Reconexão automática em caso de oscilação momentânea da rede SMB.
- [ ] Notificação na interface quando o compartilhamento remoto estiver indisponível.
```

---

### Issue 2: Otimização de Memória e Lote SQLite para Fluxos com Mais de 5 Milhões de Eventos
- **Título:** `[PERF] Optimize memory and SQLite batching for event streams > 5,000,000 items`
- **Labels:** `performance`, `sqlite`, `c++17`
- **Conteúdo:**
```markdown
### Resumo
Em ambientes com altíssima rotatividade de compilação ou processos de renderização contínua, o volume de eventos acumulados pode atingir milhões de registros.

### Melhoria Proposta
- Ajustar os parâmetros de checkpoint WAL do SQLite3 em `gbank_20_db_writer.cpp`.
- Implementar política de paginação com descarte de nós da memória para eventos históricos não visualizados.

### Critérios de Aceite
- [ ] Consumo de memória estável abaixo de 150 MB mesmo com 5.000.000 de eventos registrados.
- [ ] Gravação assíncrona fluida sem bloqueio na interface gráfica.
```

---

### Issue 3: Regras Personalizadas com Expressões Regulares no Motor de Segurança
- **Título:** `[FEATURE] Support custom user-defined regex rules in Security Rules Engine`
- **Labels:** `enhancement`, `security`
- **Conteúdo:**
```markdown
### Resumo
O motor de segurança (`agre_01_motor_regras`) possui regras heurísticas predefinidas (extensões duplas, binários em pastas temporárias, etc.).

### Melhoria Proposta
Permitir que os usuários definam suas próprias regras customizadas através de expressões regulares (regex) na interface, detectando padrões específicos de arquivos corporativos, extensões confidenciais ou palavras-chave proibidas.

### Critérios de Aceite
- [ ] Diálogo de configuração de regras customizadas com validação de sintaxe regex.
- [ ] Classificação de severidade configurável pelo usuário.
- [ ] Disparo imediato de alertas na Central de Alertas quando houver correspondência.
```

---

### Issue 4: Adicionar Traduções para Japonês e Chinês Simplificado
- **Título:** `[I18N] Add Japanese (ja_JP) and Simplified Chinese (zh_CN) language translations`
- **Labels:** `i18n`, `good first issue`
- **Conteúdo:**
```markdown
### Resumo
O Linceu Lighthouse suporta atualmente 6 idiomas (Português, Inglês, Espanhol, Francês, Italiano e Alemão). A inclusão de Japonês e Chinês Simplificado ampliará o alcance internacional da aplicação.

### Etapas de Implementação
1. Adicionar os catálogos `linceu_ja_JP.ts` e `linceu_zh_CN.ts` em `source/locale/`.
2. Traduzir strings da interface, menus, diálogos de quarentena e relatórios.
3. Registrar os novos idiomas no enum do `Internacionalizador`.
```

---

### Issue 5: Automação de Releases no GitHub Actions (Instalador e ZIP Portátil)
- **Título:** `[CI/CD] Automate GitHub Releases artifact generation on version tags`
- **Labels:** `ci`, `devops`
- **Conteúdo:**
```markdown
### Resumo
Ao enviar uma tag de versão (ex: `v2026.10.21.0`), o GitHub Actions deve compilar o projeto, empacotar o instalador `Linceu_Lighthouse-x64-Setup.exe` (via Inno Setup) e o pacote `Linceu_Lighthouse-x64-Portable.zip`, calcular os hashes SHA-256 e criar a Release oficial no GitHub.

### Critérios de Aceite
- [ ] Disparo automatizado em tags `v*`.
- [ ] Compilação em runner Windows com CMake e Qt 6.
- [ ] Empacotamento via Inno Setup e geração de `checksums.txt`.
- [ ] Publicação dos artefatos na aba de Releases.
```

---

### Issue 6: Notificações Nativas do Windows Action Center para Ameaças Críticas
- **Título:** `[FEATURE] Native Windows Action Center toast notifications for critical threats`
- **Labels:** `ui`, `enhancement`
- **Conteúdo:**
```markdown
### Resumo
Atualmente, as notificações são exibidas através de caixas de diálogo e popups internos configuráveis (`NotificationPopupConfigDialog`).

### Melhoria Proposta
Adicionar suporte opcional a notificações do Windows Action Center (Toast Notifications) via API WinRT / Win32, permitindo que alertas de risco crítico sejam visualizados mesmo com o aplicativo minimizado na bandeja do sistema.

### Critérios de Aceite
- [ ] Disparo de notificação toast quando um evento Crítico for detectado.
- [ ] Opção de habilitar/desabilitar nas preferências do usuário.
- [ ] Clique na notificação abre diretamente a Central de Alertas.
```

---

### Issue 7: Modo Daemon Headless para Monitoramento em Windows Server
- **Título:** `[CLI] Headless daemon mode for background Windows Server operation`
- **Labels:** `cli`, `sysadmin`
- **Conteúdo:**
```markdown
### Resumo
Administradores de servidores corporativos frequentemente necessitam monitorar diretórios de arquivos sensíveis em servidores sem sessão gráfica de usuário interativa aberta.

### Melhoria Proposta
Adicionar flag `--headless` ao `Linceu_Lighthouse.exe`, permitindo que o executável rode como processo em segundo plano ou serviço do Windows, persistindo todos os eventos e alertas diretamente no banco SQLite.

### Critérios de Aceite
- [ ] `Linceu_Lighthouse.exe --dir "D:\Dados" --headless`
- [ ] Execução sem inicialização de janelas Qt ou widgets gráficos.
- [ ] Consumo de CPU inferior a 1% em repouso.
```

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the suggested issues in English</summary><br>

# Starter Issues for Linceu Lighthouse Community

Here are 7 ready-to-use GitHub Issue templates for community contributors. You can create these issues directly on your repository under [Issues -> New Issue](../../issues/new):

---

### Issue 1: Support Remote SMB / UNC Network Share Paths with Automatic Reconnect
- **Title:** `[FEATURE] Support remote SMB / UNC network share paths with automatic reconnect`
- **Labels:** `enhancement`, `help wanted`, `networking`
- **Body:**
```markdown
### Summary
Currently, `ob_01_Observador` delivers high-throughput monitoring on local Windows storage volumes using the `ReadDirectoryChangesW` Win32 API.

### Proposed Improvement
Enhance observation over UNC network share paths (e.g., `\\server\share`), adding connection-loss detection and automatic exponential backoff reconnection routines.

### Acceptance Criteria
- [ ] Seamless monitoring over UNC paths without worker thread stalls.
- [ ] Automatic reconnection when intermittent SMB network interruptions resolve.
- [ ] Visual indicator in the UI when the remote share becomes temporarily unreachable.
```

---

### Issue 2: Optimize Memory and SQLite Batching for Event Streams > 5,000,000 Items
- **Title:** `[PERF] Optimize memory and SQLite batching for event streams > 5,000,000 items`
- **Labels:** `performance`, `sqlite`, `c++17`
- **Body:**
```markdown
### Summary
In environments with high-frequency build cycles or active render pipelines, cumulative filesystem events can exceed millions of records.

### Proposed Improvement
- Fine-tune SQLite3 WAL checkpoint intervals in `gbank_20_db_writer.cpp`.
- Implement memory-pruning strategies for off-screen historical events.

### Acceptance Criteria
- [ ] Memory footprint remains stable below 150 MB with 5,000,000 recorded events.
- [ ] Zero UI freezes during asynchronous batch disk commits.
```

---

### Issue 3: Support Custom User-Defined Regex Rules in Security Rules Engine
- **Title:** `[FEATURE] Support custom user-defined regex rules in Security Rules Engine`
- **Labels:** `enhancement`, `security`
- **Body:**
```markdown
### Summary
The security subsystem (`agre_01_motor_regras`) currently features hardcoded heuristic rules (double extensions, executables in temp directories, etc.).

### Proposed Improvement
Allow users to define custom regular expression (regex) rules via the GUI to detect organization-specific sensitive file naming patterns, proprietary extensions, or restricted file drops.

### Acceptance Criteria
- [ ] Configuration dialog supporting custom regex pattern input and validation.
- [ ] Configurable severity level per rule.
- [ ] Immediate alert generation in the Alert Center upon regex match.
```

---

### Issue 4: Add Japanese (ja_JP) and Simplified Chinese (zh_CN) Language Translations
- **Title:** `[I18N] Add Japanese (ja_JP) and Simplified Chinese (zh_CN) language translations`
- **Labels:** `i18n`, `good first issue`
- **Body:**
```markdown
### Summary
Linceu Lighthouse natively supports 6 languages (Portuguese, English, Spanish, French, Italian, and German). Adding Japanese and Simplified Chinese will broaden adoption across Asian markets.

### Implementation Steps
1. Add `linceu_ja_JP.ts` and `linceu_zh_CN.ts` translation catalogs under `source/locale/`.
2. Translate UI strings, menus, quarantine dialogs, and reports.
3. Register the new locales in the `Internacionalizador` language enum.
```

---

### Issue 5: Automate GitHub Releases Artifact Generation on Version Tags
- **Title:** `[CI/CD] Automate GitHub Releases artifact generation on version tags`
- **Labels:** `ci`, `devops`
- **Body:**
```markdown
### Summary
Pushing a version tag (e.g., `v2026.10.21.0`) should trigger a GitHub Actions workflow to build the project, package the Inno Setup installer (`Linceu_Lighthouse-x64-Setup.exe`), package `Linceu_Lighthouse-x64-Portable.zip`, compute SHA-256 checksums, and publish an official GitHub Release.

### Acceptance Criteria
- [ ] Automatic workflow trigger on `v*` tags.
- [ ] Windows runner compilation with CMake and Qt 6.
- [ ] Automated Inno Setup installer and portable ZIP packaging with `checksums.txt`.
- [ ] Release draft/publication with artifacts attached.
```

---

### Issue 6: Native Windows Action Center Toast Notifications for Critical Threats
- **Title:** `[FEATURE] Native Windows Action Center toast notifications for critical threats`
- **Labels:** `ui`, `enhancement`
- **Body:**
```markdown
### Summary
Notifications are currently rendered via internal dialogs and configurable popup banners (`NotificationPopupConfigDialog`).

### Proposed Improvement
Add native Windows Action Center Toast Notification integration via WinRT / Win32 APIs, allowing users to receive alerts even when Linceu Lighthouse is minimized to the system tray.

### Acceptance Criteria
- [ ] Toast notification dispatched when a Critical severity threat is identified.
- [ ] Configurable toggle in user preferences.
- [ ] Clicking the toast opens the Alert Center directly.
```

---

### Issue 7: Headless Daemon Mode for Background Windows Server Operation
- **Title:** `[CLI] Headless daemon mode for background Windows Server operation`
- **Labels:** `cli`, `sysadmin`
- **Body:**
```markdown
### Summary
Windows Server administrators often need to monitor critical data directories in headless server deployments without an active desktop session.

### Proposed Improvement
Add a `--headless` flag to `Linceu_Lighthouse.exe`, enabling it to run as a silent background agent logging filesystem events directly to the SQLite database.

### Acceptance Criteria
- [ ] Command: `Linceu_Lighthouse.exe --dir "D:\Data" --headless`
- [ ] Execution without initializing GUI widgets or window handles.
- [ ] Idle CPU utilization below 1%.
```

</details>
