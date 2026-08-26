#include "geui_17_regenerar_graficos_existentes.h"

#include <QApplication>
#include <QMessageBox>
#include <QMetaObject>
#include <QProgressDialog>
#include <QTabWidget>
#include <QTimer>
#include <QVariantHash>
#include <QVariantMap>

#include "geui_14_gerar_todos_graficos.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kStatsGraphDataByLanguageProperty = "_estatisticas_graficos_dados_por_idioma";
constexpr auto kStatsActiveLanguageProperty = "_estatisticas_idioma_ativo";

QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key),
                                  Q_ARG(QString, fallback));
    }
    return text;
}

QString obter_idioma_estatisticas(QObject* iface) {
    if (iface == nullptr) {
        return QStringLiteral("pt_BR");
    }
    const QString idioma = iface->property(kStatsActiveLanguageProperty).toString();
    return idioma.isEmpty() ? QStringLiteral("pt_BR") : idioma;
}

void sincronizar_graficos_idioma_ativo(QObject* iface, const QVariantHash& dados) {
    if (iface == nullptr) {
        return;
    }
    QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    graficos_por_idioma.insert(obter_idioma_estatisticas(iface), dados);
    iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
}

QObject* contexto_agendamento_estatisticas(QObject* iface) {
    if (iface == nullptr) {
        return nullptr;
    }
    QObject* dialog = iface->property("dialog_estatisticas").value<QObject*>();
    return dialog != nullptr ? dialog : iface;
}

bool dialogo_estatisticas_ativo(QObject* iface) {
    if (iface == nullptr) {
        return false;
    }
    auto* dialog = iface->property("dialog_estatisticas").value<QWidget*>();
    return dialog != nullptr && dialog->isVisible();
}

source::data::GeradorEstatisticas* obter_gerador(QObject* iface) {
    if (iface == nullptr) {
        return nullptr;
    }
    const qulonglong ptr = iface->property("_gerador_cpp_ptr").toULongLong();
    return ptr == 0 ? nullptr : reinterpret_cast<source::data::GeradorEstatisticas*>(ptr);
}

QVariant chamar_funcao_grafico(source::data::GeradorEstatisticas* gerador, const QString& func_name) {
    if (gerador == nullptr) {
        return {};
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
    return {};
}

void cancelar_regeneracao(source::ui::GerenciadorEstatisticasUI* self) {
    if (self == nullptr || self->interface_obj() == nullptr) {
        return;
    }
    QObject* iface = self->interface_obj();
    iface->setProperty("graficos_para_regenerar", QVariantList());
    iface->setProperty("indice_regeneracao_atual", 0);
    if (auto* progress = iface->property("progress_regeneracao").value<QProgressDialog*>()) {
        progress->close();
        progress->deleteLater();
        iface->setProperty("progress_regeneracao", QVariant());
    }
}

void regenerar_proximo_grafico(source::ui::GerenciadorEstatisticasUI* self);

void agendar_regeneracao_graficos(source::ui::GerenciadorEstatisticasUI* self, int delay_ms) {
    if (self == nullptr || self->interface_obj() == nullptr) {
        return;
    }
    QObject* contexto = contexto_agendamento_estatisticas(self->interface_obj());
    if (contexto != nullptr) {
        QTimer::singleShot(delay_ms, contexto, [self]() { regenerar_proximo_grafico(self); });
    } else {
        QTimer::singleShot(delay_ms, [self]() { regenerar_proximo_grafico(self); });
    }
}

void regenerar_proximo_grafico(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        const QVariantList fila = iface->property("graficos_para_regenerar").toList();
        const int indice = iface->property("indice_regeneracao_atual").toInt();
        if (indice >= fila.size()) {
            cancelar_regeneracao(self);
            return;
        }

        auto* tab_widget = iface->property("tab_widget").value<QTabWidget*>();
        if (tab_widget == nullptr) {
            cancelar_regeneracao(self);
            return;
        }

        const QVariantMap info = fila.at(indice).toMap();
        const int tab_index = info.value("index").toInt();
        const QString titulo_atual = info.value("titulo_atual").toString();
        const QVariantMap grafico = info.value("grafico").toMap();

        if (auto* progress = iface->property("progress_regeneracao").value<QProgressDialog*>()) {
            progress->setValue(indice);
            progress->setLabelText(
                QStringLiteral("%1 %2")
                    .arg(get_text(self->loc_obj(), "updating_chart", "Atualizando gráfico:"))
                    .arg(grafico.value("titulo").toString()));
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            if (progress->wasCanceled()) {
                cancelar_regeneracao(self);
                return;
            }
        }

        try {
            source::data::GeradorEstatisticas* gerador = obter_gerador(iface);
            if (gerador != nullptr) {
                gerador->atualizar_textos_traduzidos();
            }

            const QString func_name = grafico.value("func").toString();
            const QVariant nova_fig = chamar_funcao_grafico(gerador, func_name);
            const QString novo_titulo = grafico.value("titulo").toString();

            QVariantHash graficos_dados = iface->property("graficos_dados").toHash();
            QVariantMap novo_dado;
            novo_dado.insert("fig", nova_fig);
            novo_dado.insert("func", func_name);
            novo_dado.insert("titulo", novo_titulo);
            graficos_dados.insert(novo_titulo, novo_dado);
            if (titulo_atual != novo_titulo && graficos_dados.contains(titulo_atual)) {
                graficos_dados.remove(titulo_atual);
            }
            iface->setProperty("graficos_dados", graficos_dados);
            sincronizar_graficos_idioma_ativo(iface, graficos_dados);

            QWidget* nova_aba =
                source::ui::GerenciamentoEstatisticasUI::criar_widget_grafico_localizado(
                    novo_titulo, nova_fig, self->loc_obj());

            if (tab_index >= 0 && tab_index < tab_widget->count()) {
                const bool aba_atual = (tab_widget->currentIndex() == tab_index);
                QWidget* aba_antiga = tab_widget->widget(tab_index);
                tab_widget->removeTab(tab_index);
                if (aba_antiga != nullptr) {
                    aba_antiga->deleteLater();
                }
                tab_widget->insertTab(tab_index, nova_aba, novo_titulo);
                if (aba_atual) {
                    tab_widget->setCurrentIndex(tab_index);
                }
            } else {
                tab_widget->addTab(nova_aba, novo_titulo);
            }
        } catch (...) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro ao regenerar grafico %1").arg(grafico.value("titulo").toString()), true);
            if (!iface->property("_atualizando_idioma").toBool()) {
                QMessageBox::warning(
                    iface->property("dialog_estatisticas").value<QWidget*>(),
                    get_text(self->loc_obj(), "error", "Erro"),
                    QStringLiteral("%1 %2")
                        .arg(get_text(self->loc_obj(), "error_updating_chart", "Erro ao atualizar grafico"))
                        .arg(grafico.value("titulo").toString()));
            }
        }

        iface->setProperty("indice_regeneracao_atual", indice + 1);
        agendar_regeneracao_graficos(self, iface->property("_atualizando_idioma").toBool() ? 10 : 50);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro durante regeneracao de grafico", true);
    }
}

}  // namespace

