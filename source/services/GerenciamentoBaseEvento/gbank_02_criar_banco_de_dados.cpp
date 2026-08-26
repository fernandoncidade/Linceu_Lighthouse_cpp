#include "gbank_02_criar_banco_de_dados.h"

#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "source/security/secore_03_evento_campos.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("baseevento_create_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
}

namespace source::services::GerenciamentoBaseEvento {

void criar_banco_de_dados_path(const QString& db_path) {
    try {
        if (db_path.isEmpty()) return;

        QDir().mkpath(QFileInfo(db_path).absolutePath());

        const QString connName = conn_name(db_path);
        if (QSqlDatabase::contains(connName)) {
            QSqlDatabase::removeDatabase(connName);
        }

        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connName);
            db.setDatabaseName(db_path);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(QString("Erro ao abrir DB: %1").arg(db.lastError().text()), true);
                return;
            }

            QSqlQuery q(db);
            q.exec("PRAGMA journal_mode = WAL");
            q.exec("PRAGMA synchronous = NORMAL");
            q.exec("PRAGMA temp_store = MEMORY");
            q.exec("PRAGMA cache_size = -65536");
            q.exec("PRAGMA page_size = 4096");
            q.exec("PRAGMA mmap_size = 536870912");
            q.exec("PRAGMA busy_timeout = 30000");
            q.exec("PRAGMA auto_vacuum = NONE");
            q.exec("PRAGMA secure_delete = OFF");
            q.exec("PRAGMA read_uncommitted = ON");
            q.exec("PRAGMA count_changes = OFF");
            q.exec("PRAGMA foreign_keys = OFF");

            const QStringList tabelas_eventos = {"monitoramento", "adicionado", "excluido", "modificado", "renomeado", "movido", "escaneado"};
            const QList<QPair<QString, QString>> colunas =
                source::security::core::definicoes_colunas_evento_monitoramento();

            for (const QString& tabela : tabelas_eventos) {
                q.exec(QString("CREATE TABLE IF NOT EXISTS %1 (id INTEGER PRIMARY KEY AUTOINCREMENT)").arg(tabela));
                q.exec(QString("PRAGMA table_info(%1)").arg(tabela));
                QSet<QString> existentes;
                while (q.next()) existentes.insert(q.value(1).toString());
                for (const auto& col : colunas) {
                    if (!existentes.contains(col.first)) {
                        q.exec(QString("ALTER TABLE %1 ADD COLUMN %2 %3").arg(tabela, col.first, col.second));
                    }
                }
            }

            q.exec("CREATE TABLE IF NOT EXISTS snapshot (id INTEGER PRIMARY KEY AUTOINCREMENT, nome TEXT, diretorio TEXT, tipo TEXT, mime TEXT, data_criacao TEXT, data_modificacao TEXT, data_acesso TEXT, size_b REAL, size_kb REAL, size_mb REAL, size_gb REAL, size_tb REAL, atributos TEXT, autor TEXT, dimensoes TEXT, duracao TEXT, taxa_bits TEXT, protegido TEXT, paginas TEXT, linhas TEXT, palavras TEXT, paginas_estimadas TEXT, linhas_codigo TEXT, total_linhas TEXT, slides_estimados TEXT, arquivos TEXT, unzipped_b REAL, unzipped_kb REAL, unzipped_mb REAL, unzipped_gb REAL, unzipped_tb REAL, slides TEXT, binary_file_b REAL, binary_file_kb REAL, binary_file_mb REAL, binary_file_gb REAL, binary_file_tb REAL, planilhas TEXT, colunas TEXT, registros TEXT, tabelas TEXT, timestamp TEXT)");
            q.exec("PRAGMA table_info(snapshot)");
            QSet<QString> snapshot_existentes;
            while (q.next()) snapshot_existentes.insert(q.value(1).toString());
            if (!snapshot_existentes.contains(QStringLiteral("mime"))) {
                q.exec("ALTER TABLE snapshot ADD COLUMN mime TEXT");
            }

            for (const QString& tabela : tabelas_eventos + QStringList{"snapshot"}) {
                q.exec(QString("CREATE INDEX IF NOT EXISTS idx_%1_nome ON %1 (nome)").arg(tabela));
                if (tabela == "snapshot") q.exec(QString("CREATE INDEX IF NOT EXISTS idx_%1_diretorio ON %1 (diretorio)").arg(tabela));
                else q.exec(QString("CREATE INDEX IF NOT EXISTS idx_%1_dirs ON %1 (dir_anterior, dir_atual)").arg(tabela));
            }

            q.exec("CREATE INDEX IF NOT EXISTS idx_monitoramento_nome_id ON monitoramento(nome, id)");
            q.exec("CREATE INDEX IF NOT EXISTS idx_snapshot_nome_timestamp ON snapshot(nome, timestamp)");

            db.close();
        }
        QSqlDatabase::removeDatabase(connName);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar banco de dados", true);
    }
}

void criar_banco_de_dados(source::services::BaseEvento* self) {
    if (!self || self->db_path.isEmpty()) return;
    criar_banco_de_dados_path(self->db_path);
}

}  // namespace source::services::GerenciamentoBaseEvento
