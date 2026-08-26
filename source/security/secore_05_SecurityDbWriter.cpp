#include "secore_05_SecurityDbWriter.h"

#include <chrono>
#include <vector>

#include <QDateTime>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/security/secore_01_postgres_utils.h"
#include "source/utils/LogManager.h"

namespace source::security {
namespace {

QString iso_now() {
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

template <typename Container>
void append_batch_value(Container& values, const QVariant& value) {
    values.append(value);
}

bool exec_alert_batch(QSqlDatabase& db, const std::vector<QVariantMap>& rows) {
    if (rows.empty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO security_alerts "
        "(created_at, profile_id, nome, caminho, risk_level, rules, summary, session_id, evidence_id) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"));

    QVariantList created_at;
    QVariantList profile_id;
    QVariantList nome;
    QVariantList caminho;
    QVariantList risk_level;
    QVariantList rules;
    QVariantList summary;
    QVariantList session_id;
    QVariantList evidence_id;
    for (const QVariantMap& row : rows) {
        append_batch_value(created_at, row.value(QStringLiteral("created_at")).toString());
        append_batch_value(profile_id, row.value(QStringLiteral("profile_id")).toString());
        append_batch_value(nome, row.value(QStringLiteral("nome")).toString());
        append_batch_value(caminho, row.value(QStringLiteral("caminho")).toString());
        append_batch_value(risk_level, row.value(QStringLiteral("risk_level")).toString());
        append_batch_value(rules, row.value(QStringLiteral("rules")).toString());
        append_batch_value(summary, row.value(QStringLiteral("summary")).toString());
        append_batch_value(session_id, row.value(QStringLiteral("session_id")).toString());
        append_batch_value(evidence_id, row.value(QStringLiteral("evidence_id")).toString());
    }

    query.addBindValue(created_at);
    query.addBindValue(profile_id);
    query.addBindValue(nome);
    query.addBindValue(caminho);
    query.addBindValue(risk_level);
    query.addBindValue(rules);
    query.addBindValue(summary);
    query.addBindValue(session_id);
    query.addBindValue(evidence_id);
    return query.execBatch();
}

bool exec_session_batch(QSqlDatabase& db, const std::vector<QVariantMap>& rows) {
    if (rows.empty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO security_sessions "
        "(session_id, profile_id, label, started_at, updated_at, total_eventos, diretorios) "
        "VALUES (?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT (session_id) DO UPDATE SET "
        "profile_id = excluded.profile_id, "
        "label = excluded.label, "
        "started_at = excluded.started_at, "
        "updated_at = excluded.updated_at, "
        "total_eventos = excluded.total_eventos, "
        "diretorios = excluded.diretorios"));

    QVariantList session_id;
    QVariantList profile_id;
    QVariantList label;
    QVariantList started_at;
    QVariantList updated_at;
    QVariantList total_eventos;
    QVariantList diretorios;
    for (const QVariantMap& row : rows) {
        append_batch_value(session_id, row.value(QStringLiteral("session_id")).toString());
        append_batch_value(profile_id, row.value(QStringLiteral("profile_id")).toString());
        append_batch_value(label, row.value(QStringLiteral("label")).toString());
        append_batch_value(started_at, row.value(QStringLiteral("started_at")).toString());
        append_batch_value(updated_at, row.value(QStringLiteral("updated_at")).toString());
        append_batch_value(total_eventos, row.value(QStringLiteral("total_eventos")).toInt());
        append_batch_value(diretorios, row.value(QStringLiteral("diretorios")).toString());
    }

