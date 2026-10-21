# Referência da Linha de Comando / CLI Reference — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher a referência do CLI em português</summary><br>

# Linceu Lighthouse — Referência da Linha de Comando (CLI) & Console

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Executáveis:** `Linceu_Lighthouse.exe` (Interface gráfica com suporte a parâmetros) | `linceu-lighthouse-cli.exe` (Utilitário de console headless)  
**Plataforma Alvo:** Windows 10 / Windows 11 / Windows Server 2022+ (x64)  

---

## 1. Visão Geral

O **Linceu Lighthouse** oferece dois modelos de operação complementares:
1. **Aplicação Gráfica Desktop (`Linceu_Lighthouse.exe`)**: Interface rica em Qt6 com 17 gráficos estatísticos, árvore de navegação e controles interativos, suportando inicialização rápida via parâmetros de linha de comando.
2. **Utilitário de Console Headless (`linceu-lighthouse-cli.exe`)**: Executável de console leve, compilado especificamente para automação via PowerShell, tarefas agendadas do Windows (Task Scheduler), pipelines de CI/CD e administração remota de servidores sem ambiente gráfico ativo.

Ambos os executáveis compartilham a mesma base criptográfica, motor de regras de segurança e o repositório local SQLite em `%LOCALAPPDATA%/LinceuLighthouse/monitoramento.db`.

---

## 2. Aplicação Gráfica Desktop (`Linceu_Lighthouse.exe`)

Permite abrir a interface apontando diretamente para um diretório alvo ou executar rotinas automatizadas:

```cmd
Linceu_Lighthouse.exe [caminho_diretório] [opções]
```

### 2.1. Parâmetros Suportados

| Parâmetro | Descrição | Exemplo de Uso |
|---|---|---|
| `[caminho]` | Abre o aplicativo e inicia imediatamente a varredura e o monitoramento no diretório informado. | `Linceu_Lighthouse.exe "D:\Projetos\Sistema"` |
| `--dir <caminho>` / `-d` | Parâmetro explícito para definir a pasta alvo. | `Linceu_Lighthouse.exe --dir "C:\inetpub\wwwroot"` |
| `--profile <nome>` | Carrega um perfil multi-diretório salvo previamente no banco SQLite. | `Linceu_Lighthouse.exe --profile "Servidores-Producao"` |
| `--scan-only` | Executa a varredura inicial (baseline), cataloga os metadados e encerra sem manter a observação contínua. | `Linceu_Lighthouse.exe "D:\Arquivos" --scan-only` |
| `--export <formato> <arquivo>` | Exporta o relatório técnico após o término da varredura (`xlsx`, `csv`, `json`, `xml`, `db`). | `Linceu_Lighthouse.exe --dir "D:\Dados" --export xlsx "C:\Laudos\auditoria.xlsx"` |
| `--help` / `-h` | Exibe a sintaxe da linha de comando no console. | `Linceu_Lighthouse.exe --help` |
| `--version` / `-v` | Exibe a versão oficial compilada do software. | `Linceu_Lighthouse.exe --version` |

### 2.2. Variáveis de Ambiente Gráficas

#### `LINCEU_OPENGL_MODE`
Define o modo de aceleração e renderização utilizado pelos gráficos e pela interface:
- `auto` *(Padrão)*: Seleciona automaticamente o melhor perfil de aceleração por hardware detectado.
- `desktop`: Força o uso dos drivers OpenGL nativos da GPU (NVIDIA / AMD Radeon).
- `angle`: Utiliza a camada Direct3D 11 via ANGLE (GPUs integradas Intel e ambientes corporativos).
- `software`: Habilita a rasterização 100% via CPU por software (ideal para VMs Hyper-V/VMware e sessões RDP).

---

## 3. Utilitário Standalone de Console (`linceu-lighthouse-cli.exe`)

O utilitário `linceu-lighthouse-cli.exe` executa diretamente no prompt do Windows sem instanciar qualquer biblioteca de interface gráfica (`QCoreApplication`), oferecendo saídas em texto colorido/formatado, JSON estruturado ou CSV tabular.

