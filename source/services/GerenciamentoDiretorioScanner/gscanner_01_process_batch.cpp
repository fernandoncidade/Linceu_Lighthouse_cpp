#include "gscanner_01_process_batch.h"

#include <chrono>

#include <QDateTime>
#include <QFileInfo>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString current_timestamp_microseconds() {
    const auto now = std::chrono::system_clock::now();
    const qint64 micros_total =
        std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    const qint64 seconds = micros_total / 1000000;
    const qint64 micros = micros_total % 1000000;
    const QDateTime dt = QDateTime::fromSecsSinceEpoch(seconds);
    return dt.toString("yyyy-MM-dd HH:mm:ss.") + QStringLiteral("%1").arg(micros, 6, 10, QLatin1Char('0'));
}

QVariantMap build_evento_scan(source::services::DiretorioScanner* self, const QString& nome, const QString& caminho, const QString& tipo, const QVariantMap& m) {
    return {
        {"tipo_operacao", QStringLiteral("op_scanned")},
        {"nome", nome}, {"dir_anterior", ""}, {"dir_atual", caminho},
        {"data_criacao", m.value("data_criacao", "")}, {"data_modificacao", m.value("data_modificacao", "")}, {"data_acesso", m.value("data_acesso", "")},
        {"tipo", tipo},
        {"mime", m.value("mime", "")},
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

void _process_batch(source::services::DiretorioScanner* self, const QList<QVariantList>& batch) {
    try {
        if (!self) return;

        for (const QVariantList& row : batch) {
            try {
                const QString nome = row.value(0).toString();
                const QString caminho = row.value(1).toString();

                const QString tipo = (!caminho.isEmpty() && QFileInfo::exists(caminho)) ? self->get_file_type(caminho) : QString();
                QVariantMap item{{"nome", nome}, {"dir_atual", caminho}};
                self->current_item = item;
                QVariantMap metadados;
                if (!caminho.isEmpty() && QFileInfo::exists(caminho) && self->gerenciador_colunas) {
                    auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->gerenciador_colunas);
                    if (gc) metadados = gc->get_metadados(item);
                }

                QVariantMap evento = build_evento_scan(self, nome, caminho, tipo, metadados);
                if (self->observador && self->observador->evento_base) {
                    self->observador->evento_base->registrar_evento_no_banco(evento);
                }

                self->contador_processados += 1;
                if (self->contador_processados % self->intervalo_atualizacao == 0) {
                    self->_atualizar_progresso();
                }
            } catch (...) {
                source::utils::LogManager::get_logger().error("Erro ao processar item do lote", true);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar lote", true);
        throw;
    }
}

}  // namespace source::services::GerenciamentoDiretorioScanner
