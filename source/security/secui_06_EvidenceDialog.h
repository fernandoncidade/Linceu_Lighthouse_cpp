#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;
class QTextBrowser;

namespace source::security::ui {

class EvidenceDialog : public QDialog {
public:
    explicit EvidenceDialog(QWidget* parent, QObject* interface_obj);

public slots:
    void refresh_data();
    void refresh_texts();

private slots:
    void browse_destination();
    void export_package();
    void open_last_package_folder();

private:
    QObject* interface_;
    QObject* loc_;
    QString last_package_path_;
    QLabel* summary_label_;
    QLabel* destination_label_;
    QLineEdit* destination_edit_;
    QTextBrowser* result_browser_;
    QPushButton* browse_button_;
    QPushButton* export_button_;
    QPushButton* open_folder_button_;
    QPushButton* refresh_button_;
    QPushButton* close_button_;
};

}  // namespace source::security::ui
