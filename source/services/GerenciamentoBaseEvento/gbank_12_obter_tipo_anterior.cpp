#include "gbank_12_obter_tipo_anterior.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "gbank_09_get_tipo_from_snapshot.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_tipo_ant_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

QString obter_tipo_anterior(source::services::BaseEvento* self, const QString& nome_base) {
    try {
        if (!self) return {};

        const QString connName = conn_name(self->db_path + nome_base);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return get_tipo_from_snapshot(self, nome_base);
        }

        QSqlQuery q(db);
        q.prepare("SELECT tipo FROM monitoramento WHERE nome = ? ORDER BY id DESC LIMIT 1");
        q.addBindValue(nome_base);
        q.exec();

        if (q.next()) {
            const QString tipo = q.value(0).toString();
            db.close();
            QSqlDatabase::removeDatabase(connName);
            return tipo;
        }

        db.close();
        QSqlDatabase::removeDatabase(connName);
        return get_tipo_from_snapshot(self, nome_base);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter tipo anterior", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
