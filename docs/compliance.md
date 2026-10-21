# Mapeamento de Conformidade / Compliance Mapping — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher o guia de conformidade em português</summary><br>

# Linceu Lighthouse — Mapeamento de Conformidade Regulatória

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Frameworks:** ISO/IEC 27001:2022 | NIST SP 800-53 | CIS Controls v8 | LGPD / GDPR | SOX  

---

## 1. Visão Geral de Conformidade

Normas internacionais de governança da informação, legislações de proteção de dados e frameworks de cibersegurança exigem das organizações o **Monitoramento Contínuo de Integridade de Arquivos (FIM - File Integrity Monitoring)**, a manutenção de trilhas de auditoria imutáveis e o rastreamento rigoroso de qualquer alteração, exclusão ou movimentação não autorizada de dados sensíveis.

O **Linceu Lighthouse** cumpre esse papel operacional como uma plataforma soberana **100% Local-First**, assegurando que os dados periciados e os registros de auditoria jamais deixem os limites da estação de trabalho ou do servidor local.

---

## 2. Mapeamento dos Principais Frameworks

### 2.1. ISO/IEC 27001:2022
- **Controle A.8.15 — Registro de Logs (Logging):**
  - Exige a criação e proteção de registros de eventos de usuários, exceções e eventos de segurança.
  - *Atendimento pelo Linceu Lighthouse:* Registra o ciclo de vida integral dos arquivos (criação, modificação, renomeação, movimentação e exclusão) com histórico cronológico indelével e mais de 30 categorias de metadados antes e depois de cada ação.
- **Controle A.8.16 — Atividades de Monitoramento:**
  - Determina o monitoramento de redes, sistemas e aplicações contra comportamentos anômalos.
  - *Atendimento pelo Linceu Lighthouse:* 17 gráficos estatísticos nativos (como Mapas de Calor, Horários de Pico e Fluxos Sankey) permitem diagnosticar desvios de volume e acessos anormais fora do horário comercial.
- **Controle A.8.28 — Proteção de Código-Fonte:**
  - Exige a rastreabilidade estrita de diretórios de código e binários em ambientes de desenvolvimento e produção.

### 2.2. NIST SP 800-53 (Revisão 5)
- **SI-7 — Integridade de Software, Firmware e Informação (Software Integrity):**
  - "A organização emprega ferramentas de monitoramento de integridade para detectar alterações não autorizadas em arquivos críticos de sistema e dados organizacionais."
  - *Atendimento pelo Linceu Lighthouse:* O motor de linha de base criptográfica calcula assinaturas CNG SHA-256 de diretórios inteiros, sinalizando qualquer adulteração, substituição de DLLs ou ataque de ransomware.
- **AU-2 & AU-3 — Conteúdo dos Registros de Auditoria:**
  - Registra a genealogia das operações (caminho anterior vs. caminho atual), tipos MIME reais inspecionados por Magic Numbers e carimbos de data/hora (criação, modificação e acesso) para pastas e arquivos filhos.
- **AU-9 — Proteção das Informações de Auditoria:**
  - A persistência local em SQLite3 com gravação assíncrona WAL garante que os logs de auditoria fiquem isolados contra interceptação de rede ou falhas em nuvem.

### 2.3. CIS Controls v8
- **Controle 8 — Gerenciamento de Logs de Auditoria:**
  - Coleta, análise e retenção de logs detalhados de atividades em estações e servidores.
- **Controle 10 — Recursos de Recuperação de Dados:**
  - O subsistema de quarentena e recuperação do Linceu Lighthouse permite isolar arquivos suspeitos e restabelecê-los com conferência criptográfica rigorosa antes de qualquer gravação no disco.

### 2.4. LGPD (Lei nº 13.709/2018) & GDPR (Regulamento UE 2016/679)
- **LGPD Art. 46 / GDPR Artigo 32 — Segurança do Tratamento:**
  - Exige medidas técnicas capazes de proteger dados pessoais contra destruição acidental ou ilícita, perda, alteração ou vazamento.
  - *Atendimento pelo Linceu Lighthouse:* Detecta movimentações suspeitas em lote de pastas corporativas contendo dados pessoais (RH, Folha de Pagamento, Prontuários, Bases de Clientes), prevenindo e evidenciando incidentes de vazamento interno (*insider threats*).
  - *Privacidade Soberana:* Por operar em modelo 100% Local-First e sem IA em nuvem, a utilização do Linceu Lighthouse nunca gera transferência internacional de dados ou exposição a terceiros.

---

## 3. Entregáveis Técnicos para Perícia e Auditoria

