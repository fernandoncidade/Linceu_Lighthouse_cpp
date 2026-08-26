#include "compile_translations.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

#include <QApplication>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QTextCursor>
#include <QUrl>
#include <QVBoxLayout>

#ifdef _WIN32
#include <direct.h>
#include <process.h>
#else
#include <unistd.h>
#endif

namespace source::locale {

namespace {

std::ostream& qout = std::cout;
std::ostream& qerr = std::cerr;
std::string g_executable_dir;

char path_sep() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

const std::vector<std::string>& expected_ts_filenames() {
    static const std::vector<std::string> files = {
        "linceu_pt_BR.ts",
        "linceu_it_IT.ts",
        "linceu_fr_FR.ts",
        "linceu_es_ES.ts",
        "linceu_en_US.ts",
        "linceu_de_DE.ts"
    };
    return files;
}

std::string trim_copy(const std::string& input) {
    const std::string whitespace = " \t\r\n";
    const std::string::size_type first = input.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return std::string();
    }

    const std::string::size_type last = input.find_last_not_of(whitespace);
    return input.substr(first, last - first + 1U);
}

std::string to_lower_copy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string getenv_string(const char* name) {
    const char* value = std::getenv(name);
    return value ? std::string(value) : std::string();
}

bool ends_with_separator(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    const char last = path[path.size() - 1U];
    return last == '/' || last == '\\';
}

std::string join_path(const std::string& base, const std::string& tail) {
    if (base.empty()) {
        return tail;
    }
    if (tail.empty()) {
        return base;
    }
    if (ends_with_separator(base)) {
        return base + tail;
    }
    return base + path_sep() + tail;
}

std::string dirname_path(const std::string& path) {
    if (path.empty()) {
        return std::string();
    }

    const std::string::size_type slash = path.find_last_of("/\\");
    if (slash == std::string::npos) {
        return std::string();
    }
    return path.substr(0U, slash);
}

std::string filename_path(const std::string& path) {
    if (path.empty()) {
        return std::string();
    }

    const std::string::size_type slash = path.find_last_of("/\\");
    if (slash == std::string::npos) {
        return path;
    }
    return path.substr(slash + 1U);
}

std::string replace_extension(const std::string& path, const std::string& new_ext) {
    const std::string::size_type slash = path.find_last_of("/\\");
    const std::string::size_type dot = path.find_last_of('.');

    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return path + new_ext;
    }

    return path.substr(0U, dot) + new_ext;
}

bool file_exists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFREG) != 0;
}

bool dir_exists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFDIR) != 0;
}

unsigned long long file_size_bytes(const std::string& path, bool* ok) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (ok) {
            *ok = false;
        }
        return 0ULL;
    }

    if (ok) {
        *ok = true;
    }

    return static_cast<unsigned long long>(st.st_size);
}

std::time_t last_modified_time(const std::string& path, bool* ok) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }

    if (ok) {
        *ok = true;
    }

    return st.st_mtime;
}

std::string get_current_dir() {
    char buffer[4096];
#ifdef _WIN32
    if (_getcwd(buffer, sizeof(buffer)) == nullptr) {
#else
    if (getcwd(buffer, sizeof(buffer)) == nullptr) {
#endif
        return std::string();
    }
    return std::string(buffer);
}

std::vector<std::string> split_string(const std::string& text, char delimiter) {
    std::vector<std::string> values;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            values.push_back(item);
        }
    }
    return values;
}

std::string strip_wrapping_quotes(std::string value) {
    value = trim_copy(value);
    if (value.size() >= 2U && value[0] == '"' && value[value.size() - 1U] == '"') {
        return value.substr(1U, value.size() - 2U);
    }
    return value;
}

std::string read_file_text(const std::string& file_path, bool* ok) {
    std::ifstream input(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!input.is_open()) {
        if (ok) {
            *ok = false;
        }
        return std::string();
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    if (ok) {
        *ok = true;
    }

    return buffer.str();
}

bool has_all_expected_ts(const std::string& dir) {
    const std::vector<std::string>& files = expected_ts_filenames();
    std::size_t i = 0U;
    for (i = 0U; i < files.size(); ++i) {
        if (!file_exists(join_path(dir, files[i]))) {
            return false;
        }
    }
    return true;
}

std::string resolve_locale_dir() {
    std::vector<std::string> candidates;

    const std::string env_locale_dir = trim_copy(getenv_string("LINCEU_LOCALE_DIR"));
    if (!env_locale_dir.empty()) {
        candidates.push_back(env_locale_dir);
    }

    const std::string current_dir = get_current_dir();
    if (!current_dir.empty()) {
        candidates.push_back(join_path(current_dir, "source/locale"));
        candidates.push_back(current_dir);
    }

    const std::string compile_time_dir = dirname_path(std::string(__FILE__));
    if (!compile_time_dir.empty()) {
        candidates.push_back(compile_time_dir);
    }

    if (!g_executable_dir.empty()) {
        candidates.push_back(g_executable_dir);
        candidates.push_back(join_path(g_executable_dir, "source/locale"));
        candidates.push_back(join_path(join_path(g_executable_dir, ".."), "source/locale"));
        candidates.push_back(join_path(join_path(g_executable_dir, ".."), ".."));
    }

    std::set<std::string> seen;
    std::size_t i = 0U;
    for (i = 0U; i < candidates.size(); ++i) {
        const std::string candidate = candidates[i];
        if (candidate.empty()) {
            continue;
        }
        if (!seen.insert(candidate).second) {
            continue;
        }
        if (!dir_exists(candidate)) {
            continue;
        }
        if (has_all_expected_ts(candidate)) {
            return candidate;
        }
    }

    if (!compile_time_dir.empty()) {
        return compile_time_dir;
    }

    return current_dir;
}

std::string locale_dir() {
    static const std::string dir = resolve_locale_dir();
    return dir;
}

std::vector<std::string> list_ts_files() {
    const std::string dir = locale_dir();
    std::vector<std::string> files;
    const std::vector<std::string>& expected = expected_ts_filenames();
    files.reserve(expected.size());

    std::size_t i = 0U;
    for (i = 0U; i < expected.size(); ++i) {
        const std::string ts_file = join_path(dir, expected[i]);
        if (!file_exists(ts_file)) {
            qerr << "[WARN] Arquivo de traducao nao encontrado: " << ts_file << std::endl;
            continue;
        }
        files.push_back(ts_file);
    }

    return files;
}

bool has_full_translation_set(const std::vector<std::string>& ts_files) {
    return ts_files.size() == expected_ts_filenames().size();
}

std::string qm_path_from_ts(const std::string& ts_file_path) {
    return replace_extension(ts_file_path, ".qm");
}

#ifndef _WIN32
std::string quote_shell_arg(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());

    std::size_t i = 0U;
    for (i = 0U; i < value.size(); ++i) {
        const char ch = value[i];
        if (ch == '"') {
            escaped += "\\\"";
        } else {
            escaped += ch;
        }
    }

    return std::string("\"") + escaped + "\"";
}

