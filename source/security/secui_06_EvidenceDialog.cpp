#include "secui_06_EvidenceDialog.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>

#include "secui_00_helpers.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {

EvidenceDialog::EvidenceDialog(QWidget* parent, QObject* interface_obj)
    : QDialog(parent),
      interface_(interface_obj),
      loc_(get_localizador(interface_obj)),
      summary_label_(nullptr),
      destination_label_(nullptr),
      destination_edit_(nullptr),
      result_browser_(nullptr),
      browse_button_(nullptr),
      export_button_(nullptr),
      open_folder_button_(nullptr),
      refresh_button_(nullptr),
      close_button_(nullptr) {
    setModal(false);
    setMinimumSize(700, 360);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    summary_label_ = new QLabel(this);
    summary_label_->setWordWrap(true);
    layout->addWidget(summary_label_);

    destination_label_ = new QLabel(this);
    layout->addWidget(destination_label_);

    auto* destination_layout = new QHBoxLayout();
    destination_edit_ = new QLineEdit(this);
    browse_button_ = new QPushButton(this);
    destination_layout->addWidget(destination_edit_, 1);
    destination_layout->addWidget(browse_button_);
    layout->addLayout(destination_layout);

    result_browser_ = new QTextBrowser(this);
    result_browser_->setOpenExternalLinks(true);
    layout->addWidget(result_browser_, 1);

    auto* buttons_layout = new QHBoxLayout();
    export_button_ = new QPushButton(this);
    open_folder_button_ = new QPushButton(this);
    refresh_button_ = new QPushButton(this);
    close_button_ = new QPushButton(this);
    buttons_layout->addStretch(1);
    buttons_layout->addWidget(export_button_);
    buttons_layout->addWidget(open_folder_button_);
    buttons_layout->addWidget(refresh_button_);
    buttons_layout->addWidget(close_button_);
    layout->addLayout(buttons_layout);

    QObject::connect(browse_button_, &QPushButton::clicked, this, &EvidenceDialog::browse_destination);
    QObject::connect(export_button_, &QPushButton::clicked, this, &EvidenceDialog::export_package);
    QObject::connect(open_folder_button_, &QPushButton::clicked, this, &EvidenceDialog::open_last_package_folder);
    QObject::connect(refresh_button_, &QPushButton::clicked, this, &EvidenceDialog::refresh_data);
    QObject::connect(close_button_, &QPushButton::clicked, this, &QDialog::close);

    if (auto* localizador = get_localizador(interface_)) {
        QObject::connect(localizador, &source::ui::LocalizadorQt::idioma_alterado, this,
                         [this](const QString&) {
                             refresh_texts();
                             refresh_data();
                         });
    }

    refresh_texts();
    refresh_data();
}

void EvidenceDialog::refresh_texts() {
    setWindowTitle(get_text(loc_, QStringLiteral("security_evidence_center"),
                            QStringLiteral("Pacote de evidência")));
    destination_label_->setText(
        get_text(loc_, QStringLiteral("security_destination"), QStringLiteral("Destino do pacote")));
    browse_button_->setText(
        get_text(loc_, QStringLiteral("security_select_destination"), QStringLiteral("Selecionar destino")));
    export_button_->setText(
        get_text(loc_, QStringLiteral("security_export_now"), QStringLiteral("Exportar agora")));
    open_folder_button_->setText(
        get_text(loc_, QStringLiteral("security_open_selected_folder"),
                 QStringLiteral("Abrir pasta selecionada")));
    refresh_button_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")));
    close_button_->setText(
        get_text(loc_, QStringLiteral("security_close"), QStringLiteral("Fechar")));
}

