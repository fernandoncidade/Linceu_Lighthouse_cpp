#include "gbank_20_db_writer.h"

#include <chrono>

#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantList>

#include "source/security/secore_03_evento_campos.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {
namespace {

QString bind_placeholders(int count) {
    QStringList placeholders;
    placeholders.reserve(count);
    for (int i = 0; i < count; ++i) {
        placeholders.append(QStringLiteral("?"));
    }
    return placeholders.join(QLatin1Char(','));
}

void apply_sqlite_writer_settings(QSqlDatabase& db) {
    QSqlQuery pragma(db);
    pragma.exec(QStringLiteral("PRAGMA journal_mode=WAL"));
    pragma.exec(QStringLiteral("PRAGMA synchronous=NORMAL"));
    pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
    pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));
    pragma.exec(QStringLiteral("PRAGMA mmap_size=536870912"));
    pragma.exec(QStringLiteral("PRAGMA wal_autocheckpoint=4000"));
    pragma.exec(QStringLiteral("PRAGMA journal_size_limit=268435456"));
    pragma.exec(QStringLiteral("PRAGMA busy_timeout=5000"));
}

bool exec_insert_batch(QSqlDatabase& db,
                       const QString& tabela,
                       const QList<QVariantMap>& rows,
                       const QStringList& colunas,
                       const QString& placeholders) {
    if (rows.isEmpty()) {
        return true;
    }

    QSqlQuery query(db);
    query.prepare(QStringLiteral("INSERT INTO %1 (%2) VALUES (%3)")
                      .arg(tabela, colunas.join(QLatin1Char(',')), placeholders));

    QList<QVariantList> batch_values;
    batch_values.reserve(colunas.size());
    for (int i = 0; i < colunas.size(); ++i) {
        batch_values.append(QVariantList());
    }

    for (const QVariantMap& row : rows) {
        for (int index = 0; index < colunas.size(); ++index) {
            batch_values[index].append(row.value(colunas.at(index)));
        }
    }

    for (const QVariantList& values : batch_values) {
        query.addBindValue(values);
    }
    return query.execBatch();
}

}  // namespace

std::mutex DatabaseWriter::instance_mutex_;
std::shared_ptr<DatabaseWriter> DatabaseWriter::instance_;

std::shared_ptr<DatabaseWriter> DatabaseWriter::get_instance(const QString& db_path) {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_ || instance_->db_path_ != db_path) {
        instance_ = std::make_shared<DatabaseWriter>(db_path);
    }
    if (!instance_->started_) {
        instance_->start();
    }
    return instance_;
}

DatabaseWriter::DatabaseWriter(const QString& db_path, int batch_size, int batch_timeout_ms)
    : db_path_(db_path), batch_size_(batch_size), batch_timeout_ms_(batch_timeout_ms), stop_event_(false), started_(false), persisting_(false) {}

DatabaseWriter::~DatabaseWriter() { stop(); }

void DatabaseWriter::start() {
    try {
        if (started_) return;
        stop_event_ = false;
        thread_ = std::thread(&DatabaseWriter::worker, this);
        started_ = true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Falha ao iniciar DatabaseWriter", true);
    }
}

void DatabaseWriter::stop() {
    stop_event_ = true;
    cv_.notify_all();
    if (started_ && thread_.joinable()) {
        thread_.join();
    }
    started_ = false;
}

void DatabaseWriter::enqueue_event(const QString& tabela, const QVariantMap& evento) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back({tabela, evento});
    }
    cv_.notify_one();
}

bool DatabaseWriter::wait_until_idle(int timeout_ms) {
    try {
        if (timeout_ms < 0) {
            timeout_ms = 0;
        }

        const auto deadline = std::chrono::steady_clock::now() +
                              std::chrono::milliseconds(timeout_ms);

        while (std::chrono::steady_clock::now() <= deadline) {
            bool queue_empty = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                queue_empty = queue_.empty();
            }

            if (queue_empty && !persisting_.load()) {
                return true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return false;
    } catch (...) {
        return false;
    }
}

void DatabaseWriter::worker() {
    try {
        while (true) {
            std::deque<QPair<QString, QVariantMap>> batch;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(batch_timeout_ms_), [this]() {
                    return stop_event_ || !queue_.empty();
                });
                if (queue_.empty() && stop_event_) {
                    break;
                }
                while (!queue_.empty() && static_cast<int>(batch.size()) < batch_size_) {
                    batch.push_back(queue_.front());
                    queue_.pop_front();
                }
            }

            if (!batch.empty()) {
                persisting_.store(true);
                persist_batch(batch);
                persisting_.store(false);
            }
        }
    } catch (...) {
        persisting_.store(false);
        source::utils::LogManager::get_logger().critical("DatabaseWriter falhou", true);
    }
}

bool DatabaseWriter::persist_batch(const std::deque<QPair<QString, QVariantMap>>& batch) {
    QString connName;
    bool ok = false;
    try {
        connName = QStringLiteral("dbwriter_%1").arg(reinterpret_cast<quintptr>(db_path_.constData()));
        {
            QSqlDatabase db = QSqlDatabase::contains(connName)
                                  ? QSqlDatabase::database(connName, false)
                                  : QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                return false;
            }

            const QStringList colunas = source::security::core::colunas_evento_monitoramento();
            const QString placeholders = bind_placeholders(colunas.size());
            apply_sqlite_writer_settings(db);

            QHash<QString, QList<QVariantMap>> grouped_rows;
            QStringList ordered_tables;
            QList<QVariantMap> monitor_rows;
            monitor_rows.reserve(static_cast<int>(batch.size()));
            for (const auto& item : batch) {
                const QString& tabela = item.first;
                const QVariantMap& evento = item.second;
                if (!grouped_rows.contains(tabela)) {
                    ordered_tables.append(tabela);
                }
                grouped_rows[tabela].append(evento);
                monitor_rows.append(evento);
            }

            db.transaction();
            ok = exec_insert_batch(db, QStringLiteral("monitoramento"), monitor_rows, colunas, placeholders);
            for (const QString& tabela : ordered_tables) {
                if (!ok) {
                    break;
                }
                ok = exec_insert_batch(db, tabela, grouped_rows.value(tabela), colunas, placeholders);
            }
            if (ok) {
                ok = db.commit();
            } else {
                db.rollback();
            }
            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
        return ok;
    } catch (...) {
        if (!connName.isEmpty()) {
            QSqlDatabase::removeDatabase(connName);
        }
        source::utils::LogManager::get_logger().error("Erro ao persistir batch de eventos", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
