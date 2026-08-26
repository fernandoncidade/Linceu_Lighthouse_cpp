#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;

namespace source::security::ui {

class MultiDirectoryDialog : public QDialog {
public:
    explicit MultiDirectoryDialog(QWidget* parent, QObject* interface_obj);

public slots:
    void refresh_data();
    void refresh_texts();

private slots:
    void add_directory();
    void remove_selected_directory();
    void save_profile_name();

private:
    int selected_directory_row() const;

private:
    QObject* interface_;
    QObject* loc_;
    QLabel* summary_label_;
    QLabel* profile_label_;
    QLabel* directories_label_;
    QLineEdit* profile_name_edit_;
    QListWidget* directories_list_;
    QPushButton* add_button_;
    QPushButton* remove_button_;
    QPushButton* save_button_;
    QPushButton* refresh_button_;
    QPushButton* close_button_;
};

}  // namespace source::security::ui
