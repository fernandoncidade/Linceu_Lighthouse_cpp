#pragma once

#include <QHash>
#include <QString>
#include <QStringList>
#include <tuple>
#include <vector>

#include "source/ui/Manual/common_Manual.h"

namespace source::ui::manual {

QString normalize_language(const QString& lang = QString());
QString get_manual_title(const QString& lang = QString());
QString to_unicode_bold(const QString& text);
std::vector<ManualSection> get_manual_document(const QString& lang = QString());
std::tuple<std::vector<ManualBlock>, QStringList> get_manual_blocks(const QString& lang = QString());
QString get_manual_text(const QString& lang = QString());
std::tuple<QString, QHash<QString, int>, QStringList> get_manual_text_with_positions(const QString& lang = QString());

}  // namespace source::ui::manual
