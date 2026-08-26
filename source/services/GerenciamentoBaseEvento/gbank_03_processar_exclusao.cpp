#include "gbank_03_processar_exclusao.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUuid>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/security/secore_03_evento_campos.h"
#include "source/services/GerenciamentoBaseEvento/gbank_00_operacoes.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name() {
    return QStringLiteral("baseevento_exclusao_%1")
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}
QVariantMap row_to_map(const QSqlQuery& query) {
    QVariantMap out;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) out.insert(rec.fieldName(i), query.value(i));
    return out;
}
}

namespace source::services::GerenciamentoBaseEvento {

void processar_exclusao(source::services::BaseEvento* self, const QVariantMap& evento) {
    QString connName;
    try {
        if (!self) return;

        const QString nome = evento.value("nome").toString();
        connName = conn_name();
        bool ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (!db.open()) {
                ok = false;
            }

            if (ok) {
                QVariantMap meta;
                const QString caminho = evento.value("dir_anterior").toString();
                {
                    if (!caminho.isEmpty()) {
                        QSqlQuery q_path(db);
                        q_path.prepare("SELECT m.tipo_operacao,m.nome,m.dir_anterior,m.dir_atual,m.data_criacao,m.data_modificacao,m.data_acesso,m.tipo,m.size_b,m.size_kb,m.size_mb,m.size_gb,m.size_tb,m.atributos,m.autor,m.dimensoes,m.duracao,m.taxa_bits,m.protegido,m.paginas,m.linhas,m.palavras,m.paginas_estimadas,m.linhas_codigo,m.total_linhas,m.slides_estimados,m.arquivos,m.unzipped_b,m.unzipped_kb,m.unzipped_mb,m.unzipped_gb,m.unzipped_tb,m.slides,m.binary_file_b,m.binary_file_kb,m.binary_file_mb,m.binary_file_gb,m.binary_file_tb,m.planilhas,m.colunas,m.registros,m.tabelas,m.timestamp FROM monitoramento m WHERE (m.dir_atual = ? OR m.dir_anterior = ?) UNION ALL SELECT NULL as tipo_operacao,s.nome,s.diretorio as dir_anterior,s.diretorio as dir_atual,s.data_criacao,s.data_modificacao,s.data_acesso,s.tipo,s.size_b,s.size_kb,s.size_mb,s.size_gb,s.size_tb,s.atributos,s.autor,s.dimensoes,s.duracao,s.taxa_bits,s.protegido,s.paginas,s.linhas,s.palavras,s.paginas_estimadas,s.linhas_codigo,s.total_linhas,s.slides_estimados,s.arquivos,s.unzipped_b,s.unzipped_kb,s.unzipped_mb,s.unzipped_gb,s.unzipped_tb,s.slides,s.binary_file_b,s.binary_file_kb,s.binary_file_mb,s.binary_file_gb,s.binary_file_tb,s.planilhas,s.colunas,s.registros,s.tabelas,s.timestamp FROM snapshot s WHERE s.diretorio=? ORDER BY timestamp DESC LIMIT 1");
                        q_path.addBindValue(caminho);
                        q_path.addBindValue(caminho);
                        q_path.addBindValue(caminho);
                        if (q_path.exec() && q_path.next()) {
                            meta = row_to_map(q_path);
                        }
                    }
                }

                if (meta.isEmpty()) {
                    QSqlQuery q(db);
                    q.prepare("SELECT m.tipo_operacao,m.nome,m.dir_anterior,m.dir_atual,m.data_criacao,m.data_modificacao,m.data_acesso,m.tipo,m.size_b,m.size_kb,m.size_mb,m.size_gb,m.size_tb,m.atributos,m.autor,m.dimensoes,m.duracao,m.taxa_bits,m.protegido,m.paginas,m.linhas,m.palavras,m.paginas_estimadas,m.linhas_codigo,m.total_linhas,m.slides_estimados,m.arquivos,m.unzipped_b,m.unzipped_kb,m.unzipped_mb,m.unzipped_gb,m.unzipped_tb,m.slides,m.binary_file_b,m.binary_file_kb,m.binary_file_mb,m.binary_file_gb,m.binary_file_tb,m.planilhas,m.colunas,m.registros,m.tabelas,m.timestamp FROM monitoramento m WHERE m.nome=? UNION ALL SELECT NULL as tipo_operacao,s.nome,s.diretorio as dir_anterior,s.diretorio as dir_atual,s.data_criacao,s.data_modificacao,s.data_acesso,s.tipo,s.size_b,s.size_kb,s.size_mb,s.size_gb,s.size_tb,s.atributos,s.autor,s.dimensoes,s.duracao,s.taxa_bits,s.protegido,s.paginas,s.linhas,s.palavras,s.paginas_estimadas,s.linhas_codigo,s.total_linhas,s.slides_estimados,s.arquivos,s.unzipped_b,s.unzipped_kb,s.unzipped_mb,s.unzipped_gb,s.unzipped_tb,s.slides,s.binary_file_b,s.binary_file_kb,s.binary_file_mb,s.binary_file_gb,s.binary_file_tb,s.planilhas,s.colunas,s.registros,s.tabelas,s.timestamp FROM snapshot s WHERE s.nome=? ORDER BY timestamp DESC LIMIT 1");
                    q.addBindValue(nome);
                    q.addBindValue(nome);
                    if (q.exec() && q.next()) meta = row_to_map(q);
                }

                if (meta.isEmpty()) {
                    meta.insert("nome", nome);
                    meta.insert("dir_anterior", evento.value("dir_anterior").toString());
                    meta.insert("dir_atual", "");
                    const QString tipo = (self->observador && self->observador->gerenciador_colunas)
                                             ? self->observador->gerenciador_colunas->identificar_tipo_arquivo(nome)
                                             : QString();
                    meta.insert("tipo", tipo);
                    const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                    meta.insert("data_criacao", now);
                    meta.insert("data_modificacao", now);
                    meta.insert("data_acesso", now);
                    meta.insert("size_b", "");
                    meta.insert("size_kb", "");
                    meta.insert("size_mb", "");
                    meta.insert("size_gb", "");
                    meta.insert("size_tb", "");
                    meta.insert("atributos", "");
                    meta.insert("autor", "");
                    meta.insert("dimensoes", "");
                    meta.insert("duracao", "");
                    meta.insert("taxa_bits", "");
                    meta.insert("protegido", "");
                    meta.insert("paginas", "");
                    meta.insert("linhas", "");
                    meta.insert("palavras", "");
                    meta.insert("paginas_estimadas", "");
                    meta.insert("linhas_codigo", "");
                    meta.insert("total_linhas", "");
                    meta.insert("slides_estimados", "");
                    meta.insert("arquivos", "");
                    meta.insert("unzipped_b", "");
                    meta.insert("unzipped_kb", "");
                    meta.insert("unzipped_mb", "");
                    meta.insert("unzipped_gb", "");
                    meta.insert("unzipped_tb", "");
                    meta.insert("slides", "");
                    meta.insert("binary_file_b", "");
                    meta.insert("binary_file_kb", "");
                    meta.insert("binary_file_mb", "");
                    meta.insert("binary_file_gb", "");
                    meta.insert("binary_file_tb", "");
                    meta.insert("planilhas", "");
                    meta.insert("colunas", "");
                    meta.insert("registros", "");
                    meta.insert("tabelas", "");
                }

                const QString timestamp_from_event = evento.value("timestamp").toString().trimmed();
                const QString nowMicro = timestamp_from_event.isEmpty()
                                             ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")
                                             : timestamp_from_event;
                meta["tipo_operacao"] = QStringLiteral("op_deleted");
                meta["dir_anterior"] = evento.value("dir_anterior");
                meta["dir_atual"] = "";
                meta["timestamp"] = nowMicro;

                if (self->observador != nullptr && self->observador->interface != nullptr) {
                    if (auto* security_manager = source::security::ui::get_security_manager(
                            self->observador->interface);
                        security_manager != nullptr) {
                        meta = security_manager->enrich_event(meta);
                    }
                }
                source::security::core::garantir_campos_evento_monitoramento(meta);

                const QStringList cols = source::security::core::colunas_evento_monitoramento();
                QString placeholders;
                for (int i = 0; i < cols.size(); ++i) placeholders += (i ? ",?" : "?");

                ok = db.transaction();
                if (ok) {
                    QSqlQuery ins(db);
                    ins.prepare(QString("INSERT INTO excluido (%1) VALUES (%2)").arg(cols.join(','), placeholders));
                    for (const QString& c : cols) ins.addBindValue(meta.value(c));
                    ok = ins.exec();

                    if (ok) {
                        QSqlQuery ins2(db);
                        ins2.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(cols.join(','), placeholders));
                        for (const QString& c : cols) ins2.addBindValue(meta.value(c));
                        ok = ins2.exec();
                    }
                }

                if (ok) {
                    ok = db.commit();
                } else {
                    db.rollback();
                }

                if (ok && !evento.value("_temporario", false).toBool()) {
                    self->_atualizar_interface_apos_exclusao(meta);
                }
            }

            db.close();
        }

        QSqlDatabase::removeDatabase(connName);

        if (!ok) {
            source::utils::LogManager::get_logger().error("Erro ao processar exclusão (SQL)", true);
        }
    } catch (...) {
        if (!connName.isEmpty()) {
            QSqlDatabase::removeDatabase(connName);
        }
        source::utils::LogManager::get_logger().error("Erro ao processar exclusão", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
