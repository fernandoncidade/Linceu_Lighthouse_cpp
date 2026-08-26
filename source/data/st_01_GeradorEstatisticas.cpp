#include "st_01_GeradorEstatisticas.h"

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QMetaObject>
#include <QPainter>
#include <QVariantList>
#include <QVariantMap>

#include <stdexcept>

#include "source/data/Charts/ch_02_ChartWidgetFactory.h"
#include "source/data/GeradorEstatisticas/gst_01_BaseGerador.h"
#include "source/data/GeradorEstatisticas/gst_02_GraficoPizza.h"
#include "source/data/GeradorEstatisticas/gst_03_GraficoBarras.h"
#include "source/data/GeradorEstatisticas/gst_04_GraficoTimeline.h"
#include "source/data/GeradorEstatisticas/gst_05_GraficoTreemap.h"
#include "source/data/GeradorEstatisticas/gst_06_GraficoHistograma.h"
#include "source/data/GeradorEstatisticas/gst_07_GraficoPareto.h"
#include "source/data/GeradorEstatisticas/gst_08_GraficoLinha.h"
#include "source/data/GeradorEstatisticas/gst_09_GraficoBoxplot.h"
#include "source/data/GeradorEstatisticas/gst_10_GraficoRadarEventos.h"
#include "source/data/GeradorEstatisticas/gst_11_GraficoHeatmap.h"
#include "source/data/GeradorEstatisticas/gst_12_GraficoScatter.h"
#include "source/data/GeradorEstatisticas/gst_13_GraficoSankey.h"
#include "source/data/GeradorEstatisticas/gst_14_GraficoRadar.h"
#include "source/data/GeradorEstatisticas/gst_15_GraficoDotplot.h"
#include "source/data/GeradorEstatisticas/gst_16_GraficoSankeyEventoCaminho.h"
#include "source/data/GeradorEstatisticas/gst_17_GraficoSankeyTipoCaminho.h"
#include "source/data/GeradorEstatisticas/gst_18_GraficoArvoreDiretorios.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString resumir_valor(const QVariant& valor) {
    const int type_id =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        valor.typeId();
#else
        valor.type();
#endif
    switch (type_id) {
        case QMetaType::QVariantList:
            return QStringLiteral("%1 itens").arg(valor.toList().size());
        case QMetaType::QVariantMap:
            return QStringLiteral("%1 chaves").arg(valor.toMap().size());
        case QMetaType::QVariantHash:
            return QStringLiteral("%1 chaves").arg(valor.toHash().size());
        case QMetaType::QString:
            return valor.toString();
        case QMetaType::Bool:
            return valor.toBool() ? QStringLiteral("true") : QStringLiteral("false");
        default:
            if (valor.canConvert<double>()) return QString::number(valor.toDouble());
            return valor.toString();
    }
}

QString criar_texto_preview_grafico(const QString& nome, const QVariant& grafico) {
    QStringList linhas;
    linhas << nome;

    const QVariantMap fig_map = grafico.toMap();
    if (fig_map.isEmpty()) {
        linhas << QStringLiteral("Sem dados de gráfico disponíveis.");
        return linhas.join('\n');
    }

    if (!fig_map.value(QStringLiteral("title")).toString().trimmed().isEmpty()) {
        linhas << fig_map.value(QStringLiteral("title")).toString();
    }
    if (!fig_map.value(QStringLiteral("kind")).toString().trimmed().isEmpty()) {
        linhas << QStringLiteral("Tipo: %1").arg(fig_map.value(QStringLiteral("kind")).toString());
    }
    if (!fig_map.value(QStringLiteral("message")).toString().trimmed().isEmpty()) {
        linhas << fig_map.value(QStringLiteral("message")).toString();
    }

    int detalhes = 0;
    for (auto it = fig_map.constBegin(); it != fig_map.constEnd(); ++it) {
        const QString key = it.key();
        if (key == QStringLiteral("title") || key == QStringLiteral("kind") || key == QStringLiteral("message")) {
            continue;
        }
        linhas << QStringLiteral("%1: %2").arg(key, resumir_valor(it.value()));
        detalhes += 1;
        if (detalhes >= 10) break;
    }

    return linhas.join('\n');
}

