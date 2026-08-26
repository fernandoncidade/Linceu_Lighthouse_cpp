#include "gbank_18_processar_eventos_movimentacao.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/security/secore_03_evento_campos.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/GerenciamentoBaseEvento/gbank_19_inserir_evento_movido.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) {
    return QStringLiteral("baseevento_mov_lote_%1_%2")
        .arg(reinterpret_cast<quintptr>(db.constData()))
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}
}

namespace source::services::GerenciamentoBaseEvento {

void processar_eventos_movimentacao(source::services::BaseEvento* self, const QList<QVariantMap>& eventos_movidos, const std::function<void()>& callback) {
    if (!self || eventos_movidos.isEmpty()) return;

    QSqlDatabase db;
    QString connName;
    try {
        connName = conn_name(self->db_path);
        db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return;
        }

        QSqlQuery q(db);
        q.exec("PRAGMA synchronous = OFF");
        db.transaction();

        const QStringList colunas = source::security::core::colunas_evento_monitoramento();
        QString ph;
        for (int i = 0; i < colunas.size(); ++i) ph += (i ? ",?" : "?");

        const QString opMoved = QStringLiteral("op_moved");

        QSqlQuery insMov(db);
        insMov.prepare(QString("INSERT INTO movido (%1) VALUES (%2)").arg(colunas.join(','), ph));
        QSqlQuery insMon(db);
        insMon.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(colunas.join(','), ph));

        for (const QVariantMap& evento : eventos_movidos) {
            if (!operacao_corresponde(evento.value("tipo_operacao").toString(),
                                      QStringLiteral("op_moved"),
                                      self->observador ? self->observador->loc : nullptr)) {
                continue;
            }
            QVariantMap ev = evento;
            ev["tipo_operacao"] = opMoved;
            if (!ev.contains("timestamp")) ev["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            ev = preparar_evento_movido(self, ev);
            if (self->observador != nullptr && self->observador->interface != nullptr) {
                if (auto* security_manager = source::security::ui::get_security_manager(
                        self->observador->interface);
                    security_manager != nullptr) {
                    ev = security_manager->enrich_event(ev);
                }
            }
            source::security::core::garantir_campos_evento_monitoramento(ev);

            insMov.finish();
            insMov.prepare(QString("INSERT INTO movido (%1) VALUES (%2)").arg(colunas.join(','), ph));
            for (const QString& c : colunas) {
                if (c == "tipo_operacao") insMov.addBindValue(opMoved);
                else insMov.addBindValue(ev.value(c));
            }
            insMov.exec();

            insMon.finish();
            insMon.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(colunas.join(','), ph));
            for (const QString& c : colunas) {
                if (c == "tipo_operacao") insMon.addBindValue(opMoved);
                else insMon.addBindValue(ev.value(c));
            }
            insMon.exec();
        }

        db.commit();
        q.exec("PRAGMA synchronous = NORMAL");

        db.close();
        QSqlDatabase::removeDatabase(connName);

        if (callback) callback();
    } catch (...) {
        if (db.isValid() && db.isOpen()) {
            db.rollback();
            QSqlQuery pragma(db);
            pragma.exec("PRAGMA synchronous = NORMAL");
            db.close();
        }
        if (!connName.isEmpty()) {
            QSqlDatabase::removeDatabase(connName);
        }
        source::utils::LogManager::get_logger().error("Erro ao processar lote de eventos movidos", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
