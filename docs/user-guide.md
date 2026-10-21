# Guia do Usuário / Operations User Guide — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher o guia do usuário em português</summary><br>

# Linceu Lighthouse — Manual de Operação e Guia do Usuário

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Plataforma:** Windows 10 / Windows 11 / Windows Server 2022+ (x64)  
**Autor:** Fernando Nillsson Cidade  

---

## 1. Primeiros Passos

### 1.1. Modos de Execução e Inicialização
- **Execução Padrão:** Dê duplo clique em `Linceu_Lighthouse.exe` ou utilize o atalho criado na Área de Trabalho.
- **Linha de Comando:** Você pode iniciar a aplicação apontando diretamente para um diretório (ex: `Linceu_Lighthouse.exe "D:\MeusProjetos"`).
- **Compatibilidade Gráfica (OpenGL):** Caso execute em uma Máquina Virtual (VM) ou via Conexão de Área de Trabalho Remota (RDP) sem aceleração 3D por hardware, defina a variável de ambiente `$env:LINCEU_OPENGL_MODE = "software"` antes da execução.

### 1.2. Fluxo Rápido de Início (Passo a Passo)
1. Abra o Linceu Lighthouse.
2. Na barra lateral esquerda ou menu, clique em **Selecionar Diretório** e escolha a pasta ou unidade de disco que deseja monitorar e auditar.
3. Clique em **Iniciar/Parar** (ou pressione `F5`).
4. O sistema executará uma varredura inicial (estabelecendo a linha de base/snapshot de arquivos existentes) e entrará imediatamente em modo de **observação contínua em tempo real**.

---

## 2. Os 5 Modos de Operação

O Linceu Lighthouse estrutura todas as suas funcionalidades em 5 modos complementares de operação:

| Modo | Foco / Pergunta Central | O Que Ele Oferece |
|---|---|---|
| **Modo Monitoramento** | *O que está acontecendo agora?* | Visualização em tempo real das alterações em diretórios selecionados com alertas visuais imediatos codificados por cores (Criação, Modificação, Renomeação, Movimentação e Exclusão). |
| **Modo Histórico** | *O que aconteceu com meu arquivo?* | Consulta cronológica reversa detalhada (*File History* / *What Changed?*), permitindo reconstruir a genealogia e trajetória exata de qualquer arquivo ou pasta. |
| **Modo Inteligência & Analytics** | *Quais são os padrões operacionais?* | 17 visualizações gráficas nativas em C++/Qt (Linhas do Tempo, Mapas de Calor, Fluxos Sankey, Distribuição Pareto, Treemaps de Disco), 100% determinísticas e sem uso de IA externa. |
| **Modo Segurança & Integridade** | *Houve alguma alteração suspeita ou desvio?* | Linha de base criptográfica com hashes SHA-256, motor de regras heurísticas, pontuação de risco (0–100), quarentena e restauração assistida. |
| **Modo Evidência** | *Como comprovar as alterações formalmente?* | Geração e exportação de pacotes auditáveis imutáveis em múltiplos formatos corporativos (.xlsx, .csv, .json, .xml, .db) para conformidade e perícia forense. |

---

## 3. Operando a Tabela de Eventos

A tabela central é o ponto focal de interação do Linceu Lighthouse:

### 3.1. Campos e Colunas Principais
- **Objeto / Nome:** Nome do arquivo ou pasta com ícone representativo do tipo.
- **Evento / Operação:** Ação detectada com diferenciação visual cromática:
  - *Criado / Adicionado:* Arquivo novo detectado.
  - *Modificado:* Conteúdo, atributos ou metadados alterados.
  - *Renomeado:* O arquivo permaneceu na mesma pasta pai, alterando apenas seu nome.
  - *Movido:* O arquivo teve sua pasta pai alterada (com ou sem alteração de nome).
  - *Excluído:* Arquivo ou diretório removido do disco.
