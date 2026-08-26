#include "geui_01_mostrar_estatisticas.h"

#include <QApplication>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMetaObject>
#include <QPointer>
#include <QScreen>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSplitter>
#include <QTimer>
#include <QVariantList>
#include <QWidget>
#include <QWindow>

#include "geui_03_limpar_referencia_dialog.h"
#include "geui_05_criar_painel_selecao.h"
#include "geui_06_criar_painel_graficos.h"
#include "geui_08_criar_lista_graficos.h"
#include "geui_10_popular_checkboxes.h"
#include "geui_14_gerar_todos_graficos.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kGraficosConfigFile = "statistics_charts_config.json";
constexpr auto kGraficosSelecionadosKey = "graficos_selecionados";
constexpr auto kGraficosSelecionadosProperty = "_estatisticas_funcoes_graficos_selecionados";

QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}

QString format_text(QString pattern, const QStringList& args) {
    for (const QString& value : args) {
        const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
        if (brace_pos >= 0) {
            pattern.replace(brace_pos, 2, value);
        } else if (pattern.contains(QLatin1Char('%'))) {
            pattern = pattern.arg(value);
        }
    }
    return pattern;
}

QString montar_mensagem_erro_estatisticas(QObject* loc, const QString& detalhe) {
    const QString pattern = get_text(loc, "stats_error", "Erro ao mostrar estatísticas: {}");
    QString mensagem = format_text(pattern, {detalhe});
    if (!detalhe.trimmed().isEmpty() && !mensagem.contains(detalhe)) {
        mensagem = mensagem.trimmed();
        if (!mensagem.endsWith(':')) {
            mensagem += QStringLiteral(":");
        }
        mensagem += QStringLiteral(" ") + detalhe;
    }
    return mensagem;
}

QString config_path_graficos_estatisticas() {
    return QDir(source::utils::obter_caminho_persistente()).filePath(QString::fromUtf8(kGraficosConfigFile));
}

QStringList funcoes_graficos_na_ordem(const QVariantList& graficos) {
    QStringList funcoes;
    for (const QVariant& value : graficos) {
        const QString func = value.toMap().value(QStringLiteral("func")).toString();
        if (!func.isEmpty() && !funcoes.contains(func)) {
            funcoes.append(func);
        }
    }
    return funcoes;
}

QStringList ordenar_e_validar_funcoes(const QStringList& candidatas, const QVariantList& graficos) {
    const QStringList todas = funcoes_graficos_na_ordem(graficos);
    QSet<QString> candidatas_set;
    for (const QString& func : candidatas) {
        candidatas_set.insert(func);
    }

    QStringList selecionadas;
    for (const QString& func : todas) {
        if (candidatas_set.contains(func)) {
            selecionadas.append(func);
        }
    }
    return selecionadas;
}

QStringList carregar_funcoes_graficos_selecionados(QObject* interface_obj, const QVariantList& graficos) {
    const QStringList todas = funcoes_graficos_na_ordem(graficos);
    if (interface_obj != nullptr) {
        const QVariant prop = interface_obj->property(kGraficosSelecionadosProperty);
        if (prop.isValid()) {
            return ordenar_e_validar_funcoes(prop.toStringList(), graficos);
        }
    }

    QFile file(config_path_graficos_estatisticas());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        if (interface_obj != nullptr) {
            interface_obj->setProperty(kGraficosSelecionadosProperty, todas);
        }
        return todas;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonObject obj = doc.isObject() ? doc.object() : QJsonObject();
    const QJsonValue value = obj.value(QString::fromUtf8(kGraficosSelecionadosKey));
    if (!value.isArray()) {
        if (interface_obj != nullptr) {
            interface_obj->setProperty(kGraficosSelecionadosProperty, todas);
        }
        return todas;
    }

    QStringList candidatas;
    for (const QJsonValue& item : value.toArray()) {
        candidatas.append(item.toString());
    }

    const QStringList selecionadas = ordenar_e_validar_funcoes(candidatas, graficos);
    if (interface_obj != nullptr) {
        interface_obj->setProperty(kGraficosSelecionadosProperty, selecionadas);
    }
    return selecionadas;
}

