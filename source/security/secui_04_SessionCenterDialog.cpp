#include "secui_04_SessionCenterDialog.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

#include "secui_00_helpers.h"
#include "source/security/secore_01_postgres_utils.h"
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

QVariantList query_rows(const QString& db_path, const QString& sql, const QVariantList& binds) {
    Q_UNUSED(db_path)

    QVariantList rows;
    QSqlDatabase db = source::security::pg::open_database(
        QStringLiteral("security_sessions_dialog_worker"));
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

QVariantList fetch_sessions_delta(const QString& db_path,
                                  const QString& profile_id,
                                  const QString& last_updated_at,
                                  int limit) {
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        return query_rows(
            db_path,
            QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                           "FROM security_sessions WHERE profile_id = ? AND updated_at > ? "
                           "ORDER BY updated_at ASC LIMIT ?"),
            {profile_id, last_updated_at, safe_limit});
    }

    return query_rows(
        db_path,
        QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                       "FROM security_sessions WHERE updated_at > ? ORDER BY updated_at ASC LIMIT ?"),
        {last_updated_at, safe_limit});
}

QVariantList fetch_sessions_snapshot(const QString& db_path,
                                     const QString& profile_id,
                                     int limit) {
    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        QVariantList rows = query_rows(
            db_path,
            QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                           "FROM security_sessions WHERE profile_id = ? ORDER BY updated_at DESC LIMIT ?"),
            {profile_id, safe_limit});
        if (!rows.isEmpty()) {
            return rows;
        }

        return query_rows(
            db_path,
            QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                           "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                           "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(session_id, '') <> '' "
                           "GROUP BY session_id ORDER BY MAX(rowid) DESC LIMIT ?"),
            {profile_id, safe_limit});
    }

    QVariantList rows = query_rows(
        db_path,
        QStringLiteral("SELECT session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios "
                       "FROM security_sessions ORDER BY updated_at DESC LIMIT ?"),
        {safe_limit});
    if (!rows.isEmpty()) {
        return rows;
    }

    return query_rows(
        db_path,
        QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                       "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                       "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                       "FROM monitoramento WHERE COALESCE(session_id, '') <> '' "
                       "GROUP BY session_id ORDER BY MAX(rowid) DESC LIMIT ?"),
        {safe_limit});
}

QVariantList fetch_sessions_delta_with_fallback(const QString& db_path,
                                                const QString& profile_id,
                                                const QString& last_updated_at,
                                                int limit) {
    if (last_updated_at.isEmpty()) {
        return fetch_sessions_snapshot(db_path, profile_id, limit);
    }

    QVariantList rows = fetch_sessions_delta(db_path, profile_id, last_updated_at, limit);
    if (!rows.isEmpty()) {
        return rows;
    }

    const int safe_limit = qMax(1, limit);
    if (!profile_id.isEmpty()) {
        return query_rows(
            db_path,
            QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                           "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                           "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                           "FROM monitoramento WHERE monitor_profile_id = ? AND COALESCE(session_id, '') <> '' "
                           "GROUP BY session_id HAVING MAX(timestamp) > ? "
                           "ORDER BY MAX(timestamp) ASC LIMIT ?"),
            {profile_id, last_updated_at, safe_limit});
    }

    return query_rows(
        db_path,
        QStringLiteral("SELECT session_id, monitor_profile_id AS profile_id, "
                       "MAX(session_label) AS label, MIN(timestamp) AS started_at, MAX(timestamp) AS updated_at, "
                       "COUNT(*) AS total_eventos, GROUP_CONCAT(DISTINCT COALESCE(NULLIF(dir_atual, ''), dir_anterior)) AS diretorios "
                       "FROM monitoramento WHERE COALESCE(session_id, '') <> '' "
                       "GROUP BY session_id HAVING MAX(timestamp) > ? ORDER BY MAX(timestamp) ASC LIMIT ?"),
        {last_updated_at, safe_limit});
}

int find_row_by_session_id(QTableWidget* table, const QString& session_id) {
    if (table == nullptr) {
        return -1;
    }

    for (int row = 0; row < table->rowCount(); ++row) {
        const QTableWidgetItem* item = table->item(row, 4);
        if (item != nullptr && item->text() == session_id) {
            return row;
        }
    }
    return -1;
}

}  // namespace

