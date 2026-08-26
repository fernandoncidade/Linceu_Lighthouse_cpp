#include "gbank_14_remover_exclusao_temporaria.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_rm_exc_%1").arg(reinterpret_cast<quintptr>(db.constData())); }

QString normalizar_caminho(const QString& caminho) {
    if (caminho.trimmed().isEmpty()) return QString();
    return QDir::toNativeSeparators(QDir::cleanPath(caminho)).replace('/', '\\');
}
}

namespace source::services::GerenciamentoBaseEvento {

void _remover_exclusao_temporaria(source::services::BaseEvento* self, const QString& nome, const QString& dir_anterior, const QString& exclusao_id) {
    QString connName;
    try {
        if (!self || !self->observador) return;

        connName = conn_name(self->db_path + nome + dir_anterior);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (!db.open()) {
                QSqlDatabase::removeDatabase(connName);
                return;
            }

            const QStringList aliasesDeleted =
                aliases_operacao(QStringLiteral("op_deleted"), self->observador->loc);
            const QString placeholdersDeleted = marcadores_sql(aliasesDeleted.size());
            const QString dir_normalizado = normalizar_caminho(dir_anterior);
            const QString dir_alternativo = QString(dir_normalizado).replace('\\', '/');
            bool ok = db.transaction();

            int monitoramento_id = -1;
            if (ok && !exclusao_id.trimmed().isEmpty()) {
                QSqlQuery q_ts(db);
                q_ts.prepare(QString("SELECT id FROM monitoramento WHERE tipo_operacao IN (%1) AND nome = ? AND timestamp = ? ORDER BY id DESC LIMIT 1")
                                 .arg(placeholdersDeleted));
                for (const QString& alias : aliasesDeleted) q_ts.addBindValue(alias);
                q_ts.addBindValue(nome);
                q_ts.addBindValue(exclusao_id.trimmed());
                ok = q_ts.exec();
                if (ok && q_ts.next()) monitoramento_id = q_ts.value(0).toInt();
            }
            if (ok && monitoramento_id == -1) {
                QSqlQuery q(db);
                q.prepare(
                    QString("SELECT id FROM monitoramento "
                            "WHERE tipo_operacao IN (%1) AND nome = ? "
                            "AND (dir_anterior = ? OR REPLACE(dir_anterior, '/', '\\') = ? OR REPLACE(dir_anterior, '\\', '/') = ?) "
                            "ORDER BY id DESC LIMIT 1")
                        .arg(placeholdersDeleted));
                for (const QString& alias : aliasesDeleted) q.addBindValue(alias);
                q.addBindValue(nome);
                q.addBindValue(dir_normalizado);
                q.addBindValue(dir_normalizado);
                q.addBindValue(dir_alternativo);
                ok = q.exec();
                if (ok && q.next()) monitoramento_id = q.value(0).toInt();
            }
            if (ok && monitoramento_id != -1) {
                QSqlQuery del(db);
                del.prepare("DELETE FROM monitoramento WHERE id = ?");
                del.addBindValue(monitoramento_id);
                ok = del.exec();
            }

            int excluido_id = -1;
            if (ok && !exclusao_id.trimmed().isEmpty()) {
                QSqlQuery q2_ts(db);
                q2_ts.prepare(QString("SELECT id FROM excluido WHERE tipo_operacao IN (%1) AND nome = ? AND timestamp = ? ORDER BY id DESC LIMIT 1")
                                  .arg(placeholdersDeleted));
                for (const QString& alias : aliasesDeleted) q2_ts.addBindValue(alias);
                q2_ts.addBindValue(nome);
                q2_ts.addBindValue(exclusao_id.trimmed());
                ok = q2_ts.exec();
                if (ok && q2_ts.next()) excluido_id = q2_ts.value(0).toInt();
            }
            if (ok && excluido_id == -1) {
                QSqlQuery q2(db);
                q2.prepare(
                    QString("SELECT id FROM excluido "
                            "WHERE tipo_operacao IN (%1) AND nome = ? "
                            "AND (dir_anterior = ? OR REPLACE(dir_anterior, '/', '\\') = ? OR REPLACE(dir_anterior, '\\', '/') = ?) "
                            "ORDER BY id DESC LIMIT 1")
                        .arg(placeholdersDeleted));
                for (const QString& alias : aliasesDeleted) q2.addBindValue(alias);
                q2.addBindValue(nome);
                q2.addBindValue(dir_normalizado);
                q2.addBindValue(dir_normalizado);
                q2.addBindValue(dir_alternativo);
                ok = q2.exec();
                if (ok && q2.next()) excluido_id = q2.value(0).toInt();
            }
            if (ok && excluido_id != -1) {
                QSqlQuery del2(db);
                del2.prepare("DELETE FROM excluido WHERE id = ?");
                del2.addBindValue(excluido_id);
                ok = del2.exec();
            }

            if (ok) {
                ok = db.commit();
            } else {
                db.rollback();
            }

            if (!ok) {
                source::utils::LogManager::get_logger().error("Erro SQL ao remover exclusão temporária", true);
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
    } catch (...) {
        if (!connName.isEmpty()) {
            QSqlDatabase::removeDatabase(connName);
        }
        source::utils::LogManager::get_logger().error("Erro ao remover exclusão temporária", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
