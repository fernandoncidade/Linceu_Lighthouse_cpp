#include "secui_02_MultiDirectoryDialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QVariantMap>
#include <QVBoxLayout>

#include "secui_00_helpers.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {
namespace {

QString default_profile_name(const QVariantMap& summary) {
    return summary.value(QStringLiteral("monitor_profile_name")).toString();
}

QString primary_directory(const QVariantMap& summary) {
    return summary.value(QStringLiteral("primary_directory")).toString();
}

QStringList monitored_directories(const QVariantMap& summary) {
    return summary.value(QStringLiteral("monitor_diretorios_list")).toStringList();
}

}  // namespace

MultiDirectoryDialog::MultiDirectoryDialog(QWidget* parent, QObject* interface_obj)
    : QDialog(parent),
      interface_(interface_obj),
      loc_(get_localizador(interface_obj)),
      summary_label_(nullptr),
      profile_label_(nullptr),
      directories_label_(nullptr),
      profile_name_edit_(nullptr),
      directories_list_(nullptr),
      add_button_(nullptr),
      remove_button_(nullptr),
      save_button_(nullptr),
      refresh_button_(nullptr),
      close_button_(nullptr) {
    setModal(false);
    setMinimumSize(640, 420);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    summary_label_ = new QLabel(this);
    summary_label_->setWordWrap(true);
    layout->addWidget(summary_label_);

    profile_label_ = new QLabel(this);
    layout->addWidget(profile_label_);

    profile_name_edit_ = new QLineEdit(this);
    layout->addWidget(profile_name_edit_);

    directories_label_ = new QLabel(this);
    layout->addWidget(directories_label_);

    directories_list_ = new QListWidget(this);
    directories_list_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(directories_list_, 1);

    auto* actions_layout = new QHBoxLayout();
    add_button_ = new QPushButton(this);
    remove_button_ = new QPushButton(this);
    save_button_ = new QPushButton(this);
    refresh_button_ = new QPushButton(this);
    close_button_ = new QPushButton(this);

    actions_layout->addWidget(add_button_);
    actions_layout->addWidget(remove_button_);
    actions_layout->addStretch(1);
    actions_layout->addWidget(save_button_);
    actions_layout->addWidget(refresh_button_);
    actions_layout->addWidget(close_button_);
    layout->addLayout(actions_layout);

    QObject::connect(add_button_, &QPushButton::clicked, this, &MultiDirectoryDialog::add_directory);
    QObject::connect(remove_button_, &QPushButton::clicked, this, &MultiDirectoryDialog::remove_selected_directory);
    QObject::connect(save_button_, &QPushButton::clicked, this, &MultiDirectoryDialog::save_profile_name);
    QObject::connect(refresh_button_, &QPushButton::clicked, this, &MultiDirectoryDialog::refresh_data);
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

void MultiDirectoryDialog::refresh_texts() {
    setWindowTitle(get_text(loc_, QStringLiteral("security_manage_directories"),
                            QStringLiteral("Perfis e multi-diretório")));
    profile_label_->setText(
        get_text(loc_, QStringLiteral("security_profile_name"), QStringLiteral("Nome do perfil")));
    directories_label_->setText(
        get_text(loc_, QStringLiteral("security_monitored_directories"),
                 QStringLiteral("Diretórios monitorados")));
    add_button_->setText(
        get_text(loc_, QStringLiteral("security_add_directory"), QStringLiteral("Adicionar diretório")));
    remove_button_->setText(
        get_text(loc_, QStringLiteral("security_remove_selected"), QStringLiteral("Remover selecionado")));
    save_button_->setText(
        get_text(loc_, QStringLiteral("security_save_profile"), QStringLiteral("Salvar perfil")));
    refresh_button_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")));
    close_button_->setText(
        get_text(loc_, QStringLiteral("security_close"), QStringLiteral("Fechar")));
}

void MultiDirectoryDialog::refresh_data() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        summary_label_->setText(
            get_text(loc_, QStringLiteral("security_no_security_manager"),
                     QStringLiteral("Camada de segurança não disponível.")));
        directories_list_->clear();
        profile_name_edit_->clear();
        return;
    }

    const QVariantMap summary = security_manager->profile_summary();
    const QString profile_name = default_profile_name(summary);
    const QString primary = primary_directory(summary);
    const QStringList directories = monitored_directories(summary);

    profile_name_edit_->setText(profile_name);
    directories_list_->clear();
    for (const QString& directory : directories) {
        directories_list_->addItem(directory);
    }

    const QString summary_text =
        QStringLiteral("%1: %2\n%3: %4\n%5: %6")
            .arg(get_text(loc_, QStringLiteral("security_profile_name"), QStringLiteral("Nome do perfil")),
                 profile_name.isEmpty()
                     ? get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum"))
                     : profile_name,
                 get_text(loc_, QStringLiteral("security_primary_directory"),
                          QStringLiteral("Diretório principal")),
                 primary.isEmpty()
                     ? get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum"))
                     : primary,
                 get_text(loc_, QStringLiteral("security_total_directories"),
                          QStringLiteral("Total de diretórios")),
                 QString::number(directories.size()));
    summary_label_->setText(summary_text);
}

