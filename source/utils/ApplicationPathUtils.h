#pragma once

#include <optional>
#include <QString>

namespace source::utils {

std::optional<QString> get_app_base_path();
std::optional<QString> get_text_file_path(const QString& filename, const std::optional<QString>& folder = std::nullopt);
std::optional<QString> load_text_file(const QString& filename, const std::optional<QString>& folder = std::nullopt,
                                      const QString& encoding = QStringLiteral("utf-8"));

}  // namespace source::utils
