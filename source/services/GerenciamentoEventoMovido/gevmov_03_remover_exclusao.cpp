#include "gevmov_03_remover_exclusao.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>
#include <QUuid>

#include "gevmov_04_atualizar_linha_recente.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

namespace {
QString normalizar_caminho(const QString& caminho) {
    if (caminho.trimmed().isEmpty()) return QString();
    return QDir::toNativeSeparators(QDir::cleanPath(caminho)).replace('/', '\\');
}
}

bool remover_exclusao(QObject* interfaceMonitor, const QString& nome, const QString& dir_anterior, const QString& exclusao_id) {
    try {
        if (!interfaceMonitor) return false;
        if (!interfaceMonitor->property("_aceitar_eventos_monitoramento").toBool() ||
            interfaceMonitor->property("_encerrando_interface").toBool()) {
            return false;
        }

        auto* evento_base = interfaceMonitor->property("evento_base").value<source::services::BaseEvento*>();
        if (!evento_base) {
            const qulonglong obs_ptr = interfaceMonitor->property("_observador_cpp_ptr").toULongLong();
            if (obs_ptr != 0) {
                auto* obs = reinterpret_cast<source::services::Observador*>(obs_ptr);
                if (obs && obs->evento_base) evento_base = obs->evento_base.get();
            }
        }
        auto* loc = interfaceMonitor->property("loc").value<source::ui::LocalizadorQt*>();
        if (!evento_base || !loc) return false;

        const QString connName = QStringLiteral("mov_rm_%1_%2")
                                     .arg(reinterpret_cast<quintptr>(interfaceMonitor))
                                     .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
        bool ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(evento_base->db_path);
            if (!db.open()) {
                ok = false;
            }

            if (ok) {
                QSqlQuery pragma(db);
                if (!pragma.exec("PRAGMA synchronous = OFF")) ok = false;
                if (ok && !pragma.exec("PRAGMA journal_mode = WAL")) ok = false;
                if (ok && !pragma.exec("BEGIN IMMEDIATE TRANSACTION")) ok = false;
            }

            const QStringList deleted_aliases =
                source::services::GerenciamentoBaseEvento::aliases_operacao(QStringLiteral("op_deleted"), loc);
            const QString deleted_placeholders =
                source::services::GerenciamentoBaseEvento::marcadores_sql(deleted_aliases.size());
            const QString dir_normalizado = normalizar_caminho(dir_anterior);
            const QString dir_alternativo = QString(dir_normalizado).replace('\\', '/');
            if (ok) {
                int monitoramento_id = -1;
                if (!exclusao_id.trimmed().isEmpty()) {
                    QSqlQuery qsel_ts(db);
                    qsel_ts.prepare(QString("SELECT id FROM monitoramento WHERE tipo_operacao IN (%1) AND nome = ? AND timestamp = ? ORDER BY id DESC LIMIT 1")
                                        .arg(deleted_placeholders));
                    for (const QString& alias : deleted_aliases) qsel_ts.addBindValue(alias);
                    qsel_ts.addBindValue(nome);
                    qsel_ts.addBindValue(exclusao_id.trimmed());
                    ok = qsel_ts.exec();
                    if (ok && qsel_ts.next()) {
                        monitoramento_id = qsel_ts.value(0).toInt();
                    }
                }
                {
                    if (ok && monitoramento_id == -1) {
                        QSqlQuery qsel(db);
                        qsel.prepare(
                            QString("SELECT id FROM monitoramento "
                                    "WHERE tipo_operacao IN (%1) AND nome = ? "
                                    "AND (dir_anterior = ? OR REPLACE(dir_anterior, '/', '\\') = ? OR REPLACE(dir_anterior, '\\', '/') = ?) "
                                    "ORDER BY id DESC LIMIT 1")
                                .arg(deleted_placeholders));
                        for (const QString& alias : deleted_aliases) qsel.addBindValue(alias);
                        qsel.addBindValue(nome);
                        qsel.addBindValue(dir_normalizado);
                        qsel.addBindValue(dir_normalizado);
                        qsel.addBindValue(dir_alternativo);
                        ok = qsel.exec();
                        if (ok && qsel.next()) {
                            monitoramento_id = qsel.value(0).toInt();
                        }
                    }
                }

                if (ok && monitoramento_id != -1) {
                    QSqlQuery del1(db);
                    del1.prepare("DELETE FROM monitoramento WHERE id = ?");
                    del1.addBindValue(monitoramento_id);
                    ok = del1.exec();
                }

                if (ok) {
                    int excluido_id = -1;
                    if (!exclusao_id.trimmed().isEmpty()) {
                        QSqlQuery q2_ts(db);
                        q2_ts.prepare(QString("SELECT id FROM excluido WHERE tipo_operacao IN (%1) AND nome = ? AND timestamp = ? ORDER BY id DESC LIMIT 1")
                                           .arg(deleted_placeholders));
                        for (const QString& alias : deleted_aliases) q2_ts.addBindValue(alias);
                        q2_ts.addBindValue(nome);
                        q2_ts.addBindValue(exclusao_id.trimmed());
                        ok = q2_ts.exec();
                        if (ok && q2_ts.next()) {
                            excluido_id = q2_ts.value(0).toInt();
                        }
                    }
                    if (ok && excluido_id == -1) {
                        QSqlQuery q2(db);
                        q2.prepare(
                            QString("SELECT id FROM excluido "
                                    "WHERE tipo_operacao IN (%1) AND nome = ? "
                                    "AND (dir_anterior = ? OR REPLACE(dir_anterior, '/', '\\') = ? OR REPLACE(dir_anterior, '\\', '/') = ?) "
                                    "ORDER BY id DESC LIMIT 1")
                                .arg(deleted_placeholders));
                        for (const QString& alias : deleted_aliases) q2.addBindValue(alias);
                        q2.addBindValue(nome);
                        q2.addBindValue(dir_normalizado);
                        q2.addBindValue(dir_normalizado);
                        q2.addBindValue(dir_alternativo);
                        ok = q2.exec();
                        if (ok && q2.next()) {
                            excluido_id = q2.value(0).toInt();
                        }
                    }

                    if (ok && excluido_id != -1) {
                        QSqlQuery del2(db);
                        del2.prepare("DELETE FROM excluido WHERE id = ?");
                        del2.addBindValue(excluido_id);
                        ok = del2.exec();
                    }
                }
            }

            {
                QSqlQuery tx(db);
                if (ok) ok = tx.exec("COMMIT");
                if (!ok) tx.exec("ROLLBACK");
            }

            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(connName);

        if (!ok) {
            source::utils::LogManager::get_logger().error("Erro SQL ao remover exclusão", true);
            return false;
        }

        QTimer::singleShot(0, interfaceMonitor, [interfaceMonitor]() { atualizar_linha_recente(interfaceMonitor); });
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao remover registro de exclusão", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
