#include "gscanner_05_processar_item.h"

#include <chrono>

#include <QDateTime>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString conn_name(const QString& db) { return QStringLiteral("scanner_item_fallback_%1").arg(reinterpret_cast<quintptr>(db.constData())); }
QString current_timestamp_microseconds() {
    const auto now = std::chrono::system_clock::now();
    const qint64 micros_total =
        std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    const qint64 seconds = micros_total / 1000000;
    const qint64 micros = micros_total % 1000000;
    const QDateTime dt = QDateTime::fromSecsSinceEpoch(seconds);
    return dt.toString("yyyy-MM-dd HH:mm:ss.") + QStringLiteral("%1").arg(micros, 6, 10, QLatin1Char('0'));
}

QVariantMap build_evento(source::services::DiretorioScanner* self, const QString& nome, const QString& caminho, const QString& tipo, const QVariantMap& m) {
    return {
        {"tipo_operacao", QStringLiteral("op_scanned")},
        {"nome", nome}, {"dir_anterior", ""}, {"dir_atual", caminho},
        {"data_criacao", m.value("data_criacao", "")}, {"data_modificacao", m.value("data_modificacao", "")}, {"data_acesso", m.value("data_acesso", "")},
        {"tipo", tipo},
        {"size_b", m.value("size_b", "")}, {"size_kb", m.value("size_kb", "")}, {"size_mb", m.value("size_mb", "")}, {"size_gb", m.value("size_gb", "")}, {"size_tb", m.value("size_tb", "")},
        {"atributos", m.value("atributos", "")}, {"autor", m.value("autor", "")}, {"dimensoes", m.value("dimensoes", "")}, {"duracao", m.value("duracao", "")},
        {"taxa_bits", m.value("taxa_bits", "")}, {"protegido", m.value("protegido", "")},
        {"paginas", m.value("paginas", "")}, {"linhas", m.value("linhas", "")}, {"palavras", m.value("palavras", "")}, {"paginas_estimadas", m.value("paginas_estimadas", "")},
        {"linhas_codigo", m.value("linhas_codigo", "")}, {"total_linhas", m.value("total_linhas", "")}, {"slides_estimados", m.value("slides_estimados", "")}, {"arquivos", m.value("arquivos", "")},
        {"unzipped_b", m.value("unzipped_b", "")}, {"unzipped_kb", m.value("unzipped_kb", "")}, {"unzipped_mb", m.value("unzipped_mb", "")}, {"unzipped_gb", m.value("unzipped_gb", "")}, {"unzipped_tb", m.value("unzipped_tb", "")},
        {"slides", m.value("slides", "")}, {"binary_file_b", m.value("binary_file_b", "")}, {"binary_file_kb", m.value("binary_file_kb", "")}, {"binary_file_mb", m.value("binary_file_mb", "")}, {"binary_file_gb", m.value("binary_file_gb", "")}, {"binary_file_tb", m.value("binary_file_tb", "")},
        {"planilhas", m.value("planilhas", "")}, {"colunas", m.value("colunas", "")}, {"registros", m.value("registros", "")}, {"tabelas", m.value("tabelas", "")},
        {"timestamp", current_timestamp_microseconds()}
    };
}
}

namespace source::services::GerenciamentoDiretorioScanner {

void _processar_item(source::services::DiretorioScanner* self, const QString& nome, const QString& caminho, const QString& tipo_in) {
    try {
        if (!self) return;

        QVariantMap item{{"nome", nome}, {"dir_atual", caminho}};
        self->current_item = item;
        QVariantMap metadados;
        QString tipo = tipo_in;
        if (!caminho.isEmpty() && QFileInfo::exists(caminho) && self->gerenciador_colunas) {
            auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->gerenciador_colunas);
            if (gc) metadados = gc->get_metadados(item);
            tipo = self->get_file_type(caminho);
        } else {
            tipo.clear();
            metadados.clear();
        }

        QVariantMap evento = build_evento(self, nome, caminho, tipo, metadados);
        try {
            if (self->observador && self->observador->evento_base) {
                self->observador->evento_base->registrar_evento_no_banco(evento);
            }
        } catch (...) {
            const QString connName = conn_name(self->db_path + nome);
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
            db.setDatabaseName(self->db_path);
            if (db.open()) {
                const QStringList colunas = {"tipo_operacao","nome","dir_anterior","dir_atual","data_criacao","data_modificacao","data_acesso","tipo","size_b","size_kb","size_mb","size_gb","size_tb","atributos","autor","dimensoes","duracao","taxa_bits","protegido","paginas","linhas","palavras","paginas_estimadas","linhas_codigo","total_linhas","slides_estimados","arquivos","unzipped_b","unzipped_kb","unzipped_mb","unzipped_gb","unzipped_tb","slides","binary_file_b","binary_file_kb","binary_file_mb","binary_file_gb","binary_file_tb","planilhas","colunas","registros","tabelas","timestamp"};
                QString ph;
                for (int i = 0; i < colunas.size(); ++i) ph += (i ? ",?" : "?");
                QSqlQuery q(db);
                q.prepare(QString("INSERT INTO monitoramento (%1) VALUES (%2)").arg(colunas.join(','), ph));
                for (const QString& c : colunas) q.addBindValue(evento.value(c, ""));
                q.exec();
                db.close();
            }
            QSqlDatabase::removeDatabase(connName);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar item", true);
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
