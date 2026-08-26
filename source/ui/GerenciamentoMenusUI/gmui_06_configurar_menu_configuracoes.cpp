#include "gmui_06_configurar_menu_configuracoes.h"

#include <QAction>
#include <QActionGroup>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMetaObject>
#include <QSet>
#include <QStringList>
#include <QVariant>

#include "gmui_01_MenuPersistente.h"
#include "gmui_08_criar_submenu_cores.h"
#include "gmui_11_criar_submenu_colunas.h"
#include "gmui_13_criar_submenu_colunas_coloridas.h"
#include "gmui_16_criar_submenu_exportacao.h"
#include "source/security/secui_07_configurar_menu_seguranca.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kGraficosConfigFile = "statistics_charts_config.json";
constexpr auto kGraficosSelecionadosKey = "graficos_selecionados";
constexpr auto kGraficosSelecionadosProperty = "_estatisticas_funcoes_graficos_selecionados";

struct ItemGraficoEstatisticas {
    const char* chave;
    const char* titulo_pt_br;
    const char* func;
};

QString gt(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString t = fallback;
    if (loc) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty() && translated != key) {
            t = translated;
        }
    }
    return t;
}

const QList<ItemGraficoEstatisticas>& itens_graficos_estatisticas() {
    static const QList<ItemGraficoEstatisticas> itens = {
        {"operations_pie", "Gráfico de Pizza", "grafico_operacoes_pizza"},
        {"bar_chart", "Gráfico de Barras", "grafico_tipos_arquivo_barras"},
        {"timeline", "Linha do Tempo", "grafico_timeline_operacoes"},
        {"tree_map", "Mapa de Árvore", "grafico_treemap_tipos"},
        {"hour_histogram", "Histograma de Horas", "grafico_histograma_horarios"},
        {"pareto_analysis", "Diagrama de Pareto", "grafico_pareto_operacoes"},
        {"operations_by_day", "Operações por Dia", "grafico_cluster_linha"},
        {"size_distribution_by_operation_type", "Distribuição de Tamanhos por Tipo de Operação",
         "grafico_boxplot_distribuicao_por_tipo_operacao"},
        {"size_distribution_by_type", "Distribuição de Tamanhos por Tipo", "grafico_boxplot_distribuicao_por_tipo"},
        {"size_distribution_by_hour_of_day", "Distribuição de Tamanhos por Hora do Dia",
         "grafico_boxplot_distribuicao_por_hora_dia"},
        {"size_distribution_by_size_categories", "Distribuição de Tamanhos por Categorias de Tamanho",
         "grafico_boxplot_distribuicao_por_categoria_tamanho"},
        {"events_monitored_by_type", "Eventos Monitorados por Tipo", "grafico_eventos_monitorados_por_tipo"},
        {"events_monitored_by_operation_type", "Eventos Monitorados por Tipo de Operação",
         "grafico_eventos_monitorados_por_tipo_operacao"},
        {"events_monitored_by_week_days", "Eventos Monitorados por Dias da Semana",
         "grafico_eventos_monitorados_por_dias_semana"},
        {"events_monitored_by_hour_of_day", "Eventos Monitorados por Hora do Dia",
         "grafico_eventos_monitorados_por_hora_dia"},
        {"temporal_distribution", "Distribuição Temporal", "grafico_heatmap"},
        {"file_size_analysis", "Análise de Tamanhos de Arquivo", "grafico_scatter"},
        {"file_operations_flow", "Fluxo de Operações em Arquivos", "grafico_sankey"},
        {"event_to_path_flow", "Fluxo de Evento por Diretório", "grafico_sankey_evento_caminho"},
        {"type_to_path_flow", "Fluxo de Tipo de Arquivo por Diretório", "grafico_sankey_tipo_caminho"},
        {"operations_by_file_type", "Operações por Tipo de Arquivo", "grafico_radar"},
        {"file_size_distribution", "Distribuição de Tamanhos de Arquivo", "grafico_dotplot"},
        {"directory_tree", "Árvore de Diretórios", "grafico_arvore_diretorios"},
    };
    return itens;
}

QString config_path_graficos_estatisticas() {
    return QDir(source::utils::obter_caminho_persistente()).filePath(QString::fromUtf8(kGraficosConfigFile));
}

QJsonObject carregar_config_json() {
    QFile file(config_path_graficos_estatisticas());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.isObject() ? doc.object() : QJsonObject();
}

bool salvar_config_json(const QJsonObject& obj) {
    QFile file(config_path_graficos_estatisticas());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        source::utils::LogManager::get_logger().warning(
            QStringLiteral("Não foi possível salvar as preferências de gráficos em statistics_charts_config.json"));
        return false;
    }

    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QStringList todas_funcoes_graficos_estatisticas() {
    QStringList funcoes;
    for (const auto& item : itens_graficos_estatisticas()) {
        funcoes.append(QString::fromUtf8(item.func));
    }
    return funcoes;
}

