#include "gbank_11_limpar_registros.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_limpar_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

void limpar_registros(source::services::BaseEvento* self) {
    try {
        if (!self) return;
        const QString connName = conn_name(self->db_path);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return;
        }

        QSqlQuery q(db);
        const QStringList tabelas = {"monitoramento", "snapshot", "adicionado", "excluido", "modificado", "renomeado", "movido", "escaneado"};
        for (const QString& t : tabelas) {
            q.exec(QString("DELETE FROM %1").arg(t));
            q.exec(QString("DELETE FROM sqlite_sequence WHERE name='%1'").arg(t));
        }
        q.exec("VACUUM");

        db.close();
        QSqlDatabase::removeDatabase(connName);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar registros do banco", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