- **Tipo MIME Real:** Identificação real do conteúdo obtida por inspeção de *Magic Numbers* binários no cabeçalho do arquivo, traduzida dinamicamente no idioma ativo, preservando o identificador original entre parênteses para fins de auditoria.
- **Datas Completas (Arquivos e Pastas):** Exibe as datas de Criação, Modificação e Último Acesso, preenchidas inclusive para pastas, subpastas e diretórios filhos.
- **Tamanho:** Tamanho físico do objeto com conversão automática de unidades (B, KB, MB, GB).
- **Caminho Anterior vs. Caminho Atual:** Genealogia explícita em operações de movimentação ou renomeação.

### 3.2. Personalização e Filtros
- **Ajuste de Colunas:** Clique e arraste para redimensionar colunas ou reordená-las.
- **Salvar Estado da Tabela:** Utilize o menu para salvar o layout atual de colunas (ordem, visibilidade e largura) como perfil reutilizável.
- **Filtros Rápidos e Avançados:** Filtre por tipo de evento, extensão, faixa de datas ou digite termos na caixa de busca rápida (`Ctrl+F`).

---

## 4. Segurança, Quarentena e Restauração Segura

### 4.1. Linha de Base Criptográfica (Baseline Snapshot)
1. No menu **Segurança**, selecione **Salvar Linha de Base Criptográfica**.
2. O sistema calcula a assinatura hash SHA-256 de todos os arquivos monitorados via Windows CNG.
3. Posteriormente, qualquer alteração estrutural ou substituição de binários é imediatamente apontada como desvio de conformidade.

### 4.2. Central de Alertas (`AlertCenterDialog`)
1. Acesse **Segurança -> Central de Alertas**.
2. Os eventos suspeitos são classificados em 4 níveis determinísticos: **Baixo**, **Médio**, **Alto** e **Crítico**.
3. Clique em qualquer alerta para visualizar as regras violadas, caminhos envolvidos e pontuação de risco.

### 4.3. Quarentena e Restauração Assistida (`RecoveryDialog`)
1. Ao identificar um arquivo malicioso ou indevido, selecione-o e clique em **Enviar para Quarentena**. O arquivo tem seus bytes isolados e o hash SHA-256 registrado no banco SQLite.
2. Para recuperar um item:
   - Abra a janela **Recuperação e Quarentena**. A listagem é atualizada diferencialmente sem tremulação (*flicker-free*), preservando sua linha selecionada e rolagem.
   - Selecione o item e clique em **Restaurar**.
   - Escolha se deseja restaurar no **caminho original** ou em uma **pasta de teste alternativa**.
   - O sistema valida rigorosamente o hash SHA-256 antes da gravação no disco, prevenindo substituições acidentais ou restauração de dados adulterados.

### 4.4. Perfis Multi-Diretório (`MultiDirectoryDialog`)
1. Acesse **Segurança -> Gerenciar Múltiplos Diretórios**.
2. Defina grupos de pastas locais ou unidades montadas para monitoramento concomitante sob um mesmo perfil nomeado.

---

## 5. Visualização Estatística e Exportação de Evidências

### 5.1. Painel de 17 Gráficos Nacionais
1. Clique no botão **Estatísticas** na barra de ferramentas.
2. Navegue pelos geradores nativos de visualização:
   - **Timeline de Atividades:** Volume de eventos ao longo do tempo.
   - **Mapa de Calor (Heatmap):** Concentração de alterações por dia da semana e hora do dia.
   - **Fluxo Sankey:** Relação visual de tipos de arquivos por pasta monitorada.
   - **Distribuição Pareto:** Tipos de arquivos responsáveis pela maior parte das alterações.
   - **Treemap de Ocupação:** Distribuição do espaço em disco ocupado por categoria.

### 5.2. Exportação de Relatórios Técnicos
1. Clique em **Exportar** (`Ctrl+E`).
2. Selecione o formato desejado:
   - **Excel (.xlsx):** Planilha estruturada com formatação executiva e abas analíticas.
   - **CSV (UTF-8 BOM):** Dados brutos tabulares prontos para Power BI e Excel sem falhas de acentuação.
   - **JSON / XML:** Formato universal para integração com plataformas SIEM (Splunk, Elastic, Sentinel).
   - **SQLite (.db):** Banco de dados relacional completo para custódia forense digital.

