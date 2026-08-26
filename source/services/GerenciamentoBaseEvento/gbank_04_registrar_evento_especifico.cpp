#include "gbank_04_registrar_evento_especifico.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_reg_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

void registrar_evento_especifico(source::services::BaseEvento* self, const QString& tabela, const QVariantMap& evento) {
    try {
        if (!self || tabela.isEmpty()) return;

        const QString connName = conn_name(self->db_path + tabela);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(self->db_path);
        if (!db.open()) {
            QSqlDatabase::removeDatabase(connName);
            return;
        }

        const QStringList cols = {"nome","dir_anterior","dir_atual","data_criacao","data_modificacao","data_acesso","tipo","mime","size_b","size_kb","size_mb","size_gb","size_tb","atributos","autor","dimensoes","duracao","taxa_bits","protegido","paginas","linhas","palavras","paginas_estimadas","linhas_codigo","total_linhas","slides_estimados","arquivos","unzipped_b","unzipped_kb","unzipped_mb","unzipped_gb","unzipped_tb","slides","binary_file_b","binary_file_kb","binary_file_mb","binary_file_gb","binary_file_tb","planilhas","colunas","registros","tabelas","timestamp"};
        QString placeholders;
        for (int i = 0; i < cols.size(); ++i) placeholders += (i ? ",?" : "?");

        QSqlQuery q(db);
        q.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tabela, cols.join(','), placeholders));
        for (const QString& c : cols) {
            if (c == "timestamp") q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
            else q.addBindValue(evento.value(c));
        }
        q.exec();

        db.close();
        QSqlDatabase::removeDatabase(connName);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao registrar evento específico", true);
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
