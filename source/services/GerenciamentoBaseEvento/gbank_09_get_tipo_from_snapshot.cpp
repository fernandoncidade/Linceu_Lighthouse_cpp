#include "gbank_09_get_tipo_from_snapshot.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_tipo_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

QString get_tipo_from_snapshot(source::services::BaseEvento* self, const QString& nome) {
    try {
        if (!self || nome.isEmpty()) return {};
        const QString connName = conn_name(self->db_path + nome);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return {};
        }

        QSqlQuery q(db);
        q.prepare("SELECT tipo FROM snapshot WHERE nome = ? LIMIT 1");
        q.addBindValue(nome);
        q.exec();
        QString out;
        if (q.next()) out = q.value(0).toString();
        db.close();
        QSqlDatabase::removeDatabase(connName);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter tipo de snapshot", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