void EvidenceDialog::refresh_data() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        summary_label_->setText(
            get_text(loc_, QStringLiteral("security_no_security_manager"),
                     QStringLiteral("Camada de segurança não disponível.")));
        result_browser_->setPlainText(QString());
        return;
    }

    const QVariantMap summary = security_manager->profile_summary();
    const QVariantList evidence_rows = security_manager->list_evidence_entries(20);
    const QString profile_name = summary.value(QStringLiteral("monitor_profile_name")).toString();
    const int directory_count = summary.value(QStringLiteral("monitor_diretorios_count")).toInt();
    const QString backend = interface_ != nullptr
                                ? interface_->property("security_storage_backend").toString()
                                : QString();
    const QString target = interface_ != nullptr
                               ? interface_->property("security_storage_target").toString()
                               : QString();

    summary_label_->setText(
        QStringLiteral("%1: %2 | %3: %4 | Backend: %5")
            .arg(get_text(loc_, QStringLiteral("security_profile_name"), QStringLiteral("Nome do perfil")),
                 profile_name.isEmpty()
                     ? get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum"))
                     : profile_name,
                 get_text(loc_, QStringLiteral("security_total_directories"),
                          QStringLiteral("Total de diretórios")),
                 QString::number(directory_count),
                 backend.isEmpty() ? QStringLiteral("sqlite") : backend));

    QStringList lines;
    lines << QStringLiteral("Backend ativo: %1").arg(backend.isEmpty() ? QStringLiteral("sqlite") : backend);
    if (!target.isEmpty()) {
        lines << QStringLiteral("Destino: %1").arg(target);
    }
    lines << QStringLiteral("%1: %2")
                 .arg(get_text(loc_, QStringLiteral("security_evidence_center"),
                               QStringLiteral("Pacote de evidência")),
                      QString::number(evidence_rows.size()));

    if (!last_package_path_.isEmpty()) {
        lines << QStringLiteral("%1: %2")
                     .arg(get_text(loc_, QStringLiteral("security_last_package"),
                                   QStringLiteral("Último pacote gerado")),
                          last_package_path_);
    }

    if (evidence_rows.isEmpty()) {
        lines << get_text(loc_, QStringLiteral("security_last_package"),
                          QStringLiteral("Nenhuma evidência registrada nesta sessão."));
    } else {
        lines << QString();
        lines << QStringLiteral("Registros recentes:");
        for (const QVariant& row_value : evidence_rows) {
            const QVariantMap row = row_value.toMap();
            lines << QStringLiteral("[%1] %2 | %3 | %4")
                         .arg(row.value(QStringLiteral("created_at")).toString(),
                              row.value(QStringLiteral("evidence_id")).toString(),
                              row.value(QStringLiteral("nome")).toString(),
                              row.value(QStringLiteral("risk_level")).toString());
        }
    }

    result_browser_->setPlainText(lines.join(QStringLiteral("\n")));
}

void EvidenceDialog::browse_destination() {
    const QString selected = QFileDialog::getExistingDirectory(
        this,
        get_text(loc_, QStringLiteral("security_select_destination"),
                 QStringLiteral("Selecionar destino")));
    if (!selected.isEmpty()) {
        destination_edit_->setText(selected);
    }
}

void EvidenceDialog::export_package() {
    if (interface_ == nullptr) {
        return;
    }

    QString package_path;
    QMetaObject::invokeMethod(interface_,
                              "exportar_pacote_evidencia",
                              Q_RETURN_ARG(QString, package_path),
                              Q_ARG(QString, destination_edit_->text()));
    if (package_path.isEmpty()) {
        const QString error_text =
            get_text(loc_, QStringLiteral("security_operation_failed"),
                     QStringLiteral("Operação não concluída."));
        result_browser_->setPlainText(error_text);
        set_security_result_label(interface_, error_text);
        show_status_message(interface_, error_text);
        return;
    }

    last_package_path_ = package_path;
    const QString success = get_text(loc_, QStringLiteral("security_export_success"),
                                     QStringLiteral("Pacote de evidência exportado com sucesso."));
    result_browser_->setPlainText(QStringLiteral("%1\n%2").arg(success, package_path));
    set_security_result_label(interface_, success);
    show_status_message(interface_, success);
    refresh_data();
}

void EvidenceDialog::open_last_package_folder() {
    if (last_package_path_.isEmpty()) {
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(last_package_path_));
}

}  // namespace source::security::ui