QVariantList filtrar_graficos_selecionados(QObject* interface_obj, const QVariantList& graficos) {
    const QStringList selecionadas = carregar_funcoes_graficos_selecionados(interface_obj, graficos);
    QSet<QString> selecionadas_set;
    for (const QString& func : selecionadas) {
        selecionadas_set.insert(func);
    }

    QVariantList filtrados;
    for (const QVariant& value : graficos) {
        const QVariantMap grafico = value.toMap();
        if (selecionadas_set.contains(grafico.value(QStringLiteral("func")).toString())) {
            filtrados.append(value);
        }
    }
    return filtrados;
}

QScreen* obter_tela_alvo(QWidget* parent) {
    if (parent != nullptr) {
        QWidget* janela = parent->window();
        const QRect geometria = janela != nullptr ? janela->frameGeometry() : parent->frameGeometry();
        if (QScreen* tela = QGuiApplication::screenAt(geometria.center())) {
            return tela;
        }

        if (janela != nullptr && janela->windowHandle() != nullptr && janela->windowHandle()->screen() != nullptr) {
            return janela->windowHandle()->screen();
        }
    }

    return QGuiApplication::primaryScreen();
}

void posicionar_dialogo_na_tela(QDialog* dialog, QScreen* tela) {
    if (dialog == nullptr || tela == nullptr) {
        return;
    }

    const QRect area = tela->availableGeometry();
    QSize minimo = dialog->minimumSize();
    minimo.setWidth(qMin(minimo.width(), qMax(640, area.width() - 40)));
    minimo.setHeight(qMin(minimo.height(), qMax(480, area.height() - 40)));
    dialog->setMinimumSize(minimo);

    QSize tamanho = dialog->sizeHint().expandedTo(dialog->minimumSize());
    if (tamanho.width() > area.width()) {
        tamanho.setWidth(qMax(640, area.width() - 40));
    }
    if (tamanho.height() > area.height()) {
        tamanho.setHeight(qMax(480, area.height() - 40));
    }

    dialog->resize(tamanho);
    dialog->move(area.center() - QPoint(tamanho.width() / 2, tamanho.height() / 2));
}

QString obter_db_path(QObject* evento_base_obj) {
    if (evento_base_obj == nullptr) {
        return QString();
    }

    QString db_path;
    if (!QMetaObject::invokeMethod(evento_base_obj, "db_path", Q_RETURN_ARG(QString, db_path))) {
        db_path = evento_base_obj->property("db_path").toString();
    }
    return db_path;
}

QString obter_db_path_com_fallback(QObject* interface_obj, QObject* evento_base_obj) {
    QString db_path = obter_db_path(evento_base_obj);
    if (!db_path.isEmpty()) {
        return db_path;
    }

    if (interface_obj != nullptr) {
        const QString db_property = interface_obj->property("db_path").toString();
        if (!db_property.isEmpty()) {
            return db_property;
        }

        const QString monitoramento_db_path = interface_obj->property("_monitoramento_db_path").toString();
        if (!monitoramento_db_path.isEmpty()) {
            return monitoramento_db_path;
        }

        const qulonglong obs_ptr = interface_obj->property("_observador_cpp_ptr").toULongLong();
        if (obs_ptr != 0) {
            auto* obs = reinterpret_cast<source::services::Observador*>(obs_ptr);
            if (obs != nullptr && obs->evento_base != nullptr && !obs->evento_base->db_path.isEmpty()) {
                return obs->evento_base->db_path;
            }
        }
    }

    return QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("monitoramento.db"));
}