std::string shell_friendly_path(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}
#endif

char path_delimiter() {
#ifdef _WIN32
    return ';';
#else
    return ':';
#endif
}

std::string find_on_path(const std::string& executable_name) {
    const std::string env_path = getenv_string("PATH");
    const std::vector<std::string> entries = split_string(env_path, path_delimiter());

    std::size_t i = 0U;
    for (i = 0U; i < entries.size(); ++i) {
        const std::string dir = strip_wrapping_quotes(entries[i]);
        if (dir.empty()) {
            continue;
        }

        const std::string candidate = join_path(dir, executable_name);
        if (file_exists(candidate)) {
            return candidate;
        }
    }

    return std::string();
}

std::vector<std::string> lrelease_names() {
#ifdef _WIN32
    std::vector<std::string> names;
    names.push_back("lrelease.exe");
    names.push_back("lrelease-qt6.exe");
    names.push_back("pyside6-lrelease.exe");
    return names;
#else
    std::vector<std::string> names;
    names.push_back("lrelease");
    names.push_back("lrelease-qt6");
    names.push_back("pyside6-lrelease");
    return names;
#endif
}

#ifdef _WIN32
std::vector<int> parse_version_components(const std::string& text) {
    std::vector<int> values;
    std::string current;

    std::size_t i = 0U;
    for (i = 0U; i < text.size(); ++i) {
        const unsigned char ch = static_cast<unsigned char>(text[i]);
        if (std::isdigit(ch) != 0) {
            current += static_cast<char>(ch);
            continue;
        }

        if (!current.empty()) {
            values.push_back(std::atoi(current.c_str()));
            current.clear();
        }
    }

    if (!current.empty()) {
        values.push_back(std::atoi(current.c_str()));
    }

    return values;
}

bool version_components_desc(const std::vector<int>& lhs, const std::vector<int>& rhs) {
    const std::size_t count = std::max(lhs.size(), rhs.size());

    std::size_t i = 0U;
    for (i = 0U; i < count; ++i) {
        const int left = i < lhs.size() ? lhs[i] : 0;
        const int right = i < rhs.size() ? rhs[i] : 0;
        if (left != right) {
            return left > right;
        }
    }

    return lhs.size() > rhs.size();
}

std::vector<std::string> default_qt_lrelease_candidates() {
    namespace fs = std::filesystem;

    struct Candidate {
        std::vector<int> version;
        std::string path;
    };

    std::vector<Candidate> found;
    std::error_code ec;
    const fs::path qt_root("C:/Qt");
    if (!fs::exists(qt_root, ec) || !fs::is_directory(qt_root, ec)) {
        return std::vector<std::string>();
    }

    fs::directory_iterator it(qt_root, fs::directory_options::skip_permission_denied, ec);
    fs::directory_iterator end;
    if (ec) {
        return std::vector<std::string>();
    }

    for (; it != end; it.increment(ec)) {
        if (ec) {
            break;
        }

        const fs::directory_entry& entry = *it;
        if (!entry.is_directory(ec)) {
            ec.clear();
            continue;
        }

        const fs::path candidate = entry.path() / "mingw_64" / "bin" / "lrelease.exe";
        if (!fs::is_regular_file(candidate, ec)) {
            ec.clear();
            continue;
        }

        Candidate match;
        match.version = parse_version_components(entry.path().filename().string());
        match.path = candidate.generic_string();
        found.push_back(match);
        ec.clear();
    }

    std::sort(found.begin(), found.end(), [](const Candidate& lhs, const Candidate& rhs) {
        return version_components_desc(lhs.version, rhs.version);
    });

    std::vector<std::string> paths;
    paths.reserve(found.size());

    std::size_t i = 0U;
    for (i = 0U; i < found.size(); ++i) {
        paths.push_back(found[i].path);
    }

    return paths;
}
#endif

bool contains_substring(const std::string& text, const std::string& needle) {
    return text.find(needle) != std::string::npos;
}

std::string extract_tag_text(const std::string& block, const std::string& tag) {
    const std::string open_tag = std::string("<") + tag + ">";
    const std::string close_tag = std::string("</") + tag + ">";

    const std::string::size_type start = block.find(open_tag);
    if (start == std::string::npos) {
        return std::string();
    }

    const std::string::size_type content_start = start + open_tag.size();
    const std::string::size_type end = block.find(close_tag, content_start);
    if (end == std::string::npos || end < content_start) {
        return std::string();
    }

    return trim_copy(block.substr(content_start, end - content_start));
}

struct TranslationInfo {
    bool valid;
    bool unfinished;
    std::string text;
};

