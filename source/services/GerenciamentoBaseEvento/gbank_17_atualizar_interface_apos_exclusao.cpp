#include "gbank_17_atualizar_interface_apos_exclusao.h"

#include <QMetaObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_upd_exc_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
QVariantMap row_to_map(const QSqlQuery& query) {
    QVariantMap out;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) out.insert(rec.fieldName(i), query.value(i));
    return out;
}
}

namespace source::services::GerenciamentoBaseEvento {

void _atualizar_interface_apos_exclusao(source::services::BaseEvento* self, const QVariantMap& evento) {
    try {
        if (!self) return;
        self->eventos_excluidos += 1;

        QObject* interface = nullptr;
        if (self->callback.isValid()) interface = self->callback.value<QObject*>();
        if (!interface && self->observador) interface = self->observador->interface;
        if (!interface) return;

        if (!evento.isEmpty()) {
            if (QMetaObject::invokeMethod(interface, "inserir_evento_streaming", Qt::QueuedConnection, Q_ARG(QVariantMap, evento))) return;
        }

        const QString connName = conn_name(self->db_path);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (db.open()) {
                {
                    QSqlQuery q(db);
                    const QStringList aliasesDeleted =
                        aliases_operacao(QStringLiteral("op_deleted"),
                                         self->observador ? self->observador->loc : nullptr);
                    q.prepare(QString("SELECT * FROM monitoramento WHERE tipo_operacao IN (%1) ORDER BY id DESC LIMIT 1")
                                  .arg(marcadores_sql(aliasesDeleted.size())));
                    for (const QString& alias : aliasesDeleted) q.addBindValue(alias);
                    q.exec();
                    if (q.next()) {
                        QVariantMap ev = row_to_map(q);
                        QObject* gerenciador_tabela = interface->property("gerenciador_tabela").value<QObject*>();
                        QObject* tabela_dados = interface->property("tabela_dados").value<QObject*>();
                        if (gerenciador_tabela && tabela_dados) {
                            QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_linha_mais_recente", Qt::QueuedConnection,
                                                      Q_ARG(QObject*, tabela_dados), Q_ARG(QVariantMap, ev));
                        }
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(connName);

        QMetaObject::invokeMethod(interface, "atualizar_status", Qt::QueuedConnection);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface após exclusão", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
