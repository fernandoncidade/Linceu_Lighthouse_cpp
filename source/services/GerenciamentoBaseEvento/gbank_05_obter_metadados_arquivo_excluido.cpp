#include "gbank_05_obter_metadados_arquivo_excluido.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_meta_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
QVariantMap row_to_map(const QSqlQuery& query) {
    QVariantMap out;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) out.insert(rec.fieldName(i), query.value(i));
    return out;
}

QString metadata_priority_sql() {
    return QStringLiteral(
        "(CASE WHEN CAST(COALESCE(size_b,0) AS REAL) > 0 OR "
        "CAST(COALESCE(size_kb,0) AS REAL) > 0 OR "
        "CAST(COALESCE(size_mb,0) AS REAL) > 0 OR "
        "CAST(COALESCE(size_gb,0) AS REAL) > 0 OR "
        "CAST(COALESCE(size_tb,0) AS REAL) > 0 THEN 100 ELSE 0 END) + "
        "(CASE WHEN COALESCE(dimensoes,'') <> '' THEN 10 ELSE 0 END) + "
        "(CASE WHEN COALESCE(duracao,'') <> '' THEN 10 ELSE 0 END) + "
        "(CASE WHEN COALESCE(taxa_bits,'') <> '' THEN 8 ELSE 0 END) + "
        "(CASE WHEN COALESCE(paginas,'') <> '' THEN 6 ELSE 0 END) + "
        "(CASE WHEN COALESCE(linhas,'') <> '' THEN 6 ELSE 0 END) + "
        "(CASE WHEN COALESCE(palavras,'') <> '' THEN 6 ELSE 0 END) + "
        "(CASE WHEN COALESCE(tipo,'') <> '' THEN 4 ELSE 0 END) + "
        "(CASE WHEN COALESCE(atributos,'') <> '' THEN 2 ELSE 0 END) + "
        "(CASE WHEN COALESCE(autor,'') <> '' THEN 2 ELSE 0 END)");
}
}

namespace source::services::GerenciamentoBaseEvento {

QVariantMap obter_metadados_arquivo_excluido(source::services::BaseEvento* self, const QString& nome, const QString& caminho) {
    try {
        if (!self || (nome.isEmpty() && caminho.isEmpty())) return {};

        QVariantMap resultado;

        const QString connName = conn_name(self->db_path + nome);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (!db.open()) {
                QSqlDatabase::removeDatabase(connName);
                return {};
            }

            {
                if (!caminho.isEmpty()) {
                    QSqlQuery q_path(db);
                    q_path.prepare(QStringLiteral(
                                       "SELECT tipo_operacao,nome,dir_anterior,dir_atual,data_criacao,data_modificacao,data_acesso,"
                                       "tipo,size_b,size_kb,size_mb,size_gb,size_tb,atributos,autor,dimensoes,duracao,"
                                       "taxa_bits,protegido,paginas,linhas,palavras,paginas_estimadas,linhas_codigo,"
                                       "total_linhas,slides_estimados,arquivos,unzipped_b,unzipped_kb,unzipped_mb,"
                                       "unzipped_gb,unzipped_tb,slides,binary_file_b,binary_file_kb,binary_file_mb,"
                                       "binary_file_gb,binary_file_tb,planilhas,colunas,registros,tabelas,timestamp "
                                       "FROM monitoramento WHERE (dir_atual = ? OR dir_anterior = ?) "
                                       "ORDER BY %1 DESC, id DESC LIMIT 1")
                                       .arg(metadata_priority_sql()));
                    q_path.addBindValue(caminho);
                    q_path.addBindValue(caminho);
                    q_path.exec();
                    if (q_path.next()) {
                        resultado = row_to_map(q_path);
                    }
                }

                if (resultado.isEmpty() && !nome.isEmpty()) {
                    QSqlQuery q(db);
                    q.prepare(QStringLiteral(
                                  "SELECT tipo_operacao,nome,dir_anterior,dir_atual,data_criacao,data_modificacao,data_acesso,"
                                  "tipo,size_b,size_kb,size_mb,size_gb,size_tb,atributos,autor,dimensoes,duracao,"
                                  "taxa_bits,protegido,paginas,linhas,palavras,paginas_estimadas,linhas_codigo,"
                                  "total_linhas,slides_estimados,arquivos,unzipped_b,unzipped_kb,unzipped_mb,"
                                  "unzipped_gb,unzipped_tb,slides,binary_file_b,binary_file_kb,binary_file_mb,"
                                  "binary_file_gb,binary_file_tb,planilhas,colunas,registros,tabelas,timestamp "
                                  "FROM monitoramento WHERE nome=? ORDER BY %1 DESC, id DESC LIMIT 1")
                                  .arg(metadata_priority_sql()));
                    q.addBindValue(nome);
                    q.exec();
                    if (q.next()) {
                        resultado = row_to_map(q);
                    }
                }
            }

            if (resultado.isEmpty()) {
                if (!caminho.isEmpty()) {
                    QSqlQuery q2_path(db);
                    q2_path.prepare(QStringLiteral(
                                        "SELECT NULL as tipo_operacao,nome,diretorio as dir_anterior,diretorio as dir_atual,"
                                        "data_criacao,data_modificacao,data_acesso,tipo,size_b,size_kb,size_mb,size_gb,"
                                        "size_tb,atributos,autor,dimensoes,duracao,taxa_bits,protegido,paginas,linhas,"
                                        "palavras,paginas_estimadas,linhas_codigo,total_linhas,slides_estimados,arquivos,"
                                        "unzipped_b,unzipped_kb,unzipped_mb,unzipped_gb,unzipped_tb,slides,binary_file_b,"
                                        "binary_file_kb,binary_file_mb,binary_file_gb,binary_file_tb,planilhas,colunas,"
                                        "registros,tabelas,timestamp FROM snapshot WHERE diretorio=? "
                                        "ORDER BY %1 DESC, id DESC LIMIT 1")
                                        .arg(metadata_priority_sql()));
                    q2_path.addBindValue(caminho);
                    q2_path.exec();
                    if (q2_path.next()) {
                        resultado = row_to_map(q2_path);
                    }
                }

                if (resultado.isEmpty() && !nome.isEmpty()) {
                    QSqlQuery q2(db);
                    q2.prepare(QStringLiteral(
                                  "SELECT NULL as tipo_operacao,nome,diretorio as dir_anterior,diretorio as dir_atual,"
                                  "data_criacao,data_modificacao,data_acesso,tipo,size_b,size_kb,size_mb,size_gb,"
                                  "size_tb,atributos,autor,dimensoes,duracao,taxa_bits,protegido,paginas,linhas,"
                                  "palavras,paginas_estimadas,linhas_codigo,total_linhas,slides_estimados,arquivos,"
                                  "unzipped_b,unzipped_kb,unzipped_mb,unzipped_gb,unzipped_tb,slides,binary_file_b,"
                                  "binary_file_kb,binary_file_mb,binary_file_gb,binary_file_tb,planilhas,colunas,"
                                  "registros,tabelas,timestamp FROM snapshot WHERE nome=? "
                                  "ORDER BY %1 DESC, id DESC LIMIT 1")
                                  .arg(metadata_priority_sql()));
                    q2.addBindValue(nome);
                    q2.exec();
                    if (q2.next()) {
                        resultado = row_to_map(q2);
                    }
                }
            }

            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
        return resultado;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao buscar metadados de arquivo excluído", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
