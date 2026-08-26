#pragma once

#include <QDialog>

class QLabel;
class QPushButton;
class QTableWidget;

namespace source::security::ui {

class AlertCenterDialog : public QDialog {
public:
    explicit AlertCenterDialog(QWidget* parent, QObject* interface_obj);

public slots:
    void refresh_data();
    void refresh_texts();

private:
    void apply_rows(const QVariantList& rows, bool replace_all);

    QObject* interface_;
    QObject* loc_;
    QLabel* summary_label_;
    QTableWidget* table_;
    QPushButton* refresh_button_;
    QPushButton* close_button_;
    bool initialized_;
    qint64 last_alert_id_;
};

}  // namespace source::security::ui
