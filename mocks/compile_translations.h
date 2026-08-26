#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace source::locale {

using TranslationMap = std::unordered_map<std::string, std::string>;

std::string find_lrelease();
bool compile_with_lrelease(const std::string& ts_file_path, const std::string& qm_file_path, const std::string& tool_path);
TranslationMap parse_ts_file(const std::string& ts_file_path, bool* ok = nullptr);
bool compile_ts_to_qm_fallback(const std::string& ts_file_path, const std::string& qm_file_path);
std::pair<bool, std::string> validate_ts_file(const std::string& ts_file_path);

bool compile_translations();
bool validate_translations();
void list_translations();
void clean_compiled();
void show_stats();
void install_tools();

int run_compile_translations_cli(const std::vector<std::string>& args);

}  // namespace source::locale
