#include "gbank_15_criar_evento_exclusao.h"

#include <QDateTime>

#include "source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoBaseEvento {

QVariantMap _criar_evento_exclusao(source::services::BaseEvento* self, const QString& nome_base, const QString& dir_anterior) {
    try {
        if (!self || !self->observador) return {};

        const QString event_timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

        QVariantMap metadados = self->obter_metadados_arquivo_excluido(nome_base, dir_anterior);
        QString tipo_arquivo;
        if (!metadados.isEmpty()) {
            return {
                {"tipo_operacao", QStringLiteral("op_deleted")},
                {"nome", nome_base}, {"dir_anterior", dir_anterior}, {"dir_atual", ""},
                {"timestamp", event_timestamp},
                {"data_criacao", metadados.value("data_criacao")}, {"data_modificacao", metadados.value("data_modificacao")},
                {"data_acesso", metadados.value("data_acesso")}, {"tipo", metadados.value("tipo")},
                {"size_b", metadados.value("size_b")}, {"size_kb", metadados.value("size_kb")}, {"size_mb", metadados.value("size_mb")},
                {"size_gb", metadados.value("size_gb")}, {"size_tb", metadados.value("size_tb")}, {"atributos", metadados.value("atributos")},
                {"autor", metadados.value("autor")}, {"dimensoes", metadados.value("dimensoes")}, {"duracao", metadados.value("duracao")},
                {"taxa_bits", metadados.value("taxa_bits")}, {"protegido", metadados.value("protegido")}, {"paginas", metadados.value("paginas")},
                {"linhas", metadados.value("linhas")}, {"palavras", metadados.value("palavras")}, {"paginas_estimadas", metadados.value("paginas_estimadas")},
                {"linhas_codigo", metadados.value("linhas_codigo")}, {"total_linhas", metadados.value("total_linhas")}, {"slides_estimados", metadados.value("slides_estimados")},
                {"arquivos", metadados.value("arquivos")}, {"unzipped_b", metadados.value("unzipped_b")}, {"unzipped_kb", metadados.value("unzipped_kb")},
                {"unzipped_mb", metadados.value("unzipped_mb")}, {"unzipped_gb", metadados.value("unzipped_gb")}, {"unzipped_tb", metadados.value("unzipped_tb")},
                {"slides", metadados.value("slides")}, {"binary_file_b", metadados.value("binary_file_b")}, {"binary_file_kb", metadados.value("binary_file_kb")},
                {"binary_file_mb", metadados.value("binary_file_mb")}, {"binary_file_gb", metadados.value("binary_file_gb")}, {"binary_file_tb", metadados.value("binary_file_tb")},
                {"planilhas", metadados.value("planilhas")}, {"colunas", metadados.value("colunas")}, {"registros", metadados.value("registros")}, {"tabelas", metadados.value("tabelas")}};
        }

        if (tipo_arquivo.isEmpty()) {
            auto* loc = self->observador->loc;
            tipo_arquivo = source::services::GerenciamentoMetadados::identificar_tipo_arquivo(QString(), loc, nome_base);
        }

        const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        return {
            {"tipo_operacao", QStringLiteral("op_deleted")},
            {"nome", nome_base}, {"dir_anterior", dir_anterior}, {"dir_atual", ""}, {"timestamp", event_timestamp},
            {"data_criacao", now}, {"data_modificacao", now}, {"data_acesso", now},
            {"tipo", tipo_arquivo}, {"size_b", ""}, {"size_kb", ""}, {"size_mb", ""}, {"size_gb", ""}, {"size_tb", ""},
            {"atributos", ""}, {"autor", ""}, {"dimensoes", ""}, {"duracao", ""}, {"taxa_bits", ""}, {"protegido", ""},
            {"paginas", ""}, {"linhas", ""}, {"palavras", ""}, {"paginas_estimadas", ""}, {"linhas_codigo", ""}, {"total_linhas", ""},
            {"slides_estimados", ""}, {"arquivos", ""}, {"unzipped_b", ""}, {"unzipped_kb", ""}, {"unzipped_mb", ""}, {"unzipped_gb", ""}, {"unzipped_tb", ""},
            {"slides", ""}, {"binary_file_b", ""}, {"binary_file_kb", ""}, {"binary_file_mb", ""}, {"binary_file_gb", ""}, {"binary_file_tb", ""},
            {"planilhas", ""}, {"colunas", ""}, {"registros", ""}, {"tabelas", ""}};
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar evento de exclusão", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
