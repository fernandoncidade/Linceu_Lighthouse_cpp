#pragma once

#include <QDialog>
#include <QVariantList>

class QLabel;
class QPushButton;
class QTableWidget;

namespace source::security::ui {

class SessionCenterDialog : public QDialog {
public:
    explicit SessionCenterDialog(QWidget* parent, QObject* interface_obj);

public slots:
    void refresh_data();
    void refresh_texts();

private:
    void finish_refresh(const QVariantList& rows, bool replace_all, quint64 request_id);
    void apply_rows(const QVariantList& rows, bool replace_all);

    QObject* interface_;
    QObject* loc_;
    QLabel* summary_label_;
    QTableWidget* table_;
    QPushButton* refresh_button_;
    QPushButton* close_button_;
    bool refresh_pending_;
    bool initialized_;
    bool refresh_inflight_;
    quint64 refresh_request_id_;
    QString last_updated_at_;
};

}  // namespace source::security::ui
