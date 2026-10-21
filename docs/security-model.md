# Modelo de Segurança / Security Model & Threat Analysis — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher o modelo de segurança em português</summary><br>

# Linceu Lighthouse — Modelo de Segurança e Análise de Ameaças

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Nível de Segurança:** 100% Local-First | Zero Telemetria | Auditoria Determinística de Integridade (Sem IA Externa)  
**Contexto Operacional:** Windows 10 / 11 / Windows Server 2022+  

---

## 1. Modelo de Ameaças e Garantias de Design

O **Linceu Lighthouse** foi concebido para atender às mais rigorosas políticas de segurança corporativa, governança de TI e perícia forense digital, operando com total isolamento do ambiente externo.

### 1.1. Garantias Centrais de Segurança
1. **Privacidade Soberana e Zero Comunicação Externa:**
   - A aplicação não efetua conexões de rede de entrada ou saída (*inbound* ou *outbound*).
   - Não há código de telemetria, rastreamento analítico, *beacons* publicitários ou envio automatizado de relatórios de falhas.
   - Todo o processamento, persistência SQLite e geração de gráficos ocorrem exclusivamente na máquina local (`%LOCALAPPDATA%\LinceuLighthouse`).
2. **Determinismo sem IA Externa ou Modelos de Linguagem:**
   - O sistema não utiliza LLMs (Large Language Models), RAG (Retrieval-Augmented Generation) ou APIs de IA na nuvem.
   - Todas as decisões de classificação de eventos, cálculo de risco e alertas derivam de lógica algorítmica pura e matemática determinística, eliminando riscos de injeção de prompt, vazamento de contexto ou alucinações.
3. **Quarentena Não-Destrutiva e Restauração Criptograficamente Segura:**
   - Ao enviar um arquivo suspeito para quarentena, seus bytes são isolados e uma assinatura SHA-256 via CNG (`crypt32`) é gerada e registrada no banco de dados.
   - Qualquer rotina de recuperação exige confirmação de destino (pasta original ou diretório de teste isolado) e recalcula obrigatoriamente o hash SHA-256 antes da gravação no disco, prevenindo substituições acidentais ou restauração de dados corrompidos.
4. **Inspeção Segura do Sistema de Arquivos:**
   - A observação de eventos utiliza notificações assíncronas nativas do Windows (`ReadDirectoryChangesW`).
   - Handles de arquivos para leitura de metadados são abertos exclusivamente em modo somente leitura (`GENERIC_READ`, `FILE_SHARE_READ | FILE_SHARE_WRITE`), sem locks exclusivos que impeçam a operação normal do sistema operacional.

---

## 2. Fluxo de Captura, Correlação e Detecção de Ameaças

```
           [Evento do Sistema de Arquivos no Windows]
                               |
                               v
       [API Assíncrona ReadDirectoryChangesW (Buffer Multithread)]
                               |
                               v
         [Fila Thread-Safe de Normalização e Correlação]
                               |
                               v
            [Desambiguação Semântica: Move vs. Rename]
            (Elimina pares falsos de criação e exclusão)
                               |
                               v
         [Enriquecimento com Metadados & Magic Numbers]
           - Tipo MIME real inspecionado no cabeçalho
           - Timestamps completos (arquivos e diretórios)
           - Hash SHA-256 (Baseline e Quarentena)
                               |
                               v
            [Motor Heurístico de Regras e Risco (0-100)]
                               |
            +------------------+------------------+
            |                                     |
     (Dentro do Padrão)                    (Risco Identificado)
            |                                     |
     [Log Histórico SQLite]               [Alerta em Tempo Real]
                                          [Isolamento / Quarentena]
```

---

## 3. Matriz de Detecção Heurística de Riscos

O Linceu Lighthouse monitora e cataloga anomalias comportamentais no sistema de arquivos, mapeadas contra a taxonomia **MITRE ATT&CK**:

| Identificador do Risco | Heurística de Detecção | Severidade | Referência MITRE ATT&CK |
|---|---|:---:|:---:|
| **Modificações em Massa / Troca Rápida de Extensões** | Detecção de múltiplas alterações por segundo com alteração de extensão (comportamento típico de Ransomware). | **Crítico** | T1486 (Data Encrypted for Impact) |
| **Exclusão em Massa de Arquivos** | Exclusão de grande volume de documentos ou pastas estruturadas em curto intervalo. | **Alto** / **Crítico** | T1485 (Data Destruction), T1070 (Indicator Removal) |
| **Criação de Binários em Pastas Temporárias** | Surgimento de executáveis (`.exe`, `.dll`, `.sys`, `.ps1`) em `Temp`, `AppData` ou pastas de usuários comuns. | **Alto** | T1036 (Masquerading), T1204 (User Execution) |
| **Desvio de Linha de Base (Baseline Drift)** | Alteração de arquivos protegidos cujo hash difere do inventário inicial registrado. | **Crítico** | T1565 (Data Manipulation), T1574 (Hijack Execution Flow) |
| **Extensões Duplas Camufladas** | Nomes de arquivos contendo artifícios de camuflagem (ex: `relatorio.pdf.exe`). | **Alto** | T1036.007 (Double File Extension) |
| **Movimentação Estrutural Oculta** | Transferência de diretórios inteiros para caminhos ocultos ou não convencionais. | **Médio** / **Alto** | T1564 (Hide Artifacts) |
| **Inconsistência de Tipo MIME vs. Extensão** | Magic Number binário divergente da extensão nominal do arquivo. | **Médio** | T1036.008 (Masquerading File Type) |

---

## 4. Integridade Criptográfica e Cadeia de Custódia Forense

