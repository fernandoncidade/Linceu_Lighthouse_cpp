#pragma once

#include "source/utils/AssociatedApps.h"

#include <QDialog>
#include <QVector>

class QPushButton;
class QLabel;
class QTableWidget;
class QEvent;

namespace source::ui {

class AppChooserDialog final : public QDialog {
    Q_OBJECT

public:
    explicit AppChooserDialog(const QString& filePath,
                              const QVector<source::utils::AssociatedApp>& apps,
                              QWidget* parent = nullptr);

    source::utils::AssociatedApp selectedApp() const;

private slots:
    void chooseExecutableManually();
    void accept() override;

protected:
    void changeEvent(QEvent* event) override;

private:
    void populateTable();
    void retranslateUi();
    int selectedRow() const;

    QString m_filePath;
    QVector<source::utils::AssociatedApp> m_apps;
    QLabel* m_fileLabel = nullptr;
    QTableWidget* m_table = nullptr;
    QPushButton* m_chooseOtherButton = nullptr;
};

}  // namespace source::ui
