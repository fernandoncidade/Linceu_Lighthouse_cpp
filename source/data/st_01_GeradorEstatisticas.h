#pragma once

#include <functional>
#include <memory>

#include <QColor>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace source::data {

class BaseGerador;

class GeradorEstatisticas {
public:
    using FactoryGerador = std::function<std::shared_ptr<BaseGerador>(const QString&, QObject*, QObject*)>;

    explicit GeradorEstatisticas(const QString& db_path, QObject* localizador = nullptr, QObject* interface_principal = nullptr);

    void atualizar_textos_traduzidos();
    void adicionar_gerador(const QString& nome, FactoryGerador factory_gerador);
    void remover_gerador(const QString& nome);
    QStringList listar_geradores() const;

    QVariant grafico_operacoes_pizza();
    QVariant grafico_tipos_arquivo_barras();
    QVariant grafico_timeline_operacoes();
    QVariant grafico_treemap_tipos();
    QVariant grafico_histograma_horarios();
    QVariant grafico_pareto_operacoes();
    QVariant grafico_cluster_linha();
    QVariant grafico_boxplot_distribuicao();
    QVariant grafico_boxplot_distribuicao_por_tipo_operacao();
    QVariant grafico_boxplot_distribuicao_por_tipo();
    QVariant grafico_boxplot_distribuicao_por_hora_dia();
    QVariant grafico_boxplot_distribuicao_por_categoria_tamanho();
    QVariant grafico_boxplot_eventos();
    QVariant grafico_eventos_monitorados_por_tipo();
    QVariant grafico_eventos_monitorados_por_tipo_operacao();
    QVariant grafico_eventos_monitorados_por_dias_semana();
    QVariant grafico_eventos_monitorados_por_hora_dia();
    QVariant grafico_heatmap();
    QVariant grafico_scatter();
    QVariant grafico_sankey();
    QVariant grafico_radar();
    QVariant grafico_dotplot();
    QVariant grafico_sankey_evento_caminho();
    QVariant grafico_sankey_tipo_caminho();
    QVariant grafico_arvore_diretorios();

    QVariant gerar_grafico(const QString& tipo_grafico);
    QHash<QString, bool> salvar_graficos(const QString& diretorio);

private:
    QString db_path_;
    QObject* loc_;
    QObject* interface_;
    QHash<QString, std::shared_ptr<BaseGerador>> geradores_;

    void inicializar_geradores();
    QVariant gerar_por_chave(const QString& chave, const QString& erro_contexto);
    QString texto(const QString& key, const QString& fallback = QString()) const;
    QVariant grafico_boxplot_distribuicao_individual(const QString& title_key,
                                                     const QString& title_fallback,
                                                     const QString& source_key,
                                                     const QString& order_key,
                                                     const QString& color_key,
                                                     bool y_log_scale,
                                                     const QString& x_label_key,
                                                     const QString& x_label_fallback,
                                                     const QString& y_label_key,
                                                     const QString& y_label_fallback,
                                                     int max_items);
    QVariant grafico_eventos_monitorados_individual(const QString& title_key,
                                                    const QString& title_fallback,
                                                    const QString& source_key,
                                                    const QColor& color);
};

}  // namespace source::data
