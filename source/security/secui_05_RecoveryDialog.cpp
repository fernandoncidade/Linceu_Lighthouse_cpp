#include "secui_05_RecoveryDialog.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QtConcurrent/QtConcurrentRun>

#include "secui_00_helpers.h"
#include "source/security/secore_01_postgres_utils.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {
namespace {

QTableWidget* create_table(QWidget* parent, int column_count) {
    auto* table = new QTableWidget(parent);
    table->setColumnCount(column_count);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setWordWrap(false);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    return table;
}

QTableWidgetItem* make_item(const QString& text) {
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

void open_folder_of(const QString& path) {
    if (path.isEmpty()) {
        return;
    }
    if (path.contains(QStringLiteral("://"))) {
        return;
    }
    const QString target = QFileInfo(path).isDir() ? path : QFileInfo(path).absolutePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(target));
}

QVariantList query_rows(const QString& db_path, const QString& sql, const QVariantList& binds) {
    Q_UNUSED(db_path)

    QVariantList rows;
    QSqlDatabase db = source::security::pg::open_database(
        QStringLiteral("security_recovery_dialog_worker"));
    if (!db.isValid() || !db.isOpen()) {
        return rows;
    }

    QSqlQuery query(db);
    query.prepare(sql);
    for (const QVariant& bind : binds) {
        query.addBindValue(bind);
    }
    if (query.exec()) {
        const QSqlRecord record = query.record();
        while (query.next()) {
            QVariantMap row;
            for (int i = 0; i < record.count(); ++i) {
                row.insert(record.fieldName(i), query.value(i));
            }
            rows.append(row);
        }
    }
    return rows;
}

QVariantList fetch_quarantine_delta(const QString& db_path, qint64 last_id, int limit) {
    return query_rows(
        db_path,
        QStringLiteral("SELECT id, created_at, evidence_id, original_path, quarantine_path, sha256, status "
                       "FROM security_quarantine WHERE id > ? ORDER BY id ASC LIMIT ?"),
        {last_id, qMax(1, limit)});
}

QVariantList fetch_baseline_delta(const QString& db_path,
                                  const QString& profile_id,
                                  const QString& last_updated_at,
                                  int limit) {
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        return query_rows(
            db_path,
            QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, "
                           "signature_state, updated_at "
                           "FROM security_baseline WHERE profile_id = ? AND updated_at > ? "
                           "ORDER BY updated_at ASC LIMIT ?"),
            {profile_id, last_updated_at, safe_limit});
    }

    return query_rows(
        db_path,
        QStringLiteral("SELECT profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, "
                       "signature_state, updated_at "
                       "FROM security_baseline WHERE updated_at > ? ORDER BY updated_at ASC LIMIT ?"),
        {last_updated_at, safe_limit});
}

int find_row_by_text(QTableWidget* table, int column, const QString& value) {
    if (table == nullptr) {
        return -1;
    }

    for (int row = 0; row < table->rowCount(); ++row) {
        const QTableWidgetItem* item = table->item(row, column);
        if (item != nullptr && item->text() == value) {
            return row;
        }
    }
    return -1;
}

}  // namespace