### 3.1. Sintaxe Geral

```cmd
linceu-lighthouse-cli.exe <comando> [opções]
```

### 3.2. Comandos Disponíveis

| Comando | Descrição |
|---|---|
| `scan <caminho>` | Realiza varredura de integridade em arquivos e diretórios, gerando hashes SHA-256 e avaliando riscos de segurança. |
| `monitor <caminho>` | Inicia observação em tempo real do sistema de arquivos via Win32 `ReadDirectoryChangesW` com emissão imediata de eventos e alertas. |
| `metadata <arquivo>` | Extrai metadados técnicos avançados de arquivos PE/Executáveis (validação Authenticode, arquitetura, carimbos temporais, seções). |
| `baseline <caminho>` | Compara o estado atual dos arquivos contra a baseline criptográfica armazenada no SQLite, detectando desvios (*drift*). |
| `alerts` | Consulta e lista alertas de segurança e violações de regras registradas no banco de auditoria. |
| `quarantine <ação>` | Gerencia o cofre criptográfico de quarentena (`list` ou `restore <id> <destino>`). |
| `sessions` | Lista o histórico de sessões de monitoramento e auditoria persistidas. |

### 3.3. Opções Globais

| Opção | Descrição | Padrão |
|---|---|---|
| `--format <text\|json\|csv>` | Define o formato de saída dos dados no console ou pipe. | `text` |
| `--depth <n>` | Limita a profundidade de recursão da varredura (`0` = apenas a pasta raiz informada, `-1` = ilimitada). | `-1` |
| `--risks-only` | Filtra a saída exibindo apenas itens com pontuação de risco relevante (Score ≥ 45 ou níveis Médio/Alto/Crítico). | Desativado |
| `--duration <segundos>` | Define o tempo máximo de execução do monitoramento em segundos (`0` = contínuo até Ctrl+C). | `0` |
| `--update` | Atualiza ou insere novos registros na baseline criptográfica durante o comando `baseline`. | Desativado |
| `--no-banner` | Oprime o cabeçalho textual informativo (útil para automações e captura de pipes limpos). | Desativado |
| `--version` / `-v` | Exibe a versão oficial compilada do utilitário CLI. | — |
| `--help` / `-h` | Exibe o manual de instruções integrado no terminal. | — |

---

## 4. Exemplos Práticos de Automação

### 4.1. Varredura com Exportação JSON para Pipeline DevSecOps
```powershell
linceu-lighthouse-cli.exe scan "C:\inetpub\wwwroot" --format json > auditoria_web.json
```

### 4.2. Auditoria Rápida Apenas de Arquivos Suspeitos (Riscos)
```powershell
linceu-lighthouse-cli.exe scan "D:\Financeiro" --depth 2 --risks-only --format text
```

### 4.3. Monitoramento em Tempo Real com Duração Temporizada (ex: 60 segundos)
```powershell
linceu-lighthouse-cli.exe monitor "C:\Windows\Temp" --duration 60 --format text
```

### 4.4. Extração de Metadados e Assinatura Digital de Executável
```powershell
linceu-lighthouse-cli.exe metadata "C:\Program Files\App\bin\service.exe" --format json
```

### 4.5. Auditoria de Integridade e Desvio de Baseline Criptográfica
```powershell
# Verifica alterações não autorizadas contra a baseline salva
linceu-lighthouse-cli.exe baseline "C:\ScriptsOperacionais"

# Atualiza e salva a baseline após uma implantação aprovada
linceu-lighthouse-cli.exe baseline "C:\ScriptsOperacionais" --update
```

### 4.6. Gestão do Cofre de Quarentena
```powershell
# Listar artefatos em quarentena
linceu-lighthouse-cli.exe quarantine list --format text

# Restaurar arquivo da quarentena após análise forense
linceu-lighthouse-cli.exe quarantine restore 5 "C:\Restaurados\arquivo_analisado.dll"
```

---

## 5. Códigos de Retorno (Exit Codes)