void liberar_gerador_anterior(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    const qulonglong ptr = interface_obj->property("_gerador_cpp_ptr").toULongLong();
    if (ptr != 0) {
        delete reinterpret_cast<source::data::GeradorEstatisticas*>(ptr);
        interface_obj->setProperty("_gerador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
    }
}

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

bool grafico_sem_dados(const QVariant& grafico) {
    const QVariantMap mapa = grafico.toMap();
    return mapa.value(QStringLiteral("kind")).toString() == QStringLiteral("empty");
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void mostrar_estatisticas(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) {
            return;
        }

        QObject* interface_obj = self->interface_obj();
        QObject* loc = self->loc_obj();
        QWidget* parent = qobject_cast<QWidget*>(interface_obj);

        QDialog* dialog = interface_obj->property("dialog_estatisticas").value<QDialog*>();
        if (dialog && dialog->isVisible()) {
            dialog->raise();
            dialog->activateWindow();
            return;
        }

        const QString db_path = obter_db_path_com_fallback(interface_obj, self->evento_base_obj());
        if (db_path.isEmpty()) {
            QMessageBox::critical(
                parent,
                get_text(loc, "error", "Error"),
                montar_mensagem_erro_estatisticas(
                    loc, get_text(loc, "database_connection_error", "Falha ao localizar banco de dados")));
            return;
        }

        if (!QFileInfo::exists(db_path)) {
            source::utils::LogManager::get_logger().warning(
                QStringLiteral("Banco de dados de monitoramento não encontrado em: %1").arg(db_path));
            QMessageBox::warning(
                parent,
                get_text(loc, "warning", "Warning"),
                get_text(loc, "no_data_to_plot", "Sem dados para plotar"));
            return;
        }

        // A estatística lê logo após eventos em fluxo; aguarda escrita assíncrona finalizar.
        try {
            auto writer = source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(db_path);
            if (writer) {
                writer->wait_until_idle(1500);
            }
        } catch (...) {
        }

        try {
            const QString conn_name = QStringLiteral("stats_check_%1_%2")
                                          .arg(reinterpret_cast<quintptr>(self))
                                          .arg(QDateTime::currentMSecsSinceEpoch());
            QString erro_validacao;
            bool sem_dados = false;
            {
                QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
                db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
                db.setDatabaseName(db_path);

                if (!db.open()) {
                    erro_validacao = db.lastError().text();
                } else {
                    QSqlQuery query(db);
                    bool tabela_existe = false;
                    if (query.exec(QStringLiteral(
                            "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'monitoramento' LIMIT 1")) &&
                        query.next()) {
                        tabela_existe = true;
                    } else if (!query.lastError().text().trimmed().isEmpty()) {
                        erro_validacao = query.lastError().text();
                    }

                    if (erro_validacao.isEmpty() && !tabela_existe) {
                        sem_dados = true;
                    } else if (erro_validacao.isEmpty()) {
                        if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM monitoramento")) || !query.next()) {
                            const QString sql_erro = query.lastError().text();
                            if (sql_erro.contains(QStringLiteral("no such table"), Qt::CaseInsensitive)) {
                                sem_dados = true;
                            } else {
                                erro_validacao = sql_erro;
                            }
                        } else if (query.value(0).toInt() == 0) {
                            sem_dados = true;
                        }
                    }
                }
                db.close();
            }
            QSqlDatabase::removeDatabase(conn_name);

            if (!erro_validacao.isEmpty()) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao verificar quantidade de dados: %1").arg(erro_validacao), true);
                QMessageBox::critical(
                    parent,
                    get_text(loc, "error", "Error"),
                    montar_mensagem_erro_estatisticas(loc, erro_validacao));
                return;
            }
            if (sem_dados) {
                source::utils::LogManager::get_logger().warning("Não há dados para gerar estatísticas");
                QMessageBox::warning(parent, get_text(loc, "warning", "Warning"), get_text(loc, "no_data_to_plot", "Sem dados para plotar"));
                return;
            }
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro ao verificar quantidade de dados", true);
            QMessageBox::critical(
                parent,
                get_text(loc, "error", "Error"),
                montar_mensagem_erro_estatisticas(loc, QStringLiteral("erro desconhecido")));
            return;
        }

        const QVariantList todos_graficos = criar_lista_graficos(self);
        const QVariantList graficos = filtrar_graficos_selecionados(interface_obj, todos_graficos);
        if (graficos.isEmpty()) {
            QMessageBox::warning(
                parent,
                get_text(loc, "warning", "Warning"),
                get_text(loc, "no_stats_charts_selected",
                         "Nenhum gráfico selecionado em Configurações > Estatísticas > Plotar Gráficos."));
            return;
        }

        dialog = new QDialog(nullptr);
        dialog->setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        dialog->setWindowModality(Qt::NonModal);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowIcon(QIcon(source::utils::get_icon_path("statistics.ico")));
        dialog->setWindowTitle(get_text(loc, "statistics", "Statistics"));
        dialog->setMinimumSize(1000, 700);
        dialog->setProperty("interface_principal", QVariant::fromValue(interface_obj));
        QObject::connect(dialog, &QDialog::finished, [self]() { limpar_referencia_dialog(self); });
        QScreen* tela_estatisticas = obter_tela_alvo(parent);

        auto* layout = new QHBoxLayout(dialog);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        auto* splitter = new QSplitter(Qt::Horizontal, dialog);
        interface_obj->setProperty("splitter", QVariant::fromValue(splitter));
        interface_obj->setProperty("dialog_estatisticas", QVariant::fromValue(dialog));
        interface_obj->setProperty("_estatisticas_db_path", db_path);

        liberar_gerador_anterior(interface_obj);
        auto* gerador = new source::data::GeradorEstatisticas(db_path, loc, interface_obj);
        interface_obj->setProperty("_gerador_cpp_ptr", QVariant::fromValue<qulonglong>(reinterpret_cast<qulonglong>(gerador)));

        auto* painel = criar_painel_selecao(self, graficos);
        auto* painel_graficos = criar_painel_graficos(self);

        splitter->addWidget(painel);
        splitter->addWidget(painel_graficos);
        splitter->setSizes({painel ? painel->width() : 260, 700});

        layout->addWidget(splitter);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        try {
            popular_checkboxes(self, graficos);
            posicionar_dialogo_na_tela(dialog, tela_estatisticas);
            dialog->show();
            if (dialog->windowHandle() != nullptr && tela_estatisticas != nullptr) {
                dialog->windowHandle()->setScreen(tela_estatisticas);
                posicionar_dialogo_na_tela(dialog, tela_estatisticas);
            }
            dialog->raise();
            dialog->activateWindow();
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            QApplication::restoreOverrideCursor();
            QPointer<QObject> interface_guard(interface_obj);
            QPointer<QDialog> dialog_guard(dialog);
            QTimer::singleShot(75, dialog, [self, graficos, interface_guard, dialog_guard]() {
                if (self == nullptr || interface_guard == nullptr || dialog_guard == nullptr) {
                    return;
                }
                QTimer::singleShot(0, dialog_guard, [self, graficos, interface_guard, dialog_guard]() {
                    if (self == nullptr || interface_guard == nullptr || dialog_guard == nullptr) {
                        return;
                    }
                    gerar_todos_graficos(self, graficos);
                });
            });
        } catch (...) {
            QApplication::restoreOverrideCursor();
            throw;
        }
    } catch (...) {
        QApplication::restoreOverrideCursor();
        source::utils::LogManager::get_logger().error("Erro ao mostrar estatísticas", true);
        if (self && self->interface_obj()) {
            QObject* loc = self->loc_obj();
            QWidget* parent = qobject_cast<QWidget*>(self->interface_obj());
            QMessageBox::critical(
                parent,
                get_text(loc, "error", "Error"),
                montar_mensagem_erro_estatisticas(loc, QStringLiteral("erro desconhecido")));
        }
    }
}

}
