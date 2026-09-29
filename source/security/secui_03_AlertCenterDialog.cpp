#include "secui_03_AlertCenterDialog.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

#include "secui_00_helpers.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secore_04_traduzir_strings_ui.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::security::ui {
namespace {

QTableWidgetItem* make_item(const QString& text) {
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

}  // namespace

AlertCenterDialog::AlertCenterDialog(QWidget* parent, QObject* interface_obj)
    : QDialog(parent),
      interface_(interface_obj),
      loc_(get_localizador(interface_obj)),
      summary_label_(nullptr),
      table_(nullptr),
      refresh_button_(nullptr),
      close_button_(nullptr),
      refresh_pending_(false),
      initialized_(false),
      refresh_inflight_(false),
      refresh_request_id_(0),
      last_alert_id_(0) {
    setModal(false);
    setMinimumSize(880, 420);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    summary_label_ = new QLabel(this);
    summary_label_->setWordWrap(true);
    layout->addWidget(summary_label_);

    table_ = new QTableWidget(this);
    table_->setColumnCount(8);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setAlternatingRowColors(true);
    table_->setWordWrap(false);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table_->setColumnWidth(0, 150);
    table_->setColumnWidth(1, 95);
    table_->setColumnWidth(2, 180);
    table_->setColumnWidth(3, 140);
    table_->setColumnWidth(4, 180);
    table_->setColumnWidth(5, 120);
    table_->setColumnWidth(6, 90);
    table_->setColumnWidth(7, 90);
    layout->addWidget(table_, 1);

    auto* buttons_layout = new QHBoxLayout();
    refresh_button_ = new QPushButton(this);
    close_button_ = new QPushButton(this);
    buttons_layout->addStretch(1);
    buttons_layout->addWidget(refresh_button_);
    buttons_layout->addWidget(close_button_);
    layout->addLayout(buttons_layout);

    QObject::connect(refresh_button_, &QPushButton::clicked, this, &AlertCenterDialog::refresh_data);
    QObject::connect(close_button_, &QPushButton::clicked, this, &QDialog::close);

    if (auto* localizador = get_localizador(interface_)) {
        QObject::connect(localizador, &source::ui::LocalizadorQt::idioma_alterado, this,
                          [this](const QString&) {
                              refresh_texts();
                              initialized_ = false;
                              last_alert_id_ = 0;
                              refresh_data();
                          });
    }

    refresh_texts();
    refresh_data();
}

void AlertCenterDialog::refresh_texts() {
    setWindowTitle(
        get_text(loc_, QStringLiteral("security_alert_center"), QStringLiteral("Central de alertas")));
    refresh_button_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")));
    close_button_->setText(
        get_text(loc_, QStringLiteral("security_close"), QStringLiteral("Fechar")));

    QStringList headers;
    headers << get_text(loc_, QStringLiteral("security_created_at"), QStringLiteral("Criado em"))
            << get_text(loc_, QStringLiteral("security_risk_level"), QStringLiteral("Nível de risco"))
            << get_text(loc_, QStringLiteral("security_alert_summary"), QStringLiteral("Resumo do alerta"))
            << get_text(loc_, QStringLiteral("security_rules"), QStringLiteral("Regras"))
            << get_text(loc_, QStringLiteral("security_path"), QStringLiteral("Caminho"))
            << get_text(loc_, QStringLiteral("security_name"), QStringLiteral("Nome"))
            << get_text(loc_, QStringLiteral("security_session_label"), QStringLiteral("Sessão"))
            << get_text(loc_, QStringLiteral("security_evidence_id"), QStringLiteral("Evidência"));
    table_->setHorizontalHeaderLabels(headers);
}

void AlertCenterDialog::refresh_data() {
    auto* security_manager = get_security_manager(interface_);
    if (security_manager == nullptr) {
        summary_label_->setText(
            get_text(loc_, QStringLiteral("security_no_security_manager"),
                     QStringLiteral("Camada de segurança não disponível.")));
        table_->setRowCount(0);
        return;
    }

    if (refresh_inflight_) {
        refresh_pending_ = true;
        return;
    }

    const bool replace_all = !initialized_;
    const qint64 last_id = last_alert_id_;
    const quint64 request_id = ++refresh_request_id_;
    refresh_inflight_ = true;
    refresh_button_->setEnabled(false);

    summary_label_->setText(
        get_text(loc_, replace_all ? QStringLiteral("loading") : QStringLiteral("security_refresh"),
                 replace_all ? QStringLiteral("Carregando") : QStringLiteral("Atualizar")) +
        QStringLiteral("..."));

    QPointer<AlertCenterDialog> self_guard(this);
    run_on_security_runtime_thread(interface_, [self_guard, security_manager, replace_all, last_id, request_id]() {
        const QVariantList rows = replace_all ? security_manager->list_alerts(300)
                                              : security_manager->list_alerts_since(last_id, 120);
        if (self_guard.isNull()) {
            return;
        }

        QMetaObject::invokeMethod(
            self_guard.data(),
            [self_guard, rows, replace_all, request_id]() {
                if (self_guard.isNull()) {
                    return;
                }
                self_guard->finish_refresh(rows, replace_all, request_id);
            },
            Qt::QueuedConnection);
    });
}

void AlertCenterDialog::finish_refresh(const QVariantList& rows, bool replace_all, quint64 request_id) {
    if (request_id != refresh_request_id_) {
        return;
    }

    refresh_inflight_ = false;
    apply_rows(rows, replace_all);
    initialized_ = true;
    refresh_button_->setEnabled(true);

    if (refresh_pending_) {
        refresh_pending_ = false;
        QTimer::singleShot(0, this, [this]() { refresh_data(); });
    }
}

void AlertCenterDialog::apply_rows(const QVariantList& rows, bool replace_all) {
    auto* localizador = get_localizador(interface_);
    const QString backend = interface_ != nullptr
                                ? interface_->property("security_storage_backend").toString()
                                : QString();
    const QSignalBlocker blocker(table_);
    table_->setUpdatesEnabled(false);

    if (replace_all) {
        table_->clearContents();
        table_->setRowCount(rows.size());
    }

    for (int row_index = 0; row_index < rows.size(); ++row_index) {
        const QVariantMap row = rows.at(row_index).toMap();
        const QString risk_level = source::security::traduzir_valor_ui(
            localizador,
            QStringLiteral("risk_level"),
            row.value(QStringLiteral("risk_level")).toString());
        const QString summary = source::security::traduzir_valor_ui(
            localizador,
            QStringLiteral("alerta_resumo"),
            row.value(QStringLiteral("summary")).toString());
        const QString rules = source::security::traduzir_valor_ui(
            localizador,
            QStringLiteral("risk_reasons"),
            row.value(QStringLiteral("rules")).toString());

        int target_row = row_index;
        if (!replace_all) {
            target_row = 0;
            table_->insertRow(target_row);
        }

        table_->setItem(target_row, 0, make_item(row.value(QStringLiteral("created_at")).toString()));
        table_->setItem(target_row, 1, make_item(risk_level));
        table_->setItem(target_row, 2, make_item(summary));
        table_->setItem(target_row, 3, make_item(rules));
        table_->setItem(target_row, 4, make_item(row.value(QStringLiteral("caminho")).toString()));
        table_->setItem(target_row, 5, make_item(row.value(QStringLiteral("nome")).toString()));
        table_->setItem(target_row, 6, make_item(row.value(QStringLiteral("session_id")).toString()));
        table_->setItem(target_row, 7, make_item(row.value(QStringLiteral("evidence_id")).toString()));
        last_alert_id_ = qMax(last_alert_id_, row.value(QStringLiteral("id")).toLongLong());
    }

    table_->setUpdatesEnabled(true);
    table_->viewport()->update();

    summary_label_->setText(
        QStringLiteral("%1: %2 | Backend: %3")
            .arg(get_text(loc_, QStringLiteral("security_alert_center"), QStringLiteral("Central de alertas")),
                 QString::number(table_->rowCount()),
                 backend.isEmpty() ? QStringLiteral("sqlite") : backend));
}

}  // namespace source::security::ui