RecoveryDialog::RecoveryDialog(QWidget* parent, QObject* interface_obj)
    : QDialog(parent),
      interface_(interface_obj),
      loc_(get_localizador(interface_obj)),
      summary_label_(nullptr),
      tabs_(nullptr),
      quarantine_table_(nullptr),
      baseline_table_(nullptr),
      quarantine_open_button_(nullptr),
      quarantine_restore_button_(nullptr),
      baseline_open_button_(nullptr),
      baseline_restore_button_(nullptr),
      refresh_button_(nullptr),
      close_button_(nullptr),
      refresh_watcher_(new QFutureWatcher<QPair<QVariantList, QVariantList>>(this)),
      refresh_pending_(false),
      initialized_(false),
      last_quarantine_id_(0) {
    setModal(false);
    setMinimumSize(980, 520);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    summary_label_ = new QLabel(this);
    summary_label_->setWordWrap(true);
    layout->addWidget(summary_label_);

    tabs_ = new QTabWidget(this);
    layout->addWidget(tabs_, 1);

    auto* quarantine_tab = new QWidget(this);
    auto* quarantine_layout = new QVBoxLayout(quarantine_tab);
    quarantine_table_ = create_table(quarantine_tab, 6);
    quarantine_layout->addWidget(quarantine_table_, 1);
    auto* quarantine_buttons = new QHBoxLayout();
    quarantine_open_button_ = new QPushButton(quarantine_tab);
    quarantine_restore_button_ = new QPushButton(quarantine_tab);
    quarantine_buttons->addStretch(1);
    quarantine_buttons->addWidget(quarantine_open_button_);
    quarantine_buttons->addWidget(quarantine_restore_button_);
    quarantine_layout->addLayout(quarantine_buttons);
    tabs_->addTab(quarantine_tab, QString());

    auto* baseline_tab = new QWidget(this);
    auto* baseline_layout = new QVBoxLayout(baseline_tab);
    baseline_table_ = create_table(baseline_tab, 6);
    baseline_layout->addWidget(baseline_table_, 1);
    auto* baseline_buttons = new QHBoxLayout();
    baseline_open_button_ = new QPushButton(baseline_tab);
    baseline_restore_button_ = new QPushButton(baseline_tab);
    baseline_buttons->addStretch(1);
    baseline_buttons->addWidget(baseline_open_button_);
    baseline_buttons->addWidget(baseline_restore_button_);
    baseline_layout->addLayout(baseline_buttons);
    tabs_->addTab(baseline_tab, QString());

    auto* footer_buttons = new QHBoxLayout();
    refresh_button_ = new QPushButton(this);
    close_button_ = new QPushButton(this);
    footer_buttons->addStretch(1);
    footer_buttons->addWidget(refresh_button_);
    footer_buttons->addWidget(close_button_);
    layout->addLayout(footer_buttons);

    QObject::connect(quarantine_open_button_, &QPushButton::clicked, this,
                     &RecoveryDialog::open_selected_quarantine_folder);
    QObject::connect(quarantine_restore_button_, &QPushButton::clicked, this,
                     &RecoveryDialog::restore_selected_quarantine_copy);
    QObject::connect(baseline_open_button_, &QPushButton::clicked, this,
                     &RecoveryDialog::open_selected_baseline_folder);
    QObject::connect(baseline_restore_button_, &QPushButton::clicked, this,
                     &RecoveryDialog::restore_selected_baseline_version);
    QObject::connect(refresh_button_, &QPushButton::clicked, this, &RecoveryDialog::refresh_data);
    QObject::connect(close_button_, &QPushButton::clicked, this, &QDialog::close);
    QObject::connect(refresh_watcher_,
                     &QFutureWatcher<QPair<QVariantList, QVariantList>>::finished,
                     this,
                     [this]() {
                         const QPair<QVariantList, QVariantList> result = refresh_watcher_->result();
                         apply_quarantine_rows(result.first, false);
                         apply_baseline_rows(result.second, false);
                         refresh_button_->setEnabled(true);

                         if (refresh_pending_) {
                             refresh_pending_ = false;
                             QTimer::singleShot(0, this, [this]() { refresh_data(); });
                         }
                     });

    if (auto* localizador = get_localizador(interface_)) {
        QObject::connect(localizador, &source::ui::LocalizadorQt::idioma_alterado, this,
                         [this](const QString&) {
                             refresh_texts();
                             initialized_ = false;
                             last_quarantine_id_ = 0;
                             last_baseline_updated_at_.clear();
                             refresh_data();
                         });
    }

    refresh_texts();
    refresh_data();
}

