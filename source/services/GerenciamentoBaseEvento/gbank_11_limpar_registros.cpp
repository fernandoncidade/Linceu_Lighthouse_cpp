#include "gbank_11_limpar_registros.h"

#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "gbank_02_criar_banco_de_dados.h"
#include "gbank_20_db_writer.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {
namespace {

void fechar_todas_conexoes_db(const QString& db_path) {
    const QStringList names = QSqlDatabase::connectionNames();
    for (const QString& name : names) {
        bool match = false;
        if (name.startsWith(QStringLiteral("dbwriter_")) ||
            name.startsWith(QStringLiteral("baseevento_")) ||
            name.startsWith(QStringLiteral("limpeza_db_")) ||
            name.startsWith(QStringLiteral("stats_db_")) ||
            name.startsWith(QStringLiteral("import_db_")) ||
            name.startsWith(QStringLiteral("geve_")) ||
            name.startsWith(QStringLiteral("security_monitoramento_"))) {
            match = true;
        }
        if (!match && QSqlDatabase::contains(name)) {
            QSqlDatabase db = QSqlDatabase::database(name, false);
            if (db.isValid() && db.databaseName() == db_path) {
                match = true;
            }
        }
        if (match) {
            {
                QSqlDatabase db = QSqlDatabase::database(name, false);
                if (db.isValid() && db.isOpen()) {
                    db.close();
                }
            }
            QSqlDatabase::removeDatabase(name);
        }
    }
}

}  // namespace

void limpar_registros(source::services::BaseEvento* self) {
    try {
        if (!self || self->db_path.isEmpty()) return;
        const QString db_path = self->db_path;

        auto writer = DatabaseWriter::get_instance(db_path);
        if (writer) {
            writer->discard_pending();
            writer->suspend_processing(3000);
        }

        fechar_todas_conexoes_db(db_path);

        QFile::remove(db_path + QStringLiteral("-wal"));
        QFile::remove(db_path + QStringLiteral("-shm"));

        const bool file_removed = QFile::remove(db_path);
        if (file_removed) {
            criar_banco_de_dados_path(db_path);
        } else {
            const QString connName = QStringLiteral("baseevento_limpar_exec_%1")
                                         .arg(reinterpret_cast<quintptr>(db_path.constData()));
            if (QSqlDatabase::contains(connName)) {
                QSqlDatabase::removeDatabase(connName);
            }

            {
                QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
                db.setDatabaseName(db_path);
                if (db.open()) {
                    QSqlQuery pragma(db);
                    pragma.exec(QStringLiteral("PRAGMA busy_timeout=5000"));

                    if (db.transaction()) {
                        QSqlQuery q(db);
                        const QStringList tabelas = {
                            QStringLiteral("monitoramento"),
                            QStringLiteral("snapshot"),
                            QStringLiteral("adicionado"),
                            QStringLiteral("excluido"),
                            QStringLiteral("modificado"),
                            QStringLiteral("renomeado"),
                            QStringLiteral("movido"),
                            QStringLiteral("escaneado")
                        };
                        for (const QString& t : tabelas) {
                            q.exec(QStringLiteral("DELETE FROM %1").arg(t));
                            q.exec(QStringLiteral("DELETE FROM sqlite_sequence WHERE name='%1'").arg(t));
                        }
                        db.commit();
                    }

                    QSqlQuery checkpoint(db);
                    checkpoint.exec(QStringLiteral("PRAGMA wal_checkpoint(TRUNCATE)"));
                    checkpoint.exec(QStringLiteral("PRAGMA optimize"));

                    db.close();
                }
            }
            QSqlDatabase::removeDatabase(connName);

            fechar_todas_conexoes_db(db_path);

            QFile::remove(db_path + QStringLiteral("-wal"));
            QFile::remove(db_path + QStringLiteral("-shm"));
        }

        if (writer) {
            writer->resume_processing();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar registros do banco", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