void MultiDirectoryDialog::add_directory() {
    if (interface_ == nullptr) {
        return;
    }

    const QString selected = QFileDialog::getExistingDirectory(
        this,
        get_text(loc_, QStringLiteral("security_select_directory"), QStringLiteral("Selecionar diretório")));
    if (selected.isEmpty()) {
        return;
    }

    bool added = false;
    QMetaObject::invokeMethod(interface_,
                              "adicionar_diretorio_monitorado",
                              Q_RETURN_ARG(bool, added),
                              Q_ARG(QString, selected));
    if (!added) {
        QMessageBox::warning(
            this,
            get_text(loc_, QStringLiteral("security_operation_failed"), QStringLiteral("Operação não concluída")),
            get_text(loc_, QStringLiteral("security_operation_failed"),
                     QStringLiteral("Operação não concluída.")));
        return;
    }

    show_translated_security_feedback(interface_,
                                      QStringLiteral("security_directory_added"),
                                      QStringLiteral("Diretório adicionado ao monitoramento."));
    show_translated_status_message(interface_,
                                   QStringLiteral("security_directory_added"),
                                   QStringLiteral("Diretório adicionado ao monitoramento."),
                                   10000);
    refresh_data();
}

void MultiDirectoryDialog::remove_selected_directory() {
    const int row = selected_directory_row();
    if (row < 0) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado")),
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado.")));
        return;
    }

    QListWidgetItem* item = directories_list_->item(row);
    if (item == nullptr || interface_ == nullptr) {
        return;
    }

    bool removed = false;
    QMetaObject::invokeMethod(interface_,
                              "remover_diretorio_monitorado",
                              Q_RETURN_ARG(bool, removed),
                              Q_ARG(QString, item->text()));
    if (!removed) {
        QMessageBox::warning(
            this,
            get_text(loc_, QStringLiteral("security_operation_failed"), QStringLiteral("Operação não concluída")),
            get_text(loc_, QStringLiteral("security_operation_failed"),
                     QStringLiteral("Operação não concluída.")));
        return;
    }

    show_translated_security_feedback(interface_,
                                      QStringLiteral("security_directory_removed"),
                                      QStringLiteral("Diretório removido do monitoramento."));
    show_translated_status_message(interface_,
                                   QStringLiteral("security_directory_removed"),
                                   QStringLiteral("Diretório removido do monitoramento."),
                                   10000);
    refresh_data();
}

void MultiDirectoryDialog::save_profile_name() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        return;
    }

    const bool changed = security_manager->set_profile_name(profile_name_edit_->text());
    if (!changed) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_operation_completed"), QStringLiteral("Nenhuma alteração")),
            get_text(loc_, QStringLiteral("security_operation_completed"),
                     QStringLiteral("Nenhuma alteração foi aplicada.")));
        return;
    }

    show_translated_security_feedback(interface_,
                                      QStringLiteral("security_profile_saved"),
                                      QStringLiteral("Perfil de monitoramento atualizado."));
    show_translated_status_message(interface_,
                                   QStringLiteral("security_profile_saved"),
                                   QStringLiteral("Perfil de monitoramento atualizado."),
                                   10000);
    refresh_data();
}

int MultiDirectoryDialog::selected_directory_row() const {
    const QModelIndexList rows = directories_list_->selectionModel() != nullptr
                                     ? directories_list_->selectionModel()->selectedRows()
                                     : QModelIndexList();
    return rows.isEmpty() ? -1 : rows.first().row();
}

}  // namespace source::security::ui