void RecoveryDialog::refresh_texts() {
    setWindowTitle(get_text(loc_, QStringLiteral("security_recovery_center"),
                            QStringLiteral("Quarentena e restauração")));
    tabs_->setTabText(0,
                      get_text(loc_, QStringLiteral("security_quarantine_status"),
                               QStringLiteral("Quarentena")));
    tabs_->setTabText(1,
                      get_text(loc_, QStringLiteral("security_baseline_status"),
                               QStringLiteral("Versões monitoradas")));

    quarantine_open_button_->setText(
        get_text(loc_, QStringLiteral("security_open_selected_folder"),
                 QStringLiteral("Abrir pasta selecionada")));
    quarantine_restore_button_->setText(
        get_text(loc_, QStringLiteral("security_restore_selected_copy"),
                 QStringLiteral("Restaurar cópia selecionada")));
    baseline_open_button_->setText(
        get_text(loc_, QStringLiteral("security_open_selected_folder"),
                 QStringLiteral("Abrir pasta selecionada")));
    baseline_restore_button_->setText(
        get_text(loc_, QStringLiteral("security_restore_selected_version"),
                 QStringLiteral("Restaurar versão monitorada")));
    refresh_button_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")));
    close_button_->setText(
        get_text(loc_, QStringLiteral("security_close"), QStringLiteral("Fechar")));

    QStringList quarantine_headers;
    quarantine_headers << get_text(loc_, QStringLiteral("security_created_at"), QStringLiteral("Criado em"))
                       << get_text(loc_, QStringLiteral("security_quarantine_status"),
                                   QStringLiteral("Status da quarentena"))
                       << get_text(loc_, QStringLiteral("security_path"), QStringLiteral("Caminho"))
                       << get_text(loc_, QStringLiteral("security_snapshot_path"),
                                   QStringLiteral("Arquivo em quarentena"))
                       << get_text(loc_, QStringLiteral("security_sha256_current"), QStringLiteral("SHA-256"))
                       << get_text(loc_, QStringLiteral("security_evidence_id"), QStringLiteral("Evidência"));
    quarantine_table_->setHorizontalHeaderLabels(quarantine_headers);

    QStringList baseline_headers;
    baseline_headers << get_text(loc_, QStringLiteral("security_updated_at"), QStringLiteral("Atualizado em"))
                     << get_text(loc_, QStringLiteral("security_path"), QStringLiteral("Caminho"))
                     << get_text(loc_, QStringLiteral("security_sha256_current"), QStringLiteral("SHA-256"))
                     << get_text(loc_, QStringLiteral("security_publisher_name"), QStringLiteral("Publicador"))
                     << get_text(loc_, QStringLiteral("signed"), QStringLiteral("Assinado"))
                     << get_text(loc_, QStringLiteral("security_snapshot_path"),
                                 QStringLiteral("Snapshot"));
    baseline_table_->setHorizontalHeaderLabels(baseline_headers);
}

void RecoveryDialog::refresh_data() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        summary_label_->setText(
            get_text(loc_, QStringLiteral("security_no_security_manager"),
                     QStringLiteral("Camada de segurança não disponível.")));
        quarantine_table_->setRowCount(0);
        baseline_table_->setRowCount(0);
        return;
    }

    if (!initialized_) {
        apply_quarantine_rows(security_manager->list_quarantine_items(300), true);
        apply_baseline_rows(security_manager->list_baseline_entries(300), true);
        initialized_ = true;
        refresh_button_->setEnabled(true);
        return;
    }

    if (refresh_watcher_ != nullptr && refresh_watcher_->isRunning()) {
        refresh_pending_ = true;
        return;
    }

    const QString db_path = security_manager->db_path();
    const QString profile_id =
        security_manager->profile_summary().value(QStringLiteral("monitor_profile_id")).toString();
    const qint64 last_quarantine_id = last_quarantine_id_;
    const QString last_baseline_updated_at = last_baseline_updated_at_;

    refresh_button_->setEnabled(false);
    summary_label_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")) + QStringLiteral("..."));
    refresh_watcher_->setFuture(QtConcurrent::run([db_path, profile_id, last_quarantine_id, last_baseline_updated_at]() {
        return qMakePair(fetch_quarantine_delta(db_path, last_quarantine_id, 120),
                         fetch_baseline_delta(db_path, profile_id, last_baseline_updated_at, 120));
    }));
}

void RecoveryDialog::open_selected_quarantine_folder() {
    const int row = selected_row(quarantine_table_);
    if (row < 0) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado")),
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado.")));
        return;
    }
    open_folder_of(quarantine_table_->item(row, 2) != nullptr ? quarantine_table_->item(row, 2)->text()
                                                               : QString());
}