bool salvar_preview_png(const QString& arquivo_destino, const QString& texto) {
    QImage image(1280, 720, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    painter.drawText(image.rect().adjusted(28, 28, -28, -28), Qt::AlignLeft | Qt::TextWordWrap, texto);
    painter.end();

    return image.save(arquivo_destino, "PNG");
}

QVariantMap criar_figura_boxplot_simples(const QString& title,
                                         const QVariantMap& source_map,
                                         const QVariantList& order_hint,
                                         const QVariantMap& color_map,
                                         bool y_log_scale,
                                         const QString& x_label,
                                         const QString& y_label,
                                         int max_items) {
    QVariantMap figure;
    figure.insert(QStringLiteral("kind"), QStringLiteral("boxplot_single"));
    figure.insert(QStringLiteral("title"), title);
    figure.insert(QStringLiteral("source_map"), source_map);
    figure.insert(QStringLiteral("order_hint"), order_hint);
    figure.insert(QStringLiteral("color_map"), color_map);
    figure.insert(QStringLiteral("y_log_scale"), y_log_scale);
    if (!x_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("x_label"), x_label);
    }
    if (!y_label.trimmed().isEmpty()) {
        figure.insert(QStringLiteral("y_label"), y_label);
    }
    if (max_items > 0) {
        figure.insert(QStringLiteral("max_items"), max_items);
    }
    return figure;
}

source::ui::LocalizadorQt* obter_localizador(QObject* loc) {
    return qobject_cast<source::ui::LocalizadorQt*>(loc);
}

