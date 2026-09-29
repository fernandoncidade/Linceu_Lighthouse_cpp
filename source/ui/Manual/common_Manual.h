#pragma once

#include <QString>
#include <vector>

namespace source {
namespace ui {
namespace manual {

// Bloco detalhado com resumo, parágrafos explicativos e marcadores dentro de uma seção do manual.
struct ManualDetails {
    QString summary;
    std::vector<QString> paragraphs;
    std::vector<QString> bullets;
};

// Seção principal do manual contendo identificador único, título, parágrafos, marcadores e subdetalhes.
struct ManualSection {
    QString id;
    QString title;
    std::vector<QString> paragraphs;
    std::vector<QString> bullets;
    std::vector<ManualDetails> details;
};

// Bloco atômico de texto para renderização sequencial no diálogo de manual da interface.
struct ManualBlock {
    QString kind;
    QString text;
    QString section_id;
};

// Entrada de tecla de atalho e sua descrição localizada.
struct ShortcutEntry {
    QString sequence;
    QString description;
};

// Retorna o diretório persistente de dados da aplicação.
QString data_dir();

// Converte texto alfanumérico para caracteres matemáticos em negrito Unicode.
QString to_unicode_bold(const QString& s);

// Retorna as entradas de atalhos localizadas para o idioma solicitado.
std::vector<ShortcutEntry> shortcut_entries(const QString& lang);

// Retorna as linhas de atalhos formatadas no padrão "Atalho: Descrição".
std::vector<QString> shortcut_lines(const QString& lang);

} // namespace manual
} // namespace ui
} // namespace source