TranslationInfo extract_translation_info(const std::string& block) {
    TranslationInfo info;
    info.valid = false;
    info.unfinished = false;

    const std::string::size_type tag_start = block.find("<translation");
    if (tag_start == std::string::npos) {
        return info;
    }

    const std::string::size_type open_end = block.find('>', tag_start);
    if (open_end == std::string::npos) {
        return info;
    }

    const std::string attrs = block.substr(tag_start, open_end - tag_start + 1U);
    info.unfinished = contains_substring(attrs, "type=\"unfinished\"") || contains_substring(attrs, "type='unfinished'");
    info.valid = true;

    if (open_end > 0U && block[open_end - 1U] == '/') {
        info.text.clear();
        return info;
    }

    const std::string close_tag = "</translation>";
    const std::string::size_type close_start = block.find(close_tag, open_end + 1U);
    if (close_start == std::string::npos || close_start < open_end + 1U) {
        info.text.clear();
        return info;
    }

    info.text = trim_copy(block.substr(open_end + 1U, close_start - (open_end + 1U)));
    return info;
}

void count_message_stats(const std::string& text, int* total_messages, int* translated_messages) {
    int total = 0;
    int translated = 0;

    std::string::size_type search_from = 0U;
    while (true) {
        const std::string::size_type msg_start = text.find("<message", search_from);
        if (msg_start == std::string::npos) {
            break;
        }

        const std::string::size_type msg_end_tag = text.find("</message>", msg_start);
        if (msg_end_tag == std::string::npos) {
            break;
        }

        const std::string::size_type msg_end = msg_end_tag + std::string("</message>").size();
        const std::string block = text.substr(msg_start, msg_end - msg_start);

        ++total;

        const TranslationInfo info = extract_translation_info(block);
        if (info.valid && !info.unfinished && !info.text.empty()) {
            ++translated;
        }

        search_from = msg_end;
    }

    if (total_messages) {
        *total_messages = total;
    }
    if (translated_messages) {
        *translated_messages = translated;
    }
}

std::string format_percent(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(1) << value;
    return out.str();
}

std::string language_code_from_ts(const std::string& ts_file_path) {
    const std::string name = filename_path(ts_file_path);
    const std::string prefix = "linceu_";
    const std::string suffix = ".ts";

    if (name.find(prefix) == 0U && name.size() > prefix.size() + suffix.size()) {
        return name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
    }

    return name;
}

}  // namespace

void set_executable_dir_for_cli(const std::string& argv0) {
    g_executable_dir = dirname_path(argv0);
}

std::string find_lrelease() {
    const std::vector<std::string> names = lrelease_names();

    const std::string env_lrelease = trim_copy(getenv_string("LRELEASE"));
    if (!env_lrelease.empty() && file_exists(env_lrelease)) {
        return env_lrelease;
    }

    const std::string env_qt_lrelease = trim_copy(getenv_string("QT_LRELEASE"));
    if (!env_qt_lrelease.empty() && file_exists(env_qt_lrelease)) {
        return env_qt_lrelease;
    }

    std::size_t i = 0U;
    for (i = 0U; i < names.size(); ++i) {
        const std::string on_path = find_on_path(names[i]);
        if (!on_path.empty()) {
            return on_path;
        }
    }

    const std::string qtdir = trim_copy(getenv_string("QTDIR"));
    if (!qtdir.empty()) {
        for (i = 0U; i < names.size(); ++i) {
            const std::string candidate = join_path(join_path(qtdir, "bin"), names[i]);
            if (file_exists(candidate)) {
                return candidate;
            }
        }
    }

#ifdef _WIN32
    const std::vector<std::string> qt_candidates = default_qt_lrelease_candidates();

    for (i = 0U; i < qt_candidates.size(); ++i) {
        if (file_exists(qt_candidates[i])) {
            return qt_candidates[i];
        }
    }
#endif

    if (!g_executable_dir.empty()) {
        const std::vector<std::string> around_exe = {
            g_executable_dir,
            join_path(g_executable_dir, ".."),
            join_path(join_path(g_executable_dir, ".."), "bin"),
            join_path(join_path(join_path(g_executable_dir, ".."), ".."), "bin")
        };

        std::size_t b = 0U;
        for (b = 0U; b < around_exe.size(); ++b) {
            for (i = 0U; i < names.size(); ++i) {
                const std::string candidate = join_path(around_exe[b], names[i]);
                if (file_exists(candidate)) {
                    return candidate;
                }
            }
        }
    }

    return std::string();
}

bool compile_with_lrelease(const std::string& ts_file_path, const std::string& qm_file_path, const std::string& tool_path) {
    const std::string ts_name = filename_path(ts_file_path);
    qout << "Compilando com " << tool_path << ": " << ts_name << std::endl;

#ifdef _WIN32
    std::vector<const char*> args;
    args.push_back(tool_path.c_str());
    args.push_back("-silent");
    args.push_back(ts_file_path.c_str());
    args.push_back("-qm");
    args.push_back(qm_file_path.c_str());
    args.push_back(nullptr);

    const int exit_code = _spawnv(_P_WAIT, tool_path.c_str(), &args[0]);
#else
    const std::string command = quote_shell_arg(shell_friendly_path(tool_path))
        + " -silent"
        + " "
        + quote_shell_arg(shell_friendly_path(ts_file_path))
        + " -qm "
        + quote_shell_arg(shell_friendly_path(qm_file_path));
    const int exit_code = std::system(command.c_str());
#endif
    if (exit_code != 0) {
        qerr << "  [ERRO] Falha ao compilar. Codigo: " << exit_code << std::endl;
        return false;
    }

    if (!file_exists(qm_file_path)) {
        qerr << "  [ERRO] lrelease finalizou sem gerar o .qm: " << qm_file_path << std::endl;
        return false;
    }

    bool size_ok = false;
    const unsigned long long size = file_size_bytes(qm_file_path, &size_ok);
    if (!size_ok || size == 0ULL) {
        qerr << "  [ERRO] Arquivo .qm gerado esta vazio: " << qm_file_path << std::endl;
        return false;
    }

    qout << "  [OK] Compilado com sucesso" << std::endl;
    return true;
}