void RecoveryDialog::restore_selected_quarantine_copy() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        return;
    }

    const int row = selected_row(quarantine_table_);
    if (row < 0) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado")),
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado.")));
        return;
    }

    const qint64 quarantine_id = quarantine_table_->item(row, 0) != nullptr
                                     ? quarantine_table_->item(row, 0)->data(Qt::UserRole).toLongLong()
                                     : 0;
    const QString original_path =
        quarantine_table_->item(row, 2) != nullptr ? quarantine_table_->item(row, 2)->text() : QString();
    const QString destination = QFileDialog::getSaveFileName(
        this,
        get_text(loc_, QStringLiteral("security_select_destination"),
                 QStringLiteral("Selecionar destino")),
        original_path);
    if (destination.isEmpty()) {
        return;
    }

    QString error_message;
    if (!security_manager->restore_quarantine_item(quarantine_id, destination, &error_message)) {
        QMessageBox::warning(
            this,
            get_text(loc_, QStringLiteral("security_operation_failed"), QStringLiteral("Operação não concluída")),
            error_message.isEmpty()
                ? get_text(loc_, QStringLiteral("security_operation_failed"),
                           QStringLiteral("Operação não concluída."))
                : error_message);
        return;
    }

    const QString success = get_text(loc_, QStringLiteral("security_restore_success"),
                                     QStringLiteral("Arquivo restaurado com sucesso."));
    set_security_result_label(interface_, success);
    show_status_message(interface_, success);
    QMessageBox::information(this,
                             get_text(loc_, QStringLiteral("security_operation_completed"),
                                      QStringLiteral("Operação concluída")),
                             success);
}

void RecoveryDialog::open_selected_baseline_folder() {
    const int row = selected_row(baseline_table_);
    if (row < 0) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado")),
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado.")));
        return;
    }
    open_folder_of(baseline_table_->item(row, 1) != nullptr ? baseline_table_->item(row, 1)->text() : QString());
}

void RecoveryDialog::restore_selected_baseline_version() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        return;
    }

    const int row = selected_row(baseline_table_);
    if (row < 0) {
        QMessageBox::information(
            this,
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado")),
            get_text(loc_, QStringLiteral("security_no_selection"), QStringLiteral("Nenhum item selecionado.")));
        return;
    }

    const QString original_path =
        baseline_table_->item(row, 1) != nullptr ? baseline_table_->item(row, 1)->text() : QString();
    const QString destination = QFileDialog::getSaveFileName(
        this,
        get_text(loc_, QStringLiteral("security_select_destination"),
                 QStringLiteral("Selecionar destino")),
        original_path);
    if (destination.isEmpty()) {
        return;
    }

    QString error_message;
    if (!security_manager->restore_monitored_version(original_path, destination, &error_message)) {
        QMessageBox::warning(
            this,
            get_text(loc_, QStringLiteral("security_operation_failed"), QStringLiteral("Operação não concluída")),
            error_message.isEmpty()
                ? get_text(loc_, QStringLiteral("security_operation_failed"),
                           QStringLiteral("Operação não concluída."))
                : error_message);
        return;
    }

    const QString success = get_text(loc_, QStringLiteral("security_restore_success"),
                                     QStringLiteral("Arquivo restaurado com sucesso."));
    set_security_result_label(interface_, success);
    show_status_message(interface_, success);
    QMessageBox::information(this,
                             get_text(loc_, QStringLiteral("security_operation_completed"),
                                      QStringLiteral("Operação concluída")),
                             success);
}

int RecoveryDialog::selected_row(QTableWidget* table) const {
    if (table == nullptr || table->selectionModel() == nullptr) {
        return -1;
    }
    const QModelIndexList rows = table->selectionModel()->selectedRows();
    return rows.isEmpty() ? -1 : rows.first().row();
}

