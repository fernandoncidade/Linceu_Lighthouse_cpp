#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace source::security {

class SecurityDbWriter {
public:
    static SecurityDbWriter& instance();

    SecurityDbWriter(const SecurityDbWriter&) = delete;
    SecurityDbWriter& operator=(const SecurityDbWriter&) = delete;

    void start();
    void stop();
    bool wait_until_idle(int timeout_ms = 2000);
    bool suspend_processing(int timeout_ms = 2000);
    void resume_processing();
    void discard_pending();

    void enqueue_alert_insert(const QVariantMap& row);
    void enqueue_session_upsert(const QVariantMap& row);
    void enqueue_baseline_upsert(const QVariantMap& row);
    void enqueue_baseline_snapshot_upsert(const QVariantMap& row);
    void enqueue_quarantine_metadata(const QVariantMap& row);
    void enqueue_quarantine_blob_capture(const QVariantMap& row);
    void enqueue_evidence_upsert(const QVariantMap& row);
    void enqueue_evidence_payload_upsert(const QVariantMap& row);

private:
    enum class OperationKind {
        AlertInsert,
        SessionUpsert,
        BaselineUpsert,
        BaselineSnapshotUpsert,
        QuarantineMetadataInsert,
        QuarantineBlobCapture,
        EvidenceUpsert,
        EvidencePayloadUpsert,
    };

    struct Operation {
        OperationKind kind;
        QVariantMap payload;
    };

    SecurityDbWriter();
    ~SecurityDbWriter();

    void enqueue_fast(OperationKind kind, const QVariantMap& payload);
    void enqueue_heavy(OperationKind kind, const QVariantMap& payload);
    void worker();
    bool persist_fast_batch(const std::deque<Operation>& batch);
    bool persist_heavy_operation(const Operation& operation);

private:
    int batch_size_;
    int batch_timeout_ms_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<Operation> fast_queue_;
    std::deque<Operation> heavy_queue_;
    std::atomic<bool> stop_requested_;
    std::atomic<bool> started_;
    std::atomic<bool> persisting_;
    std::atomic<bool> suspended_;
    std::atomic<bool> suspended_ack_;
    std::thread worker_thread_;
};

}  // namespace source::security