O `linceu-lighthouse-cli.exe` emite códigos de saída padronizados para garantir integração segura em scripts PowerShell, Bash e esteiras CI/CD:

| Código | Significado |
|---|---|
| `0` | Operação concluída com sucesso; nenhum risco Alto/Crítico ou desvio grave detectado. |
| `1` | Risco de segurança Alto ou Crítico identificado, ou desvio grave de baseline (*drift*) detectado. |
| `2` | Diretório ou arquivo alvo inexistente, ou permissão de acesso negada pelo sistema de arquivos. |
| `3` | Sintaxe de comando inválida, argumentos obrigatórios ausentes ou parâmetros desconhecidos. |

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the CLI reference in English</summary><br>

# Linceu Lighthouse — Command-Line Interface (CLI) & Console Reference

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Executables:** `Linceu_Lighthouse.exe` (GUI with CLI parameters) | `linceu-lighthouse-cli.exe` (Headless Console CLI)  
**Target Platform:** Windows 10 / Windows 11 / Windows Server 2022+ (x64)  

---

## 1. Overview

**Linceu Lighthouse** provides two distinct and complementary operational models:
1. **Desktop Graphical Application (`Linceu_Lighthouse.exe`)**: A rich Qt6 visual application featuring 17 statistical charts, interactive tree views, and live controls, capable of starting directly from command-line arguments.
2. **Headless Console Utility (`linceu-lighthouse-cli.exe`)**: A lightweight standalone console executable compiled specifically for automated PowerShell scripts, Windows Task Scheduler jobs, CI/CD DevSecOps pipelines, and remote headless server administration.

Both executables share the exact same cryptographic hashing engine, heuristic security rules, and local SQLite audit database located at `%LOCALAPPDATA%/LinceuLighthouse/monitoramento.db`.

---

## 2. Desktop Graphical Application (`Linceu_Lighthouse.exe`)

Allows launching the user interface directly targeting a directory or invoking automated inventory scans:

```cmd
Linceu_Lighthouse.exe [directory_path] [options]
```

### 2.1. Supported Arguments

| Argument | Description | Example |
|---|---|---|
| `[path]` | Immediately launches the GUI and begins baseline scanning and real-time monitoring on the specified target path. | `Linceu_Lighthouse.exe "D:\Projects\System"` |
| `--dir <path>` / `-d` | Explicit flag to define the target monitoring directory. | `Linceu_Lighthouse.exe --dir "C:\inetpub\wwwroot"` |
| `--profile <name>` | Loads a saved multi-directory observation profile from the local SQLite storage. | `Linceu_Lighthouse.exe --profile "Production-Servers"` |
| `--scan-only` | Performs full baseline inventory scan, extracts metadata, and exits without keeping continuous real-time observation. | `Linceu_Lighthouse.exe "D:\Files" --scan-only` |
| `--export <format> <file>` | Exports technical audit report upon scan completion (`xlsx`, `csv`, `json`, `xml`, `db`). | `Linceu_Lighthouse.exe --dir "D:\Data" --export xlsx "C:\Reports\audit.xlsx"` |
| `--help` / `-h` | Prints available command-line arguments to stdout. | `Linceu_Lighthouse.exe --help` |
| `--version` / `-v` | Prints official software version string. | `Linceu_Lighthouse.exe --version` |

### 2.2. Graphics Environment Variables

#### `LINCEU_OPENGL_MODE`
Controls the OpenGL backend configuration used by the statistical chart engines and Qt6:
- `auto` *(Default)*: Automatically negotiates the best available hardware acceleration mode supported by the host GPU drivers.
- `desktop`: Forces native desktop OpenGL context creation (recommended for dedicated NVIDIA and AMD GPUs).
- `angle`: Leverages Microsoft Direct3D 11 acceleration via ANGLE (ideal for integrated Intel graphics and locked-down corporate laptops).
- `software`: Forces 100% CPU software rasterization (indispensable for VMs Hyper-V/VMware and Remote Desktop RDP sessions).

---

## 3. Standalone Console Utility (`linceu-lighthouse-cli.exe`)

