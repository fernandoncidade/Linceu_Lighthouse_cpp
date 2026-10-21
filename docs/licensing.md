# Licenciamento e Esclarecimento Comercial / Licensing & Commercial Model — Linceu Lighthouse

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">🇧🇷 Português (BR)</a> | <a href="#enus">🇺🇸 English (US)</a>
</p>

---

## <a id="ptbr"></a>🇧🇷 Português (BR)

<details>
<summary>Clique para expandir / recolher o documento de licenciamento em português</summary><br>

# Linceu Lighthouse — Licenciamento, Termos de Código Aberto e Esclarecimento Comercial

**Versão:** `2026.10.21.0`  
**Data:** `21 de outubro de 2026`  
**Licença do Código-Fonte:** Licença MIT  
**Autor e Detentor dos Direitos Autorais:** Fernando Nillsson Cidade  
**Edição Comercial Oficial:** [Instalar via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)  

---

## 1. Visão Geral do Modelo de Licenciamento

O **Linceu Lighthouse** adota um modelo aberto transparente que concilia a liberdade do ecossistema de software livre com a distribuição corporativa oficial:

| Componente | Licença | Usos Permitidos |
|---|---|---|
| **Código-Fonte (Repositório Completo)** | [Licença MIT](../LICENSE) | 100% Livre: Uso comercial, modificação, distribuição, sublicenciamento e uso privado. |
| **Documentação em `source/assets/`** | Autoria do Projeto (Uso Livre para Desenvolvedores) | Criada para a Microsoft Store; desenvolvedores possuem liberdade total para alterar, excluir, adicionar e adaptar em seus próprios forks e plataformas. |
| **Binários Compilados por Terceiros (via Código)** | Licença MIT | Totalmente livres para compilar, executar, empacotar e redistribuir internamente ou comercialmente sob a responsabilidade do desenvolvedor. |
| **Binário Oficial Compilado na Microsoft Store** | Edição Oficial do Autor | Proibida a revenda, comercialização de terceiros ou engenharia reversa do binário oficial fornecido pelo autor. |
| **Bibliotecas de Terceiros (Qt 6, SQLite)** | LGPL v3 / Domínio Público | Cumprem integralmente os requisitos de atribuição de código aberto upstream. |

---

## 2. Direitos de Código Aberto (Licença MIT)

Todo o código-fonte do Linceu Lighthouse — abrangendo o motor de observação em C++17 (`ReadDirectoryChangesW`), o subsistema de metadados, o banco de dados assíncrono em SQLite (`DatabaseWriter`), o motor gráfico de 17 visualizações em `source/data/Charts/`, as rotinas de segurança criptográfica (CNG SHA-256), os recursos de internacionalização em 6 idiomas e os presets do CMake — está licenciado sob a permissiva **Licença MIT**:

```text
Copyright (c) 2026 Fernando Nillsson Cidade

É concedida permissão, gratuitamente, a qualquer pessoa que obtenha uma cópia
deste software e dos arquivos de documentação associados (o "Software"), para lidar
com o Software sem restrições, incluindo, sem limitação, os direitos de usar,
copiar, modificar, mesclar, publicar, distribuir, sublicenciar e/ou vender
cópias do Software, e de permitir que as pessoas a quem o Software é
fornecido o façam, sob as seguintes condições:

O aviso de direitos autorais acima e este aviso de permissão deverão ser incluídos em todas
as cópias ou partes substanciais do Software.
```

---

## 3. Documentação em `source/assets/` e Liberdade Total para Desenvolvedores

A documentação institucional e regulatória mantida no diretório `source/assets/` (abrangendo os módulos `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY` e `RELEASE`) possui status e finalidade específicos:

1. **Finalidade Original:** Estes documentos foram elaborados por Fernando Nillsson Cidade exclusivamente para viabilizar a publicação, conformidade regulatória e comercialização da aplicação oficial na **Microsoft Store** (cumprindo exigências contratuais de EULA, acessibilidade, privacidade e conformidade com diretrizes de marketplace corporativo).
2. **Autorização Expressa para Desenvolvedores:** O autor **autoriza expressamente e concede total liberdade** para que desenvolvedores, contribuidores e forks realizem quaisquer alterações, exclusões, adições ou modificações gerais nessa documentação e no código associado.
3. **Condição de Responsabilidade:** Os desenvolvedores são livres para adaptar ou remover esses textos em seus próprios forks e distribuições, desde que respeitem as diretrizes, normas e a legislação vigente de seus respectivos países e das plataformas em que decidirem publicar suas próprias compilações.

---

## 4. Diferenciação dos Binários Oficiais Compilados pelo Autor

Há uma distinção jurídica e operacional fundamental entre o **código-fonte** e o **binário oficial compilado pelo autor**:

- **Liberdade Absoluta no Código-Fonte:** O código-fonte integral está aberto e disponível publicamente neste repositório. Qualquer pessoa é livre para inspecionar, auditar, compilar, modificar e criar executáveis próprios a partir do código.
- **Restrição sobre o Binário Oficial do Autor (Microsoft Store):** Apenas o binário compilado, empacotado, assinado digitalmente e disponibilizado oficialmente pelo autor (**Fernando Nillsson Cidade**), notadamente o produto comercial distribuído na Microsoft Store:
  - **NÃO possui autorização** para ser comercializado, revendido ou redistribuído comercialmente por terceiros como se fosse produto deles;
  - **NÃO possui autorização** para ser submetido a engenharia reversa (prática que, ademais, torna-se totalmente inócua e desnecessária, visto que o código-fonte original completo já é disponibilizado publicamente pelo próprio autor neste repositório).
- **Compilação Própria Permitida:** Caso terceiros desejem comercializar ou distribuir uma solução baseada neste projeto, devem compilar seus próprios binários a partir do código-fonte aberto, gerando seus próprios pacotes e assumindo a responsabilidade legal por suas distribuições, sem utilizar as chaves de assinatura oficial, marca ou pacotes prontos do autor.

---

## 5. Licenças de Terceiros e Higiene de Propriedade Intelectual

1. **Framework Qt (Qt 6.x):** Licenciado sob a **GNU LGPL v3**. O Linceu Lighthouse vincula dinamicamente as DLLs do Qt (`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Sql.dll`, `Qt6Xml.dll`, `Qt6Concurrent.dll`, `Qt6OpenGL.dll`, `Qt6OpenGLWidgets.dll`, `Qt6Quick.dll`, `Qt6Qml.dll`), permitindo substituição das bibliotecas conforme o item 4(d) da LGPL v3. Os avisos de terceiros encontram-se em `source/assets/LICENSES/` e `source/assets/NOTICES/`.
2. **SQLite3:** Dedicado formalmente ao **Domínio Público**.
3. **APIs Nativas do Windows:** Bibliotecas nativas do sistema operacional Windows (`kernel32`, `crypt32`, `wintrust`, `version`, `user32`, `shell32`).

</details>

---

## <a id="enus"></a>🇺🇸 English (US)

<details>
<summary>Click to expand / collapse the licensing document in English</summary><br>

# Linceu Lighthouse — Licensing, Open Source Terms & Commercial Clarification

