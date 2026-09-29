#pragma once

#include <QDialog>
#include <QFutureWatcher>
#include <QPair>
#include <QVariantList>

class QLabel;
class QPushButton;
class QTableWidget;
class QTabWidget;

namespace source::security::ui {

class RecoveryDialog : public QDialog {
    Q_OBJECT
public:
    explicit RecoveryDialog(QWidget* parent, QObject* interface_obj);
    ~RecoveryDialog() override;

public slots:
    void refresh_data();
    void refresh_texts();

private slots:
    void open_selected_quarantine_folder();
    void restore_selected_quarantine_copy();
    void open_selected_baseline_folder();
    void restore_selected_baseline_version();

private:
    int selected_row(QTableWidget* table) const;
    void apply_quarantine_rows(const QVariantList& rows, bool replace_all);
    void apply_baseline_rows(const QVariantList& rows, bool replace_all);
    void update_summary();

private:
    QObject* interface_;
    QObject* loc_;
    QLabel* summary_label_;
    QTabWidget* tabs_;
    QTableWidget* quarantine_table_;
    QTableWidget* baseline_table_;
    QPushButton* quarantine_open_button_;
    QPushButton* quarantine_restore_button_;
    QPushButton* baseline_open_button_;
    QPushButton* baseline_restore_button_;
    QPushButton* refresh_button_;
    QPushButton* close_button_;
    QFutureWatcher<QPair<QVariantList, QVariantList>>* refresh_watcher_;
    bool refresh_pending_;
    bool initialized_;
    qint64 last_quarantine_id_;
    QString last_baseline_updated_at_;
};

}  // namespace source::security::ui