O Linceu Lighthouse gera relatórios técnicos e pacotes probatórios estruturados:
1. **Planilhas Estruturadas em Excel (.xlsx):** Relatórios completos com formatação executiva, metadados detalhados e totalização de eventos por tipo e diretório.
2. **Formato JSON / XML Estruturado:** Ideal para ingestão e correlação em plataformas SIEM corporativas (Splunk, Microsoft Sentinel, Elastic, Graylog).
3. **Planilhas CSV Padronizadas (UTF-8 BOM):** Prontas para abertura direta no Microsoft Excel, LibreOffice Calc e Power BI sem corrupção de acentuação.
4. **Base Relacional SQLite (.db):** Banco de dados independente que serve como evidência forense digital custodiada e imutável do período auditado.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the compliance guide in English</summary><br>

# Linceu Lighthouse — Regulatory Compliance Mapping

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Frameworks:** ISO/IEC 27001:2022 | NIST SP 800-53 | CIS Controls v8 | GDPR / LGPD | SOX  

---

## 1. Compliance Overview

International cybersecurity standards, privacy laws, and corporate governance frameworks mandate that organizations establish continuous **File Integrity Monitoring (FIM)**, maintain immutable audit trails, and actively detect unauthorized modifications, deletions, or exfiltration of sensitive organizational assets.

**Linceu Lighthouse** addresses these requirements through its **Sovereign Privacy (Local-First)** architecture, ensuring that all audited records, hashes, and forensics remain strictly localized on the host machine without external data transmission.

---

## 2. Framework Mappings

### 2.1. ISO/IEC 27001:2022
- **Control A.8.15 — Logging:**
  - Mandates recording of event logs covering system activities, exceptions, and security events.
  - *Linceu Lighthouse implementation:* Captures full file lifecycle trajectories (creation, modification, renaming, cross-folder movement, and deletion) enriched with 30+ attribute dimensions and timestamps before and after each event.
- **Control A.8.16 — Monitoring Activities:**
  - Requires monitoring systems for abnormal behavior or potential security incidents.
  - *Linceu Lighthouse implementation:* 17 native deterministic chart generators (such as Hourly Activity Heatmaps and Sankey Directory-to-Type Flows) highlight volume anomalies and off-hours modifications.
- **Control A.8.28 — Secure Coding & Source Code Protection:**
  - Requires continuous monitoring of code repository folders against unapproved changes or accidental deletions.

### 2.2. NIST SP 800-53 (Revision 5)
- **SI-7 — Software, Firmware, and Information Integrity:**
  - "The organization employs integrity verification tools to detect unauthorized changes to software, firmware, and organizational information."
  - *Linceu Lighthouse implementation:* Cryptographic baseline engine computes SHA-256 signatures via Windows CNG (`crypt32`), immediately identifying modified binaries, ransomware encryption runs, and unauthorized drift.
- **AU-2 & AU-3 — Event Logging & Content of Audit Records:**
  - Logs complete operational genealogies (previous vs. current paths), true MIME types inspected via file header magic numbers, and folder-level timestamp hierarchies.
- **AU-9 — Protection of Audit Information:**
  - Local embedded SQLite3 persistence in WAL mode ensures audit trails remain immune to network eavesdropping, transmission interception, or cloud downtime.

### 2.3. CIS Controls v8
- **Control 8 — Audit Log Management:**
  - Collection, review, and retention of detailed event logs across monitored server and desktop endpoints.
- **Control 10 — Data Recovery Capabilities:**
  - The built-in quarantine and recovery subsystem isolates suspected files and enforces cryptographic SHA-256 validation prior to restoring files to active directories.

### 2.4. GDPR (EU 2016/679) & LGPD (Brazilian Law 13.709/2018)
- **GDPR Article 32 / LGPD Art. 46 — Security of Processing:**
  - Requires technical and organizational measures to safeguard personal data against unauthorized disclosure, alteration, or accidental destruction.
  - *Linceu Lighthouse implementation:* Detects mass folder moves and anomalous file deletions across corporate repositories holding Personally Identifiable Information (HR, Legal, Medical, Accounting), facilitating early detection of insider threats.
  - *Sovereign Privacy Guarantee:* Operating 100% locally with zero external AI models ensures no international data transfers or third-party cloud exposures occur during audits.

---

## 3. Executive & Forensic Audit Deliverables

Linceu Lighthouse produces standardized compliance evidence packages:
1. **Executive Microsoft Excel Spreadsheets (.xlsx):** Detailed workbooks featuring executive summaries, event distribution metrics, and chronological logs.
2. **Structured JSON / XML:** Ready for direct ingestion into enterprise SIEM and SOAR platforms (Splunk, Microsoft Sentinel, Elastic, Graylog).
3. **Structured CSV (UTF-8 BOM):** Optimized for pivot tables and analytics in Microsoft Excel and Power BI without character encoding issues.
4. **Relational SQLite Database (.db):** Standalone, queryable database file that serves as an immutable digital chain of custody for external audits and forensic investigations.

</details>
