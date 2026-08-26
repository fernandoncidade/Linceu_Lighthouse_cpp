#pragma once

#include <QSqlDatabase>
#include <QString>

namespace source::security::pg {

struct ConnectionConfig {
    QString host;
    int port = 5432;
    QString database;
    QString user;
    QString password;
    QString sslmode;
    int connect_timeout_s = 2;
    QString application_name;
    QString client_library_dir;

    bool is_configured() const;
};

const ConnectionConfig& config();
void prepare_runtime();
bool is_driver_available();
bool is_local_cache_active();
QString last_error();
QString describe_target();
QString effective_backend_name();
QString effective_target();
QString local_cache_path();
QSqlDatabase open_local_cache_database(const QString& scope);
QSqlDatabase open_database(const QString& scope);
void close_thread_local_connections();
void close_all_local_cache_connections();

}  // namespace source::security::pg
