#include "gbank_06_registrar_evento_no_banco.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/security/secore_03_evento_campos.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/GerenciamentoBaseEvento/gbank_19_inserir_evento_movido.h"
#include "gbank_20_db_writer.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_regdb_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

void registrar_evento_no_banco(source::services::BaseEvento* self, const QVariantMap& evento_in) {
    try {
        if (!self) return;
        QVariantMap evento = evento_in;

        if (!evento.contains("tipo_operacao")) {
            source::utils::LogManager::get_logger().error("Erro: Campo obrigatório 'tipo_operacao' ausente no evento", true);
            return;
        }
        if (!evento.contains("nome")) {
            source::utils::LogManager::get_logger().error("Erro: Campo obrigatório 'nome' ausente no evento", true);
            return;
        }

        if (evento.value("dir_anterior").toString().isEmpty() || evento.value("dir_anterior").toString() == ".") evento["dir_anterior"] = "";
        if (evento.value("dir_atual").toString().isEmpty() || evento.value("dir_atual").toString() == ".") evento["dir_atual"] = "";

        auto* loc = (self->observador != nullptr) ? self->observador->loc : nullptr;
        QString tipo = chave_canonica_operacao(evento.value("tipo_operacao").toString(), loc);
        if (tipo.isEmpty()) {
            source::utils::LogManager::get_logger().error(
                QString("Erro: Tipo de operação desconhecido: %1")
                    .arg(evento.value("tipo_operacao").toString()),
                true);
            return;
        }
        evento["tipo_operacao"] = tipo;

        if (operacao_corresponde(tipo, QStringLiteral("op_moved"), loc)) {
            evento = preparar_evento_movido(self, evento);
            evento["tipo_operacao"] = tipo;
        }

        if (self->observador != nullptr && self->observador->interface != nullptr) {
            if (auto* security_manager = source::security::ui::get_security_manager(
                    self->observador->interface);
                security_manager != nullptr) {
                evento = security_manager->enrich_event(evento);
            }
        }

        evento["tipo_operacao"] = tipo;
        const QString tabela = tabela_para_operacao(tipo, loc);
        if (tabela.isEmpty()) {
            source::utils::LogManager::get_logger().error(QString("Erro: Tipo de operação desconhecido: %1").arg(tipo), true);
            return;
        }

        if (!evento.contains("timestamp")) evento["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        source::security::core::garantir_campos_evento_monitoramento(evento);

        try {
            auto writer = DatabaseWriter::get_instance(self->db_path);
            writer->enqueue_event(tabela, evento);
        } catch (...) {
            const QString connName = conn_name(self->db_path + tabela);
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (db.open()) {
                const QStringList colunas = source::security::core::colunas_evento_monitoramento();
                QString ph;
                for (int i = 0; i < colunas.size(); ++i) ph += (i ? ",?" : "?");
                QSqlQuery q(db);
                q.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tabela, colunas.join(','), ph));
                for (const QString& c : colunas) q.addBindValue(evento.value(c));
                q.exec();

                QSqlQuery q2(db);
                q2.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(colunas.join(','), ph));
                for (const QString& c : colunas) q2.addBindValue(evento.value(c));
                q2.exec();
                db.close();
                QSqlDatabase::removeDatabase(connName);
            }
        }

        if (operacao_corresponde(tipo, QStringLiteral("op_deleted"), loc)) self->_atualizar_interface_apos_exclusao(evento);
        else self->_atualizar_interface_apos_evento(evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao registrar evento no banco", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
