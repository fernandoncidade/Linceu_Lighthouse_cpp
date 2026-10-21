# Política de Segurança / Security Policy — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher a política em português</summary><br>

# Política de Segurança

## 1. Versões com Suporte Oficial

Atualizações e correções de segurança são fornecidas para os releases mais recentes:

| Versão | Suportada |
|:---:|:---:|
| `v2026.10.x` | :white_check_mark: |
| `< v2026.10` | :x: |

---

## 2. Reportando uma Vulnerabilidade

Levamos a segurança e a integridade do **Linceu Lighthouse** muito a sério.

Se você identificou uma potencial vulnerabilidade de segurança (como falha no cálculo de hashes criptográficos, vulnerabilidade na rotina de quarentena ou restauração, bypass de regras locais de segurança, estouro de buffer em caminhos longos ou corrupção na base SQLite):

1. **NÃO divulgue publicamente a vulnerabilidade através de issues ou discussões abertas.**
2. Submeta o reporte de forma privada através do recurso **GitHub Security Advisories** na aba [Security](../../security/advisories/new) do repositório ou contate o mantenedor diretamente.
3. Por favor, inclua em seu relato:
   - Descrição detalhada da vulnerabilidade encontrada;
   - Passo a passo para reprodução, comandos PowerShell ou amostras de eventos de arquivos;
   - Vetor de ataque e impacto potencial estimado;
   - Sugestões de correção ou mitigação, caso possua.

### Prazos e Processo de Resposta
- Confirmaremos o recebimento do seu reporte em até **48 horas**.
- Realizaremos uma avaliação técnica inicial e estimaremos o prazo de correção.
- Após a correção, um patch de segurança será publicado e os créditos devidamente atribuídos em coordenação com o pesquisador.

---

## 3. Compromissos Arquiteturais de Segurança

- **100% Autônomo, Local e Offline:** A aplicação jamais envia caminhos de arquivos, metadados, hashes ou históricos através da rede. Não há telemetria nem comunicação externa.
- **Inteligência Determinística sem IA Externa:** A classificação de eventos, auditoria de integridade e pontuação de risco utilizam algoritmos matemáticos determinísticos, eliminando riscos de vazamento de contexto ou dependência de serviços remotos.
- **Quarentena e Restauração Criptograficamente Segura:** Arquivos isolados em quarentena recebem hashes SHA-256 e, durante qualquer procedimento de recuperação, a integridade é rigorosamente recalculada e confrontada antes de qualquer movimentação no sistema de arquivos.
- **Integridade da Base de Dados:** O armazenamento SQLite local em `%LOCALAPPDATA%\LinceuLighthouse` opera com transações ACID e escrita em lote assíncrona, prevenindo perda de dados mesmo em desligamentos inesperados.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the policy in English</summary><br>

# Security Policy

## 1. Supported Versions

Security updates are provided for the latest releases:

| Version | Supported |
|:---:|:---:|
| `v2026.10.x` | :white_check_mark: |
| `< v2026.10` | :x: |

---

## 2. Reporting a Vulnerability

We take the security and integrity of **Linceu Lighthouse** very seriously.

If you believe you have discovered a security vulnerability in Linceu Lighthouse (such as an incorrect cryptographic hash verification, vulnerability in quarantine/restoration routines, local security rules bypass, buffer overflow with long paths, or SQLite database corruption):

1. **Do NOT report security vulnerabilities through public GitHub issues or discussions.**
2. Report the vulnerability privately via **GitHub Security Advisory** under the repository's [Security tab](../../security/advisories/new) or contact the maintainer directly.
3. Please include:
   - A detailed description of the vulnerability.
   - Exact steps to reproduce, sample files, or PowerShell / file event test cases.
   - The potential impact and attack vector.
   - Any suggested mitigations or patches.

### Response Time & Process
- We will acknowledge receipt of your vulnerability report within **48 hours**.
- We will provide an initial assessment and estimated remediation timeline.
- Once fixed, a security release will be tagged and public disclosure coordinated with the reporter.

---

## 3. Core Architectural Security Commitments

- **100% Local-First & Offline:** The application never transmits audited files, paths, metadata, hashes, or logs over the network. Zero telemetry, zero analytics, zero external sockets.
- **Deterministic Intelligence without External AI:** Event classification, integrity auditing, and risk scoring rely strictly on deterministic algorithms, eliminating prompt injection risks, data exfiltration, or cloud dependencies.
- **Cryptographically Safe Quarantine & Restoration:** Files quarantined are hashed using SHA-256, and all restoration procedures strictly verify file hashes before executing disk writes.
- **Database Integrity:** Embedded SQLite storage in `%LOCALAPPDATA%\LinceuLighthouse` employs robust ACID transactions and asynchronous batch writers to safeguard event history against unexpected power loss.

</details>
