#pragma once

#include <QDate>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QSpinBox;
class QTabWidget;
class QTextEdit;
QT_END_NAMESPACE

namespace mocks {

struct LangInput {
    QString version;
    int day = 1;
    int month_index = 0;
    int year = 2026;

    LangInput();
    LangInput(const QString& ver, int d, int m, int y);
};

struct FileEntry {
    QString key;
    QString rel_path;
    QString default_group;
    QString lang;
    bool is_updatable = true;
};

struct FileCheckStatus {
    QString key;
    QString rel_path;
    QString group;
    QString lang;
    QString absolute_path;
    bool exists = false;
    QString detected_version;
    QString detected_date;
    QStringList found_patterns;
    QStringList missing_patterns;
    bool is_updatable = true;
};

struct UpdateResult {
    QString key;
    QString rel_path;
    QString group;
    bool ok = false;
    QString msg;
};

QString project_root_dir();
QString format_version_from_date(int year, int month, int day, const QString& existing_version = QString());
bool parse_version_to_date(const QString& version_str, int& day, int& month_index, int& year, int* revision = nullptr);
bool is_release_notes_file(const QString& rel_path);
bool is_updatable_file(const QString& rel_path);
QVector<FileEntry> get_all_file_entries(const QString& base_dir);
QList<FileCheckStatus> check_all_files(const QString& base_dir);
QList<UpdateResult> apply_updates(const LangInput& pt, const LangInput& en, const LangInput& es,
                                  const LangInput& fr, const LangInput& it, const LangInput& de,
                                  const QString& base_dir);
QString readme_version(const QString& version);
QString update_readme_section(const QString& text, const QString& section_id, const QString& version);
QString update_readme_versions(const QString& text, const LangInput& pt, const LangInput& en, const LangInput& es,
                               const LangInput& fr, const LangInput& it, const LangInput& de);
LangInput current_default_input();
QString current_composed_version(const QString& revision = QStringLiteral("0"));

class VersionEditor : public QWidget {
public:
    explicit VersionEditor(QWidget* parent = nullptr);

    void set_date_and_version(QLineEdit* ver_edit, QSpinBox* day_spin, QComboBox* month_combo, QSpinBox* year_spin, const QDate& date);
    void update_version_from_date(QLineEdit* ver_edit, int day, int month, int year);

private:
    QString selected_base_dir_;

    QLineEdit* folder_path_ = nullptr;
    QTabWidget* tab_widget_ = nullptr;
    QTextEdit* general_status_ = nullptr;
    QHash<QString, QTextEdit*> group_status_map_;

    QLineEdit* pt_version_ = nullptr;
    QSpinBox* pt_day_ = nullptr;
    QComboBox* pt_month_ = nullptr;
    QSpinBox* pt_year_ = nullptr;

    QLineEdit* en_version_ = nullptr;
    QSpinBox* en_day_ = nullptr;
    QComboBox* en_month_ = nullptr;
    QSpinBox* en_year_ = nullptr;

    QLineEdit* es_version_ = nullptr;
    QSpinBox* es_day_ = nullptr;
    QComboBox* es_month_ = nullptr;
    QSpinBox* es_year_ = nullptr;

    QLineEdit* fr_version_ = nullptr;
    QSpinBox* fr_day_ = nullptr;
    QComboBox* fr_month_ = nullptr;
    QSpinBox* fr_year_ = nullptr;

    QLineEdit* it_version_ = nullptr;
    QSpinBox* it_day_ = nullptr;
    QComboBox* it_month_ = nullptr;
    QSpinBox* it_year_ = nullptr;

    QLineEdit* de_version_ = nullptr;
    QSpinBox* de_day_ = nullptr;
    QComboBox* de_month_ = nullptr;
    QSpinBox* de_year_ = nullptr;

    void on_select_folder();
    void on_check();
    void on_save();
    void on_sync_pt_version();
    void on_reload_data();
    void set_today_all();
    void load_versions_from_disk_files();
};

}  // namespace mocks
