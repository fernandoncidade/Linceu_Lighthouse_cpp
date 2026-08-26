#include "AppChooserDialog.h"

#include <QAbstractItemView>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

namespace source::ui {
namespace {

QString trKey(const char* key, const QString& fallback) {
    const QString source = QString::fromLatin1(key);
    const QString translated = QCoreApplication::translate("LinceuLighthouse", key);
    if (translated.isEmpty() || translated == source) {
        return fallback;
    }
    return translated;
}

}  // namespace

AppChooserDialog::AppChooserDialog(const QString& filePath,
                                   const QVector<source::utils::AssociatedApp>& apps,
                                   QWidget* parent)
    : QDialog(parent),
      m_filePath(filePath),
      m_apps(apps) {
    resize(820, 420);

    auto* layout = new QVBoxLayout(this);

    m_fileLabel = new QLabel(this);
    m_fileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(m_fileLabel);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(3);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    layout->addWidget(m_table);

    populateTable();

    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { accept(); });

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_chooseOtherButton = buttons->addButton(QString(), QDialogButtonBox::ActionRole);
    connect(m_chooseOtherButton, &QPushButton::clicked, this, &AppChooserDialog::chooseExecutableManually);
    connect(buttons, &QDialogButtonBox::accepted, this, &AppChooserDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &AppChooserDialog::reject);
    layout->addWidget(buttons);

    retranslateUi();
}

source::utils::AssociatedApp AppChooserDialog::selectedApp() const {
    const int row = selectedRow();
    if (row < 0 || row >= m_apps.size()) {
        return {};
    }
    return m_apps.at(row);
}

void AppChooserDialog::populateTable() {
    m_table->setRowCount(m_apps.size());
    for (int row = 0; row < m_apps.size(); ++row) {
        const source::utils::AssociatedApp& app = m_apps.at(row);
        m_table->setItem(row, 0, new QTableWidgetItem(app.displayName));
        m_table->setItem(row, 1, new QTableWidgetItem(app.source));
        m_table->setItem(row, 2, new QTableWidgetItem(app.executablePath));
    }

    m_table->resizeColumnsToContents();
    if (!m_apps.isEmpty()) {
        m_table->selectRow(0);
    }
}

int AppChooserDialog::selectedRow() const {
    const QList<QTableWidgetSelectionRange> ranges = m_table->selectedRanges();
    return ranges.isEmpty() ? -1 : ranges.first().topRow();
}

void AppChooserDialog::chooseExecutableManually() {
#ifdef Q_OS_WIN
    const QString filter = trKey("app_chooser_executable_filter",
                                 QStringLiteral("Executáveis (*.exe);;Todos os arquivos (*)"));
#else
    const QString filter = trKey("app_chooser_all_files_filter",
                                 QStringLiteral("Todos os arquivos (*)"));
#endif

    const QString exe = QFileDialog::getOpenFileName(this,
                                                     trKey("app_chooser_choose_executable",
                                                           QStringLiteral("Escolher aplicativo")),
                                                     QString(),
                                                     filter);
    if (exe.isEmpty()) {
        return;
    }

    const QFileInfo exeInfo(exe);
    source::utils::AssociatedApp app;
    app.kind = source::utils::AssociatedApp::Kind::Executable;
    app.displayName = exeInfo.completeBaseName().isEmpty() ? exe : exeInfo.completeBaseName();
    app.executablePath = exe;
    app.source = trKey("app_chooser_manual_source",
                       QStringLiteral("Selecionado manualmente pelo usuário"));

    m_apps.push_back(app);
    populateTable();
    m_table->selectRow(m_apps.size() - 1);
}

void AppChooserDialog::accept() {
    if (selectedRow() < 0) {
        QMessageBox::warning(this,
                             trKey("app_chooser_no_app_title",
                                   QStringLiteral("Nenhum aplicativo selecionado")),
                             trKey("app_chooser_no_app_message",
                                   QStringLiteral("Selecione um aplicativo para abrir o arquivo.")));
        return;
    }
    QDialog::accept();
}

void AppChooserDialog::changeEvent(QEvent* event) {
    QDialog::changeEvent(event);
    if (event != nullptr && event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
}

void AppChooserDialog::retranslateUi() {
    setWindowTitle(trKey("app_chooser_title", QStringLiteral("Escolher aplicativo")));

    if (m_fileLabel != nullptr) {
        const QFileInfo info(m_filePath);
        m_fileLabel->setText(
            trKey("app_chooser_prompt",
                  QStringLiteral("Escolha o aplicativo para abrir:\n%1")).arg(info.fileName()));
    }

    if (m_table != nullptr) {
        m_table->setHorizontalHeaderLabels({
            trKey("app_chooser_column_app", QStringLiteral("Aplicativo")),
            trKey("app_chooser_column_source", QStringLiteral("Origem")),
            trKey("app_chooser_column_executable", QStringLiteral("Executável"))
        });
    }

    if (m_chooseOtherButton != nullptr) {
        m_chooseOtherButton->setText(
            trKey("app_chooser_choose_other",
                  QStringLiteral("Escolher outro executável...")));
    }
}

}  // namespace source::ui