    query.addBindValue(session_id);
    query.addBindValue(profile_id);
    query.addBindValue(label);
    query.addBindValue(started_at);
    query.addBindValue(updated_at);
    query.addBindValue(total_eventos);
    query.addBindValue(diretorios);
    return query.execBatch();
}

bool exec_baseline_batch(QSqlDatabase& db, const std::vector<QVariantMap>& rows) {
    if (rows.empty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO security_baseline "
        "(profile_id, caminho, sha256, size_b, mtime_utc, snapshot_path, publisher, signature_state, updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT (profile_id, caminho) DO UPDATE SET "
        "sha256 = excluded.sha256, "
        "size_b = excluded.size_b, "
        "mtime_utc = excluded.mtime_utc, "
        "snapshot_path = excluded.snapshot_path, "
        "publisher = excluded.publisher, "
        "signature_state = excluded.signature_state, "
        "updated_at = excluded.updated_at"));

    QVariantList profile_id;
    QVariantList caminho;
    QVariantList sha256;
    QVariantList size_b;
    QVariantList mtime_utc;
    QVariantList snapshot_path;
    QVariantList publisher;
    QVariantList signature_state;
    QVariantList updated_at;
    for (const QVariantMap& row : rows) {
        append_batch_value(profile_id, row.value(QStringLiteral("profile_id")).toString());
        append_batch_value(caminho, row.value(QStringLiteral("caminho")).toString());
        append_batch_value(sha256, row.value(QStringLiteral("sha256")).toString());
        append_batch_value(size_b, row.value(QStringLiteral("size_b")).toLongLong());
        append_batch_value(mtime_utc, row.value(QStringLiteral("mtime_utc")).toString());
        append_batch_value(snapshot_path, row.value(QStringLiteral("snapshot_path")).toString());
        append_batch_value(publisher, row.value(QStringLiteral("publisher")).toString());
        append_batch_value(signature_state, row.value(QStringLiteral("signature_state")).toString());
        append_batch_value(updated_at, row.value(QStringLiteral("updated_at")).toString());
    }

    query.addBindValue(profile_id);
    query.addBindValue(caminho);
    query.addBindValue(sha256);
    query.addBindValue(size_b);
    query.addBindValue(mtime_utc);
    query.addBindValue(snapshot_path);
    query.addBindValue(publisher);
    query.addBindValue(signature_state);
    query.addBindValue(updated_at);
    return query.execBatch();
}

