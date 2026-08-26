#pragma once

#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QSpinBox;
class QTextEdit;
QT_END_NAMESPACE

namespace mocks {

class VersionEditor : public QWidget {
public:
    explicit VersionEditor(QWidget* parent = nullptr);

private:
    QString selected_base_dir_;

    QLineEdit* folder_path_;
    QTextEdit* status_;

    QLineEdit* pt_version_;
    QSpinBox* pt_day_;
    QComboBox* pt_month_;
    QSpinBox* pt_year_;

    QLineEdit* en_version_;
    QSpinBox* en_day_;
    QComboBox* en_month_;
    QSpinBox* en_year_;

    QLineEdit* es_version_;
    QSpinBox* es_day_;
    QComboBox* es_month_;
    QSpinBox* es_year_;

    QLineEdit* fr_version_;
    QSpinBox* fr_day_;
    QComboBox* fr_month_;
    QSpinBox* fr_year_;

    QLineEdit* it_version_;
    QSpinBox* it_day_;
    QComboBox* it_month_;
    QSpinBox* it_year_;

    QLineEdit* de_version_;
    QSpinBox* de_day_;
    QComboBox* de_month_;
    QSpinBox* de_year_;

    void on_select_folder();
    void on_check();
    void on_save();
};

}  // namespace mocks
