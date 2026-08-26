#include "geui_04_atualizar_graficos_apos_mudanca_idioma.h"

#include <QDialog>
#include <QMetaObject>
#include <QPushButton>
#include <QRegularExpression>
#include <QStackedLayout>
#include <QTabWidget>
#include <QVariantHash>
#include <QVariantMap>
#include <QWidget>

#include "geui_08_criar_lista_graficos.h"
#include "geui_09_criar_mapeamento_funcoes.h"
#include "geui_13_obter_estados_checkboxes.h"
#include "geui_14_gerar_todos_graficos.h"
#include "geui_20_atualizar_textos_painel_selecao.h"
#include "geui_22_atualizar_layout_apos_mudanca_botao.h"
#include "geui_23_atualizar_checkboxes_graficos.h"
#include "geui_27_ajustar_largura_painel_selecao.h"
#include "geui_30_botao_rotacionado.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_11_traduzir_tipo_operacao.h"
#include "source/ui/GerenciamentoLocalizadorQt/glqt_12_traduzir_metadados.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kStatsTabWidgetsProperty = "_estatisticas_tab_widgets";
constexpr auto kStatsGraphDataByLanguageProperty = "_estatisticas_graficos_dados_por_idioma";
constexpr auto kStatsActiveLanguageProperty = "_estatisticas_idioma_ativo";
constexpr auto kStatsContainerProperty = "_estatisticas_container";

QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}

QString get_text(source::ui::LocalizadorQt* loc, const QString& key, const QString& fallback = QString()) {
    return get_text(static_cast<QObject*>(loc), key, fallback);
}

QString obter_idioma_atual(QObject* loc) {
    const QString idioma = (loc != nullptr) ? loc->property("idioma_atual").toString() : QString();
    return idioma.isEmpty() ? QStringLiteral("pt_BR") : idioma;
}

QString normalizar_texto_comparacao(QString texto) {
    texto = texto.normalized(QString::NormalizationForm_D);
    static const QRegularExpression kCombiningMarks(QStringLiteral("[\\u0300-\\u036f]"));
    texto.remove(kCombiningMarks);
    return texto.toLower().simplified();
}

QString traduzir_operacao(source::ui::LocalizadorQt* loc, const QString& valor) {
    if (loc == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }
    return source::ui::GerenciamentoLocalizadorQt::traduzir_tipo_operacao(loc, valor, QString());
}

QString traduzir_tipo_arquivo(source::ui::LocalizadorQt* loc, const QString& valor) {
    if (loc == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }
    return source::ui::GerenciamentoLocalizadorQt::traduzir_metadados(loc, valor, QStringLiteral("tipo"));
}