---

## 6. Atalhos de Teclado Úteis

| Atalho | Ação Executada |
|:---:|---|
| `F5` | Iniciar / Parar varredura e monitoramento contínuo |
| `Ctrl+O` | Abrir diálogo para selecionar diretório de monitoramento |
| `Ctrl+E` | Abrir diálogo de Exportação de Relatórios e Evidências |
| `Ctrl+F` | Focar instantaneamente no campo de busca rápida |
| `Ctrl+S` | Salvar estado e preferências da tabela de eventos |
| `Del` | Excluir arquivo selecionado (com confirmação de segurança) |
| `F1` | Abrir o Manual Completo do Usuário |

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the user guide in English</summary><br>

# Linceu Lighthouse — End-User Operations Guide

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Platform:** Windows 10 / Windows 11 / Windows Server 2022+ (x64)  
**Author:** Fernando Nillsson Cidade  

---

## 1. Getting Started

### 1.1. Launching and Startup Modes
- **Standard Execution:** Double-click `Linceu_Lighthouse.exe` or click the desktop shortcut.
- **Command-Line Startup:** Launch targeting a directory directly via terminal (e.g., `Linceu_Lighthouse.exe "D:\MyProjects"`).
- **Graphics Compatibility (OpenGL):** When operating inside virtual machines (VMware, VirtualBox, Hyper-V) or over Remote Desktop (RDP) lacking 3D hardware drivers, set `$env:LINCEU_OPENGL_MODE = "software"` prior to launch.

### 1.2. Quick-Start Workflow (Step-by-Step)
1. Launch Linceu Lighthouse.
2. In the left sidebar or menu, click **Select Directory** and choose the folder or storage volume you want to monitor and audit.
3. Click **Start/Stop** (or press `F5`).
4. The system executes an initial baseline scan (enumerating existing files and recording timestamps) and immediately transitions into **continuous real-time filesystem observation**.

---

## 2. The 5 Operational Modes

Linceu Lighthouse structures its functionality across 5 complementary operational modes:

| Mode | Core Focus / Question | Key Capabilities |
|---|---|---|
| **Monitoring Mode** | *What is happening right now?* | Real-time observation of selected folders with instant color-coded visual alerts for Creation, Modification, Renaming, Moving, and Deletion. |
| **History Mode** | *What happened to my file?* | In-depth chronological reverse investigation (*File History* / *What Changed?*), reconstructing the complete genealogy and trajectory of any file or directory. |
| **Intelligence & Analytics Mode** | *What are the operational patterns?* | 17 native C++/Qt hardware-accelerated charts (Timelines, Heatmaps, Sankey Flows, Pareto Distributions, Disk Treemaps), 100% deterministic without external AI. |
| **Security & Integrity Mode** | *Was there any suspicious tampering or drift?* | Cryptographic SHA-256 baselining, heuristic rule engine, cumulative risk scoring (0–100), quarantine, and assisted recovery. |
| **Evidence Mode** | *How can I formally prove these changes?* | Generation and export of tamper-evident compliance packages across multiple enterprise formats (.xlsx, .csv, .json, .xml, .db) for forensics and audits. |

---

## 3. Working with the Event Table

The central event table is the primary operational workspace:

### 3.1. Core Table Columns
- **Object / Name:** Filename or folder name with corresponding system type icon.
- **Event / Operation:** Distinct color-coded activity type:
  - *Created / Added:* Newly detected file or folder.
  - *Modified:* Content, attributes, or metadata changed.
  - *Renamed:* File remained in the same parent directory, only changing its name.
  - *Moved:* File had its parent directory altered (with or without name changes).
  - *Deleted:* File or directory removed from disk.