TranslationMap parse_ts_file(const std::string& ts_file_path, bool* ok) {
    TranslationMap locale;

    bool read_ok = false;
    const std::string text = read_file_text(ts_file_path, &read_ok);
    if (!read_ok) {
        if (ok) {
            *ok = false;
        }
        qerr << "Erro ao ler arquivo: " << ts_file_path << std::endl;
        return locale;
    }

    std::string::size_type search_from = 0U;
    while (true) {
        const std::string::size_type msg_start = text.find("<message", search_from);
        if (msg_start == std::string::npos) {
            break;
        }

        const std::string::size_type msg_end_tag = text.find("</message>", msg_start);
        if (msg_end_tag == std::string::npos) {
            break;
        }

        const std::string::size_type msg_end = msg_end_tag + std::string("</message>").size();
        const std::string block = text.substr(msg_start, msg_end - msg_start);

        const std::string source_text = extract_tag_text(block, "source");
        const TranslationInfo trans_info = extract_translation_info(block);

        if (!source_text.empty() && trans_info.valid && !trans_info.unfinished && !trans_info.text.empty()) {
            locale[source_text] = trans_info.text;
        }

        search_from = msg_end;
    }

    if (ok) {
        *ok = true;
    }

    return locale;
}

bool compile_ts_to_qm_fallback(const std::string& ts_file_path, const std::string& qm_file_path) {
    (void)ts_file_path;
    (void)qm_file_path;
    qerr << "  [ERRO] Nao existe fallback valido sem lrelease." << std::endl;
    qerr << "         Configure LRELEASE/QT_LRELEASE ou ajuste o PATH." << std::endl;
    return false;
}

std::pair<bool, std::string> validate_ts_file(const std::string& ts_file_path) {
    bool read_ok = false;
    const std::string text = read_file_text(ts_file_path, &read_ok);
    if (!read_ok) {
        return std::make_pair(false, "Erro: nao foi possivel abrir o arquivo");
    }

    if (!contains_substring(text, "<TS")) {
        return std::make_pair(false, "Elemento raiz nao parece ser TS");
    }

    if (!contains_substring(text, "<context")) {
        return std::make_pair(false, "Nenhum contexto encontrado");
    }

    int total_messages = 0;
    int translated_messages = 0;
    count_message_stats(text, &total_messages, &translated_messages);

    if (total_messages <= 0) {
        return std::make_pair(false, "Nenhuma mensagem encontrada");
    }

    const double completion = static_cast<double>(translated_messages) * 100.0 / static_cast<double>(total_messages);

    std::ostringstream message;
    message << translated_messages << "/" << total_messages << " traducoes (" << format_percent(completion) << "%)";
    return std::make_pair(true, message.str());
}

bool compile_translations() {
    qout << "=== Compilando traducoes do Linceu Lighthouse ===" << std::endl << std::endl;
    qout << "Diretorio de traducoes: " << locale_dir() << std::endl;

    const std::vector<std::string> ts_files = list_ts_files();
    if (ts_files.empty()) {
        qerr << "Nenhum arquivo .ts esperado foi encontrado." << std::endl;
        return false;
    }
    if (!has_full_translation_set(ts_files)) {
        qerr << "[ERRO] Nem todos os 6 arquivos .ts esperados estao disponiveis." << std::endl;
        return false;
    }

    const std::string tool_path = find_lrelease();
    if (tool_path.empty()) {
        qerr << "[ERRO] lrelease nao encontrado." << std::endl;
        qerr << "       Configure LRELEASE, QT_LRELEASE ou ajuste o PATH." << std::endl;
        return false;
    }

    qout << "[OK] Ferramenta encontrada: " << tool_path << std::endl << std::endl;

    int success_count = 0;

    std::size_t i = 0U;
    for (i = 0U; i < ts_files.size(); ++i) {
        const std::string qm_file = qm_path_from_ts(ts_files[i]);
        if (compile_with_lrelease(ts_files[i], qm_file, tool_path)) {
            ++success_count;
        }
    }

    qout << std::endl << "=== Resultado ===" << std::endl;
    qout << "Sucessos: " << success_count << "/" << ts_files.size() << std::endl;

    return success_count == static_cast<int>(ts_files.size());
}

bool validate_translations() {
    qout << "Validando arquivos de traducao..." << std::endl;

    const std::vector<std::string> ts_files = list_ts_files();
    if (ts_files.empty()) {
        qout << "Nenhum arquivo .ts encontrado!" << std::endl;
        return false;
    }

    int valid_count = 0;

    std::size_t i = 0U;
    for (i = 0U; i < ts_files.size(); ++i) {
        const std::pair<bool, std::string> result = validate_ts_file(ts_files[i]);
        qout << (result.first ? "[OK] " : "[ERRO] ") << filename_path(ts_files[i]) << ": " << result.second << std::endl;
        if (result.first) {
            ++valid_count;
        }
    }

    qout << std::endl << "Resultado: " << valid_count << "/" << ts_files.size() << " arquivos validos" << std::endl;
    return valid_count == static_cast<int>(ts_files.size());
}

void list_translations() {
    qout << "Arquivos de traducao disponiveis:" << std::endl;

    const std::vector<std::string> ts_files = list_ts_files();
    if (ts_files.empty()) {
        qout << "Nenhum arquivo .ts encontrado!" << std::endl;
        return;
    }

    std::size_t i = 0U;
    for (i = 0U; i < ts_files.size(); ++i) {
        const std::string qm_file = qm_path_from_ts(ts_files[i]);
        const bool qm_exists = file_exists(qm_file);

        const std::pair<bool, std::string> validation = validate_ts_file(ts_files[i]);
        qout << "  " << (qm_exists ? "[OK] " : "[MISS] ") << filename_path(ts_files[i])
             << " - " << (validation.first ? "[OK] " : "[ERRO] ") << validation.second << std::endl;

        if (qm_exists) {
            bool ts_ok = false;
            bool qm_ok = false;
            const std::time_t ts_time = last_modified_time(ts_files[i], &ts_ok);
            const std::time_t qm_time = last_modified_time(qm_file, &qm_ok);
            if (ts_ok && qm_ok && ts_time > qm_time) {
                qout << "    [WARN] Arquivo .ts mais recente que .qm" << std::endl;
            }
        }
    }
}