bool exec_quarantine_metadata_batch(QSqlDatabase& db, const std::vector<QVariantMap>& rows) {
    if (rows.empty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO security_quarantine "
        "(created_at, evidence_id, original_path, quarantine_path, original_name, sha256, status, blob_state, blob_size_b, blob_updated_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

    QVariantList created_at;
    QVariantList evidence_id;
    QVariantList original_path;
    QVariantList quarantine_path;
    QVariantList original_name;
    QVariantList sha256;
    QVariantList status;
    QVariantList blob_state;
    QVariantList blob_size_b;
    QVariantList blob_updated_at;
    for (const QVariantMap& row : rows) {
        append_batch_value(created_at, row.value(QStringLiteral("created_at")).toString());
        append_batch_value(evidence_id, row.value(QStringLiteral("evidence_id")).toString());
        append_batch_value(original_path, row.value(QStringLiteral("original_path")).toString());
        append_batch_value(quarantine_path, row.value(QStringLiteral("quarantine_path")).toString());
        append_batch_value(original_name, row.value(QStringLiteral("original_name")).toString());
        append_batch_value(sha256, row.value(QStringLiteral("sha256")).toString());
        append_batch_value(status, row.value(QStringLiteral("status")).toString());
        append_batch_value(blob_state, row.value(QStringLiteral("blob_state")).toString());
        append_batch_value(blob_size_b, row.value(QStringLiteral("blob_size_b")).toLongLong());
        append_batch_value(blob_updated_at, row.value(QStringLiteral("blob_updated_at")).toString());
    }

    query.addBindValue(created_at);
    query.addBindValue(evidence_id);
    query.addBindValue(original_path);
    query.addBindValue(quarantine_path);
    query.addBindValue(original_name);
    query.addBindValue(sha256);
    query.addBindValue(status);
    query.addBindValue(blob_state);
    query.addBindValue(blob_size_b);
    query.addBindValue(blob_updated_at);
    return query.execBatch();
}

bool exec_evidence_batch(QSqlDatabase& db, const std::vector<QVariantMap>& rows) {
    if (rows.empty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO security_evidence "
        "(evidence_id, created_at, profile_id, profile_name, session_id, nome, caminho, tipo_operacao, risk_level, evidence_ref, evidence_hash) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT (evidence_id) DO UPDATE SET "
        "created_at = excluded.created_at, "
        "profile_id = excluded.profile_id, "
        "profile_name = excluded.profile_name, "
        "session_id = excluded.session_id, "
        "nome = excluded.nome, "
        "caminho = excluded.caminho, "
        "tipo_operacao = excluded.tipo_operacao, "
        "risk_level = excluded.risk_level, "
        "evidence_ref = excluded.evidence_ref, "
        "evidence_hash = excluded.evidence_hash"));

    QVariantList evidence_id;
    QVariantList created_at;
    QVariantList profile_id;
    QVariantList profile_name;
    QVariantList session_id;
    QVariantList nome;
    QVariantList caminho;
    QVariantList tipo_operacao;
    QVariantList risk_level;
    QVariantList evidence_ref;
    QVariantList evidence_hash;
    for (const QVariantMap& row : rows) {
        append_batch_value(evidence_id, row.value(QStringLiteral("evidence_id")).toString());
        append_batch_value(created_at, row.value(QStringLiteral("created_at")).toString());
        append_batch_value(profile_id, row.value(QStringLiteral("profile_id")).toString());
        append_batch_value(profile_name, row.value(QStringLiteral("profile_name")).toString());
        append_batch_value(session_id, row.value(QStringLiteral("session_id")).toString());
        append_batch_value(nome, row.value(QStringLiteral("nome")).toString());
        append_batch_value(caminho, row.value(QStringLiteral("caminho")).toString());
        append_batch_value(tipo_operacao, row.value(QStringLiteral("tipo_operacao")).toString());
        append_batch_value(risk_level, row.value(QStringLiteral("risk_level")).toString());
        append_batch_value(evidence_ref, row.value(QStringLiteral("evidence_ref")).toString());
        append_batch_value(evidence_hash, row.value(QStringLiteral("evidence_hash")).toString());
    }

    query.addBindValue(evidence_id);
    query.addBindValue(created_at);
    query.addBindValue(profile_id);
    query.addBindValue(profile_name);
    query.addBindValue(session_id);
    query.addBindValue(nome);
    query.addBindValue(caminho);
    query.addBindValue(tipo_operacao);
    query.addBindValue(risk_level);
    query.addBindValue(evidence_ref);
    query.addBindValue(evidence_hash);
    return query.execBatch();
}

}  // namespace

SecurityDbWriter& SecurityDbWriter::instance() {
    static SecurityDbWriter writer;
    return writer;
}

SecurityDbWriter::SecurityDbWriter()
    : batch_size_(512),
      batch_timeout_ms_(20),
      stop_requested_(false),
      started_(false),
      persisting_(false),
      suspended_(false),
      suspended_ack_(false) {}

SecurityDbWriter::~SecurityDbWriter() {
    stop();
}

void SecurityDbWriter::start() {
    if (started_.exchange(true)) {
        return;
    }

    stop_requested_ = false;
    worker_thread_ = std::thread(&SecurityDbWriter::worker, this);
}

void SecurityDbWriter::stop() {
    stop_requested_ = true;
    suspended_ = false;
    cv_.notify_all();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    started_ = false;
    suspended_ack_ = false;
}

bool SecurityDbWriter::wait_until_idle(int timeout_ms) {
    if (timeout_ms < 0) {
        timeout_ms = 0;
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() <= deadline) {
        bool queues_empty = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queues_empty = fast_queue_.empty() && heavy_queue_.empty();
        }

        if (queues_empty && !persisting_.load()) {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return false;
}

void SecurityDbWriter::enqueue_fast(OperationKind kind, const QVariantMap& payload) {
    start();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        fast_queue_.push_back({kind, payload});
    }
    cv_.notify_one();
}