**Version:** `2026.10.21.0`  
**Date:** `October 21, 2026`  
**Source Code License:** MIT License  
**Author & Copyright Owner:** Fernando Nillsson Cidade  
**Official Commercial Release:** [Install via Microsoft Store](https://apps.microsoft.com/detail/9NN8Z5Z700TM)  

---

## 1. Overview of the Licensing Model

**Linceu Lighthouse** adopts a transparent open-source model that balances community freedom with official enterprise distribution:

| Component | License | Permitted Uses |
|---|---|---|
| **Source Code (Full Repository)** | [MIT License](../LICENSE) | 100% Free: Commercial use, modification, distribution, sublicensing, and private use. |
| **Documentation in `source/assets/`** | Project Authorship (Free Adaptation) | Created for Microsoft Store; developers have complete freedom to modify, delete, add, and adapt for their own forks and platforms. |
| **Self-compiled Binaries (from Source)** | MIT License | Completely free to compile, run, package, and distribute internally or commercially under the developer's responsibility. |
| **Official Compiled Binary (Microsoft Store)** | Author's Official Release | Third-party commercial resale, redistribution, or reverse engineering of the author's official compiled binary is prohibited. |
| **Third-Party Libraries (Qt 6, SQLite)** | LGPL v3 / Public Domain | Complies with all upstream open source attribution requirements. |

---

## 2. Source Code Open Source Rights (MIT License)

The complete source code of Linceu Lighthouse — including the C++17 observation core (`ReadDirectoryChangesW`), metadata extraction engine, asynchronous SQLite batch persistence (`DatabaseWriter`), the 17-chart visualization suite under `source/data/Charts/`, cryptographic security algorithms (CNG SHA-256), multi-language catalogs across 6 languages, and CMake presets — is licensed under the permissive **MIT License**:

```text
Copyright (c) 2026 Fernando Nillsson Cidade

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## 3. Documentation in `source/assets/` and Total Developer Freedom

The institutional and regulatory documentation maintained under the `source/assets/` directory (encompassing `ABOUT`, `CLC`, `COPYRIGHT`, `EULA`, `LICENSES`, `NOTICES`, `PRIVACY_POLICY`, and `RELEASE`) carries a specific context:

1. **Original Purpose:** These documents were authored by Fernando Nillsson Cidade exclusively to enable the official certification, compliance, and commercial publishing of the application on the **Microsoft Store** (satisfying mandatory EULA, accessibility, and privacy requirements for enterprise app marketplaces).
2. **Express Developer Authorization:** The author **expressly authorizes and grants full freedom** to developers, contributors, and forks to make any changes, deletions, additions, or general modifications to this documentation and associated code.
3. **Compliance Requirement:** Developers are completely free to adapt or remove these assets in their own forks and custom builds, provided they respect the guidelines, standards, and legislation applicable in their respective countries and target distribution platforms.

---

## 4. Clarification on the Author's Official Pre-Compiled Binaries

There is a clear legal and operational distinction between the **open source code** and the **author's official compiled binaries**:

- **Absolute Freedom in Source Code:** The full source code is publicly accessible in this repository under the MIT License. Anyone is free to audit, compile, modify, build custom executables, and embed the engine into their own solutions.
- **Restrictions on the Author's Official Pre-Compiled Binary (Microsoft Store):** The official binary compiled, packaged, digitally signed, and distributed directly by the author (**Fernando Nillsson Cidade**), specifically the commercial edition sold on the Microsoft Store:
  - **May NOT** be commercially resold, redistributed for profit, or re-hosted by third parties as their own commercial product;
  - **May NOT** be subjected to reverse engineering (which is entirely pointless anyway, since the author already provides the full, unabridged source code openly in this repository).
- **Custom Builds are Welcome:** Third parties wishing to distribute or commercialize solutions derived from this project must compile their own binaries from the open source code, publish them under their own responsibility, and provide their own branding and packaging without misusing the author's official signing identity, trademarks, or pre-packaged store releases.

---

## 5. Third-Party Licenses & IP Hygiene

1. **Qt Framework (Qt 6.x):**
   - Licensed under the **GNU LGPL v3**.
   - Linceu Lighthouse dynamically links against Qt shared libraries (`.dll`), allowing users to replace Qt libraries with newer or modified builds as required by LGPL v3 section 4(d).
   - Upstream notices and source code references are preserved in `source/assets/LICENSES/` and `source/assets/NOTICES/`.
2. **SQLite3:**
   - Dedicated in the **Public Domain**.
   - Used for local, offline historical persistence and audit storage.
3. **Windows System SDK:**
   - Standard Win32 system APIs (`kernel32`, `crypt32`, `wintrust`, `version`, `user32`, `shell32`) provided natively by Microsoft Windows.

</details>