void clean_compiled() {
    qout << "Removendo arquivos .qm..." << std::endl;

    const std::string dir = locale_dir();
    int removed_count = 0;
    int found_count = 0;

    const std::vector<std::string>& files = expected_ts_filenames();
    std::size_t i = 0U;
    for (i = 0U; i < files.size(); ++i) {
        const std::string qm_path = replace_extension(join_path(dir, files[i]), ".qm");

        if (!file_exists(qm_path)) {
            continue;
        }

        ++found_count;

        if (std::remove(qm_path.c_str()) == 0) {
            qout << "[OK] Removido: " << filename_path(qm_path) << std::endl;
            ++removed_count;
        } else {
            qerr << "[ERRO] Falha ao remover " << filename_path(qm_path) << std::endl;
        }
    }

    if (found_count == 0) {
        qout << "Nenhum arquivo .qm encontrado!" << std::endl;
        return;
    }

    qout << std::endl << "Resultado: " << removed_count << " arquivos removidos" << std::endl;
}

void show_stats() {
    qout << "Estatisticas das traducoes:" << std::endl;

    const std::vector<std::string> ts_files = list_ts_files();
    if (ts_files.empty()) {
        qout << "Nenhum arquivo .ts encontrado!" << std::endl;
        return;
    }

    int valid_files = 0;
    int total_messages = 0;
    int total_translated = 0;

    std::size_t i = 0U;
    for (i = 0U; i < ts_files.size(); ++i) {
        bool read_ok = false;
        const std::string text = read_file_text(ts_files[i], &read_ok);
        if (!read_ok) {
            qout << "  " << filename_path(ts_files[i]) << ": Erro - nao foi possivel abrir arquivo" << std::endl;
            continue;
        }

        int file_messages = 0;
        int file_translated = 0;
        count_message_stats(text, &file_messages, &file_translated);

        total_messages += file_messages;
        total_translated += file_translated;
        ++valid_files;

        const double completion = file_messages > 0
            ? (static_cast<double>(file_translated) * 100.0 / static_cast<double>(file_messages))
            : 0.0;

        qout << "  " << language_code_from_ts(ts_files[i]) << ": " << file_translated << "/" << file_messages
             << " (" << format_percent(completion) << "%)" << std::endl;
    }

    const double overall_completion = total_messages > 0
        ? (static_cast<double>(total_translated) * 100.0 / static_cast<double>(total_messages))
        : 0.0;

    qout << std::endl
         << "Total: " << total_translated << "/" << total_messages
         << " (" << format_percent(overall_completion) << "%)" << std::endl;
    qout << "Arquivos validos: " << valid_files << "/" << ts_files.size() << std::endl;
}

void install_tools() {
    qout << "Tentando instalar ferramentas do PySide6..." << std::endl;

#ifdef _WIN32
    const std::string null_redirect = " >NUL 2>&1";
#else
    const std::string null_redirect = " >/dev/null 2>&1";
#endif

    std::string python;
    if (std::system((std::string("python --version") + null_redirect).c_str()) == 0) {
        python = "python";
    } else if (std::system((std::string("py --version") + null_redirect).c_str()) == 0) {
        python = "py";
    }

    if (python.empty()) {
        qerr << "[ERRO] python nao encontrado no PATH" << std::endl;
        return;
    }

    const std::vector<std::string> packages = {"PySide6-Essentials", "PySide6-Addons"};

    std::size_t i = 0U;
    for (i = 0U; i < packages.size(); ++i) {
        qout << "Instalando " << packages[i] << "..." << std::endl;
        const std::string command = python + " -m pip install " + packages[i];
        const int exit_code = std::system(command.c_str());
        if (exit_code == 0) {
            qout << "[OK] " << packages[i] << " instalado com sucesso" << std::endl;
        } else {
            qerr << "[ERRO] Falha ao instalar " << packages[i] << ". Codigo: " << exit_code << std::endl;
        }
    }
}

int run_compile_translations_cli(const std::vector<std::string>& args) {
    qout << "=== Compilador de Traducoes - Linceu Lighthouse ===" << std::endl << std::endl;

    const std::string command = args.size() > 1U ? to_lower_copy(args[1]) : std::string("compile");
    bool ok = true;

    if (command == "compile") {
        ok = compile_translations();
    } else if (command == "validate") {
        ok = validate_translations();
    } else if (command == "list") {
        list_translations();
    } else if (command == "clean") {
        clean_compiled();
    } else if (command == "stats") {
        show_stats();
    } else if (command == "install") {
        install_tools();
    } else if (command == "all") {
        qout << "1. Validando arquivos..." << std::endl;
        if (validate_translations()) {
            qout << std::endl << "2. Compilando traducoes..." << std::endl;
            ok = compile_translations();
        } else {
            qout << std::endl << "[WARN] Alguns arquivos tem problemas. Corrija antes de compilar." << std::endl;
            ok = false;
        }
    } else {
        qerr << "Comando desconhecido: " << command << std::endl;
        qerr << "Comandos disponiveis: compile, validate, list, clean, stats, install, all" << std::endl;
        return 2;
    }

    return ok ? 0 : 1;
}

}  // namespace source::locale