Para conferir suporte probatório incontestável a laudos e relatórios de auditoria:
- Cada linha de base (baseline) calcula hashes criptográficos **SHA-256** utilizando o subsistema nativo Windows Cryptography API: Next Generation (CNG).
- Os pacotes de exportação de evidências (.xlsx, .json, .csv, .db) preservam o estado fático dos metadados, caminhos de origem e destino e carimbos de tempo sem possibilidade de adulteração externa.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the security model in English</summary><br>

# Linceu Lighthouse — Security Model & Threat Analysis

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Security Level:** 100% Local-First | Zero Telemetry | Deterministic Integrity Auditing (No External AI)  
**Operating Context:** Windows 10 / 11 / Windows Server 2022+  

---

## 1. Threat Model & Architectural Guarantees

**Linceu Lighthouse** is engineered to satisfy the strictest corporate security policies, defense compliance standards, and digital forensic requirements, operating in complete isolation from external networks.

### 1.1. Core Security Guarantees
1. **Sovereign Privacy & Zero External Telemetry:**
   - The application makes no outbound or inbound network connections.
   - Zero telemetry, usage beacons, analytics beacons, or automated crash uploaders exist in the codebase.
   - All processing, SQLite storage, and statistical visualization are strictly local (`%LOCALAPPDATA%\LinceuLighthouse`).
2. **Deterministic Intelligence without External AI Models:**
   - No Large Language Models (LLMs), RAG pipelines, or cloud AI services are utilized.
   - All event classification, risk scoring, and alert generation derive from pure algorithmic heuristics and deterministic mathematics, eliminating prompt injection risks, context exfiltration, or hallucinations.
3. **Non-Destructive Quarantine & Cryptographically Validated Recovery:**
   - When isolating suspicious files into quarantine, their binary contents are secured and a cryptographic SHA-256 hash is generated via Windows CNG (`crypt32`).
   - Every restoration action prompts for destination confirmation (original folder or isolated test location) and strictly recalculates the file's SHA-256 hash prior to writing to disk, preventing accidental overwrites or restoring tampered artifacts.
4. **Safe Read-Only Filesystem Observation:**
   - Filesystem activity is observed via native Windows asynchronous notifications (`ReadDirectoryChangesW`).
   - Metadata inspection handles are opened in shared read-only modes (`GENERIC_READ`, `FILE_SHARE_READ | FILE_SHARE_WRITE`), preventing file locks that could disrupt ongoing host operations.

---

## 2. Event Capture, Semantic Correlation & Threat Pipeline

```
              [Windows Filesystem Event Notification]
                                |
                                v
      [Asynchronous ReadDirectoryChangesW (Multithreaded Buffer)]
                                |
                                v
          [Thread-Safe Normalization & Correlation Queue]
                                |
                                v
             [Semantic Move vs. Rename Disambiguation]
            (Suppresses false-positive add/delete pairs)
                                |
                                v
          [Metadata Enrichment & Magic Number Inspection]
            - Real MIME identification via header magic numbers
            - Full timestamp hierarchies (files and parent/child folders)
            - SHA-256 fingerprinting (Baseline & Quarantine)
                                |
                                v
              [Deterministic Rule & Risk Engine (0-100)]
                                |
             +------------------+------------------+
             |                                     |
     (Normal Operation)                    (Threat Detected)
             |                                     |
     [SQLite Event Log]                   [Real-Time Alert Center]
                                          [Quarantine Isolation]
```

---

## 3. Heuristic Threat Detection Matrix

Linceu Lighthouse monitors and classifies filesystem behavioral anomalies mapped directly to the **MITRE ATT&CK** matrix:

| Threat Identifier | Detection Heuristic | Severity | MITRE ATT&CK Reference |
|---|---|:---:|:---:|
| **Mass Modification / Rapid Extension Swapping** | High frequency of file modifications accompanied by extension renames (characteristic ransomware behavior). | **Critical** | T1486 (Data Encrypted for Impact) |
| **Mass File Deletions** | Rapid unapproved deletion of large volumes of documents or folder trees. | **High** / **Critical** | T1485 (Data Destruction), T1070 (Indicator Removal) |
| **Executable Creation in Temp / User Dirs** | New executables (`.exe`, `.dll`, `.sys`, `.ps1`) appearing in `Temp`, `AppData`, or non-admin user directories. | **High** | T1036 (Masquerading), T1204 (User Execution) |
| **Baseline Drift (Modified Binaries)** | Protected file modification where current SHA-256 diverges from recorded baseline snapshot. | **Critical** | T1565 (Data Manipulation), T1574 (Hijack Execution Flow) |
| **Camouflaged Double Extensions** | Executable files disguised with deceptive naming structures (e.g., `invoice.pdf.exe`). | **High** | T1036.007 (Double File Extension) |
| **Hidden Structural Movement** | Relocation of entire corporate directory structures into hidden or unusual locations. | **Medium** / **High** | T1564 (Hide Artifacts) |
| **MIME / Magic Number Mismatch** | Discrepancy between file header binary signature and nominal extension. | **Medium** | T1036.008 (Masquerading File Type) |

---

## 4. Cryptographic Integrity & Forensic Chain of Custody

To guarantee technical and legal defensibility in audits and forensic investigations:
- Baselines and quarantine records leverage Windows Cryptography API: Next Generation (CNG) to generate tamper-evident **SHA-256** checksums.
- Exported audit packages (.xlsx, .json, .csv, .db) capture complete operational genealogies, origin/destination paths, and exact filesystem timestamps, serving as a verifiable digital chain of custody.

</details>