void SecurityDbWriter::enqueue_heavy(OperationKind kind, const QVariantMap& payload) {
    start();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        heavy_queue_.push_back({kind, payload});
    }
    cv_.notify_one();
}

void SecurityDbWriter::enqueue_alert_insert(const QVariantMap& row) {
    enqueue_fast(OperationKind::AlertInsert, row);
}

void SecurityDbWriter::enqueue_session_upsert(const QVariantMap& row) {
    enqueue_fast(OperationKind::SessionUpsert, row);
}

void SecurityDbWriter::enqueue_baseline_upsert(const QVariantMap& row) {
    enqueue_fast(OperationKind::BaselineUpsert, row);
}

void SecurityDbWriter::enqueue_baseline_snapshot_upsert(const QVariantMap& row) {
    enqueue_heavy(OperationKind::BaselineSnapshotUpsert, row);
}

void SecurityDbWriter::enqueue_quarantine_metadata(const QVariantMap& row) {
    enqueue_fast(OperationKind::QuarantineMetadataInsert, row);
}

void SecurityDbWriter::enqueue_quarantine_blob_capture(const QVariantMap& row) {
    enqueue_heavy(OperationKind::QuarantineBlobCapture, row);
}

void SecurityDbWriter::enqueue_evidence_upsert(const QVariantMap& row) {
    enqueue_fast(OperationKind::EvidenceUpsert, row);
}

void SecurityDbWriter::enqueue_evidence_payload_upsert(const QVariantMap& row) {
    enqueue_heavy(OperationKind::EvidencePayloadUpsert, row);
}

bool SecurityDbWriter::suspend_processing(int timeout_ms) {
    if (timeout_ms < 0) {
        timeout_ms = 0;
    }

    suspended_ = true;
    cv_.notify_all();

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() <= deadline) {
        const bool worker_ready = !started_.load() || suspended_ack_.load();
        if (!persisting_.load() && worker_ready) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return !persisting_.load();
}

void SecurityDbWriter::resume_processing() {
    suspended_ack_ = false;
    suspended_ = false;
    cv_.notify_all();
}

void SecurityDbWriter::discard_pending() {
    std::lock_guard<std::mutex> lock(mutex_);
    fast_queue_.clear();
    heavy_queue_.clear();
}

void SecurityDbWriter::worker() {
    try {
        while (!stop_requested_.load()) {
            if (suspended_.load()) {
                suspended_ack_.store(true);
                source::security::pg::close_thread_local_connections();
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {
                    return stop_requested_.load() ||
                           (!suspended_.load() && (!fast_queue_.empty() || !heavy_queue_.empty()));
                });
                suspended_ack_.store(false);
                continue;
            }

            std::deque<Operation> fast_batch;
            Operation heavy_operation;
            bool has_heavy_operation = false;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock,
                             std::chrono::milliseconds(batch_timeout_ms_),
                             [this]() {
                                 return stop_requested_.load() || suspended_.load() ||
                                        !fast_queue_.empty() || !heavy_queue_.empty();
                             });

                if (suspended_.load()) {
                    continue;
                }

                while (!fast_queue_.empty() &&
                       static_cast<int>(fast_batch.size()) < batch_size_) {
                    fast_batch.push_back(fast_queue_.front());
                    fast_queue_.pop_front();
                }

                if (fast_batch.empty() && !heavy_queue_.empty()) {
                    heavy_operation = heavy_queue_.front();
                    heavy_queue_.pop_front();
                    has_heavy_operation = true;
                }
            }

            if (!fast_batch.empty()) {
                persisting_.store(true);
                persist_fast_batch(fast_batch);
                persisting_.store(false);

                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (!heavy_queue_.empty() && fast_queue_.size() < static_cast<size_t>(batch_size_ / 2)) {
                        heavy_operation = heavy_queue_.front();
                        heavy_queue_.pop_front();
                        has_heavy_operation = true;
                    }
                }
            }

            if (has_heavy_operation) {
                persisting_.store(true);
                persist_heavy_operation(heavy_operation);
                persisting_.store(false);
            }
        }

        while (true) {
            std::deque<Operation> fast_batch;
            Operation heavy_operation;
            bool has_heavy_operation = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                while (!fast_queue_.empty() &&
                       static_cast<int>(fast_batch.size()) < batch_size_) {
                    fast_batch.push_back(fast_queue_.front());
                    fast_queue_.pop_front();
                }

                if (fast_batch.empty() && !heavy_queue_.empty()) {
                    heavy_operation = heavy_queue_.front();
                    heavy_queue_.pop_front();
                    has_heavy_operation = true;
                }
            }

            if (fast_batch.empty() && !has_heavy_operation) {
                break;
            }

            if (!fast_batch.empty()) {
                persisting_.store(true);
                persist_fast_batch(fast_batch);
                persisting_.store(false);
            }

            if (has_heavy_operation) {
                persisting_.store(true);
                persist_heavy_operation(heavy_operation);
                persisting_.store(false);
            }
        }

        source::security::pg::close_thread_local_connections();
    } catch (...) {
        persisting_.store(false);
        suspended_ack_.store(false);
        source::utils::LogManager::get_logger().critical(
            "SecurityDbWriter falhou durante a persistencia assíncrona.", true);
    }
}