QString traduzir_categoria_tamanho(source::ui::LocalizadorQt* loc, const QString& valor) {
    if (loc == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    const QString normalizado = normalizar_texto_comparacao(valor);
    struct CategoriaAlias {
        const char* key;
        const char* fallback;
        const char* aliases[6];
    };

    static const CategoriaAlias categorias[] = {
        {"very_small", "Muito Pequeno", {"muito pequeno", "very small", "muy pequeno", "tres petit", "molto piccolo", "sehr klein"}},
        {"small", "Pequeno", {"pequeno", "small", "pequeno", "petit", "piccolo", "klein"}},
        {"medium", "Médio", {"medio", "medium", "mediano", "moyen", "medio", "mittel"}},
        {"large", "Grande", {"grande", "large", "grande", "grand", "grande", "gross"}},
        {"very_large", "Muito Grande", {"muito grande", "very large", "muy grande", "tres grand", "molto grande", "sehr gross"}}};

    for (const auto& categoria : categorias) {
        if (valor == QString::fromUtf8(categoria.key)) {
            return get_text(loc, QString::fromUtf8(categoria.key), QString::fromUtf8(categoria.fallback));
        }
        for (const char* alias : categoria.aliases) {
            if (normalizado == QString::fromUtf8(alias)) {
                return get_text(loc, QString::fromUtf8(categoria.key), QString::fromUtf8(categoria.fallback));
            }
        }
    }

    return valor;
}

QString traduzir_tipo_ou_categoria(source::ui::LocalizadorQt* loc, const QString& valor) {
    const QString traduzido = traduzir_tipo_arquivo(loc, valor);
    if (traduzido != valor) {
        return traduzido;
    }
    return traduzir_categoria_tamanho(loc, valor);
}

QString traduzir_rotulo_hora(source::ui::LocalizadorQt* loc, const QString& valor) {
    if (loc == nullptr || valor.trimmed().isEmpty()) {
        return valor;
    }

    static const QRegularExpression kHourPattern(QStringLiteral("^\\s*(\\d{1,2})"));
    const QRegularExpressionMatch match = kHourPattern.match(valor);
    bool ok = false;
    const int hour = match.hasMatch() ? match.captured(1).toInt(&ok) : -1;
    if (!ok || hour < 0) {
        return valor;
    }

    return get_text(loc, QStringLiteral("hour_format"), QStringLiteral("%1h")).arg(hour);
}

QVariantList traduzir_lista_strings(const QVariantList& origem,
                                    const std::function<QString(const QString&)>& tradutor) {
    QVariantList destino;
    destino.reserve(origem.size());
    for (const QVariant& item : origem) {
        destino.append(tradutor(item.toString()));
    }
    return destino;
}

QVariantMap traduzir_mapa_chaves(const QVariantMap& origem,
                                 const std::function<QString(const QString&)>& tradutor) {
    QVariantMap destino;
    for (auto it = origem.constBegin(); it != origem.constEnd(); ++it) {
        destino.insert(tradutor(it.key()), it.value());
    }
    return destino;
}

QVariantList traduzir_lista_preferindo_raw(const QVariantList& raw,
                                           const QVariantList& origem,
                                           const std::function<QString(const QString&)>& tradutor) {
    return traduzir_lista_strings(raw.isEmpty() ? origem : raw, tradutor);
}

QVariantMap traduzir_mapa_chaves_preferindo_raw(const QVariantMap& raw,
                                                const QVariantMap& origem,
                                                const std::function<QString(const QString&)>& tradutor) {
    return traduzir_mapa_chaves(raw.isEmpty() ? origem : raw, tradutor);
}

QVariantMap traduzir_resumo(const QVariantMap& resumo,
                            const std::function<QString(const QString&)>& tradutor) {
    QVariantMap traduzido = resumo;
    traduzido.insert(QStringLiteral("labels"),
                     traduzir_lista_strings(resumo.value(QStringLiteral("labels")).toList(), tradutor));
    return traduzido;
}

QVariantList traduzir_series_linha(const QVariantList& origem,
                                   const std::function<QString(const QString&)>& tradutor) {
    QVariantList destino;
    destino.reserve(origem.size());
    for (const QVariant& item : origem) {
        QVariantMap serie = item.toMap();
        serie.insert(QStringLiteral("operations"),
                     traduzir_mapa_chaves(serie.value(QStringLiteral("operations")).toMap(), tradutor));
        destino.append(serie);
    }
    return destino;
}

QVariantList traduzir_series_radar(const QVariantList& origem,
                                   const std::function<QString(const QString&)>& tradutor) {
    QVariantList destino;
    destino.reserve(origem.size());
    for (const QVariant& item : origem) {
        QVariantMap serie = item.toMap();
        serie.insert(QStringLiteral("operacao"), tradutor(serie.value(QStringLiteral("operacao")).toString()));
        destino.append(serie);
    }
    return destino;
}

QVariantList encurtar_rotulos_radar(const QVariantList& origem) {
    QVariantList destino;
    destino.reserve(origem.size());
    for (const QVariant& item : origem) {
        const QString texto = item.toString();
        destino.append(texto.size() <= 14 ? texto : texto.left(11) + QStringLiteral("..."));
    }
    return destino;
}

void retraduzir_radar_simples(QVariantMap& fig,
                              const QVariantList& raw_labels,
                              const std::function<QString(const QString&)>& tradutor) {
    const QVariantList fonte_rotulos = raw_labels.isEmpty() ? fig.value(QStringLiteral("types")).toList() : raw_labels;
    const QVariantList tipos = traduzir_lista_strings(fonte_rotulos, tradutor);
    fig.insert(QStringLiteral("types"), tipos);
    fig.insert(QStringLiteral("types_short"), encurtar_rotulos_radar(tipos));

    QVariantList series_atualizadas;
    series_atualizadas.reserve(fig.value(QStringLiteral("series")).toList().size());
    for (const QVariant& item : fig.value(QStringLiteral("series")).toList()) {
        QVariantMap serie = item.toMap();
        serie.insert(QStringLiteral("operacao"), fig.value(QStringLiteral("title")).toString());
        series_atualizadas.append(serie);
    }
    fig.insert(QStringLiteral("series"), series_atualizadas);
}

QVariantList traduzir_pontos(const QVariantList& origem,
                             const std::function<QString(const QString&)>& traduzir_op,
                             const std::function<QString(const QString&)>& traduzir_tipo) {
    QVariantList destino;
    destino.reserve(origem.size());
    for (const QVariant& item : origem) {
        QVariantMap ponto = item.toMap();
        if (ponto.contains(QStringLiteral("tipo_operacao"))) {
            const QString raw = ponto.value(QStringLiteral("tipo_operacao_raw")).toString().trimmed();
            ponto.insert(QStringLiteral("tipo_operacao"),
                         traduzir_op(raw.isEmpty() ? ponto.value(QStringLiteral("tipo_operacao")).toString() : raw));
        }
        if (ponto.contains(QStringLiteral("tipo"))) {
            const QString raw = ponto.value(QStringLiteral("tipo_raw")).toString().trimmed();
            ponto.insert(QStringLiteral("tipo"),
                         traduzir_tipo(raw.isEmpty() ? ponto.value(QStringLiteral("tipo")).toString() : raw));
        }
        destino.append(ponto);
    }
    return destino;
}

QVariantMap traduzir_cache_por_tipo(const QVariantMap& origem,
                                    const std::function<QString(const QString&)>& tradutor) {
    QVariantMap destino;
    for (auto it = origem.constBegin(); it != origem.constEnd(); ++it) {
        destino.insert(tradutor(it.key()), it.value());
    }
    return destino;
}

QString titulo_interno_para_funcao(source::ui::LocalizadorQt* loc,
                                   const QString& func,
                                   const QString& titulo_aba) {
    if (loc == nullptr) {
        return titulo_aba;
    }

    if (func == QStringLiteral("grafico_heatmap")) {
        return get_text(loc, QStringLiteral("temporal_peak_cloud_title"),
                        QStringLiteral("%1 (nuvem de pico por operação)"))
            .arg(titulo_aba);
    }

    return titulo_aba;
}

QString subtitulo_para_funcao(source::ui::LocalizadorQt* loc,
                              const QString& func,
                              const QString& atual) {
    if (loc == nullptr) {
        return atual;
    }

    if (func == QStringLiteral("grafico_sankey")) {
        return get_text(loc, QStringLiteral("sankey_operations_subtitle"),
                        QStringLiteral("Últimos 20 arquivos/pastas por operação"));
    }
    if (func == QStringLiteral("grafico_sankey_evento_caminho")) {
        return get_text(loc, QStringLiteral("sankey_event_path_subtitle"),
                        QStringLiteral("Últimos 20 arquivos/pastas por evento"));
    }
    if (func == QStringLiteral("grafico_sankey_tipo_caminho")) {
        return get_text(loc, QStringLiteral("sankey_type_path_subtitle"),
                        QStringLiteral("10 tipos/extensões mais recentes para 10 diretórios mais recentes"));
    }

    return atual;
}

QVariantMap retraduzir_figura(source::ui::LocalizadorQt* loc,
                              const QString& func,
                              const QVariantMap& origem,
                              const QString& titulo_aba) {
    QVariantMap fig = origem;
    fig.insert(QStringLiteral("title"), titulo_interno_para_funcao(loc, func, titulo_aba));

    const QString kind = fig.value(QStringLiteral("kind")).toString();
    const auto traduzir_op = [loc](const QString& valor) { return traduzir_operacao(loc, valor); };
    const auto traduzir_tipo = [loc](const QString& valor) { return traduzir_tipo_ou_categoria(loc, valor); };
    const auto traduzir_hora = [loc](const QString& valor) { return traduzir_rotulo_hora(loc, valor); };
    const auto traduzir_dia_semana = [loc](const QString& valor) {
        struct WeekdayAlias {
            const char* key;
            const char* fallback;
            const char* aliases[6];
        };

        static const WeekdayAlias weekdays[] = {
            {"weekday_monday", "Segunda", {"segunda", "monday", "lunes", "montag", "lundi", "lunedi"}},
            {"weekday_tuesday", "Terça", {"terca", "tuesday", "martes", "dienstag", "mardi", "martedi"}},
            {"weekday_wednesday", "Quarta", {"quarta", "wednesday", "miercoles", "mittwoch", "mercredi", "mercoledi"}},
            {"weekday_thursday", "Quinta", {"quinta", "thursday", "jueves", "donnerstag", "jeudi", "giovedi"}},
            {"weekday_friday", "Sexta", {"sexta", "friday", "viernes", "freitag", "vendredi", "venerdi"}},
            {"weekday_saturday", "Sábado", {"sabado", "saturday", "sabado", "samstag", "samedi", "sabato"}},
            {"weekday_sunday", "Domingo", {"domingo", "sunday", "domingo", "sonntag", "dimanche", "domenica"}}};

        const QString normalizado = normalizar_texto_comparacao(valor);
        for (const auto& weekday : weekdays) {
            if (valor == QString::fromUtf8(weekday.key)) {
                return get_text(loc, QString::fromUtf8(weekday.key), QString::fromUtf8(weekday.fallback));
            }
            for (const char* alias : weekday.aliases) {
                if (normalizado == QString::fromUtf8(alias)) {
                    return get_text(loc, QString::fromUtf8(weekday.key), QString::fromUtf8(weekday.fallback));
                }
            }
        }
        return valor;
    };

    if (kind == QStringLiteral("empty")) {
        fig.insert(QStringLiteral("message"),
                   get_text(loc, QStringLiteral("no_data_to_plot"), QStringLiteral("Sem dados para exibir")));
        return fig;
    }

    if (func == QStringLiteral("grafico_operacoes_pizza") ||
        func == QStringLiteral("grafico_pareto_operacoes")) {
        fig.insert(QStringLiteral("labels"),
                   traduzir_lista_strings(fig.value(QStringLiteral("labels")).toList(), traduzir_op));
    }

    if (func == QStringLiteral("grafico_tipos_arquivo_barras") ||
        func == QStringLiteral("grafico_treemap_tipos")) {
        fig.insert(QStringLiteral("labels"),
                   traduzir_lista_strings(fig.value(QStringLiteral("labels")).toList(), traduzir_tipo));
    }

    if (func == QStringLiteral("grafico_timeline_operacoes")) {
        fig.insert(QStringLiteral("points"),
                   traduzir_pontos(fig.value(QStringLiteral("points")).toList(), traduzir_op, traduzir_tipo));
        fig.insert(QStringLiteral("op_colors"),
                   traduzir_mapa_chaves(fig.value(QStringLiteral("op_colors")).toMap(), traduzir_op));
    }

    if (func == QStringLiteral("grafico_cluster_linha")) {
        fig.insert(QStringLiteral("series"),
                   traduzir_series_linha(fig.value(QStringLiteral("series")).toList(), traduzir_op));
        fig.insert(QStringLiteral("operation_order"),
                   traduzir_lista_strings(fig.value(QStringLiteral("operation_order")).toList(), traduzir_op));
        fig.insert(QStringLiteral("op_colors"),
                   traduzir_mapa_chaves(fig.value(QStringLiteral("op_colors")).toMap(), traduzir_op));
    }

    if (func == QStringLiteral("grafico_boxplot_distribuicao")) {
        fig.insert(QStringLiteral("by_operation"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("by_operation")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("operation_order"),
                   traduzir_lista_strings(fig.value(QStringLiteral("operation_order")).toList(), traduzir_op));
        fig.insert(QStringLiteral("op_colors"),
                   traduzir_mapa_chaves(fig.value(QStringLiteral("op_colors")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("by_type"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("by_type")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("top_types"),
                   traduzir_lista_strings(fig.value(QStringLiteral("top_types")).toList(), traduzir_tipo));
        fig.insert(QStringLiteral("by_size_category"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("by_size_category")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("category_order"),
                   traduzir_lista_strings(fig.value(QStringLiteral("category_order")).toList(), traduzir_tipo));
    }

    if (func == QStringLiteral("grafico_boxplot_distribuicao_por_tipo_operacao")) {
        fig.insert(QStringLiteral("box_stats"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("box_stats_raw")).toMap(),
                                                       fig.value(QStringLiteral("box_stats")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("source_map"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("source_map")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("order_hint"),
                   traduzir_lista_preferindo_raw(fig.value(QStringLiteral("order_hint_raw")).toList(),
                                                 fig.value(QStringLiteral("order_hint")).toList(), traduzir_op));
        fig.insert(QStringLiteral("color_map"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("color_map_raw")).toMap(),
                                                       fig.value(QStringLiteral("color_map")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("operation_type"), QStringLiteral("Tipo de Operação")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("size_megabytes_mb"),
                            QStringLiteral("Tamanho (Megabytes) (MB)")));
    }

    if (func == QStringLiteral("grafico_boxplot_distribuicao_por_tipo")) {
        fig.insert(QStringLiteral("box_stats"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("box_stats_raw")).toMap(),
                                                       fig.value(QStringLiteral("box_stats")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("source_map"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("source_map")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("order_hint"),
                   traduzir_lista_preferindo_raw(fig.value(QStringLiteral("order_hint_raw")).toList(),
                                                 fig.value(QStringLiteral("order_hint")).toList(), traduzir_tipo));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("type"), QStringLiteral("Tipo")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("size_megabytes_mb"),
                            QStringLiteral("Tamanho (Megabytes) (MB)")));
    }

    if (func == QStringLiteral("grafico_boxplot_distribuicao_por_hora_dia")) {
        fig.insert(QStringLiteral("box_stats"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("box_stats_raw")).toMap(),
                                                       fig.value(QStringLiteral("box_stats")).toMap(), traduzir_hora));
        fig.insert(QStringLiteral("source_map"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("source_map")).toMap(), traduzir_hora));
        fig.insert(QStringLiteral("order_hint"),
                   traduzir_lista_preferindo_raw(fig.value(QStringLiteral("order_hint_raw")).toList(),
                                                 fig.value(QStringLiteral("order_hint")).toList(), traduzir_hora));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("hour_of_day"), QStringLiteral("Hora do Dia")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("size_megabytes_mb"),
                            QStringLiteral("Tamanho (Megabytes) (MB)")));
    }

    if (func == QStringLiteral("grafico_boxplot_distribuicao_por_categoria_tamanho")) {
        fig.insert(QStringLiteral("box_stats"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("box_stats_raw")).toMap(),
                                                       fig.value(QStringLiteral("box_stats")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("source_map"),
                   traduzir_cache_por_tipo(fig.value(QStringLiteral("source_map")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("order_hint"),
                   traduzir_lista_preferindo_raw(fig.value(QStringLiteral("order_hint_raw")).toList(),
                                                 fig.value(QStringLiteral("order_hint")).toList(), traduzir_tipo));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("category"), QStringLiteral("Categoria")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("size_megabytes_mb"),
                            QStringLiteral("Tamanho (Megabytes) (MB)")));
    }

    if (func == QStringLiteral("grafico_boxplot_eventos")) {
        fig.insert(QStringLiteral("top_types"),
                   traduzir_resumo(fig.value(QStringLiteral("top_types")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("top_operations"),
                   traduzir_resumo(fig.value(QStringLiteral("top_operations")).toMap(), traduzir_op));
    }

    if (func == QStringLiteral("grafico_eventos_monitorados_por_tipo")) {
        retraduzir_radar_simples(fig, fig.value(QStringLiteral("raw_labels")).toList(), traduzir_tipo);
    }

    if (func == QStringLiteral("grafico_eventos_monitorados_por_tipo_operacao")) {
        retraduzir_radar_simples(fig, fig.value(QStringLiteral("raw_labels")).toList(), traduzir_op);
    }

    if (func == QStringLiteral("grafico_eventos_monitorados_por_dias_semana")) {
        retraduzir_radar_simples(fig, fig.value(QStringLiteral("raw_labels")).toList(), traduzir_dia_semana);
    }

    if (func == QStringLiteral("grafico_eventos_monitorados_por_hora_dia")) {
        retraduzir_radar_simples(fig, fig.value(QStringLiteral("raw_labels")).toList(), traduzir_hora);
    }

    if (func == QStringLiteral("grafico_heatmap")) {
        fig.insert(QStringLiteral("points"),
                   traduzir_pontos(fig.value(QStringLiteral("points")).toList(), traduzir_op, traduzir_tipo));
        fig.insert(QStringLiteral("operation_order"),
                   traduzir_lista_strings(fig.value(QStringLiteral("operation_order")).toList(), traduzir_op));
        fig.insert(QStringLiteral("op_colors"),
                   traduzir_mapa_chaves(fig.value(QStringLiteral("op_colors")).toMap(), traduzir_op));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("time_minutes"), QStringLiteral("Tempo")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("quantity"), QStringLiteral("Quantidade")));
    }

    if (func == QStringLiteral("grafico_scatter")) {
        fig.insert(QStringLiteral("points"),
                   traduzir_pontos(fig.value(QStringLiteral("points")).toList(), traduzir_op, traduzir_tipo));
        fig.insert(QStringLiteral("operation_order"),
                   traduzir_lista_strings(fig.value(QStringLiteral("operation_order")).toList(), traduzir_op));
        fig.insert(QStringLiteral("op_colors"),
                   traduzir_mapa_chaves(fig.value(QStringLiteral("op_colors")).toMap(), traduzir_op));

        const QString escala = fig.value(QStringLiteral("time_scale")).toString();
        QString x_label = get_text(loc, QStringLiteral("time_seconds"), QStringLiteral("Tempo (segundos)"));
        if (escala == QStringLiteral("milliseconds")) {
            x_label = get_text(loc, QStringLiteral("time_milliseconds"),
                               QStringLiteral("Tempo (milissegundos)"));
        } else if (escala == QStringLiteral("centiseconds")) {
            x_label = get_text(loc, QStringLiteral("time_centiseconds"),
                               QStringLiteral("Tempo (centésimos de segundo)"));
        } else if (escala == QStringLiteral("minutes")) {
            x_label = get_text(loc, QStringLiteral("time_minutes"), QStringLiteral("Tempo (minutos)"));
        } else if (escala == QStringLiteral("hours")) {
            x_label = get_text(loc, QStringLiteral("time_hours"), QStringLiteral("Tempo (horas)"));
        } else if (escala == QStringLiteral("days")) {
            x_label = get_text(loc, QStringLiteral("time_days"), QStringLiteral("Tempo (dias)"));
        }

        fig.insert(QStringLiteral("x_label"), x_label);
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("file_size_mb"),
                            QStringLiteral("Tamanho do Arquivo (MB)")));
    }

    if (func == QStringLiteral("grafico_sankey")) {
        const QVariantList operacoes =
            traduzir_lista_strings(fig.value(QStringLiteral("operations")).toList(), traduzir_op);
        const QVariantList tipos =
            traduzir_lista_strings(fig.value(QStringLiteral("types")).toList(), traduzir_tipo);

        QVariantMap node_colors;
        const QVariantMap cores_originais = fig.value(QStringLiteral("node_colors")).toMap();
        const QVariantList operacoes_originais = fig.value(QStringLiteral("operations")).toList();
        const QVariantList tipos_originais = fig.value(QStringLiteral("types")).toList();

        for (int i = 0; i < operacoes_originais.size() && i < operacoes.size(); ++i) {
            node_colors.insert(operacoes.at(i).toString(),
                               cores_originais.value(operacoes_originais.at(i).toString()));
        }
        for (int i = 0; i < tipos_originais.size() && i < tipos.size(); ++i) {
            node_colors.insert(tipos.at(i).toString(),
                               cores_originais.value(tipos_originais.at(i).toString()));
        }

        QVariantList links;
        for (const QVariant& item : fig.value(QStringLiteral("links")).toList()) {
            QVariantMap link = item.toMap();
            link.insert(QStringLiteral("source"),
                        traduzir_op(link.value(QStringLiteral("source")).toString()));
            link.insert(QStringLiteral("target"),
                        traduzir_tipo(link.value(QStringLiteral("target")).toString()));
            links.append(link);
        }

        fig.insert(QStringLiteral("operations"), operacoes);
        fig.insert(QStringLiteral("types"), tipos);
        fig.insert(QStringLiteral("nodes"), operacoes + tipos);
        fig.insert(QStringLiteral("node_colors"), node_colors);
        fig.insert(QStringLiteral("links"), links);
        fig.insert(QStringLiteral("subtitle"),
                   subtitulo_para_funcao(loc, func, fig.value(QStringLiteral("subtitle")).toString()));
    }

    if (func == QStringLiteral("grafico_radar")) {
        const QVariantList tipos =
            traduzir_lista_strings(fig.value(QStringLiteral("types")).toList(), traduzir_tipo);
        QVariantList tipos_curtos;
        tipos_curtos.reserve(tipos.size());
        for (const QVariant& tipo : tipos) {
            const QString texto = tipo.toString();
            tipos_curtos.append(texto.size() < 15 ? texto : texto.left(12) + QStringLiteral("..."));
        }

        fig.insert(QStringLiteral("types"), tipos);
        fig.insert(QStringLiteral("types_short"), tipos_curtos);
        fig.insert(QStringLiteral("series"),
                   traduzir_series_radar(fig.value(QStringLiteral("series")).toList(), traduzir_op));
    }

    if (func == QStringLiteral("grafico_dotplot")) {
        fig.insert(QStringLiteral("points"),
                   traduzir_pontos(fig.value(QStringLiteral("points")).toList(),
                                   [](const QString& valor) { return valor; }, traduzir_tipo));
        fig.insert(QStringLiteral("types"),
                   traduzir_lista_preferindo_raw(fig.value(QStringLiteral("types_raw")).toList(),
                                                 fig.value(QStringLiteral("types")).toList(), traduzir_tipo));
        fig.insert(QStringLiteral("means_by_type"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("means_by_type_raw")).toMap(),
                                                       fig.value(QStringLiteral("means_by_type")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("counts_by_type"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("counts_by_type_raw")).toMap(),
                                                       fig.value(QStringLiteral("counts_by_type")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("type_colors"),
                   traduzir_mapa_chaves_preferindo_raw(fig.value(QStringLiteral("type_colors_raw")).toMap(),
                                                       fig.value(QStringLiteral("type_colors")).toMap(), traduzir_tipo));
        fig.insert(QStringLiteral("x_label"),
                   get_text(loc, QStringLiteral("file_types"), QStringLiteral("Tipos de Arquivo")));
        fig.insert(QStringLiteral("y_label"),
                   get_text(loc, QStringLiteral("file_size_mb"),
                            QStringLiteral("Tamanho do Arquivo (MB)")));
    }

    if (func == QStringLiteral("grafico_sankey_evento_caminho")) {
        const QVariantList eventos =
            traduzir_lista_strings(fig.value(QStringLiteral("events")).toList(), traduzir_op);
        const QVariantList caminhos = fig.value(QStringLiteral("paths")).toList();
        const QVariantMap cores_originais = fig.value(QStringLiteral("node_colors")).toMap();
        const QVariantList eventos_originais = fig.value(QStringLiteral("events")).toList();

        QVariantMap node_colors;
        for (int i = 0; i < eventos_originais.size() && i < eventos.size(); ++i) {
            node_colors.insert(eventos.at(i).toString(),
                               cores_originais.value(eventos_originais.at(i).toString()));
        }
        for (const QVariant& caminho : caminhos) {
            node_colors.insert(caminho.toString(), cores_originais.value(caminho.toString()));
        }

        QVariantList links;
        for (const QVariant& item : fig.value(QStringLiteral("links")).toList()) {
            QVariantMap link = item.toMap();
            link.insert(QStringLiteral("source"),
                        traduzir_op(link.value(QStringLiteral("source")).toString()));
            links.append(link);
        }

        fig.insert(QStringLiteral("events"), eventos);
        fig.insert(QStringLiteral("nodes"), eventos + caminhos);
        fig.insert(QStringLiteral("node_colors"), node_colors);
        fig.insert(QStringLiteral("links"), links);
        fig.insert(QStringLiteral("subtitle"),
                   subtitulo_para_funcao(loc, func, fig.value(QStringLiteral("subtitle")).toString()));
    }

    if (func == QStringLiteral("grafico_sankey_tipo_caminho")) {
        const QVariantList tipos =
            traduzir_lista_strings(fig.value(QStringLiteral("types")).toList(), traduzir_tipo);
        const QVariantList caminhos = fig.value(QStringLiteral("paths")).toList();
        const QVariantMap cores_originais = fig.value(QStringLiteral("node_colors")).toMap();
        const QVariantList tipos_originais = fig.value(QStringLiteral("types")).toList();

        QVariantMap node_colors;
        for (int i = 0; i < tipos_originais.size() && i < tipos.size(); ++i) {
            node_colors.insert(tipos.at(i).toString(),
                               cores_originais.value(tipos_originais.at(i).toString()));
        }
        for (const QVariant& caminho : caminhos) {
            node_colors.insert(caminho.toString(), cores_originais.value(caminho.toString()));
        }

        QVariantList links;
        for (const QVariant& item : fig.value(QStringLiteral("links")).toList()) {
            QVariantMap link = item.toMap();
            link.insert(QStringLiteral("source"),
                        traduzir_tipo(link.value(QStringLiteral("source")).toString()));
            links.append(link);
        }

        fig.insert(QStringLiteral("types"), tipos);
        fig.insert(QStringLiteral("nodes"), tipos + caminhos);
        fig.insert(QStringLiteral("node_colors"), node_colors);
        fig.insert(QStringLiteral("links"), links);
        fig.insert(QStringLiteral("subtitle"),
                   subtitulo_para_funcao(loc, func, fig.value(QStringLiteral("subtitle")).toString()));
    }

    return fig;
}

void set_toggle_button_text(QWidget* widget, const QString& text) {
    if (widget == nullptr) return;

    if (auto* rotated = dynamic_cast<source::ui::GerenciamentoEstatisticasUI::BotaoRotacionado*>(widget)) {
        rotated->setText(text);
        rotated->updateGeometry();
        return;
    }

    if (auto* button = qobject_cast<QPushButton*>(widget)) {
        button->setText(text);
        button->adjustSize();
        button->updateGeometry();
    }
}

QStackedLayout* obter_layout_estatisticas(QObject* iface) {
    if (iface == nullptr) return nullptr;
    auto* container = iface->property(kStatsContainerProperty).value<QWidget*>();
    return (container != nullptr) ? qobject_cast<QStackedLayout*>(container->layout()) : nullptr;
}

QVariantHash obter_tabs_por_idioma(QObject* iface) {
    return (iface != nullptr) ? iface->property(kStatsTabWidgetsProperty).toHash() : QVariantHash();
}

QTabWidget* obter_tab_widget_idioma(QObject* iface, const QString& idioma) {
    if (iface == nullptr || idioma.isEmpty()) return nullptr;
    return obter_tabs_por_idioma(iface).value(idioma).value<QTabWidget*>();
}

void registrar_tab_widget_idioma(QObject* iface, const QString& idioma, QTabWidget* tab_widget) {
    if (iface == nullptr || idioma.isEmpty() || tab_widget == nullptr) return;

    QVariantHash tabs_por_idioma = obter_tabs_por_idioma(iface);
    tabs_por_idioma.insert(idioma, QVariant::fromValue(tab_widget));
    iface->setProperty(kStatsTabWidgetsProperty, tabs_por_idioma);
}

void salvar_graficos_idioma(QObject* iface, const QString& idioma) {
    if (iface == nullptr || idioma.isEmpty()) return;

    QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    graficos_por_idioma.insert(idioma, iface->property("graficos_dados").toHash());
    iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
}

void salvar_graficos_idioma(QObject* iface, const QString& idioma, const QVariantHash& graficos) {
    if (iface == nullptr || idioma.isEmpty()) return;

    QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    graficos_por_idioma.insert(idioma, graficos);
    iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
}

void restaurar_graficos_idioma(QObject* iface, const QString& idioma) {
    if (iface == nullptr || idioma.isEmpty()) return;

    const QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    iface->setProperty("graficos_dados", graficos_por_idioma.value(idioma).toHash());
}

QVariantHash obter_cache_idioma(QObject* iface, const QString& idioma) {
    if (iface == nullptr || idioma.isEmpty()) {
        return {};
    }
    return iface->property(kStatsGraphDataByLanguageProperty).toHash().value(idioma).toHash();
}

QVariantMap encontrar_entry_por_func(const QVariantHash& cache, const QString& func) {
    for (auto it = cache.constBegin(); it != cache.constEnd(); ++it) {
        const QVariantMap entry = it.value().toMap();
        if (entry.value(QStringLiteral("func")).toString() == func) {
            return entry;
        }
    }
    return {};
}

bool cache_completo_para_lista(const QVariantHash& cache, const QVariantList& graficos_atualizados) {
    if (cache.isEmpty()) {
        return false;
    }

    for (const QVariant& item : graficos_atualizados) {
        const QString func = item.toMap().value(QStringLiteral("func")).toString();
        if (func.isEmpty()) {
            continue;
        }
        if (encontrar_entry_por_func(cache, func).isEmpty()) {
            return false;
        }
    }
    return true;
}

QVariantHash encontrar_cache_fonte(QObject* iface,
                                   const QString& idioma_preferido,
                                   const QVariantList& graficos_atualizados) {
    if (iface == nullptr) {
        return {};
    }

    QVariantHash caches = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    const QVariantHash preferido = caches.value(idioma_preferido).toHash();
    if (cache_completo_para_lista(preferido, graficos_atualizados)) {
        return preferido;
    }

    if (!preferido.isEmpty()) {
        return preferido;
    }

    for (auto it = caches.constBegin(); it != caches.constEnd(); ++it) {
        const QVariantHash candidato = it.value().toHash();
        if (cache_completo_para_lista(candidato, graficos_atualizados)) {
            return candidato;
        }
    }

    for (auto it = caches.constBegin(); it != caches.constEnd(); ++it) {
        const QVariantHash candidato = it.value().toHash();
        if (!candidato.isEmpty()) {
            return candidato;
        }
    }

    return {};
}

QVariantHash traduzir_cache_idioma(source::ui::LocalizadorQt* loc,
                                   const QVariantHash& cache_fonte,
                                   const QVariantList& graficos_atualizados) {
    QVariantHash traduzido;
    if (loc == nullptr || cache_fonte.isEmpty()) {
        return traduzido;
    }

    for (const QVariant& item : graficos_atualizados) {
        const QVariantMap grafico = item.toMap();
        const QString func = grafico.value(QStringLiteral("func")).toString();
        const QString titulo_alvo = grafico.value(QStringLiteral("titulo")).toString();
        const QVariantMap entry_origem = encontrar_entry_por_func(cache_fonte, func);
        if (entry_origem.isEmpty()) {
            continue;
        }

        QVariantMap entry = entry_origem;
        entry.insert(QStringLiteral("titulo"), titulo_alvo);
        entry.insert(QStringLiteral("fig"),
                     retraduzir_figura(loc, func, entry_origem.value(QStringLiteral("fig")).toMap(), titulo_alvo));
        traduzido.insert(titulo_alvo, entry);
    }

    return traduzido;
}

void reconstruir_abas_do_cache(source::ui::GerenciadorEstatisticasUI* self,
                               QTabWidget* tab_widget,
                               const QVariantHash& cache,
                               const QVariantList& graficos_atualizados) {
    if (self == nullptr || tab_widget == nullptr) {
        return;
    }

    while (tab_widget->count() > 0) {
        QWidget* pagina = tab_widget->widget(0);
        tab_widget->removeTab(0);
        if (pagina != nullptr) {
            pagina->deleteLater();
        }
    }

    for (const QVariant& item : graficos_atualizados) {
        const QVariantMap grafico = item.toMap();
        const QString titulo = grafico.value(QStringLiteral("titulo")).toString();
        const QString func = grafico.value(QStringLiteral("func")).toString();
        QVariantMap entry = cache.value(titulo).toMap();
        if (entry.isEmpty()) {
            entry = encontrar_entry_por_func(cache, func);
        }
        if (entry.isEmpty()) {
            continue;
        }

        const QVariant fig = entry.value(QStringLiteral("fig"));
        tab_widget->addTab(
            source::ui::GerenciamentoEstatisticasUI::criar_widget_grafico_localizado(
                titulo, fig, self->loc_obj()),
            titulo);
    }
}

bool geracao_em_andamento(QObject* iface) {
    if (iface == nullptr) return false;

    return iface->property("worker").isValid() ||
           iface->property("gerando_grafico").toBool() ||
           !iface->property("fila_graficos").toList().isEmpty();
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_graficos_apos_mudanca_idioma(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(self->loc_obj());
        auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>();
        if (!dialog || !dialog->isVisible()) return;

        iface->setProperty("_atualizando_idioma", true);
        dialog->setWindowTitle(get_text(self->loc_obj(), "statistics", "Statistics"));

        if (auto* btn_toggle = iface->property("btn_toggle_painel").value<QWidget*>()) {
            if (iface->property("painel_recolhido").toBool()) {
                set_toggle_button_text(btn_toggle,
                                       get_text(self->loc_obj(), "expand_selection_panel", "Expandir painel de seleção"));
            } else {
                set_toggle_button_text(btn_toggle,
                                       get_text(self->loc_obj(), "hide_selection_panel", "Ocultar painel de seleção"));
            }
            atualizar_layout_apos_mudanca_botao(self);
        }

        atualizar_textos_painel_selecao(self);
        ajustar_largura_painel_selecao(self);

        const QString idioma_novo = obter_idioma_atual(self->loc_obj());
        const QString idioma_anterior = iface->property(kStatsActiveLanguageProperty).toString();
        QStackedLayout* stack = obter_layout_estatisticas(iface);
        if (stack == nullptr) {
            iface->setProperty("_atualizando_idioma", false);
            return;
        }

        const QHash<QString, bool> estados_checkboxes = obter_estados_checkboxes(self);
        const QVariantList graficos_atualizados = criar_lista_graficos(self);
        const QVariantHash mapeamento_funcoes = criar_mapeamento_funcoes(self, graficos_atualizados);

        const bool mudando_para_outro_idioma = !idioma_anterior.isEmpty() && idioma_novo != idioma_anterior;
        if (mudando_para_outro_idioma && geracao_em_andamento(iface)) {
            salvar_graficos_idioma(iface, idioma_anterior);
            cancelar_geracao_graficos(self);
        } else if (mudando_para_outro_idioma) {
            salvar_graficos_idioma(iface, idioma_anterior);
        }

        auto* tab_widget = obter_tab_widget_idioma(iface, idioma_novo);
        bool precisa_gerar = (tab_widget == nullptr);

        if (tab_widget == nullptr) {
            auto* container = iface->property(kStatsContainerProperty).value<QWidget*>();
            tab_widget = new QTabWidget(container);
            stack->addWidget(tab_widget);
            registrar_tab_widget_idioma(iface, idioma_novo, tab_widget);
        }

        if (stack->currentWidget() != tab_widget) {
            stack->setCurrentWidget(tab_widget);
        }

        iface->setProperty("tab_widget", QVariant::fromValue(tab_widget));
        iface->setProperty(kStatsActiveLanguageProperty, idioma_novo);

        QVariantHash cache_alvo = obter_cache_idioma(iface, idioma_novo);
        if (!cache_completo_para_lista(cache_alvo, graficos_atualizados)) {
            const QVariantHash cache_fonte =
                encontrar_cache_fonte(iface, idioma_anterior, graficos_atualizados);
            if (!cache_fonte.isEmpty() && loc != nullptr) {
                cache_alvo = traduzir_cache_idioma(loc, cache_fonte, graficos_atualizados);
                if (!cache_alvo.isEmpty()) {
                    salvar_graficos_idioma(iface, idioma_novo, cache_alvo);
                }
            }
        }

        iface->setProperty("graficos_dados", cache_alvo);

        const bool cache_pronto = cache_completo_para_lista(cache_alvo, graficos_atualizados);
        if (cache_pronto && tab_widget->count() < graficos_atualizados.size()) {
            reconstruir_abas_do_cache(self, tab_widget, cache_alvo, graficos_atualizados);
        }

        if (!precisa_gerar && !cache_pronto) {
            precisa_gerar = true;
        }

        atualizar_checkboxes_graficos(self, graficos_atualizados, estados_checkboxes, mapeamento_funcoes);

        if (precisa_gerar) {
            iface->setProperty("graficos_dados", QVariantHash());
            salvar_graficos_idioma(iface, idioma_novo);
            gerar_todos_graficos(self, graficos_atualizados);
        }

        iface->setProperty("_atualizando_idioma", false);
    } catch (...) {
        if (self && self->interface_obj()) {
            self->interface_obj()->setProperty("_atualizando_idioma", false);
        }
        source::utils::LogManager::get_logger().error("Erro ao atualizar gráficos após mudança de idioma", true);
    }
}

}
