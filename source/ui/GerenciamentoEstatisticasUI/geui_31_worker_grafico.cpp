#include "geui_31_worker_grafico.h"

#include <exception>

#include <QVariantMap>

#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

namespace {
QVariant chamar_funcao_grafico(source::data::GeradorEstatisticas* gerador, const QString& func_name) {
    if (gerador == nullptr) {
        return QVariant();
    }

    if (func_name == QStringLiteral("grafico_operacoes_pizza")) return gerador->grafico_operacoes_pizza();
    if (func_name == QStringLiteral("grafico_tipos_arquivo_barras")) return gerador->grafico_tipos_arquivo_barras();
    if (func_name == QStringLiteral("grafico_timeline_operacoes")) return gerador->grafico_timeline_operacoes();
    if (func_name == QStringLiteral("grafico_treemap_tipos")) return gerador->grafico_treemap_tipos();
    if (func_name == QStringLiteral("grafico_histograma_horarios")) return gerador->grafico_histograma_horarios();
    if (func_name == QStringLiteral("grafico_pareto_operacoes")) return gerador->grafico_pareto_operacoes();
    if (func_name == QStringLiteral("grafico_cluster_linha")) return gerador->grafico_cluster_linha();
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao")) return gerador->grafico_boxplot_distribuicao();
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_tipo_operacao")) {
        return gerador->grafico_boxplot_distribuicao_por_tipo_operacao();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_tipo")) {
        return gerador->grafico_boxplot_distribuicao_por_tipo();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_hora_dia")) {
        return gerador->grafico_boxplot_distribuicao_por_hora_dia();
    }
    if (func_name == QStringLiteral("grafico_boxplot_distribuicao_por_categoria_tamanho")) {
        return gerador->grafico_boxplot_distribuicao_por_categoria_tamanho();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_tipo")) {
        return gerador->grafico_eventos_monitorados_por_tipo();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_tipo_operacao")) {
        return gerador->grafico_eventos_monitorados_por_tipo_operacao();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_dias_semana")) {
        return gerador->grafico_eventos_monitorados_por_dias_semana();
    }
    if (func_name == QStringLiteral("grafico_eventos_monitorados_por_hora_dia")) {
        return gerador->grafico_eventos_monitorados_por_hora_dia();
    }
    if (func_name == QStringLiteral("grafico_boxplot_eventos")) return gerador->grafico_boxplot_eventos();
    if (func_name == QStringLiteral("grafico_heatmap")) return gerador->grafico_heatmap();
    if (func_name == QStringLiteral("grafico_scatter")) return gerador->grafico_scatter();
    if (func_name == QStringLiteral("grafico_sankey")) return gerador->grafico_sankey();
    if (func_name == QStringLiteral("grafico_sankey_evento_caminho")) return gerador->grafico_sankey_evento_caminho();
    if (func_name == QStringLiteral("grafico_sankey_tipo_caminho")) return gerador->grafico_sankey_tipo_caminho();
    if (func_name == QStringLiteral("grafico_radar")) return gerador->grafico_radar();
    if (func_name == QStringLiteral("grafico_dotplot")) return gerador->grafico_dotplot();
    if (func_name == QStringLiteral("grafico_arvore_diretorios")) return gerador->grafico_arvore_diretorios();

    return QVariant();
}

}

GraficoWorker::GraficoWorker(const QVariantList& graficos,
                             const QString& db_path,
                             QObject* localizador,
                             QObject* interface_principal,
                             QObject* parent)
    : QThread(parent),
      graficos_(graficos),
      db_path_(db_path),
      loc_(localizador),
      interface_(interface_principal),
      parar_flag_(false) {}

void GraficoWorker::run() {
    try {
        source::data::GeradorEstatisticas gerador(db_path_, loc_.data(), interface_.data());
        const int total = graficos_.size();
        for (int i = 0; i < total; ++i) {
            if (parar_flag_.load(std::memory_order_relaxed) || isInterruptionRequested()) break;

            const QVariantMap g = graficos_.at(i).toMap();

            try {
                const QVariant dados = preparar_dados_grafico(g.value("func"), &gerador);
                if (parar_flag_.load(std::memory_order_relaxed) || isInterruptionRequested()) {
                    break;
                }
                emit dados_grafico_prontos(g.value("titulo").toString(), dados, g.value("func"), g.value("titulo").toString());
            } catch (const std::exception& e) {
                if (parar_flag_.load(std::memory_order_relaxed) || isInterruptionRequested()) {
                    break;
                }
                emit erro_ocorrido(g.value("titulo").toString(), QString::fromUtf8(e.what()));
            } catch (...) {
                if (parar_flag_.load(std::memory_order_relaxed) || isInterruptionRequested()) {
                    break;
                }
                emit erro_ocorrido(g.value("titulo").toString(), QStringLiteral("erro desconhecido"));
            }

            if (parar_flag_.load(std::memory_order_relaxed) || isInterruptionRequested()) {
                break;
            }
            emit progresso_atualizado(i + 1, total);
        }
        emit todos_concluidos();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro na execução da thread GraficoWorker", true);
    }
}

QVariant GraficoWorker::preparar_dados_grafico(const QVariant& func_gerador,
                                               source::data::GeradorEstatisticas* gerador) const {
    try {
        const QString func_name = func_gerador.toString();
        if (func_name.isEmpty()) {
            return QVariant();
        }

        return chamar_funcao_grafico(gerador, func_name);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao preparar dados do gráfico", true);
        return QVariant();
    }
}

void GraficoWorker::parar() {
    parar_flag_.store(true, std::memory_order_relaxed);
    requestInterruption();
}

}