- **Real MIME Type:** Inspected via binary *Magic Numbers* in file headers, localized dynamically in the active UI language, with the raw MIME string preserved in parentheses for audit purposes.
- **Full Timestamps (Files and Folders):** Creation, Modification, and Last Access dates read directly from the filesystem for files, subfolders, and parent directories.
- **Size:** Human-readable physical byte size with automatic unit conversion (B, KB, MB, GB).
- **Previous Path vs. Current Path:** Explicit operational trajectory during rename or move events.

### 3.2. Layout Customization & Filters
- **Column Customization:** Drag headers to resize or reorder columns.
- **Save Table State:** Use the menu to persist your customized column arrangement, visibility, and widths as a reusable profile.
- **Multi-Dimensional Filters:** Filter by event operation, file extension, date range, or use the instant search bar (`Ctrl+F`).

---

## 4. Security, Quarantine & Cryptographically Validated Restoration

### 4.1. Cryptographic Baseline Snapshot
1. In the **Security** menu, select **Save Cryptographic Baseline**.
2. The engine computes SHA-256 hashes for all monitored items via Windows CNG.
3. Subsequent file tampering, unauthorized modifications, or binary replacements are immediately flagged as baseline drift.

### 4.2. Real-Time Alert Center (`AlertCenterDialog`)
1. Open **Security -> Alert Center**.
2. Flagged anomalies are categorized into 4 deterministic severity levels: **Low**, **Medium**, **High**, and **Critical**.
3. Select any alert to inspect triggered rules, involved file paths, and calculated risk scores.

### 4.3. Quarantine and Assisted Recovery (`RecoveryDialog`)
1. To isolate a suspicious file, right-click and select **Send to Quarantine**. The file contents are secured and its SHA-256 hash stored in the SQLite database.
2. To recover a quarantined file:
   - Open the **Recovery & Quarantine** window. The list synchronizes differentially without visual flicker, maintaining your selected row and scroll position.
   - Select the file and click **Restore**.
   - Choose whether to restore to the **original location** or an **isolated test directory**.
   - The engine strictly recalculates and verifies the SHA-256 hash before executing the disk write, preventing accidental overwrites or tampered restores.

### 4.4. Multi-Directory Profiles (`MultiDirectoryDialog`)
1. Navigate to **Security -> Manage Multiple Directories**.
2. Group distinct directory paths and storage volumes into unified, persistent observation profiles.

---

## 5. Statistical Visualizations & Evidence Export

### 5.1. 17 Native Chart Generators
1. Click the **Statistics** button in the main toolbar.
2. Explore native visualization generators:
   - **Activity Timeline:** Volume of events over time.
   - **Hourly Heatmap:** Modification density by day of week and hour of day.
   - **Sankey Diagram:** Flow of file types across monitored parent folders.
   - **Pareto Chart:** Identification of top file types responsible for bulk filesystem changes.
   - **Disk Space Treemap:** Hierarchical visualization of storage occupancy by category.

### 5.2. Exporting Evidence Packages
1. Click **Export** (`Ctrl+E`).
2. Choose your preferred output format:
   - **Excel Spreadsheet (.xlsx):** Formatted workbook with analytical sheets and summaries.
   - **CSV (UTF-8 BOM):** Structured tabular file optimized for Power BI and Excel without character encoding corruption.
   - **JSON / XML:** Standard structured format for SIEM ingestion (Splunk, Elastic, Sentinel).
   - **SQLite Database (.db):** Standalone relational database providing an immutable digital chain of custody.

---

## 6. Keyboard Shortcuts Reference

| Shortcut | Description |
|:---:|---|
| `F5` | Start / Stop baseline scan and continuous monitoring |
| `Ctrl+O` | Open directory selection dialog |
| `Ctrl+E` | Open Export Reports & Evidence dialog |
| `Ctrl+F` | Focus instant search field |
| `Ctrl+S` | Save current table layout preferences |
| `Del` | Delete selected file (with safety confirmation) |
| `F1` | Open Complete Operations User Guide |

</details>