bool SecurityDbWriter::persist_fast_batch(const std::deque<Operation>& batch) {
    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("security_writer_fast"));
    if (!db.isValid() || !db.isOpen()) {
        return false;
    }

    if (!db.transaction()) {
        return false;
    }

    std::vector<QVariantMap> alert_rows;
    std::vector<QVariantMap> session_rows;
    std::vector<QVariantMap> baseline_rows;
    std::vector<QVariantMap> quarantine_rows;
    std::vector<QVariantMap> evidence_rows;
    alert_rows.reserve(batch.size());
    session_rows.reserve(batch.size());
    baseline_rows.reserve(batch.size());
    quarantine_rows.reserve(batch.size());
    evidence_rows.reserve(batch.size());

    for (const Operation& operation : batch) {
        switch (operation.kind) {
            case OperationKind::AlertInsert:
                alert_rows.push_back(operation.payload);
                break;
            case OperationKind::SessionUpsert:
                session_rows.push_back(operation.payload);
                break;
            case OperationKind::BaselineUpsert:
                baseline_rows.push_back(operation.payload);
                break;
            case OperationKind::QuarantineMetadataInsert:
                quarantine_rows.push_back(operation.payload);
                break;
            case OperationKind::EvidenceUpsert:
                evidence_rows.push_back(operation.payload);
                break;
            case OperationKind::BaselineSnapshotUpsert:
            case OperationKind::QuarantineBlobCapture:
            case OperationKind::EvidencePayloadUpsert:
                break;
        }
    }

    const bool ok = exec_alert_batch(db, alert_rows) &&
                    exec_session_batch(db, session_rows) &&
                    exec_baseline_batch(db, baseline_rows) &&
                    exec_quarantine_metadata_batch(db, quarantine_rows) &&
                    exec_evidence_batch(db, evidence_rows);

    if (!ok) {
        db.rollback();
        return false;
    }

    return db.commit();
}

