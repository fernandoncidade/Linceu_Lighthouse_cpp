#include "gbank_13_notificar_evento.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMetaObject>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <QUuid>

#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

namespace {
bool interface_aceita_eventos(QObject* interface_obj) {
    if (!interface_obj) {
        return false;
    }

    if (interface_obj->property("_encerrando_interface").toBool()) {
        return false;
    }

    if (interface_obj->property("_aceitar_eventos_monitoramento").isValid()) {
        return interface_obj->property("_aceitar_eventos_monitoramento").toBool();
    }

    return true;
}

void garantir_sistema_evento_inicializado(QObject* interface_obj) {
    if (!interface_obj) return;

    const bool possui_processador = interface_obj->property("processador_evento").value<QObject*>() != nullptr;
    const bool possui_buffer = interface_obj->property("evento_buffer").value<QObject*>() != nullptr;
    const bool possui_timer_exclusao = interface_obj->property("exclusao_timer").value<QObject*>() != nullptr;
    if (possui_processador && possui_buffer && possui_timer_exclusao) {
        return;
    }

    if (QThread::currentThread() == interface_obj->thread()) {
        source::services::_inicializar_sistema_evento(interface_obj);
        return;
    }

    QMetaObject::invokeMethod(interface_obj,
                              [interface_obj]() { source::services::_inicializar_sistema_evento(interface_obj); },
                              Qt::BlockingQueuedConnection);
}

bool eh_sidecar_sqlite(const QString& valor) {
    const QString lower = valor.toLower();
    return lower.endsWith(QStringLiteral(".db-shm")) || lower.endsWith(QStringLiteral(".db-wal"));
}

QString normalizar_caminho_sql(const QString& caminho) {
    if (caminho.trimmed().isEmpty()) {
        return {};
    }
    return QDir::toNativeSeparators(QDir::cleanPath(caminho)).replace('/', '\\');
}

QStringList candidatos_unicos(const QStringList& valores) {
    QStringList saida;
    QSet<QString> vistos;
    for (const QString& valor : valores) {
        const QString trimmed = valor.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        const QString chave = trimmed.toLower();
        if (vistos.contains(chave)) {
            continue;
        }
        vistos.insert(chave);
        saida.append(trimmed);
    }
    return saida;
}

void remover_adicoes_sidecar_correlacionadas(source::services::BaseEvento* self,
                                             const QVariantMap& evento,
                                             const QString& operacao) {
    if (self == nullptr || self->db_path.trimmed().isEmpty()) {
        return;
    }
    if (operacao != QStringLiteral("op_renamed") && operacao != QStringLiteral("op_moved")) {
        return;
    }

    const QString nome = QFileInfo(evento.value(QStringLiteral("nome")).toString()).fileName();
    const QString dir_anterior = normalizar_caminho_sql(evento.value(QStringLiteral("dir_anterior")).toString());
    const QString dir_atual = normalizar_caminho_sql(evento.value(QStringLiteral("dir_atual")).toString());

    if (!eh_sidecar_sqlite(nome) && !eh_sidecar_sqlite(dir_anterior) && !eh_sidecar_sqlite(dir_atual)) {
        return;
    }

    const QStringList nomes = candidatos_unicos({
        nome,
        QFileInfo(dir_anterior).fileName(),
        QFileInfo(dir_atual).fileName()
    });
    const QStringList caminhos = candidatos_unicos({
        dir_anterior,
        dir_atual
    });
    if (nomes.isEmpty() && caminhos.isEmpty()) {
        return;
    }

    try {
        DatabaseWriter::get_instance(self->db_path)->wait_until_idle(2000);

        const QString connName = QStringLiteral("baseevento_rm_add_sidecar_%1")
                                     .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
            db.setDatabaseName(self->db_path);
            if (!db.open()) {
                db.close();
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA busy_timeout=5000"));
                db.transaction();

                auto* loc = self->observador ? self->observador->loc : nullptr;
                const QStringList aliases = aliases_operacao(QStringLiteral("op_added"), loc);
                const QString markers = marcadores_sql(aliases.size());

                const auto executar_delete = [&](const QString& tabela) {
                    bool ok = true;
                    if (caminhos.isEmpty()) {
                        for (const QString& nome_candidato : nomes) {
                            QSqlQuery q(db);
                            q.prepare(QStringLiteral("DELETE FROM %1 WHERE tipo_operacao IN (%2) AND nome = ?")
                                          .arg(tabela, markers));
                            for (const QString& alias : aliases) q.addBindValue(alias);
                            q.addBindValue(nome_candidato);
                            ok = q.exec() && ok;
                        }
                    }

                    for (const QString& caminho_candidato : caminhos) {
                        const QString caminho_alt = QString(caminho_candidato).replace('\\', '/');
                        QSqlQuery q(db);
                        q.prepare(QStringLiteral(
                                      "DELETE FROM %1 WHERE tipo_operacao IN (%2) AND "
                                      "(dir_atual = ? OR REPLACE(dir_atual, '/', '\\') = ? OR "
                                      "REPLACE(dir_atual, '\\', '/') = ?)")
                                      .arg(tabela, markers));
                        for (const QString& alias : aliases) q.addBindValue(alias);
                        q.addBindValue(caminho_candidato);
                        q.addBindValue(caminho_candidato);
                        q.addBindValue(caminho_alt);
                        ok = q.exec() && ok;
                    }
                    return ok;
                };

                const bool ok_monitoramento = executar_delete(QStringLiteral("monitoramento"));
                const bool ok_adicionado = executar_delete(QStringLiteral("adicionado"));

                if (ok_monitoramento && ok_adicionado) {
                    db.commit();
                } else {
                    db.rollback();
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(connName);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao remover adicoes correlacionadas de sidecar SQLite", true);
    }
}
}  // namespace

void notificar_evento(source::services::BaseEvento* self, const QString& tipo_operacao, const QString& nome, const QString& dir_anterior, const QString& dir_atual) {
    if (!self || !self->observador) return;
    if (!self->observador->ativo || self->observador->desligando) return;

    try {
        const QString nome_base = QFileInfo(nome).fileName();
        auto* loc = self->observador->loc;
        const QString operacao = chave_canonica_operacao(tipo_operacao, loc);
        if (operacao.isEmpty()) {
            return;
        }
        QVariantMap evento;

        if (operacao == QStringLiteral("op_deleted")) {
            evento = self->_criar_evento_exclusao(nome_base, dir_anterior);
            if (evento.isEmpty()) return;
            if (!evento.contains("timestamp") || evento.value("timestamp").toString().isEmpty()) {
                evento["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
            }

            QObject* interface_obj = self->observador->interface;
            if (interface_obj && interface_aceita_eventos(interface_obj)) {
                garantir_sistema_evento_inicializado(interface_obj);
                const QVariant evento_processado = source::services::verificar_movimentacao(interface_obj, evento);
                if (!evento_processado.isValid() || evento_processado.isNull()) {
                    // A ausência de match imediato pode ser um "move" em andamento.
                    // O evento fica apenas em memória e será confirmado depois por timeout
                    // (ou descartado quando reconciliado como "movido").
                    evento["_temporario"] = true;
                    return;
                }

                evento = evento_processado.toMap();
            }
        } else {
            evento = self->_criar_evento_padrao(operacao, nome_base, dir_anterior, dir_atual);
            if (evento.isEmpty()) return;

            QObject* interface_obj = self->observador->interface;
            if (interface_obj && interface_aceita_eventos(interface_obj)) {
                garantir_sistema_evento_inicializado(interface_obj);
                const QVariant evento_processado = source::services::verificar_movimentacao(interface_obj, evento);
                if (evento_processado.isValid() && !evento_processado.isNull()) {
                    evento = evento_processado.toMap();
                } else {
                    return;
                }
            }
        }
        if (!evento.isEmpty()) {
            const QString operacao_final = chave_canonica_operacao(evento.value("tipo_operacao").toString(), loc);
            remover_adicoes_sidecar_correlacionadas(self, evento, operacao_final);
            self->registrar_evento_no_banco(evento);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao notificar evento", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
