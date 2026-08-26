#include "geui_08_criar_lista_graficos.h"

#include <QMetaObject>
#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

QVariantList criar_lista_graficos(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        QVariantList lista;
        const QList<QPair<QString, QString>> mapa = {
            {"operations_pie", "grafico_operacoes_pizza"}, {"bar_chart", "grafico_tipos_arquivo_barras"},
            {"timeline", "grafico_timeline_operacoes"}, {"tree_map", "grafico_treemap_tipos"},
            {"hour_histogram", "grafico_histograma_horarios"}, {"pareto_analysis", "grafico_pareto_operacoes"},
            {"operations_by_day", "grafico_cluster_linha"},
            {"size_distribution_by_operation_type", "grafico_boxplot_distribuicao_por_tipo_operacao"},
            {"size_distribution_by_type", "grafico_boxplot_distribuicao_por_tipo"},
            {"size_distribution_by_hour_of_day", "grafico_boxplot_distribuicao_por_hora_dia"},
            {"size_distribution_by_size_categories", "grafico_boxplot_distribuicao_por_categoria_tamanho"},
            {"events_monitored_by_type", "grafico_eventos_monitorados_por_tipo"},
            {"events_monitored_by_operation_type", "grafico_eventos_monitorados_por_tipo_operacao"},
            {"events_monitored_by_week_days", "grafico_eventos_monitorados_por_dias_semana"},
            {"events_monitored_by_hour_of_day", "grafico_eventos_monitorados_por_hora_dia"},
            {"temporal_distribution", "grafico_heatmap"},
            {"file_size_analysis", "grafico_scatter"}, {"file_operations_flow", "grafico_sankey"},
            {"event_to_path_flow", "grafico_sankey_evento_caminho"}, {"type_to_path_flow", "grafico_sankey_tipo_caminho"},
            {"operations_by_file_type", "grafico_radar"}, {"file_size_distribution", "grafico_dotplot"},
            {"directory_tree", "grafico_arvore_diretorios"}
        };

        for (const auto& item : mapa) {
            QVariantMap g;
            g.insert("titulo", get_text(self ? self->loc_obj() : nullptr, item.first, item.first));
            g.insert("func", item.second);
            lista.append(g);
        }
        return lista;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar lista de gráficos", true);
        return {};
    }
}

}