bool SecurityDbWriter::persist_heavy_operation(const Operation& operation) {
    QSqlDatabase db = source::security::pg::open_database(QStringLiteral("security_writer_heavy"));
    if (!db.isValid() || !db.isOpen()) {
        return false;
    }

    switch (operation.kind) {
        case OperationKind::BaselineSnapshotUpsert: {
            QSqlQuery query(db);
            query.prepare(QStringLiteral(
                "INSERT INTO security_baseline_snapshots "
                "(snapshot_path, snapshot_text, updated_at) "
                "VALUES (?, ?, ?) "
                "ON CONFLICT (snapshot_path) DO UPDATE SET "
                "snapshot_text = excluded.snapshot_text, "
                "updated_at = excluded.updated_at"));
            query.addBindValue(operation.payload.value(QStringLiteral("snapshot_path")).toString());
            query.addBindValue(operation.payload.value(QStringLiteral("snapshot_text")).toString());
            query.addBindValue(operation.payload.value(QStringLiteral("updated_at")).toString());
            return query.exec();
        }

        case OperationKind::EvidencePayloadUpsert: {
            QSqlQuery query(db);
            query.prepare(QStringLiteral(
                "INSERT INTO security_evidence_payloads "
                "(evidence_id, event_payload, context_payload, updated_at) "
                "VALUES (?, ?, ?, ?) "
                "ON CONFLICT (evidence_id) DO UPDATE SET "
                "event_payload = excluded.event_payload, "
                "context_payload = excluded.context_payload, "
                "updated_at = excluded.updated_at"));
            query.addBindValue(operation.payload.value(QStringLiteral("evidence_id")).toString());
            query.addBindValue(operation.payload.value(QStringLiteral("event_payload")).toString());
            query.addBindValue(operation.payload.value(QStringLiteral("context_payload")).toString());
            query.addBindValue(operation.payload.value(QStringLiteral("updated_at")).toString());
            return query.exec();
        }

        case OperationKind::QuarantineBlobCapture: {
            const QString source_path = operation.payload.value(QStringLiteral("source_path")).toString();
            const QString quarantine_path =
                operation.payload.value(QStringLiteral("quarantine_path")).toString();
            const QString original_name = operation.payload.value(QStringLiteral("original_name")).toString();

            QFile source_file(source_path);
            const bool opened = source_file.open(QIODevice::ReadOnly);
            const QByteArray content = opened ? source_file.readAll() : QByteArray();
            if (opened) {
                source_file.close();
            }

            const QString blob_state = (opened && !content.isEmpty()) ? QStringLiteral("ready")
                                                                      : QStringLiteral("missing");
            const QString status = (opened && !content.isEmpty()) ? QStringLiteral("captured")
                                                                  : QStringLiteral("copy_failed");
            const QString updated_at = iso_now();

            if (opened && !content.isEmpty()) {
                QSqlQuery insert_blob(db);
                insert_blob.prepare(QStringLiteral(
                    "INSERT INTO security_quarantine_blobs "
                    "(quarantine_path, original_name, blob_content, size_b, created_at) "
                    "VALUES (?, ?, ?, ?, ?) "
                    "ON CONFLICT (quarantine_path) DO UPDATE SET "
                    "original_name = excluded.original_name, "
                    "blob_content = excluded.blob_content, "
                    "size_b = excluded.size_b, "
                    "created_at = excluded.created_at"));
                insert_blob.addBindValue(quarantine_path);
                insert_blob.addBindValue(original_name);
                insert_blob.addBindValue(content);
                insert_blob.addBindValue(content.size());
                insert_blob.addBindValue(updated_at);
                if (!insert_blob.exec()) {
                    return false;
                }
            }

            QSqlQuery update_meta(db);
            update_meta.prepare(QStringLiteral(
                "UPDATE security_quarantine "
                "SET status = ?, blob_state = ?, blob_size_b = ?, blob_updated_at = ? "
                "WHERE quarantine_path = ?"));
            update_meta.addBindValue(status);
            update_meta.addBindValue(blob_state);
            update_meta.addBindValue(content.size());
            update_meta.addBindValue(updated_at);
            update_meta.addBindValue(quarantine_path);
            return update_meta.exec();
        }

        case OperationKind::AlertInsert:
        case OperationKind::SessionUpsert:
        case OperationKind::BaselineUpsert:
        case OperationKind::QuarantineMetadataInsert:
        case OperationKind::EvidenceUpsert:
            return true;
    }

    return true;
}

}  // namespace source::security