SessionCenterDialog::SessionCenterDialog(QWidget* parent, QObject* interface_obj)
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
      refresh_request_id_(0) {
    setModal(false);
    setMinimumSize(860, 400);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    summary_label_ = new QLabel(this);
    summary_label_->setWordWrap(true);
    layout->addWidget(summary_label_);

    table_ = new QTableWidget(this);
    table_->setColumnCount(5);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setAlternatingRowColors(true);
    table_->setWordWrap(false);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    layout->addWidget(table_, 1);

    auto* buttons_layout = new QHBoxLayout();
    refresh_button_ = new QPushButton(this);
    close_button_ = new QPushButton(this);
    buttons_layout->addStretch(1);
    buttons_layout->addWidget(refresh_button_);
    buttons_layout->addWidget(close_button_);
    layout->addLayout(buttons_layout);

    QObject::connect(refresh_button_, &QPushButton::clicked, this, &SessionCenterDialog::refresh_data);
    QObject::connect(close_button_, &QPushButton::clicked, this, &QDialog::close);

    if (auto* localizador = get_localizador(interface_)) {
        QObject::connect(localizador, &source::ui::LocalizadorQt::idioma_alterado, this,
                         [this](const QString&) {
                             refresh_texts();
                             initialized_ = false;
                             last_updated_at_.clear();
                             refresh_data();
                         });
    }

    refresh_texts();
    refresh_data();
}

void SessionCenterDialog::refresh_texts() {
    setWindowTitle(
        get_text(loc_, QStringLiteral("security_session_center"), QStringLiteral("Sessões em tempo real")));
    refresh_button_->setText(
        get_text(loc_, QStringLiteral("security_refresh"), QStringLiteral("Atualizar")));
    close_button_->setText(
        get_text(loc_, QStringLiteral("security_close"), QStringLiteral("Fechar")));

    QStringList headers;
    headers << get_text(loc_, QStringLiteral("security_updated_at"), QStringLiteral("Atualizado em"))
            << get_text(loc_, QStringLiteral("security_session_label"), QStringLiteral("Sessão"))
            << get_text(loc_, QStringLiteral("security_total_events"), QStringLiteral("Total de eventos"))
            << get_text(loc_, QStringLiteral("security_monitored_directories"),
                        QStringLiteral("Diretórios monitorados"))
            << get_text(loc_, QStringLiteral("security_evidence_id"), QStringLiteral("Identificador"));
    table_->setHorizontalHeaderLabels(headers);
}

void SessionCenterDialog::refresh_data() {
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

    const QString db_path = security_manager->db_path();
    const QString profile_id =
        security_manager->profile_summary().value(QStringLiteral("monitor_profile_id")).toString();
    const QString last_updated_at = last_updated_at_;

    const bool replace_all = !initialized_;
    const quint64 request_id = ++refresh_request_id_;
    refresh_inflight_ = true;
    refresh_button_->setEnabled(false);
    summary_label_->setText(
        get_text(loc_, replace_all ? QStringLiteral("loading") : QStringLiteral("security_refresh"),
                 replace_all ? QStringLiteral("Carregando") : QStringLiteral("Atualizar")) +
        QStringLiteral("..."));

    QPointer<SessionCenterDialog> self_guard(this);
    run_on_security_runtime_thread(interface_, [self_guard,
                                                db_path,
                                                profile_id,
                                                last_updated_at,
                                                replace_all,
                                                request_id]() {
        const QVariantList rows =
            replace_all ? fetch_sessions_snapshot(db_path, profile_id, 300)
                        : fetch_sessions_delta_with_fallback(db_path, profile_id, last_updated_at, 120);
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

void SessionCenterDialog::finish_refresh(const QVariantList& rows,
                                         bool replace_all,
                                         quint64 request_id) {
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

void SessionCenterDialog::apply_rows(const QVariantList& rows, bool replace_all) {
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
        const QString label = source::security::traduzir_valor_ui(
            localizador,
            QStringLiteral("session_label"),
            row.value(QStringLiteral("label")).toString());

        int target_row = row_index;
        if (!replace_all) {
            target_row = find_row_by_session_id(table_, row.value(QStringLiteral("session_id")).toString());
            if (target_row < 0) {
                target_row = 0;
                table_->insertRow(target_row);
            }
        }

        table_->setItem(target_row, 0, make_item(row.value(QStringLiteral("updated_at")).toString()));
        table_->setItem(target_row, 1, make_item(label));
        table_->setItem(target_row, 2, make_item(row.value(QStringLiteral("total_eventos")).toString()));
        table_->setItem(target_row, 3, make_item(row.value(QStringLiteral("diretorios")).toString()));
        table_->setItem(target_row, 4, make_item(row.value(QStringLiteral("session_id")).toString()));
        const QString candidate_updated_at = row.value(QStringLiteral("updated_at")).toString();
        if (last_updated_at_ < candidate_updated_at) {
            last_updated_at_ = candidate_updated_at;
        }
    }

    table_->setUpdatesEnabled(true);
    table_->resizeColumnsToContents();
    table_->viewport()->update();

    summary_label_->setText(
        QStringLiteral("%1: %2 | Backend: %3")
            .arg(get_text(loc_, QStringLiteral("security_session_center"), QStringLiteral("Sessões em tempo real")),
                 QString::number(table_->rowCount()),
                 backend.isEmpty() ? QStringLiteral("sqlite") : backend));
}

}  // namespace source::security::ui
