#pragma once

#include <QString>
#include <vector>

namespace source {
namespace ui {
namespace manual {

struct ManualDetails {
    QString summary;
    std::vector<QString> paragraphs;
    std::vector<QString> bullets;
};

struct ManualSection {
    QString id;
    QString title;
    std::vector<QString> paragraphs;
    std::vector<QString> bullets;
    std::vector<ManualDetails> details;
};

struct ManualBlock {
    QString kind;
    QString text;
    QString section_id;
};

struct ShortcutEntry {
    QString sequence;
    QString description;
};

QString data_dir();
QString to_unicode_bold(const QString& s);
std::vector<ShortcutEntry> shortcut_entries(const QString& lang);
std::vector<QString> shortcut_lines(const QString& lang);

} // namespace manual
} // namespace ui
} // namespace source