namespace {

QString to_qstring(const std::string& value) {
    return QString::fromLocal8Bit(value.c_str());
}

QString project_root_dir() {
    QDir dir(QCoreApplication::applicationDirPath());

    for (int depth = 0; depth < 8; ++depth) {
        const QString locale_file = dir.filePath(QStringLiteral("source/locale/linceu_pt_BR.ts"));
        const QString cmake_file = dir.filePath(QStringLiteral("CMakeLists.txt"));
        if (QFileInfo::exists(locale_file) && QFileInfo::exists(cmake_file)) {
            return QDir::cleanPath(dir.absolutePath());
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QDir::cleanPath(QDir::currentPath());
}

QString default_locale_dir() {
    const QString path = QDir(project_root_dir()).filePath(QStringLiteral("source/locale"));
    return QFileInfo::exists(path) ? QDir(path).absolutePath() : QDir::currentPath();
}

QString find_window_icon_path() {
    const QString root = project_root_dir();
    const QStringList candidates = {
        QDir(root).filePath(QStringLiteral("source/assets/icones/linceu_sem_fundo.ico"))
    };

    for (const QString& candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return QFileInfo(candidate).absoluteFilePath();
        }
    }

    return {};
}

QString find_default_lrelease_path() {
    return to_qstring(source::locale::find_lrelease());
}

bool is_cli_command(const std::string& command) {
    static const std::vector<std::string> commands = {
        "compile", "validate", "list", "clean", "stats", "install", "all"};
    return std::find(commands.begin(), commands.end(), command) != commands.end();
}

std::vector<std::string> normalize_cli_args(const std::vector<std::string>& args) {
    if (args.size() > 2U && args[1] == "--cli") {
        std::vector<std::string> normalized;
        normalized.reserve(args.size() - 1U);
        normalized.push_back(args[0]);
        for (std::size_t i = 2U; i < args.size(); ++i) {
            normalized.push_back(args[i]);
        }
        return normalized;
    }

    return args;
}

class CompileTranslationsWindow final : public QWidget {
public:
    explicit CompileTranslationsWindow(QWidget* parent = nullptr)
        : QWidget(parent),
          process_(new QProcess(this)) {
        setWindowTitle(QStringLiteral("Compile Translations"));
        setMinimumSize(980, 720);

        const QString icon_path = find_window_icon_path();
        if (!icon_path.isEmpty()) {
            setWindowIcon(QIcon(icon_path));
        }

        process_->setProcessChannelMode(QProcess::MergedChannels);
        connect(process_, &QProcess::started, this, [this]() { on_process_started(); });
        connect(process_, &QProcess::readyReadStandardOutput, this, [this]() { append_process_output(); });
        connect(process_,
                qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
                this,
                [this](int exit_code, QProcess::ExitStatus status) {
                    on_process_finished(exit_code, status);
                });

        auto* root_layout = new QVBoxLayout(this);
        root_layout->setContentsMargins(16, 16, 16, 16);
        root_layout->setSpacing(12);

        auto* intro = new QLabel(
            QStringLiteral("Interface dedicada para validar, compilar e inspecionar as traducoes .ts e .qm."),
            this);
        intro->setWordWrap(true);
        root_layout->addWidget(intro);

        auto* paths_group = new QGroupBox(QStringLiteral("Entradas"), this);
        auto* paths_layout = new QGridLayout(paths_group);
        paths_layout->setHorizontalSpacing(8);
        paths_layout->setVerticalSpacing(8);

        auto* locale_label = new QLabel(QStringLiteral("Diretorio de traducoes:"), paths_group);
        locale_dir_edit_ = new QLineEdit(default_locale_dir(), paths_group);
        auto* browse_locale = new QPushButton(QStringLiteral("Procurar..."), paths_group);
        connect(browse_locale, &QPushButton::clicked, this, [this]() { browse_locale_dir(); });

        auto* lrelease_label = new QLabel(QStringLiteral("Executavel lrelease:"), paths_group);
        lrelease_edit_ = new QLineEdit(find_default_lrelease_path(), paths_group);
        lrelease_edit_->setPlaceholderText(QStringLiteral("Opcional - deteccao automatica se vazio"));
        auto* browse_lrelease = new QPushButton(QStringLiteral("Procurar..."), paths_group);
        connect(browse_lrelease, &QPushButton::clicked, this, [this]() { browse_lrelease_path(); });

        auto* executable_label = new QLabel(QStringLiteral("Executavel atual:"), paths_group);
        executable_value_ = new QLabel(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()), paths_group);
        executable_value_->setTextInteractionFlags(Qt::TextSelectableByMouse);
        executable_value_->setWordWrap(true);

        paths_layout->addWidget(locale_label, 0, 0);
        paths_layout->addWidget(locale_dir_edit_, 0, 1);
        paths_layout->addWidget(browse_locale, 0, 2);
        paths_layout->addWidget(lrelease_label, 1, 0);
        paths_layout->addWidget(lrelease_edit_, 1, 1);
        paths_layout->addWidget(browse_lrelease, 1, 2);
        paths_layout->addWidget(executable_label, 2, 0);
        paths_layout->addWidget(executable_value_, 2, 1, 1, 2);
        root_layout->addWidget(paths_group);

        auto* actions_group = new QGroupBox(QStringLiteral("Acoes"), this);
        auto* actions_layout = new QGridLayout(actions_group);
        actions_layout->setHorizontalSpacing(8);
        actions_layout->setVerticalSpacing(8);

        compile_button_ = new QPushButton(QStringLiteral("Compilar"), actions_group);
        validate_button_ = new QPushButton(QStringLiteral("Validar"), actions_group);
        list_button_ = new QPushButton(QStringLiteral("Listar"), actions_group);
        stats_button_ = new QPushButton(QStringLiteral("Estatisticas"), actions_group);
        clean_button_ = new QPushButton(QStringLiteral("Limpar .qm"), actions_group);
        install_button_ = new QPushButton(QStringLiteral("Instalar Ferramentas"), actions_group);
        all_button_ = new QPushButton(QStringLiteral("Validar + Compilar"), actions_group);
        cancel_button_ = new QPushButton(QStringLiteral("Cancelar"), actions_group);
        cancel_button_->setEnabled(false);

        connect(compile_button_, &QPushButton::clicked, this, [this]() {
            start_command(QStringLiteral("compile"), QStringLiteral("Compilar traducoes"));
        });
        connect(validate_button_, &QPushButton::clicked, this, [this]() {
            start_command(QStringLiteral("validate"), QStringLiteral("Validar traducoes"));
        });
        connect(list_button_, &QPushButton::clicked, this, [this]() {
            start_command(QStringLiteral("list"), QStringLiteral("Listar traducoes"));
        });
        connect(stats_button_, &QPushButton::clicked, this, [this]() {
            start_command(QStringLiteral("stats"), QStringLiteral("Estatisticas das traducoes"));
        });
        connect(clean_button_, &QPushButton::clicked, this, [this]() {
            if (QMessageBox::question(this,
                                      QStringLiteral("Confirmar"),
                                      QStringLiteral("Deseja remover os arquivos .qm do diretorio selecionado?")) ==
                QMessageBox::Yes) {
                start_command(QStringLiteral("clean"), QStringLiteral("Limpar arquivos .qm"));
            }
        });
        connect(install_button_, &QPushButton::clicked, this, [this]() {
            if (QMessageBox::question(this,
                                      QStringLiteral("Confirmar"),
                                      QStringLiteral("Deseja tentar instalar as ferramentas auxiliares agora?")) ==
                QMessageBox::Yes) {
                start_command(QStringLiteral("install"), QStringLiteral("Instalar ferramentas"));
            }
        });
        connect(all_button_, &QPushButton::clicked, this, [this]() {
            start_command(QStringLiteral("all"), QStringLiteral("Validar e compilar traducoes"));
        });
        connect(cancel_button_, &QPushButton::clicked, this, [this]() { cancel_running_process(); });

        actions_layout->addWidget(compile_button_, 0, 0);
        actions_layout->addWidget(validate_button_, 0, 1);
        actions_layout->addWidget(list_button_, 0, 2);
        actions_layout->addWidget(stats_button_, 0, 3);
        actions_layout->addWidget(clean_button_, 1, 0);
        actions_layout->addWidget(install_button_, 1, 1);
        actions_layout->addWidget(all_button_, 1, 2);
        actions_layout->addWidget(cancel_button_, 1, 3);
        root_layout->addWidget(actions_group);

        auto* status_row = new QHBoxLayout();
        auto* status_title = new QLabel(QStringLiteral("Status:"), this);
        status_value_ = new QLabel(QStringLiteral("Pronto"), this);
        status_value_->setTextInteractionFlags(Qt::TextSelectableByMouse);
        status_row->addWidget(status_title);
        status_row->addWidget(status_value_, 1);
        root_layout->addLayout(status_row);

        auto* output_group = new QGroupBox(QStringLiteral("Saida"), this);
        auto* output_layout = new QVBoxLayout(output_group);
        output_layout->setSpacing(8);

        output_log_ = new QPlainTextEdit(output_group);
        output_log_->setReadOnly(true);
        output_log_->setLineWrapMode(QPlainTextEdit::NoWrap);
        output_log_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        output_layout->addWidget(output_log_, 1);

        auto* output_buttons = new QHBoxLayout();
        auto* open_locale_button = new QPushButton(QStringLiteral("Abrir Diretorio Locale"), output_group);
        auto* clear_log_button = new QPushButton(QStringLiteral("Limpar Log"), output_group);
        connect(open_locale_button, &QPushButton::clicked, this, [this]() { open_locale_dir(); });
        connect(clear_log_button, &QPushButton::clicked, output_log_, &QPlainTextEdit::clear);
        output_buttons->addWidget(open_locale_button);
        output_buttons->addWidget(clear_log_button);
        output_buttons->addStretch(1);
        output_layout->addLayout(output_buttons);
        root_layout->addWidget(output_group, 1);

        append_log(QStringLiteral("Executavel atual: %1")
                       .arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath())));
        append_log(QStringLiteral("Diretorio locale inicial: %1")
                       .arg(QDir::toNativeSeparators(locale_dir_edit_->text())));
        if (!lrelease_edit_->text().trimmed().isEmpty()) {
            append_log(QStringLiteral("lrelease detectado: %1")
                           .arg(QDir::toNativeSeparators(lrelease_edit_->text().trimmed())));
        }
    }

