#pragma once

#include <QSqlDatabase>
#include <QString>

/**
 * @brief Armazenamento local persistente para o subsistema de segurança (Security Database).
 * 
 * O Linceu Lighthouse opera de forma 100% Local-First. Todos os dados de segurança
 * (perfis multi-diretório, baseline criptográfico, regras de alerta, quarentena,
 * correlação de sessões e exportação de evidências) são armazenados e consultados
 * exclusivamente em banco de dados SQLite local ("security/security.db") com suporte a WAL.
 */
namespace source::security::pg {

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