namespace source::ui::GerenciamentoEstatisticasUI {

void regenerar_graficos_existentes(source::ui::GerenciadorEstatisticasUI* self,
                                   const QVariantList& graficos_atualizados,
                                   const QVariantHash& mapeamento_funcoes) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        if (!dialogo_estatisticas_ativo(iface)) {
            return;
        }

        auto* tab = iface->property("tab_widget").value<QTabWidget*>();
        if (tab == nullptr || tab->count() == 0) {
            return;
        }

        if (auto* progress = iface->property("progress_regeneracao").value<QProgressDialog*>()) {
            progress->close();
            progress->deleteLater();
            iface->setProperty("progress_regeneracao", QVariant());
        }

        const QVariantHash checkboxes = iface->property("checkboxes_graficos").toHash();
        const QVariantHash graficos_dados = iface->property("graficos_dados").toHash();

        QVariantList graficos_para_atualizar;
        for (int i = 0; i < tab->count(); ++i) {
            const QString titulo_atual = tab->tabText(i);
            QString func_correspondente;

            for (auto it = checkboxes.cbegin(); it != checkboxes.cend(); ++it) {
                const QVariantMap data = it.value().toMap();
                const QVariantMap grafico_data = data.value("grafico_data").toMap();
                const QString func = grafico_data.value("func").toString();
                if (it.key() == titulo_atual || mapeamento_funcoes.value(func).toString() == titulo_atual) {
                    func_correspondente = func;
                    break;
                }
            }

            if (func_correspondente.isEmpty() && graficos_dados.contains(titulo_atual)) {
                func_correspondente = graficos_dados.value(titulo_atual).toMap().value("func").toString();
            }
            if (func_correspondente.isEmpty()) {
                continue;
            }

            QVariantMap grafico_atualizado;
            for (const QVariant& item : graficos_atualizados) {
                const QVariantMap grafico = item.toMap();
                if (grafico.value("func").toString() == func_correspondente) {
                    grafico_atualizado = grafico;
                    break;
                }
            }
            if (grafico_atualizado.isEmpty()) {
                continue;
            }

            QVariantMap info;
            info.insert("index", i);
            info.insert("titulo_atual", titulo_atual);
            info.insert("grafico", grafico_atualizado);
            graficos_para_atualizar.append(info);
        }

        if (graficos_para_atualizar.isEmpty()) {
            return;
        }

        iface->setProperty("graficos_para_regenerar", graficos_para_atualizar);
        iface->setProperty("indice_regeneracao_atual", 0);

        if (!iface->property("_atualizando_idioma").toBool()) {
            auto* progress = new QProgressDialog(
                get_text(self->loc_obj(), "updating_charts", "Atualizando graficos..."),
                get_text(self->loc_obj(), "cancel", "Cancelar"),
                0,
                graficos_para_atualizar.size(),
                iface->property("dialog_estatisticas").value<QWidget*>());
            progress->setWindowTitle(get_text(self->loc_obj(), "please_wait", "Aguarde"));
            progress->setModal(false);
            progress->setMinimumDuration(0);
            progress->setAutoClose(false);
            QObject::connect(progress, &QProgressDialog::canceled, iface, [self]() { cancelar_regeneracao(self); });
            iface->setProperty("progress_regeneracao", QVariant::fromValue(progress));
        }

        agendar_regeneracao_graficos(self, 50);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao preparar regeneracao de graficos", true);
    }
}

}  // namespace source::ui::GerenciamentoEstatisticasUI