private:
    void append_log(const QString& text) {
        if (text.isEmpty()) {
            return;
        }

        output_log_->moveCursor(QTextCursor::End);
        output_log_->insertPlainText(text);
        if (!text.endsWith(QLatin1Char('\n'))) {
            output_log_->insertPlainText(QStringLiteral("\n"));
        }
        output_log_->moveCursor(QTextCursor::End);
    }

    void browse_locale_dir() {
        const QString selected =
            QFileDialog::getExistingDirectory(this,
                                              QStringLiteral("Selecionar diretorio de traducoes"),
                                              locale_dir_edit_->text().trimmed());
        if (!selected.isEmpty()) {
            locale_dir_edit_->setText(QDir::cleanPath(selected));
        }
    }

    void browse_lrelease_path() {
        const QString selected = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Selecionar executavel lrelease"),
            QFileInfo(lrelease_edit_->text().trimmed()).absolutePath(),
            QStringLiteral("Executavel lrelease (lrelease*.exe);;Todos os arquivos (*.*)"));
        if (!selected.isEmpty()) {
            lrelease_edit_->setText(QDir::cleanPath(selected));
        }
    }

    void open_locale_dir() {
        const QString locale_dir = locale_dir_edit_->text().trimmed();
        if (!locale_dir.isEmpty() && QFileInfo(locale_dir).isDir()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(locale_dir));
            return;
        }

        QMessageBox::warning(this,
                             QStringLiteral("Diretorio invalido"),
                             QStringLiteral("Selecione um diretorio locale valido antes de abrir."));
    }

    bool validate_inputs(QString* locale_dir, QString* lrelease_path) {
        const QString executable_path = QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath();
        if (!QFileInfo::exists(executable_path)) {
            QMessageBox::critical(this,
                                  QStringLiteral("Executavel nao encontrado"),
                                  QStringLiteral("O executavel atual nao foi encontrado."));
            return false;
        }

        const QString locale = QDir::cleanPath(locale_dir_edit_->text().trimmed());
        if (locale.isEmpty() || !QFileInfo(locale).isDir()) {
            QMessageBox::warning(
                this,
                QStringLiteral("Diretorio invalido"),
                QStringLiteral("Informe um diretorio locale valido antes de executar o comando."));
            return false;
        }

        const QString lrelease = QDir::cleanPath(lrelease_edit_->text().trimmed());
        if (!lrelease.isEmpty() && !QFileInfo::exists(lrelease)) {
            QMessageBox::warning(
                this,
                QStringLiteral("Executavel invalido"),
                QStringLiteral("O caminho informado para lrelease nao existe."));
            return false;
        }

        if (locale_dir != nullptr) {
            *locale_dir = locale;
        }
        if (lrelease_path != nullptr) {
            *lrelease_path = lrelease;
        }
        return true;
    }

    void set_running_state(bool running) {
        compile_button_->setEnabled(!running);
        validate_button_->setEnabled(!running);
        list_button_->setEnabled(!running);
        stats_button_->setEnabled(!running);
        clean_button_->setEnabled(!running);
        install_button_->setEnabled(!running);
        all_button_->setEnabled(!running);
        cancel_button_->setEnabled(running);
    }

    void start_command(const QString& command, const QString& label) {
        if (process_->state() != QProcess::NotRunning) {
            return;
        }

        QString locale_dir;
        QString lrelease_path;
        if (!validate_inputs(&locale_dir, &lrelease_path)) {
            return;
        }

        current_command_label_ = label;
        append_log(QStringLiteral("\n=== %1 ===").arg(label));
        append_log(QStringLiteral("Horario: %1")
                       .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
        append_log(QStringLiteral("Locale dir: %1").arg(QDir::toNativeSeparators(locale_dir)));
        if (!lrelease_path.isEmpty()) {
            append_log(QStringLiteral("LRELEASE: %1").arg(QDir::toNativeSeparators(lrelease_path)));
        }

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert(QStringLiteral("LINCEU_LOCALE_DIR"), locale_dir);
        if (!lrelease_path.isEmpty()) {
            env.insert(QStringLiteral("LRELEASE"), lrelease_path);
            env.insert(QStringLiteral("QT_LRELEASE"), lrelease_path);
        }

        process_->setWorkingDirectory(project_root_dir());
        process_->setProcessEnvironment(env);
        process_->setProgram(QCoreApplication::applicationFilePath());
        process_->setArguments(QStringList() << QStringLiteral("--cli") << command);
        process_->start();

        if (!process_->waitForStarted(3000)) {
            append_log(QStringLiteral("[ERRO] Falha ao iniciar o processo interno."));
            status_value_->setText(QStringLiteral("Falha ao iniciar"));
            set_running_state(false);
        }
    }

    void on_process_started() {
        set_running_state(true);
        status_value_->setText(QStringLiteral("Executando: %1").arg(current_command_label_));
    }

    void append_process_output() {
        const QByteArray output = process_->readAllStandardOutput();
        if (!output.isEmpty()) {
            append_log(QString::fromLocal8Bit(output));
        }
    }

    void on_process_finished(int exit_code, QProcess::ExitStatus exit_status) {
        append_process_output();
        set_running_state(false);

        const bool ok = (exit_status == QProcess::NormalExit && exit_code == 0);
        const QString status_text = ok
                                        ? QStringLiteral("Concluido com sucesso")
                                        : QStringLiteral("Finalizado com erro (codigo %1)").arg(exit_code);
        append_log(QStringLiteral("=== %1 ===\n").arg(status_text));
        status_value_->setText(status_text);
    }

    void cancel_running_process() {
        if (process_->state() == QProcess::NotRunning) {
            return;
        }

        append_log(QStringLiteral("[INFO] Cancelando processo em execucao..."));
        process_->kill();
    }

    QLineEdit* locale_dir_edit_ = nullptr;
    QLineEdit* lrelease_edit_ = nullptr;
    QLabel* executable_value_ = nullptr;
    QLabel* status_value_ = nullptr;
    QPlainTextEdit* output_log_ = nullptr;
    QPushButton* compile_button_ = nullptr;
    QPushButton* validate_button_ = nullptr;
    QPushButton* list_button_ = nullptr;
    QPushButton* stats_button_ = nullptr;
    QPushButton* clean_button_ = nullptr;
    QPushButton* install_button_ = nullptr;
    QPushButton* all_button_ = nullptr;
    QPushButton* cancel_button_ = nullptr;
    QProcess* process_ = nullptr;
    QString current_command_label_;
};

}  // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    if (argc > 0) {
        args.reserve(static_cast<std::size_t>(argc));
    }

    int i = 0;
    for (i = 0; i < argc; ++i) {
        args.push_back(argv[i] ? argv[i] : "");
    }

    if (argc > 0 && argv[0] != nullptr) {
        source::locale::set_executable_dir_for_cli(argv[0]);
    }

    const std::vector<std::string> cli_args = normalize_cli_args(args);
    if (cli_args.size() > 1U && is_cli_command(cli_args[1])) {
        return source::locale::run_compile_translations_cli(cli_args);
    }

    QApplication app(argc, argv);
    CompileTranslationsWindow window;
    window.show();
    return app.exec();
}
