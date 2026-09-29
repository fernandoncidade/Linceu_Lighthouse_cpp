#include "geve_08_limpar_dados_monitorados.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

#include <exception>

#include "geve_09_limpar_dados_interface.h"
#include "source/services/GerenciamentoBaseEvento/gbank_02_criar_banco_de_dados.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEventosUI {
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

void limpar_dados_monitorados(source::ui::GerenciadorEventosUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        bool observador_ativo = false;
        if (interface_obj->property("_observador_cpp_ptr").toULongLong() != 0) {
            observador_ativo = true;
        }
        QObject* observador = interface_obj->property("observador").value<QObject*>();
        if (observador != nullptr && observador->property("ativo").isValid()) {
            observador_ativo = observador->property("ativo").toBool();
        }

        if (observador_ativo) {
            QObject* gerenciador_botoes = interface_obj->property("gerenciador_botoes").value<QObject*>();
            if (gerenciador_botoes != nullptr) {
                QMetaObject::invokeMethod(gerenciador_botoes, "alternar_analise_diretorio");
            }
        }

        const QString db_path = QDir(source::utils::obter_caminho_persistente())
                                    .filePath(QStringLiteral("monitoramento.db"));

        auto writer = source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(db_path);
        if (writer) {
            writer->discard_pending();
            writer->suspend_processing(3000);
        }

        fechar_todas_conexoes_db(db_path);

        QFile::remove(db_path + QStringLiteral("-wal"));
        QFile::remove(db_path + QStringLiteral("-shm"));

        const bool file_removed = QFile::remove(db_path);
        if (file_removed) {
            source::services::GerenciamentoBaseEvento::criar_banco_de_dados_path(db_path);
        } else {
            const QString conn_name = QStringLiteral("geve_limpar_db");
            if (QSqlDatabase::contains(conn_name)) {
                QSqlDatabase::removeDatabase(conn_name);
            }

            {
                QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
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
            QSqlDatabase::removeDatabase(conn_name);

            fechar_todas_conexoes_db(db_path);

            QFile::remove(db_path + QStringLiteral("-wal"));
            QFile::remove(db_path + QStringLiteral("-shm"));
        }

        if (writer) {
            writer->resume_processing();
        }

        interface_obj->setProperty("_monitoramento_db_tem_dados", false);
        interface_obj->setProperty("contador_eventos", 0);
        interface_obj->setProperty("excluidos_recentemente", QVariantMap());
        interface_obj->setProperty("_scan_contador", 0);
        interface_obj->setProperty("_scan_total", 0);
        interface_obj->setProperty("_scan_em_execucao", false);

        limpar_dados_interface(self);
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao limpar dados monitorados: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar dados monitorados", true);
    }
}

}  // namespace source::ui::GerenciamentoEventosUI
