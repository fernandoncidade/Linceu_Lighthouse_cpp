#include "secore_03_evento_campos.h"

namespace source::security::core {
namespace {

QList<QPair<QString, QString>> criar_definicoes_colunas() {
    return {
        {"tipo_operacao", "TEXT"},
        {"nome", "TEXT"},
        {"dir_anterior", "TEXT"},
        {"dir_atual", "TEXT"},
        {"data_criacao", "TEXT"},
        {"data_modificacao", "TEXT"},
        {"data_acesso", "TEXT"},
        {"tipo", "TEXT"},
        {"size_b", "REAL"},
        {"size_kb", "REAL"},
        {"size_mb", "REAL"},
        {"size_gb", "REAL"},
        {"size_tb", "REAL"},
        {"atributos", "TEXT"},
        {"autor", "TEXT"},
        {"dimensoes", "TEXT"},
        {"duracao", "TEXT"},
        {"taxa_bits", "TEXT"},
        {"protegido", "TEXT"},
        {"paginas", "TEXT"},
        {"linhas", "TEXT"},
        {"palavras", "TEXT"},
        {"paginas_estimadas", "TEXT"},
        {"linhas_codigo", "TEXT"},
        {"total_linhas", "TEXT"},
        {"slides_estimados", "TEXT"},
        {"arquivos", "TEXT"},
        {"unzipped_b", "REAL"},
        {"unzipped_kb", "REAL"},
        {"unzipped_mb", "REAL"},
        {"unzipped_gb", "REAL"},
        {"unzipped_tb", "REAL"},
        {"slides", "TEXT"},
        {"binary_file_b", "REAL"},
        {"binary_file_kb", "REAL"},
        {"binary_file_mb", "REAL"},
        {"binary_file_gb", "REAL"},
        {"binary_file_tb", "REAL"},
        {"planilhas", "TEXT"},
        {"colunas", "TEXT"},
        {"registros", "TEXT"},
        {"tabelas", "TEXT"},
        {"timestamp", "TEXT"},
        {"monitor_profile_id", "TEXT"},
        {"monitor_profile_name", "TEXT"},
        {"monitor_diretorios", "TEXT"},
        {"sha256_atual", "TEXT"},
        {"sha256_baseline", "TEXT"},
        {"baseline_status", "TEXT"},
        {"integrity_flags", "TEXT"},
        {"alerta_regras", "TEXT"},
        {"alerta_resumo", "TEXT"},
        {"alerta_nivel", "TEXT"},
        {"session_id", "TEXT"},
        {"session_label", "TEXT"},
        {"diff_resumo", "TEXT"},
        {"risk_score", "INTEGER"},
        {"risk_level", "TEXT"},
        {"risk_reasons", "TEXT"},
        {"actor_user", "TEXT"},
        {"actor_process", "TEXT"},
        {"actor_pid", "INTEGER"},
        {"attribution_source", "TEXT"},
        {"quarantine_status", "TEXT"},
        {"quarantine_path", "TEXT"},
        {"publisher", "TEXT"},
        {"signature_state", "TEXT"},
        {"script_risk", "TEXT"},
        {"evidence_id", "TEXT"},
        {"evidence_manifest", "TEXT"},
        {"evidence_hash", "TEXT"},
    };
}

}  // namespace

const QList<QPair<QString, QString>>& definicoes_colunas_evento_monitoramento() {
    static const QList<QPair<QString, QString>> kDefinicoes = criar_definicoes_colunas();
    return kDefinicoes;
}

const QStringList& colunas_evento_monitoramento() {
    static const QStringList kColunas = []() {
        QStringList cols;
        const auto definicoes = definicoes_colunas_evento_monitoramento();
        cols.reserve(definicoes.size());
        for (const auto& coluna : definicoes) {
            cols.append(coluna.first);
        }
        return cols;
    }();
    return kColunas;
}

void garantir_campos_evento_monitoramento(QVariantMap& evento) {
    const auto& colunas = colunas_evento_monitoramento();
    for (const QString& coluna : colunas) {
        if (!evento.contains(coluna)) {
            evento.insert(coluna, QVariant());
        }
    }
}

}  // namespace source::security::core
