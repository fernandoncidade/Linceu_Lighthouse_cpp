# Notas de Lançamento / Release Notes — v2026.10.21.0

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher as notas de versão em português</summary><br>

# Linceu Lighthouse — Release v2026.10.21.0

**Linceu Lighthouse v2026.10.21.0**

**Tag de Versão:** `v2026.10.21.0`  
**Data de Publicação:** 21 de outubro de 2026
**Autor:** Fernando Nillsson Cidade  
**Download Direto:** [Baixar Última Versão no GitHub](../../releases/latest) | [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)  

---

## 1. Artefatos de Instalação e Hashes Criptográficos

Valide a integridade do pacote baixado antes da execução utilizando o cmdlet nativo do PowerShell:

```powershell
Get-FileHash -Path ".\Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe" -Algorithm SHA256
```

| Nome do Artefato | Formato | Arquitetura Alvo | Descrição |
|---|---|:---:|---|
| `Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe` | Instalador Inno Setup | x64 (MSVC / MinGW) | Instalador gráfico completo com integração ao shell do Windows e atalhos na Área de Trabalho. |
| `Linceu_Lighthouse-v2026.10.21.0-x64-Portable.zip` | Arquivo ZIP Portátil | x64 | Versão portátil autônoma sem necessidade de instalação ou privilégios de administrador. |
| `Source code (zip)` | Arquivo ZIP | Qualquer | Código-fonte completo em C++17 e Qt 6. |
| `Source code (tar.gz)` | Arquivo Tarball | Qualquer | Código-fonte completo em C++17 e Qt 6. |

---

## 2. Destaques da Versão 2026.9.28.0

Esta versão consolida grandes avanços em performance multithread, estabilidade de observação, análise forense e usabilidade:

- **Sessões Assíncronas em Tempo Real (`SessionCenterDialog`):** Carregamento em segundo plano via thread dedicada (`run_on_security_runtime_thread`), mantendo a interface perfeitamente fluida mesmo em diretórios com centenas de milhares de arquivos históricos, com dicas de ferramentas (tooltips) para caminhos múltiplos.
- **Quarentena Inteligente sem Tremulação (`RecoveryDialog`):** Mecanismo de atualização diferencial na interface (`find_row_by_text`), preservando seleção de linhas, ordenação de colunas e posição de rolagem ativa durante a ocorrência contínua de novos eventos.
- **Restauração Segura com Validação Criptográfica SHA-256:** Diálogo de escolha de diretório de destino (restauração no caminho original ou em pasta alternativa isolada de teste) com conferência de integridade rigorosa via hash SHA-256 antes da escrita no disco.
- **Central de Alertas em Tempo Real (`AlertCenterDialog`):** Classificação determinística em 4 níveis de severidade (Baixo, Médio, Alto e Crítico) com correlação direta de regras violadas e pacotes de evidência.
- **Gerenciador de Perfis Multi-Diretório (`MultiDirectoryDialog`):** Criação e persistência de perfis para auditoria simultânea de múltiplas pastas e unidades locais.
- **Visualização Estatística sem IA (17 Gráficos Nativos):** Renderização direta em C++/Qt com aceleração por hardware (OpenGL), incluindo Linhas do Tempo, Mapas de Calor (Heatmaps), Fluxos Sankey, Pareto e Treemaps de Ocupação.
- **Internacionalização Completa em 6 Idiomas:** Suporte nativo e alternância em tempo de execução entre Português (BR), Inglês (US), Espanhol (ES), Francês (FR), Italiano (IT) e Alemão (DE).
- **Backend Gráfico Flexível (`LINCEU_OPENGL_MODE`):** Compatibilidade estendida com placas dedicadas, gráficos integrados e modo software para máquinas virtuais e ambientes RDP.

---

## 3. Requisitos de Sistema

- **Sistema Operacional:** Windows 11, Windows 10, Windows Server 2022 / 2019 (64-bit x64).
- **Processador:** Arquitetura x86-64 (Intel Core i3/i5/i7/i9 ou AMD Ryzen).
- **Memória RAM:** Mínimo de 256 MB (consumo típico em operação contínua < 120 MB).
- **Espaço em Disco:** ~80 MB livres para instalação e banco SQLite local.