The `linceu-lighthouse-cli.exe` utility runs purely in the console without initializing GUI components (`QCoreApplication`), delivering structured human-readable text, clean JSON, or tabular CSV streams.

### 3.1. General Syntax

```cmd
linceu-lighthouse-cli.exe <command> [options]
```

### 3.2. Available Commands

| Command | Description |
|---|---|
| `scan <path>` | Scans files and directories, computes SHA-256 hashes, and evaluates heuristic security risk scores. |
| `monitor <path>` | Starts real-time filesystem observation via Win32 `ReadDirectoryChangesW` with instant event logging and alert triggers. |
| `metadata <file>` | Extracts deep technical metadata from PE/executable files (Authenticode digital signature, architecture, timestamps, section headers). |
| `baseline <path>` | Compares current filesystem state against cryptographic baselines stored in SQLite, reporting detected drift. |
| `alerts` | Queries and lists recorded security alerts and rule violations from the audit database. |
| `quarantine <action>` | Manages the cryptographic quarantine vault (`list` or `restore <id> <destination>`). |
| `sessions` | Lists the historical record of filesystem observation and forensic audit sessions. |

### 3.3. Global Options

| Option | Description | Default |
|---|---|---|
| `--format <text\|json\|csv>` | Output data presentation format. | `text` |
| `--depth <n>` | Directory recursion depth limit (`0` = target root only, `-1` = unlimited recursive). | `-1` |
| `--risks-only` | Filters output to display only items with security warnings or elevated risk (Score ≥ 45 or Medium/High/Critical). | Disabled |
| `--duration <seconds>` | Maximum monitoring execution duration in seconds (`0` = continuous observation until Ctrl+C). | `0` |
| `--update` | Updates or registers new baseline hash fingerprints during the `baseline` command. | Disabled |
| `--no-banner` | Suppresses header banners for clean scripting pipes. | Disabled |
| `--version` / `-v` | Displays compiled CLI version. | — |
| `--help` / `-h` | Displays built-in CLI help reference. | — |

---

## 4. Practical Automation Scenarios

### 4.1. Scan with JSON Output for DevSecOps Pipelines
```powershell
linceu-lighthouse-cli.exe scan "C:\inetpub\wwwroot" --format json > web_audit.json
```

### 4.2. Scan Only Suspicious Files (Risks Only)
```powershell
linceu-lighthouse-cli.exe scan "D:\Finance" --depth 2 --risks-only --format text
```

### 4.3. Real-Time Monitoring with Time Limit (e.g., 60 seconds)
```powershell
linceu-lighthouse-cli.exe monitor "C:\Windows\Temp" --duration 60 --format text
```

### 4.4. Extract PE File Metadata & Digital Signature Verification
```powershell
linceu-lighthouse-cli.exe metadata "C:\Program Files\App\bin\service.exe" --format json
```

### 4.5. Cryptographic Baseline & Drift Detection
```powershell
# Check for unauthorized changes against the recorded baseline
linceu-lighthouse-cli.exe baseline "C:\ProductionScripts"

# Update/register baseline following an authorized deployment
linceu-lighthouse-cli.exe baseline "C:\ProductionScripts" --update
```

### 4.6. Quarantine Vault Management
```powershell
# List items currently held in quarantine
linceu-lighthouse-cli.exe quarantine list --format text

# Safely restore an analyzed file from quarantine
linceu-lighthouse-cli.exe quarantine restore 5 "C:\Restored\verified_file.dll"
```

---

## 5. Standard Exit Codes

`linceu-lighthouse-cli.exe` returns standard process exit codes for dependable automation in PowerShell scripts, Bash, and CI/CD runners:

| Code | Meaning |
|---|---|
| `0` | Execution succeeded; no High/Critical risks or integrity drifts detected. |
| `1` | High or Critical security threat detected, or severe cryptographic baseline drift detected. |
| `2` | Target path does not exist, or filesystem access was denied by the operating system. |
| `3` | Invalid command syntax, missing required arguments, or unknown option. |

</details>