QStringList funcoes_validas_na_ordem(const QJsonArray& arr) {
    QSet<QString> selecionadas_lidas;
    const QStringList todas = todas_funcoes_graficos_estatisticas();
    QSet<QString> validas;
    for (const QString& func : todas) {
        validas.insert(func);
    }

    for (const QJsonValue& value : arr) {
        const QString func = value.toString();
        if (validas.contains(func)) {
            selecionadas_lidas.insert(func);
        }
    }

    QStringList selecionadas;
    for (const QString& func : todas) {
        if (selecionadas_lidas.contains(func)) {
            selecionadas.append(func);
        }
    }
    return selecionadas;
}

QStringList carregar_funcoes_graficos_selecionados() {
    const QJsonObject obj = carregar_config_json();
    const QJsonValue value = obj.value(QString::fromUtf8(kGraficosSelecionadosKey));
    if (!value.isArray()) {
        return todas_funcoes_graficos_estatisticas();
    }
    return funcoes_validas_na_ordem(value.toArray());
}

void salvar_funcoes_graficos_selecionados(QObject* iface, const QStringList& funcoes) {
    QJsonObject obj = carregar_config_json();
    QJsonArray arr;
    for (const QString& func : funcoes) {
        arr.append(func);
    }

    obj.insert(QString::fromUtf8(kGraficosSelecionadosKey), arr);
    salvar_config_json(obj);
    if (iface != nullptr) {
        iface->setProperty(kGraficosSelecionadosProperty, funcoes);
    }
}

void criar_submenu_estatisticas(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    if (self == nullptr || menu_configuracoes == nullptr || self->interface_obj() == nullptr) {
        return;
    }

    QObject* iface = self->interface_obj();
    auto* parent_widget = qobject_cast<QWidget*>(iface);
    auto* submenu_estatisticas = new source::ui::GerenciamentoMenusUI::MenuPersistente(
        gt(self->loc_obj(), QStringLiteral("statistics"), QStringLiteral("Estatísticas")).remove(':').trimmed(),
        parent_widget);
    auto* submenu_plotar = new source::ui::GerenciamentoMenusUI::MenuPersistente(
        gt(self->loc_obj(), QStringLiteral("plot_graphs"), QStringLiteral("Plotar Gráficos")),
        parent_widget);

    menu_configuracoes->addMenu(submenu_estatisticas);
    submenu_estatisticas->addMenu(submenu_plotar);

    const QStringList selecionadas = carregar_funcoes_graficos_selecionados();
    iface->setProperty(kGraficosSelecionadosProperty, selecionadas);
    salvar_funcoes_graficos_selecionados(iface, selecionadas);

    for (const auto& item : itens_graficos_estatisticas()) {
        const QString func = QString::fromUtf8(item.func);
        auto* acao = new QAction(gt(self->loc_obj(), QString::fromUtf8(item.chave), QString::fromUtf8(item.titulo_pt_br)), iface);
        acao->setCheckable(true);
        acao->setChecked(selecionadas.contains(func));
        acao->setData(func);

        QObject::connect(acao, &QAction::toggled, iface, [iface, submenu_plotar]() {
            QStringList novas_selecionadas;
            for (QAction* item_acao : submenu_plotar->actions()) {
                if (item_acao != nullptr && item_acao->isCheckable() && item_acao->isChecked()) {
                    novas_selecionadas.append(item_acao->data().toString());
                }
            }
            salvar_funcoes_graficos_selecionados(iface, novas_selecionadas);
        });

        submenu_plotar->addAction(acao);
    }
}
}

namespace source::ui::GerenciamentoMenusUI {

void configurar_menu_configuracoes(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (!self || !menu_configuracoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();

        auto* submenu_filtros = new MenuPersistente(gt(self->loc_obj(), "filters", "Filtros"), qobject_cast<QWidget*>(iface));
        menu_configuracoes->addMenu(submenu_filtros);

        auto* grupo = new QActionGroup(iface);
        grupo->setExclusive(false);
        const QStringList ops = {"op_moved", "op_renamed", "op_added", "op_deleted", "op_modified", "op_scanned"};
        for (const QString& op : ops) {
            auto* acao = new QAction(gt(self->loc_obj(), op, op), iface);
            acao->setCheckable(true);
            acao->setChecked(true);
            acao->setData(op);
            QObject::connect(acao, &QAction::triggered, iface, [iface, acao]() {
                iface->setProperty("sender_action", QVariant::fromValue(acao));
                QMetaObject::invokeMethod(iface, "alternar_filtro");
                iface->setProperty("sender_action", QVariant());
            });
            grupo->addAction(acao);
            submenu_filtros->addAction(acao);
        }

        submenu_filtros->addSeparator();
        auto* acao_avanc = new QAction(gt(self->loc_obj(), "advanced_filters", "Filtros avançados"), iface);
        QObject::connect(acao_avanc, &QAction::triggered, iface, [iface]() { QMetaObject::invokeMethod(iface, "abrir_janela_filtros"); });
        submenu_filtros->addAction(acao_avanc);

        criar_submenu_colunas(self, menu_configuracoes);
        criar_submenu_colunas_coloridas(self, menu_configuracoes);
        criar_submenu_cores(self, menu_configuracoes);
        criar_submenu_exportacao(self, menu_configuracoes);
        criar_submenu_estatisticas(self, menu_configuracoes);
        source::security::ui::criar_submenu_seguranca(self, menu_configuracoes);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar menu configurações", true);
    }
}

}