void RecoveryDialog::apply_quarantine_rows(const QVariantList& rows, bool replace_all) {
    auto* localizador = get_localizador(interface_);
    const QSignalBlocker blocker(quarantine_table_);
    quarantine_table_->setUpdatesEnabled(false);
    if (replace_all) {
        quarantine_table_->clearContents();
        quarantine_table_->setRowCount(rows.size());
    }
    for (int row_index = 0; row_index < rows.size(); ++row_index) {
        const QVariantMap row = rows.at(row_index).toMap();
        QTableWidgetItem* created_item = make_item(row.value(QStringLiteral("created_at")).toString());
        created_item->setData(Qt::UserRole, row.value(QStringLiteral("id")).toLongLong());

        const QString status = source::security::traduzir_valor_ui(
            localizador,
            QStringLiteral("quarantine_status"),
            row.value(QStringLiteral("status")).toString());

        int target_row = row_index;
        if (!replace_all) {
            target_row = find_row_by_text(quarantine_table_, 3, row.value(QStringLiteral("quarantine_path")).toString());
            if (target_row < 0) {
                target_row = 0;
                quarantine_table_->insertRow(target_row);
            }
        }

        quarantine_table_->setItem(target_row, 0, created_item);
        quarantine_table_->setItem(target_row, 1, make_item(status));
        quarantine_table_->setItem(target_row, 2, make_item(row.value(QStringLiteral("original_path")).toString()));
        quarantine_table_->setItem(target_row, 3, make_item(row.value(QStringLiteral("quarantine_path")).toString()));
        quarantine_table_->setItem(target_row, 4, make_item(row.value(QStringLiteral("sha256")).toString()));
        quarantine_table_->setItem(target_row, 5, make_item(row.value(QStringLiteral("evidence_id")).toString()));
        last_quarantine_id_ = qMax(last_quarantine_id_, row.value(QStringLiteral("id")).toLongLong());
    }
    quarantine_table_->setUpdatesEnabled(true);
    quarantine_table_->resizeColumnsToContents();
    quarantine_table_->viewport()->update();
    update_summary();
}

void RecoveryDialog::apply_baseline_rows(const QVariantList& rows, bool replace_all) {
    const QSignalBlocker blocker(baseline_table_);
    baseline_table_->setUpdatesEnabled(false);
    if (replace_all) {
        baseline_table_->clearContents();
        baseline_table_->setRowCount(rows.size());
    }
    for (int row_index = 0; row_index < rows.size(); ++row_index) {
        const QVariantMap row = rows.at(row_index).toMap();
        int target_row = row_index;
        if (!replace_all) {
            target_row = find_row_by_text(baseline_table_, 1, row.value(QStringLiteral("caminho")).toString());
            if (target_row < 0) {
                target_row = 0;
                baseline_table_->insertRow(target_row);
            }
        }

        baseline_table_->setItem(target_row, 0, make_item(row.value(QStringLiteral("updated_at")).toString()));
        baseline_table_->setItem(target_row, 1, make_item(row.value(QStringLiteral("caminho")).toString()));
        baseline_table_->setItem(target_row, 2, make_item(row.value(QStringLiteral("sha256")).toString()));
        baseline_table_->setItem(target_row, 3, make_item(row.value(QStringLiteral("publisher")).toString()));
        baseline_table_->setItem(target_row, 4, make_item(row.value(QStringLiteral("signature_state")).toString()));
        baseline_table_->setItem(target_row, 5, make_item(row.value(QStringLiteral("snapshot_path")).toString()));
        const QString candidate_updated_at = row.value(QStringLiteral("updated_at")).toString();
        if (last_baseline_updated_at_ < candidate_updated_at) {
            last_baseline_updated_at_ = candidate_updated_at;
        }
    }
    baseline_table_->setUpdatesEnabled(true);
    baseline_table_->resizeColumnsToContents();
    baseline_table_->viewport()->update();
    update_summary();
}

void RecoveryDialog::update_summary() {
    const QString backend = interface_ != nullptr
                                ? interface_->property("security_storage_backend").toString()
                                : QString();
    summary_label_->setText(
        QStringLiteral("%1: %2 | %3: %4 | Backend: %5")
            .arg(get_text(loc_, QStringLiteral("security_quarantine_status"), QStringLiteral("Quarentena")),
                 QString::number(quarantine_table_->rowCount()),
                 get_text(loc_, QStringLiteral("security_baseline_status"),
                          QStringLiteral("Versões monitoradas")),
                 QString::number(baseline_table_->rowCount()),
                 backend.isEmpty() ? QStringLiteral("sqlite") : backend));
}

}  // namespace source::security::ui