QString traduzir_tipo_monitorado(QObject* loc, const QString& valor) {
    auto* localizador = obter_localizador(loc);
    if (localizador == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    const QString traduzido = localizador->traduzir_metadados(valor.trimmed());
    return traduzido.trimmed().isEmpty() ? valor : traduzido;
}

QString traduzir_operacao_monitorada(QObject* loc, const QString& valor) {
    auto* localizador = obter_localizador(loc);
    if (localizador == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    const QString traduzido = localizador->traduzir_tipo_operacao(valor.trimmed());
    return traduzido.trimmed().isEmpty() ? valor : traduzido;
}

QPair<QVariantList, QVector<double>> extrair_resumo_radar(const QVariantMap& summary) {
    QVariantList labels;
    QVector<double> values;

    const QVariantList labels_raw = summary.value(QStringLiteral("labels")).toList();
    const QVariantList values_raw = summary.value(QStringLiteral("values")).toList();
    const int count = qMin(labels_raw.size(), values_raw.size());
    for (int i = 0; i < count; ++i) {
        const QString label = labels_raw.at(i).toString().trimmed();
        if (label.isEmpty()) {
            continue;
        }
        labels.append(label);
        values.append(values_raw.at(i).toDouble());
    }

    if (!labels.isEmpty()) {
        return {labels, values};
    }

    for (auto it = summary.constBegin(); it != summary.constEnd(); ++it) {
        if (it.key() == QStringLiteral("labels") || it.key() == QStringLiteral("values")) {
            continue;
        }
        const QString label = it.key().trimmed();
        if (label.isEmpty()) {
            continue;
        }
        labels.append(label);
        values.append(it.value().toDouble());
    }

    return {labels, values};
}
}  // namespace

namespace source::data {

GeradorEstatisticas::GeradorEstatisticas(const QString& db_path, QObject* localizador, QObject* interface_principal)
    : db_path_(db_path), loc_(localizador), interface_(interface_principal) {
    inicializar_geradores();
}

void GeradorEstatisticas::inicializar_geradores() {
    try {
        geradores_.insert("pizza", std::make_shared<GraficoPizza>(db_path_, loc_, interface_));
        geradores_.insert("barras", std::make_shared<GraficoBarras>(db_path_, loc_, interface_));
        geradores_.insert("timeline", std::make_shared<GraficoTimeline>(db_path_, loc_, interface_));
        geradores_.insert("treemap", std::make_shared<GraficoTreemap>(db_path_, loc_, interface_));
        geradores_.insert("histograma", std::make_shared<GraficoHistograma>(db_path_, loc_, interface_));
        geradores_.insert("pareto", std::make_shared<GraficoPareto>(db_path_, loc_, interface_));
        geradores_.insert("linha", std::make_shared<GraficoLinha>(db_path_, loc_, interface_));
        geradores_.insert("boxplot", std::make_shared<GraficoBoxplot>(db_path_, loc_, interface_));
        geradores_.insert("boxplot_eventos", std::make_shared<GraficoRadarEventos>(db_path_, loc_, interface_));
        geradores_.insert("heatmap", std::make_shared<GraficoHeatmap>(db_path_, loc_, interface_));
        geradores_.insert("scatter", std::make_shared<GraficoScatter>(db_path_, loc_, interface_));
        geradores_.insert("sankey", std::make_shared<GraficoSankey>(db_path_, loc_, interface_));
        geradores_.insert("radar", std::make_shared<GraficoRadar>(db_path_, loc_, interface_));
        geradores_.insert("dotplot", std::make_shared<GraficoDotplot>(db_path_, loc_, interface_));
        geradores_.insert("sankey_evento_caminho", std::make_shared<GraficoSankeyEventoCaminho>(db_path_, loc_, interface_));
        geradores_.insert("sankey_tipo_caminho", std::make_shared<GraficoSankeyTipoCaminho>(db_path_, loc_, interface_));
        geradores_.insert("arvore_diretorios", std::make_shared<GraficoArvoreDiretorios>(db_path_, loc_, interface_));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar geradores", true);
    }
}

QString GeradorEstatisticas::texto(const QString& key, const QString& fallback) const {
    if (!loc_) return fallback;
    QString out = fallback;
    QMetaObject::invokeMethod(loc_, "get_text", Q_RETURN_ARG(QString, out), Q_ARG(QString, key), Q_ARG(QString, fallback));
    return out;
}

void GeradorEstatisticas::atualizar_textos_traduzidos() {
    for (auto& g : geradores_) {
        if (g) g->atualizar_textos_traduzidos();
    }
}

void GeradorEstatisticas::adicionar_gerador(const QString& nome, FactoryGerador factory_gerador) {
    try {
        if (!factory_gerador) {
            const QString msg = QStringLiteral("Factory inválida para gerador '%1'").arg(nome);
            source::utils::LogManager::get_logger().error(msg);
            throw std::invalid_argument(msg.toStdString());
        }

        const std::shared_ptr<BaseGerador> gerador = factory_gerador(db_path_, loc_, interface_);
        if (!gerador) {
            const QString msg = QStringLiteral("Factory retornou gerador nulo para '%1'").arg(nome);
            source::utils::LogManager::get_logger().error(msg);
            throw std::runtime_error(msg.toStdString());
        }

        geradores_[nome] = gerador;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao adicionar gerador '%1'").arg(nome), true);
        throw;
    }
}

void GeradorEstatisticas::remover_gerador(const QString& nome) {
    if (geradores_.contains(nome)) geradores_.remove(nome);
    else source::utils::LogManager::get_logger().warning(QStringLiteral("Tentativa de remover gerador inexistente: %1").arg(nome));
}

QStringList GeradorEstatisticas::listar_geradores() const { return geradores_.keys(); }

QVariant GeradorEstatisticas::gerar_por_chave(const QString& chave, const QString& erro_contexto) {
    try {
        if (!geradores_.contains(chave) || !geradores_.value(chave)) {
            const QString msg = QStringLiteral("Gerador '%1' não encontrado").arg(chave);
            source::utils::LogManager::get_logger().error(msg);
            throw std::runtime_error(msg.toStdString());
        }

        return geradores_.value(chave)->gerar();
    } catch (...) {
        source::utils::LogManager::get_logger().error(erro_contexto, true);
        throw;
    }
}

QVariant GeradorEstatisticas::grafico_operacoes_pizza() { return gerar_por_chave("pizza", "Erro ao gerar gráfico de pizza"); }
QVariant GeradorEstatisticas::grafico_tipos_arquivo_barras() { return gerar_por_chave("barras", "Erro ao gerar gráfico de barras"); }
QVariant GeradorEstatisticas::grafico_timeline_operacoes() { return gerar_por_chave("timeline", "Erro ao gerar gráfico timeline"); }
QVariant GeradorEstatisticas::grafico_treemap_tipos() { return gerar_por_chave("treemap", "Erro ao gerar gráfico treemap"); }
QVariant GeradorEstatisticas::grafico_histograma_horarios() { return gerar_por_chave("histograma", "Erro ao gerar histograma"); }
QVariant GeradorEstatisticas::grafico_pareto_operacoes() { return gerar_por_chave("pareto", "Erro ao gerar gráfico de pareto"); }
QVariant GeradorEstatisticas::grafico_cluster_linha() { return gerar_por_chave("linha", "Erro ao gerar gráfico de linha"); }
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao() { return gerar_por_chave("boxplot", "Erro ao gerar boxplot"); }
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_individual(const QString& title_key,
                                                                      const QString& title_fallback,
                                                                      const QString& source_key,
                                                                      const QString& order_key,
                                                                      const QString& color_key,
                                                                      bool y_log_scale,
                                                                      const QString& x_label_key,
                                                                      const QString& x_label_fallback,
                                                                      const QString& y_label_key,
                                                                      const QString& y_label_fallback,
                                                                      int max_items) {
    const QString title = texto(title_key, title_fallback);
    const QVariantMap grouped = grafico_boxplot_distribuicao().toMap();
    if (grouped.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    if (grouped.value(QStringLiteral("kind")).toString() == QStringLiteral("empty")) {
        QVariantMap empty = grouped;
        empty.insert(QStringLiteral("title"), title);
        return empty;
    }

    QVariantMap source_map;
    QVariantList order_hint;
    QVariantMap color_map;

    if (source_key == QStringLiteral("by_hour_daily_counts")) {
        const QVariantList by_hour_daily_counts = grouped.value(source_key).toList();
        for (const QVariant& hour_value : by_hour_daily_counts) {
            const QVariantMap hour_map = hour_value.toMap();
            const QString raw_hour = hour_map.value(QStringLiteral("hour")).toString().trimmed();
            const QVariantList counts = hour_map.value(QStringLiteral("counts")).toList();
            if (raw_hour.isEmpty() || counts.isEmpty()) {
                continue;
            }
            const QString hour_label = texto(QStringLiteral("hour_format"), QStringLiteral("%1h")).arg(raw_hour);
            source_map.insert(hour_label, counts);
            order_hint.append(hour_label);
        }
    } else {
        source_map = grouped.value(source_key).toMap();
        order_hint = grouped.value(order_key).toList();
        color_map = color_key.trimmed().isEmpty() ? QVariantMap() : grouped.value(color_key).toMap();
    }

    if (source_map.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    QVariantMap figure =
        criar_figura_boxplot_simples(title, source_map, order_hint, color_map, y_log_scale,
                                     texto(x_label_key, x_label_fallback), texto(y_label_key, y_label_fallback),
                                     max_items);
    if (source_key == QStringLiteral("by_hour_daily_counts") || source_key == QStringLiteral("by_hour_of_day")) {
        figure.insert(QStringLiteral("boxplot_layout"), QStringLiteral("hour_of_day_compact"));
    }
    return figure;
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_tipo_operacao() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_operation_type"),
        QStringLiteral("Distribuição de Tamanhos por Tipo de Operação"),
        QStringLiteral("by_operation"),
        QStringLiteral("operation_order"),
        QStringLiteral("op_colors"),
        true,
        QStringLiteral("operation_type"),
        QStringLiteral("Tipo de Operação"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        -1);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_tipo() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_type"),
        QStringLiteral("Distribuição de Tamanhos por Tipo"),
        QStringLiteral("by_type"),
        QStringLiteral("top_types"),
        QString(),
        true,
        QStringLiteral("type"),
        QStringLiteral("Tipo"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        30);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_hora_dia() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_hour_of_day"),
        QStringLiteral("Distribuição de Tamanhos por Hora do Dia"),
        QStringLiteral("by_hour_of_day"),
        QStringLiteral("hour_order"),
        QString(),
        true,
        QStringLiteral("hour_of_day"),
        QStringLiteral("Hora do Dia"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        24);
}
QVariant GeradorEstatisticas::grafico_boxplot_distribuicao_por_categoria_tamanho() {
    return grafico_boxplot_distribuicao_individual(
        QStringLiteral("size_distribution_by_size_categories"),
        QStringLiteral("Distribuição de Tamanhos por Categorias de Tamanho"),
        QStringLiteral("by_size_category"),
        QStringLiteral("category_order"),
        QString(),
        true,
        QStringLiteral("category"),
        QStringLiteral("Categoria"),
        QStringLiteral("size_megabytes_mb"),
        QStringLiteral("Tamanho (Megabytes) (MB)"),
        5);
}
QVariant GeradorEstatisticas::grafico_boxplot_eventos() { return gerar_por_chave("boxplot_eventos", "Erro ao gerar boxplot de eventos"); }
QVariant GeradorEstatisticas::grafico_eventos_monitorados_individual(const QString& title_key,
                                                                     const QString& title_fallback,
                                                                     const QString& source_key,
                                                                     const QColor& color) {
    const QString title = texto(title_key, title_fallback);
    const QVariantMap grouped = grafico_boxplot_eventos().toMap();
    if (grouped.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    if (grouped.value(QStringLiteral("kind")).toString() == QStringLiteral("empty")) {
        QVariantMap empty = grouped;
        empty.insert(QStringLiteral("title"), title);
        return empty;
    }

    QVariantList rawLabels;
    QStringList translatedLabels;
    QVector<double> values;

    if (source_key == QStringLiteral("top_types") || source_key == QStringLiteral("top_operations")) {
        const auto summary = extrair_resumo_radar(grouped.value(source_key).toMap());
        rawLabels = summary.first;
        values = summary.second;
        translatedLabels.reserve(rawLabels.size());
        for (const QVariant& label : rawLabels) {
            const QString text = label.toString();
            translatedLabels.append(source_key == QStringLiteral("top_types")
                                        ? traduzir_tipo_monitorado(loc_, text)
                                        : traduzir_operacao_monitorada(loc_, text));
        }
    } else if (source_key == QStringLiteral("by_weekday")) {
        static const char* weekdayKeys[] = {"weekday_monday",   "weekday_tuesday", "weekday_wednesday",
                                            "weekday_thursday", "weekday_friday",  "weekday_saturday",
                                            "weekday_sunday"};
        static const char* weekdayFallbacks[] = {"Segunda", "Terça", "Quarta", "Quinta",
                                                 "Sexta",   "Sábado", "Domingo"};
        const QVariantList groupedValues = grouped.value(source_key).toList();
        translatedLabels.reserve(7);
        values.reserve(7);
        for (int i = 0; i < 7; ++i) {
            rawLabels.append(QString::fromUtf8(weekdayKeys[i]));
            translatedLabels.append(texto(QString::fromUtf8(weekdayKeys[i]), QString::fromUtf8(weekdayFallbacks[i])));
            values.append((i < groupedValues.size()) ? groupedValues.at(i).toDouble() : 0.0);
        }
    } else if (source_key == QStringLiteral("by_hour")) {
        const QVariantList groupedValues = grouped.value(source_key).toList();
        translatedLabels.reserve(24);
        values.reserve(24);
        const QString hourFormat = texto(QStringLiteral("hour_format"), QStringLiteral("%1h"));
        for (int hour = 0; hour < 24; ++hour) {
            rawLabels.append(QString::number(hour));
            translatedLabels.append(hourFormat.arg(hour));
            values.append((hour < groupedValues.size()) ? groupedValues.at(hour).toDouble() : 0.0);
        }
    }

    if (translatedLabels.isEmpty() || values.isEmpty()) {
        return QVariantMap{{QStringLiteral("kind"), QStringLiteral("empty")},
                           {QStringLiteral("title"), title},
                           {QStringLiteral("message"),
                            texto(QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir"))}};
    }

    QVariantMap figure =
        source::charts_lgpl::make_radar_single_figure(title, translatedLabels, values, color, title);
    figure.insert(QStringLiteral("smooth_curve"), false);
    figure.insert(QStringLiteral("mask_grid_under_series"), true);
    figure.insert(QStringLiteral("raw_labels"), rawLabels);
    return figure;
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_tipo() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_type"), QStringLiteral("Eventos Monitorados por Tipo"),
        QStringLiteral("top_types"), QColor(QStringLiteral("#2563eb")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_tipo_operacao() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_operation_type"),
        QStringLiteral("Eventos Monitorados por Tipo de Operação"), QStringLiteral("top_operations"),
        QColor(QStringLiteral("#dc2626")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_dias_semana() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_week_days"), QStringLiteral("Eventos Monitorados por Dias da Semana"),
        QStringLiteral("by_weekday"), QColor(QStringLiteral("#059669")));
}
QVariant GeradorEstatisticas::grafico_eventos_monitorados_por_hora_dia() {
    return grafico_eventos_monitorados_individual(
        QStringLiteral("events_monitored_by_hour_of_day"), QStringLiteral("Eventos Monitorados por Hora do Dia"),
        QStringLiteral("by_hour"), QColor(QStringLiteral("#7c3aed")));
}
QVariant GeradorEstatisticas::grafico_heatmap() { return gerar_por_chave("heatmap", "Erro ao gerar heatmap"); }
QVariant GeradorEstatisticas::grafico_scatter() { return gerar_por_chave("scatter", "Erro ao gerar scatter"); }
QVariant GeradorEstatisticas::grafico_sankey() { return gerar_por_chave("sankey", "Erro ao gerar sankey"); }
QVariant GeradorEstatisticas::grafico_radar() { return gerar_por_chave("radar", "Erro ao gerar radar"); }
QVariant GeradorEstatisticas::grafico_dotplot() { return gerar_por_chave("dotplot", "Erro ao gerar dotplot"); }
QVariant GeradorEstatisticas::grafico_sankey_evento_caminho() { return gerar_por_chave("sankey_evento_caminho", "Erro ao gerar sankey evento→caminho"); }
QVariant GeradorEstatisticas::grafico_sankey_tipo_caminho() { return gerar_por_chave("sankey_tipo_caminho", "Erro ao gerar sankey tipo→caminho"); }
QVariant GeradorEstatisticas::grafico_arvore_diretorios() { return gerar_por_chave("arvore_diretorios", "Erro ao gerar árvore de diretórios"); }

QVariant GeradorEstatisticas::gerar_grafico(const QString& tipo_grafico) {
    if (!geradores_.contains(tipo_grafico) || !geradores_.value(tipo_grafico)) {
        const QString msg = QStringLiteral("Tipo de gráfico '%1' não encontrado").arg(tipo_grafico);
        source::utils::LogManager::get_logger().error(msg);
        throw std::runtime_error(msg.toStdString());
    }
    return gerar_por_chave(tipo_grafico, QStringLiteral("Erro ao gerar gráfico '%1'").arg(tipo_grafico));
}

QHash<QString, bool> GeradorEstatisticas::salvar_graficos(const QString& diretorio) {
    QHash<QString, bool> resultados;
    const QHash<QString, std::function<QVariant()>> graficos = {
        {texto("operations_pie", "operations_pie"), [this]() { return grafico_operacoes_pizza(); }},
        {texto("bar_chart", "bar_chart"), [this]() { return grafico_tipos_arquivo_barras(); }},
        {texto("timeline", "timeline"), [this]() { return grafico_timeline_operacoes(); }},
        {texto("tree_map", "tree_map"), [this]() { return grafico_treemap_tipos(); }},
        {texto("hour_histogram", "hour_histogram"), [this]() { return grafico_histograma_horarios(); }},
        {texto("pareto_analysis", "pareto_analysis"), [this]() { return grafico_pareto_operacoes(); }},
        {texto("operations_by_day", "operations_by_day"), [this]() { return grafico_cluster_linha(); }},
        {texto("size_distribution_by_operation_type", "size_distribution_by_operation_type"),
         [this]() { return grafico_boxplot_distribuicao_por_tipo_operacao(); }},
        {texto("size_distribution_by_type", "size_distribution_by_type"),
         [this]() { return grafico_boxplot_distribuicao_por_tipo(); }},
        {texto("size_distribution_by_hour_of_day", "size_distribution_by_hour_of_day"),
         [this]() { return grafico_boxplot_distribuicao_por_hora_dia(); }},
        {texto("size_distribution_by_size_categories", "size_distribution_by_size_categories"),
         [this]() { return grafico_boxplot_distribuicao_por_categoria_tamanho(); }},
        {texto("events_monitored_by_type", "events_monitored_by_type"),
         [this]() { return grafico_eventos_monitorados_por_tipo(); }},
        {texto("events_monitored_by_operation_type", "events_monitored_by_operation_type"),
         [this]() { return grafico_eventos_monitorados_por_tipo_operacao(); }},
        {texto("events_monitored_by_week_days", "events_monitored_by_week_days"),
         [this]() { return grafico_eventos_monitorados_por_dias_semana(); }},
        {texto("events_monitored_by_hour_of_day", "events_monitored_by_hour_of_day"),
         [this]() { return grafico_eventos_monitorados_por_hora_dia(); }},
        {texto("temporal_distribution", "temporal_distribution"), [this]() { return grafico_heatmap(); }},
        {texto("file_size_analysis", "file_size_analysis"), [this]() { return grafico_scatter(); }},
        {texto("file_operations_flow", "file_operations_flow"), [this]() { return grafico_sankey(); }},
        {texto("event_to_path_flow", "event_to_path_flow"), [this]() { return grafico_sankey_evento_caminho(); }},
        {texto("type_to_path_flow", "type_to_path_flow"), [this]() { return grafico_sankey_tipo_caminho(); }},
        {texto("operations_by_file_type", "operations_by_file_type"), [this]() { return grafico_radar(); }},
        {texto("file_size_distribution", "file_size_distribution"), [this]() { return grafico_dotplot(); }},
        {texto("directory_tree", "directory_tree"), [this]() { return grafico_arvore_diretorios(); }}
    };

    QDir dir(diretorio);
    if (!dir.exists() && !dir.mkpath(QStringLiteral("."))) {
        source::utils::LogManager::get_logger().error(QStringLiteral("Erro ao criar diretório %1").arg(diretorio));
        for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) resultados[it.key()] = false;
        return resultados;
    }
    if (!QFileInfo(dir.absolutePath()).isWritable()) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Sem permissão de escrita no diretório %1").arg(diretorio));
        for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) resultados[it.key()] = false;
        return resultados;
    }

    for (auto it = graficos.constBegin(); it != graficos.constEnd(); ++it) {
        const QString nome = it.key();
        try {
            const QVariant grafico = it.value()();
            const QString texto_preview = criar_texto_preview_grafico(nome, grafico);
            const bool ok = salvar_preview_png(dir.filePath(nome + QStringLiteral(".png")), texto_preview);
            resultados[nome] = ok;
        } catch (...) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro ao processar gráfico %1").arg(nome), true);
            resultados[nome] = false;
        }
    }

    return resultados;
}

}  // namespace source::data