---

## 4. Instruções de Instalação e Atualização

### Atualização a partir de versões anteriores
Se você já utiliza uma versão anterior do Linceu Lighthouse, execute o novo instalador `Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe`. Todos os seus bancos de dados históricos, perfis e configurações em `%LOCALAPPDATA%\LinceuLighthouse` serão preservados integralmente.

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the release notes in English</summary><br>

# Linceu Lighthouse — Release v2026.10.21.0

**Linceu Lighthouse v2026.10.21.0**

**Version Tag:** `v2026.10.21.0`  
**Publication Date:** October 21, 2026
**Author:** Fernando Nillsson Cidade  
**Direct Download:** [Download Latest GitHub Release](../../releases/latest) | [Install via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)  

---

## 1. Installation Packages & Cryptographic Checksums

Verify package integrity prior to launch using native Windows PowerShell:

```powershell
Get-FileHash -Path ".\Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe" -Algorithm SHA256
```

| Package Name | Format | Target Arch | Description |
|---|---|:---:|---|
| `Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe` | Inno Setup Installer | x64 (MSVC / MinGW) | Full graphical installer with Windows desktop shortcuts and shell integration. |
| `Linceu_Lighthouse-v2026.10.21.0-x64-Portable.zip` | Portable ZIP Archive | x64 | Standalone portable build; unpack and run without administrative rights. |
| `Source code (zip)` | ZIP Archive | Any | Full source code in C++17 and Qt 6. |
| `Source code (tar.gz)` | Tarball Archive | Any | Full source code in C++17 and Qt 6. |

---

## 2. Highlights in Release v2026.10.21.0

This release consolidates significant advancements in multithreaded performance, filesystem observation reliability, forensic auditing, and desktop usability:

- **Asynchronous Real-Time Sessions (`SessionCenterDialog`):** Non-blocking background data loading via worker threads (`run_on_security_runtime_thread`), keeping the UI responsive even across massive historical event databases, accompanied by rich tooltips for multi-path setups.
- **Flicker-Free Quarantine & Recovery (`RecoveryDialog`):** Differential table synchronization (`find_row_by_text`) that preserves row selection, column sorting, and active scroll position during live event streams.
- **Secure Restoration with SHA-256 Cryptographic Verification:** Interactive directory picker (restoring to original location or an isolated test folder) with mandatory SHA-256 hash checks before writing restored files to disk.
- **Real-Time Alert Center (`AlertCenterDialog`):** Deterministic classification across 4 severity tiers (Low, Medium, High, Critical) with direct correlation to violated rules and forensic evidence packages.
- **Multi-Directory Profile Management (`MultiDirectoryDialog`):** Define, save, and reload observation profiles across multiple distinct local folders and partitions.
- **Deterministic Analytics Without External AI (17 Native Charts):** Direct C++/Qt hardware-accelerated rendering (OpenGL) for Activity Timelines, Hourly Heatmaps, Sankey Flows, Pareto Charts, and Disk Treemaps.
- **Full Dynamic Internationalization (6 Languages):** Seamless runtime language switching between Portuguese (BR), English (US), Spanish (ES), French (FR), Italian (IT), and German (DE).
- **Flexible Graphics Pipeline (`LINCEU_OPENGL_MODE`):** Configurable OpenGL context initialization for dedicated GPUs, integrated chips, and software fallbacks on virtual machines or RDP.

---

## 3. System Requirements

- **Operating System:** Windows 11, Windows 10, Windows Server 2022 / 2019 (64-bit x64).
- **Processor:** x86-64 architecture (Intel Core i3/i5/i7/i9 or AMD Ryzen).
- **Memory (RAM):** 256 MB minimum (typical continuous memory usage < 120 MB).
- **Storage:** ~80 MB free disk space for installation and local SQLite database.

---

## 4. Upgrade Instructions

### Upgrading from Previous Releases
If upgrading from an earlier version, simply execute the installer `Linceu_Lighthouse-v2026.10.21.0-x64-Setup.exe`. All existing SQLite event databases, baseline snapshots, and user preferences stored in `%LOCALAPPDATA%\LinceuLighthouse` are automatically preserved.

</details>
