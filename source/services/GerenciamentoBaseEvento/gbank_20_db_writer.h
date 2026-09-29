#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>

#include <QString>
#include <QVariantMap>

class QSqlDatabase;

namespace source::services::GerenciamentoBaseEvento {

class DatabaseWriter {
public:
    static std::shared_ptr<DatabaseWriter> get_instance(const QString& db_path);

    explicit DatabaseWriter(const QString& db_path, int batch_size = 500, int batch_timeout_ms = 100);
    ~DatabaseWriter();

    void start();
    void stop();
    void enqueue_event(const QString& tabela, const QVariantMap& evento);
    bool wait_until_idle(int timeout_ms = 2000);
    bool suspend_processing(int timeout_ms = 2000);
    void resume_processing();
    void discard_pending();

private:
    void worker();
    bool persist_batch(const std::deque<QPair<QString, QVariantMap>>& batch);
    bool persist_batch_internal(QSqlDatabase& db, const std::deque<QPair<QString, QVariantMap>>& batch);

private:
    QString db_path_;
    int batch_size_;
    int batch_timeout_ms_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<QPair<QString, QVariantMap>> queue_;
    std::atomic<bool> stop_event_;
    std::atomic<bool> started_;
    std::atomic<bool> suspended_;
    std::atomic<bool> suspended_ack_;
    std::atomic<bool> persisting_;
    std::thread thread_;

    static std::mutex instance_mutex_;
    static std::shared_ptr<DatabaseWriter> instance_;
};

}  // namespace source::services::GerenciamentoBaseEvento
