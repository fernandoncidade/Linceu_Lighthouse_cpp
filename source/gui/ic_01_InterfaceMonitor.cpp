#include "ic_01_InterfaceMonitor.h"

#include <algorithm>

#include <QDir>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QHeaderView>
#include <QHideEvent>
#include <QLabel>
#include <QMenu>
#include <QMetaObject>
#include <QItemSelectionModel>
#include <QLayout>
#include <QPointer>
#include <QSplitter>
#include <QStackedLayout>
#include <QTableView>
#include <QTableWidget>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <QWindow>
#include <QShowEvent>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#ifdef interface
#undef interface
#endif
#endif

#include "ic_02_Inicializador.h"
#include "ic_03_Configurador.h"
#include "ic_04_Atualizador.h"
#include "ic_05_GerenciadorProgresso.h"
#include "ic_07_ManipuladorTabela.h"
#include "ic_08_Internacionalizador.h"
#include "source/data/Charts/ch_06_WidgetChartView.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/security/secui_01_SecurityUiController.h"
#include "source/services/GerenciamentoBaseEvento/gbank_20_db_writer.h"
#include "source/services/GerenciamentoTabela/gtab_47_EventTableModel.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/ui/ui_05_GerenciadorProgressoUI.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/ui/ui_07_GerenciadorDados.h"
#include "source/ui/ui_08_GerenciadorEventosArquivo.h"
#include "source/ui/ui_09_GerenciadorMonitoramento.h"
#include "source/ui/ui_10_GerenciadorLimpeza.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

constexpr const char* kTableLanguageViewsProperty = "_tabelas_idioma_views";
constexpr const char* kTableLanguageModelsProperty = "_tabelas_idioma_models";
constexpr const char* kTableLanguageStackProperty = "_tabelas_idioma_stack";
constexpr const char* kTableLanguageContainerProperty = "_tabelas_idioma_container";
constexpr const char* kTableLanguageActiveProperty = "_tabela_idioma_ativa";
constexpr const char* kTableLanguageEnabledProperty = "_tabelas_idioma_inicializadas";
constexpr const char* kAssociatedWindowFilterInstalledProperty = "_interface_sync_filter_installed";
constexpr const char* kAssociatedWindowSyncPendingProperty = "_interface_window_sync_pending";
constexpr const char* kAssociatedWindowOrderProperty = "_interface_window_order";
constexpr const char* kAssociatedWindowSyncingProperty = "_interface_window_syncing";
constexpr const char* kInterfaceChartWarmupProperty = "_interface_chart_gl_warmup_ready";
constexpr const char* kInterfaceChartWarmupObjectName = "_interface_chart_gl_warmup";

void preaquecer_superficie_grafica_interface(source::gui::InterfaceMonitor* self) {
    if (self == nullptr || self->property(kInterfaceChartWarmupProperty).toBool()) {
        return;
    }

    QWidget* host = self->centralWidget();
    if (host == nullptr) {
        host = self;
    }
    if (host == nullptr) {
        return;
    }

    auto* warmup =
        host->findChild<source::charts_lgpl::WidgetChartView*>(QString::fromLatin1(kInterfaceChartWarmupObjectName));
    if (warmup == nullptr) {
        warmup = new source::charts_lgpl::WidgetChartView(host);
        warmup->setObjectName(QString::fromLatin1(kInterfaceChartWarmupObjectName));
        warmup->setAnimationsEnabled(false);
        warmup->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        warmup->setFocusPolicy(Qt::NoFocus);
        warmup->setFixedSize(1, 1);
    }

    warmup->move(0, 0);
    warmup->lower();
    warmup->show();
    warmup->update();
    self->setProperty(kInterfaceChartWarmupProperty, true);
}

source::services::Observador* obter_observador_cpp(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }
    const qulonglong ptr = interface_obj->property("_observador_cpp_ptr").toULongLong();
    if (ptr == 0) {
        return nullptr;
    }
    return reinterpret_cast<source::services::Observador*>(ptr);
}

QString get_text(source::ui::LocalizadorQt* loc, const QString& key, const QString& fallback = QString()) {
    if (loc == nullptr) {
        return fallback;
    }
    const QString value = loc->get_text(key);
    return value.isEmpty() ? fallback : value;
}

QString format_with_one_arg(QString pattern, const QString& arg) {
    const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
    if (brace_pos >= 0) {
        pattern.replace(brace_pos, 2, arg);
        return pattern;
    }
    if (pattern.contains(QStringLiteral("%1"))) {
        return pattern.arg(arg);
    }
    if (pattern.isEmpty()) {
        return arg;
    }
    return pattern + QStringLiteral(" ") + arg;
}

void atualizar_tipos_operacao_interface(QObject* interface_obj,
                                        source::ui::LocalizadorQt* loc) {
    if (interface_obj == nullptr || loc == nullptr) {
        return;
    }

    QVariantMap tipos_operacao;
    tipos_operacao.insert(QStringLiteral("op_renamed"), loc->get_text(QStringLiteral("op_renamed")));
    tipos_operacao.insert(QStringLiteral("op_added"), loc->get_text(QStringLiteral("op_added")));
    tipos_operacao.insert(QStringLiteral("op_deleted"), loc->get_text(QStringLiteral("op_deleted")));
    tipos_operacao.insert(QStringLiteral("op_modified"), loc->get_text(QStringLiteral("op_modified")));
    tipos_operacao.insert(QStringLiteral("op_moved"), loc->get_text(QStringLiteral("op_moved")));
    tipos_operacao.insert(QStringLiteral("op_scanned"), loc->get_text(QStringLiteral("op_scanned")));
    interface_obj->setProperty("tipos_operacao", tipos_operacao);
}

void sincronizar_idioma_observador(source::services::Observador* observador,
                                   const QString& idioma) {
    if (observador == nullptr || observador->loc == nullptr || idioma.trimmed().isEmpty()) {
        return;
    }
    if (observador->loc->property("idioma_atual").toString() == idioma) {
        return;
    }
    observador->loc->set_idioma(idioma);
}

QStringList diretorios_monitorados_ativos(QObject* interface_obj) {
    QStringList diretorios;
    if (interface_obj == nullptr) {
        return diretorios;
    }

    if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        security_manager != nullptr) {
        diretorios = security_manager->monitored_directories();
    }

    if (diretorios.isEmpty()) {
        diretorios = interface_obj->property("security_multidiretorios").toStringList();
    }

    if (diretorios.isEmpty()) {
        const QVariantMap resumo = interface_obj->property("security_profile_summary").toMap();
        diretorios = resumo.value(QStringLiteral("monitor_diretorios_list")).toStringList();
    }

    const QString diretorio_atual = interface_obj->property("diretorio_atual").toString().trimmed();
    QStringList unicos;
    const auto adicionar_diretorio = [&unicos](const QString& diretorio) {
        const QString nativo = QDir::toNativeSeparators(diretorio.trimmed());
        if (!nativo.isEmpty() && !unicos.contains(nativo)) {
            unicos.append(nativo);
        }
    };

    if (!diretorios.isEmpty()) {
        for (const QString& diretorio : diretorios) {
            adicionar_diretorio(diretorio);
        }
    } else {
        adicionar_diretorio(diretorio_atual);
    }

    return unicos;
}

void fechar_dialogo_se_existir(QObject* interface_obj, const char* property_name) {
    if (interface_obj == nullptr || property_name == nullptr) {
        return;
    }
    QObject* dialog_obj = interface_obj->property(property_name).value<QObject*>();
    QWidget* dialog_widget = qobject_cast<QWidget*>(dialog_obj);
    if (dialog_widget != nullptr) {
        dialog_widget->close();
    }
}

void fechar_janelas_associadas_a_interface(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    const auto top_level_widgets = QApplication::topLevelWidgets();
    for (QWidget* widget : top_level_widgets) {
        if (widget == nullptr || widget == interface_obj) {
            continue;
        }

        QObject* interface_associada = widget->property("interface_principal").value<QObject*>();
        if (interface_associada == interface_obj) {
            widget->close();
        }
    }
}

QWidget* janela_principal_interface(QObject* interface_obj) {
    auto* widget = qobject_cast<QWidget*>(interface_obj);
    return widget != nullptr ? widget->window() : nullptr;
}

bool janela_transiente_excluida_da_hierarquia(QWidget* widget) {
    QWidget* topo = widget != nullptr ? widget->window() : nullptr;
    if (topo == nullptr) {
        return true;
    }

    if (topo->property("_ignore_interface_window_sync").toBool()) {
        return true;
    }

    if (qobject_cast<QMenu*>(topo) != nullptr) {
        return true;
    }

    const Qt::WindowFlags flags = topo->windowFlags();
    if ((flags & Qt::Popup) != 0 || (flags & Qt::ToolTip) != 0) {
        return true;
    }

    return false;
}

bool janela_associada_a_interface(QWidget* widget, QObject* interface_obj) {
    if (widget == nullptr || interface_obj == nullptr) {
        return false;
    }

    QWidget* principal = janela_principal_interface(interface_obj);
    QWidget* topo = widget->window();
    if (principal == nullptr || topo == nullptr || topo == principal) {
        return false;
    }

    if (janela_transiente_excluida_da_hierarquia(topo)) {
        return false;
    }

    if (topo->property("interface_principal").value<QObject*>() == interface_obj) {
        return true;
    }

    for (QObject* parent = topo->parent(); parent != nullptr; parent = parent->parent()) {
        if (parent == interface_obj) {
            return true;
        }
    }

    return false;
}

QList<QWidget*> janelas_associadas_interface(QObject* interface_obj) {
    QList<QWidget*> widgets;
    if (interface_obj == nullptr) {
        return widgets;
    }

    const auto top_level_widgets = QApplication::topLevelWidgets();
    for (QWidget* widget : top_level_widgets) {
        if (janela_associada_a_interface(widget, interface_obj)) {
            widgets.append(widget->window());
        }
    }

    return widgets;
}

qulonglong id_janela(QWidget* widget) {
    QWidget* topo = widget != nullptr ? widget->window() : nullptr;
    return reinterpret_cast<qulonglong>(topo);
}

QList<QWidget*> coletar_janelas_hierarquia(source::gui::InterfaceMonitor* self) {
    QList<QWidget*> widgets;
    if (self == nullptr) {
        return widgets;
    }

    if (QWidget* principal = janela_principal_interface(self); principal != nullptr) {
        widgets.append(principal->window());
    }

    for (QWidget* widget : janelas_associadas_interface(self)) {
        QWidget* topo = widget != nullptr ? widget->window() : nullptr;
        if (topo != nullptr && !widgets.contains(topo)) {
            widgets.append(topo);
        }
    }

    return widgets;
}

QWidget* janela_hierarquia_ativa(source::gui::InterfaceMonitor* self) {
    QWidget* ativa = QApplication::activeWindow();
    if (ativa == nullptr || self == nullptr) {
        return nullptr;
    }

    QWidget* topo = ativa->window();
    QWidget* principal = janela_principal_interface(self);
    if (topo == principal || janela_associada_a_interface(topo, self)) {
        return topo;
    }

    return nullptr;
}

void promover_janela_na_ordem(source::gui::InterfaceMonitor* self, QWidget* janela) {
    if (self == nullptr || janela == nullptr) {
        return;
    }

    const qulonglong id = id_janela(janela);
    if (id == 0) {
        return;
    }

    QVariantList ordem = self->property(kAssociatedWindowOrderProperty).toList();
    for (int i = ordem.size() - 1; i >= 0; --i) {
        if (ordem.at(i).toULongLong() == id) {
            ordem.removeAt(i);
        }
    }
    ordem.prepend(QVariant::fromValue<qulonglong>(id));
    self->setProperty(kAssociatedWindowOrderProperty, ordem);
}

QList<QWidget*> ordenar_janelas_para_hierarquia(source::gui::InterfaceMonitor* self) {
    QList<QWidget*> coletadas = coletar_janelas_hierarquia(self);
    if (self == nullptr || coletadas.isEmpty()) {
        return coletadas;
    }

    QVariantList ordem_ids = self->property(kAssociatedWindowOrderProperty).toList();
    QVariantList ordem_atualizada;
    QList<QWidget*> ordenadas;

    for (const QVariant& valor : ordem_ids) {
        const qulonglong id = valor.toULongLong();
        auto it = std::find_if(coletadas.begin(), coletadas.end(), [id](QWidget* widget) {
            return id_janela(widget) == id;
        });
        if (it != coletadas.end()) {
            ordem_atualizada.append(QVariant::fromValue<qulonglong>(id));
            ordenadas.append(*it);
            coletadas.erase(it);
        }
    }

    for (QWidget* widget : coletadas) {
        const qulonglong id = id_janela(widget);
        if (id == 0) {
            continue;
        }
        ordem_atualizada.append(QVariant::fromValue<qulonglong>(id));
        ordenadas.append(widget);
    }

    if (QWidget* ativa = janela_hierarquia_ativa(self); ativa != nullptr) {
        promover_janela_na_ordem(self, ativa);
        ordem_ids = self->property(kAssociatedWindowOrderProperty).toList();
        QList<QWidget*> reordenadas;
        for (const QVariant& valor : ordem_ids) {
            const qulonglong id = valor.toULongLong();
            auto it = std::find_if(ordenadas.begin(), ordenadas.end(), [id](QWidget* widget) {
                return id_janela(widget) == id;
            });
            if (it != ordenadas.end()) {
                reordenadas.append(*it);
                ordenadas.erase(it);
            }
        }
        for (QWidget* widget : ordenadas) {
            reordenadas.append(widget);
        }
        ordenadas = reordenadas;
    }

    QVariantList ordem_final;
    for (QWidget* widget : ordenadas) {
        const qulonglong id = id_janela(widget);
        if (id != 0 && !ordem_final.contains(QVariant::fromValue<qulonglong>(id))) {
            ordem_final.append(QVariant::fromValue<qulonglong>(id));
        }
    }
    self->setProperty(kAssociatedWindowOrderProperty, ordem_final);

    return ordenadas;
}

void trazer_para_frente(QWidget* widget) {
    if (widget == nullptr || !widget->isVisible()) {
        return;
    }
    widget->raise();
}

void colocar_atras_de(QWidget* widget, QWidget* referencia) {
    if (widget == nullptr || referencia == nullptr || widget == referencia || !widget->isVisible()) {
        return;
    }

#ifdef Q_OS_WIN
    widget->createWinId();
    referencia->createWinId();
    ::SetWindowPos(reinterpret_cast<HWND>(widget->winId()),
                   reinterpret_cast<HWND>(referencia->winId()),
                   0,
                   0,
                   0,
                   0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#else
    widget->lower();
#endif
}

void sincronizar_janelas_associadas(source::gui::InterfaceMonitor* self) {
    if (self == nullptr || self->property(kAssociatedWindowSyncingProperty).toBool()) {
        return;
    }

    if (janela_hierarquia_ativa(self) == nullptr) {
        return;
    }

    const QList<QWidget*> janelas = ordenar_janelas_para_hierarquia(self);
    if (janelas.isEmpty()) {
        return;
    }

    self->setProperty(kAssociatedWindowSyncingProperty, true);
    try {
        QWidget* referencia = nullptr;
        for (QWidget* widget : janelas) {
            if (widget == nullptr || !widget->isVisible() || widget->isMinimized()) {
                continue;
            }
            if (!widget->property(kAssociatedWindowFilterInstalledProperty).toBool()) {
                widget->installEventFilter(self);
                widget->setProperty(kAssociatedWindowFilterInstalledProperty, true);
            }
            if (referencia == nullptr) {
                trazer_para_frente(widget);
            } else {
                colocar_atras_de(widget, referencia);
            }
            referencia = widget;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao sincronizar hierarquia de janelas associadas", true);
    }
    self->setProperty(kAssociatedWindowSyncingProperty, false);
}

void agendar_sincronizacao_janelas_associadas(source::gui::InterfaceMonitor* self) {
    if (self == nullptr || self->property(kAssociatedWindowSyncPendingProperty).toBool()) {
        return;
    }

    self->setProperty(kAssociatedWindowSyncPendingProperty, true);
    QPointer<source::gui::InterfaceMonitor> guard(self);
    QTimer::singleShot(0, self, [guard]() {
        if (guard == nullptr) {
            return;
        }
        guard->setProperty(kAssociatedWindowSyncPendingProperty, false);
        sincronizar_janelas_associadas(guard);
    });
}

QStringList idiomas_tabela_suportados() {
    return {
        QStringLiteral("pt_BR"),
        QStringLiteral("en_US"),
        QStringLiteral("es_ES"),
        QStringLiteral("fr_FR"),
        QStringLiteral("it_IT"),
        QStringLiteral("de_DE")
    };
}

QVariantMap obter_mapa_objetos(QObject* interface_obj, const char* property_name) {
    return interface_obj != nullptr ? interface_obj->property(property_name).toMap() : QVariantMap();
}

QTableView* obter_tabela_idioma(QObject* interface_obj, const QString& idioma) {
    const QVariantMap mapa = obter_mapa_objetos(interface_obj, kTableLanguageViewsProperty);
    return qobject_cast<QTableView*>(mapa.value(idioma).value<QObject*>());
}

source::services::GerenciamentoTabela::EventTableModel* obter_modelo_idioma(QObject* interface_obj,
                                                                            const QString& idioma) {
    const QVariantMap mapa = obter_mapa_objetos(interface_obj, kTableLanguageModelsProperty);
    return qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(
        mapa.value(idioma).value<QObject*>());
}

QString idioma_tabela_ativo(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return QStringLiteral("pt_BR");
    }

    QString idioma = interface_obj->property(kTableLanguageActiveProperty).toString();
    if (!idioma.isEmpty()) {
        return idioma;
    }

    QObject* loc = interface_obj->property("loc").value<QObject*>();
    idioma = loc != nullptr ? loc->property("idioma_atual").toString() : QString();
    if (!idioma.isEmpty()) {
        return idioma;
    }

    return QStringLiteral("pt_BR");
}

QString resolver_idioma_tabela(QObject* interface_obj, const QString& idioma_solicitado) {
    QString idioma = idioma_solicitado.trimmed();
    if (idioma.isEmpty()) {
        idioma = idioma_tabela_ativo(interface_obj);
    }
    if (!obter_tabela_idioma(interface_obj, idioma)) {
        for (const QString& candidato : idiomas_tabela_suportados()) {
            if (obter_tabela_idioma(interface_obj, candidato) != nullptr) {
                return candidato;
            }
        }
    }
    return idioma;
}

void configurar_selecao_colunas_view(QTableView* tabela_view) {
    if (tabela_view == nullptr) {
        return;
    }

    tabela_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    tabela_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tabela_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela_view->setAlternatingRowColors(true);
    tabela_view->setSortingEnabled(false);

    if (auto* vertical = tabela_view->verticalHeader()) {
        vertical->setVisible(true);
        vertical->setDefaultAlignment(Qt::AlignCenter);
        vertical->setSectionResizeMode(QHeaderView::Fixed);
        vertical->setDefaultSectionSize(tabela_view->verticalHeader()->defaultSectionSize());
    }

    auto* header = tabela_view->horizontalHeader();
    if (header == nullptr) {
        return;
    }

    header->setSectionsMovable(true);
    header->setHighlightSections(true);
    header->setDefaultAlignment(Qt::AlignCenter);
    header->setTextElideMode(Qt::ElideNone);
    header->setStretchLastSection(false);
    header->setSectionResizeMode(QHeaderView::Interactive);

    QFont fonte = header->font();
    fonte.setBold(true);
    header->setFont(fonte);

    if (header->property("_header_multicol_select_connected").toBool()) {
        return;
    }

    QObject* interface_obj = tabela_view->property("interface_monitor").value<QObject*>();

    QObject::connect(header, &QHeaderView::sectionClicked, tabela_view,
                     [tabela_view, interface_obj](int logical_index) {
                         if (tabela_view == nullptr || tabela_view->model() == nullptr ||
                             logical_index < 0 || logical_index >= tabela_view->model()->columnCount()) {
                             return;
                         }

                         const Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
                         const bool ctrl = mods.testFlag(Qt::ControlModifier);
                         const bool shift = mods.testFlag(Qt::ShiftModifier);
                         const bool ordenacao_habilitada =
                             interface_obj != nullptr &&
                             interface_obj->property("_ordenacao_linhas_habilitada").toBool();

                         auto* horizontal = tabela_view->horizontalHeader();
                         if (ordenacao_habilitada && !ctrl && !shift && horizontal != nullptr) {
                             Qt::SortOrder nova_ordem = Qt::AscendingOrder;
                             const int coluna_atual =
                                 interface_obj != nullptr
                                     ? interface_obj->property("_ordenacao_linhas_coluna").toInt()
                                     : -1;
                             const Qt::SortOrder ordem_atual =
                                 interface_obj != nullptr
                                     ? static_cast<Qt::SortOrder>(
                                           interface_obj->property("_ordenacao_linhas_ordem").toInt())
                                     : Qt::AscendingOrder;

                             if (coluna_atual == logical_index) {
                                 nova_ordem = (ordem_atual == Qt::AscendingOrder)
                                                  ? Qt::DescendingOrder
                                                  : Qt::AscendingOrder;
                             }

                             horizontal->setSortIndicatorShown(true);
                             horizontal->setSortIndicator(logical_index, nova_ordem);
                             tabela_view->model()->sort(logical_index, nova_ordem);
                             return;
                         }

                         auto* sm = tabela_view->selectionModel();
                         if (sm == nullptr) {
                             return;
                         }

                         const int rows = tabela_view->model()->rowCount();
                         if (rows <= 0) {
                             return;
                         }

                         const QModelIndex top_left = tabela_view->model()->index(0, logical_index);
                         const QModelIndex bottom_right =
                             tabela_view->model()->index(rows - 1, logical_index);
                         const QItemSelection col_selection(top_left, bottom_right);

                         const int anchor_column =
                             horizontal != nullptr
                                 ? horizontal->property("_header_anchor_column").toInt()
                                 : -1;

                         if (shift) {
                             const int effective_anchor =
                                 (anchor_column >= 0 &&
                                  anchor_column < tabela_view->model()->columnCount())
                                     ? anchor_column
                                     : logical_index;
                             const int start_col = std::min(effective_anchor, logical_index);
                             const int end_col = std::max(effective_anchor, logical_index);

                             const QModelIndex range_top_left =
                                 tabela_view->model()->index(0, start_col);
                             const QModelIndex range_bottom_right =
                                 tabela_view->model()->index(rows - 1, end_col);
                             const QItemSelection range_selection(range_top_left, range_bottom_right);

                             const QItemSelectionModel::SelectionFlags flags =
                                 (ctrl ? QItemSelectionModel::Select
                                       : QItemSelectionModel::ClearAndSelect) |
                                 QItemSelectionModel::Columns;
                             sm->select(range_selection, flags);
                         } else if (ctrl) {
                             if (sm->isColumnSelected(logical_index, QModelIndex())) {
                                 sm->select(col_selection, QItemSelectionModel::Deselect |
                                                               QItemSelectionModel::Columns);
                             } else {
                                 sm->select(col_selection, QItemSelectionModel::Select |
                                                               QItemSelectionModel::Columns);
                             }
                         } else {
                             sm->select(col_selection, QItemSelectionModel::ClearAndSelect |
                                                           QItemSelectionModel::Columns);
                         }

                         auto* header_atual = tabela_view->horizontalHeader();
                         if (header_atual == nullptr) {
                             return;
                         }

                         if (!shift) {
                             header_atual->setProperty("_header_anchor_column", logical_index);
                         } else if (anchor_column < 0) {
                             header_atual->setProperty("_header_anchor_column", logical_index);
                         }
                     });
    header->setProperty("_header_multicol_select_connected", true);
}

void aplicar_estado_ordenacao_view(QTableView* tabela_view, QObject* interface_obj) {
    if (tabela_view == nullptr || tabela_view->model() == nullptr) {
        return;
    }

    auto* header = tabela_view->horizontalHeader();
    if (header == nullptr) {
        return;
    }

    const bool ordenacao_habilitada =
        interface_obj != nullptr && interface_obj->property("_ordenacao_linhas_habilitada").toBool();
    const int coluna_ordenada =
        interface_obj != nullptr ? interface_obj->property("_ordenacao_linhas_coluna").toInt() : -1;
    const Qt::SortOrder ordem =
        interface_obj != nullptr
            ? static_cast<Qt::SortOrder>(interface_obj->property("_ordenacao_linhas_ordem").toInt())
            : Qt::AscendingOrder;

    if (!ordenacao_habilitada) {
        header->setSortIndicatorShown(false);
        return;
    }

    if (coluna_ordenada >= 0 && coluna_ordenada < tabela_view->model()->columnCount()) {
        header->setSortIndicatorShown(true);
        header->setSortIndicator(coluna_ordenada, ordem);
        tabela_view->model()->sort(coluna_ordenada, ordem);
        return;
    }

    header->setSortIndicatorShown(false);
}

void configurar_view_modelo(QTableView* tabela_view,
                            source::services::GerenciamentoTabela::EventTableModel* model,
                            source::services::GerenciadorColunas* gc) {
    if (tabela_view == nullptr || model == nullptr) {
        return;
    }

    model->bind_view(tabela_view);
    configurar_selecao_colunas_view(tabela_view);

    tabela_view->setStyleSheet(QStringLiteral(
        "QTableView::item:hover { background-color: palette(alternate-base); }"
        "QTableView { selection-background-color: transparent; selection-color: palette(text); }"
        "QTableView::item:selected { background-color: transparent; color: palette(text); border: 1px solid palette(highlight);"
        "border-radius: 3px; padding: 1px; }"
        "QTableCornerButton::section { background-color: palette(button); border: 1px solid palette(mid); }"
        "QTableCornerButton::section:pressed { background-color: palette(highlight); }"));
    tabela_view->setPalette(QApplication::palette());

    auto* header = tabela_view->horizontalHeader();
    if (header == nullptr) {
        return;
    }

    const QFontMetrics fm(header->font());
    constexpr int kPadding = 40;
    constexpr int kMinWidth = 10;

    for (int i = 0; i < model->columnCount(); ++i) {
        const QString header_text =
            model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().replace('\n', ' ');
        const int largura = std::max(kMinWidth, fm.horizontalAdvance(header_text) + kPadding);
        tabela_view->setColumnWidth(i, largura);
        header->setSectionResizeMode(i, QHeaderView::Interactive);

        if (gc != nullptr) {
            const QString key = model->column_key(i);
            const QVariantMap coluna = gc->COLUNAS_DISPONIVEIS.value(key);
            tabela_view->setColumnHidden(i, !coluna.value("visivel", true).toBool());
        }
    }

    aplicar_estado_ordenacao_view(tabela_view,
                                  tabela_view->property("interface_monitor").value<QObject*>());
}

void reaplicar_filtros_tabela_ativa(source::gui::InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr || interface_monitor->painel_filtros == nullptr) {
        return;
    }

    interface_monitor->painel_filtros->set_tabela_dados(
        qobject_cast<QWidget*>(interface_monitor->tabela_dados));

    auto* administrador = interface_monitor->painel_filtros->administrador_filtros_obj();
    if (administrador != nullptr) {
        QTimer::singleShot(0, interface_monitor->painel_filtros, [administrador]() {
            administrador->aplicar_filtros();
        });
    }
}

}  // namespace

InterfaceMonitor::InterfaceMonitor(QWidget* parent)
    : QMainWindow(parent),
      loc(nullptr),
      gerenciador_tabela(nullptr),
      gerenciador_colunas(nullptr),
      gerenciador_eventos_arquivo(nullptr),
      gerenciador_dados(nullptr),
      gerenciador_monitoramento(nullptr),
      gerenciador_limpeza(nullptr),
      gerenciador_botoes(nullptr),
      gerenciador_botoes_ui(nullptr),
      gerenciador_menus_ui(nullptr),
      gerenciador_eventos_ui(nullptr),
      gerenciador_progresso_ui(nullptr),
      gerenciador_estatisticas_ui(nullptr),
      painel_filtros(nullptr),
      evento_base(nullptr),
      observador(nullptr),
      tabela_dados(nullptr),
      event_table_model(nullptr),
      security_manager(nullptr) {
    try {
        auto* app = qobject_cast<QApplication*>(QApplication::instance());
        Internacionalizador::inicializar_sistema_traducao(app);

        loc = new source::ui::LocalizadorQt(this);
        QObject::connect(loc, &source::ui::LocalizadorQt::idioma_alterado, this, [this](const QString&) {
            atualizar_interface();
        });
        QObject::connect(loc, &source::ui::LocalizadorQt::traducoes_carregadas, this,
                         &InterfaceMonitor::atualizar_tradutor_qt);
        setProperty("loc", QVariant::fromValue(static_cast<QObject*>(loc)));

        Inicializador::inicializar_atributos(this);
        security_manager = new source::security::SecurityManager(this);
        setProperty("security_manager", QVariant::fromValue(static_cast<QObject*>(security_manager)));
        auto* security_ui_controller = new source::security::ui::SecurityUiController(this);
        setProperty("security_ui_controller", QVariant::fromValue(static_cast<QObject*>(security_ui_controller)));
        Inicializador::inicializar_componentes(this);

                gerenciador_tabela = new source::services::GerenciadorTabela(this);
                gerenciador_tabela->_carregar_configuracoes_cores();
                QObject::connect(gerenciador_tabela, &source::services::GerenciadorTabela::cores_processadas, this,
                                                 &InterfaceMonitor::atualizar_cores_tabela);
                setProperty("gerenciador_tabela", QVariant::fromValue(static_cast<QObject*>(gerenciador_tabela)));

        Inicializador::inicializar_gerenciadores(this);

        configurar_tabela();
        setup_ui();
                ativar_model_view_streaming();
                setup_menu_bar();

        if (painel_filtros != nullptr) {
            painel_filtros->sincronizar_com_menu_principal();
        }

        if (gerenciador_eventos_arquivo == nullptr) {
            gerenciador_eventos_arquivo = new source::ui::GerenciadorEventosArquivo(this);
        }
        if (gerenciador_dados == nullptr) {
            gerenciador_dados = new source::ui::GerenciadorDados(this);
        }
        if (gerenciador_monitoramento == nullptr) {
            gerenciador_monitoramento = new source::ui::GerenciadorMonitoramento(this);
        }
        if (gerenciador_limpeza == nullptr) {
            gerenciador_limpeza = new source::ui::GerenciadorLimpeza(this);
        }
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().critical(
            QString("Erro fatal na inicialização da InterfaceMonitor (std::exception): %1").arg(e.what()), true);
        QApplication::quit();
    } catch (...) {
        source::utils::LogManager::get_logger().critical("Erro fatal na inicialização da InterfaceMonitor", true);
        QApplication::quit();
    }
}

InterfaceMonitor::~InterfaceMonitor() {
    if (painel_filtros != nullptr) {
        painel_filtros->close();
        delete painel_filtros;
        painel_filtros = nullptr;
    }

    delete gerenciador_eventos_arquivo;
    gerenciador_eventos_arquivo = nullptr;

    delete gerenciador_dados;
    gerenciador_dados = nullptr;

    delete gerenciador_monitoramento;
    gerenciador_monitoramento = nullptr;

    delete gerenciador_limpeza;
    gerenciador_limpeza = nullptr;

    delete gerenciador_botoes;
    gerenciador_botoes = nullptr;

    delete gerenciador_botoes_ui;
    gerenciador_botoes_ui = nullptr;

    delete gerenciador_menus_ui;
    gerenciador_menus_ui = nullptr;

    delete gerenciador_eventos_ui;
    gerenciador_eventos_ui = nullptr;

    delete gerenciador_progresso_ui;
    gerenciador_progresso_ui = nullptr;

    delete gerenciador_estatisticas_ui;
    gerenciador_estatisticas_ui = nullptr;

    security_manager = nullptr;
}

void InterfaceMonitor::setup_ui() { Configurador::setup_ui(this); }

void InterfaceMonitor::setup_menu_bar() { Configurador::setup_menu_bar(this); }

void InterfaceMonitor::atualizar_interface() {
    if (property("_atualizando_interface").toBool()) {
        return;
    }
    setProperty("_atualizando_interface", true);
    try {
        Atualizador::atualizar_interface(this);
        atualizar_status();

        if (gerenciador_botoes_ui != nullptr) {
            gerenciador_botoes_ui->update_buttons_text(loc);
        }

        if (painel_filtros != nullptr) {
            painel_filtros->atualizar_interface();
        }

        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->retraduzir_dados_existentes();
        }

        const QString idioma_atual = (loc != nullptr) ? loc->property("idioma_atual").toString() : QString();
        atualizar_tradutor_qt(idioma_atual);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em InterfaceMonitor::atualizar_interface", true);
    }
    setProperty("_atualizando_interface", false);
}

void InterfaceMonitor::abrir_janela_filtros() { Atualizador::abrir_janela_filtros(this); }

void InterfaceMonitor::atualizar_status() { Atualizador::atualizar_status(this); }

void InterfaceMonitor::configurar_tabela() { ManipuladorTabela::configurar_tabela(this); }

void InterfaceMonitor::ativar_model_view_streaming() {
    try {
        if (tabela_dados == nullptr) {
            return;
        }

        if (property(kTableLanguageEnabledProperty).toBool()) {
            alternar_tabela_idioma();
            return;
        }

        QWidget* widget_atual = qobject_cast<QWidget*>(tabela_dados);
        if (widget_atual == nullptr) {
            return;
        }

        QVariantList snapshot_existente;
        if (auto* tabela_view_existente = qobject_cast<QTableView*>(widget_atual)) {
            if (auto* modelo_existente =
                    qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(
                        tabela_view_existente->model())) {
                snapshot_existente = modelo_existente->snapshot_events();
            }
        }

        QWidget* parent_widget = widget_atual->parentWidget();
        QLayout* parent_layout = (parent_widget != nullptr) ? parent_widget->layout() : nullptr;
        auto* splitter_parent = qobject_cast<QSplitter*>(parent_widget);

        auto* container_tabelas = new QWidget(parent_widget);
        container_tabelas->setObjectName(QStringLiteral("container_tabelas_idioma"));
        auto* pilha_tabelas = new QStackedLayout(container_tabelas);
        pilha_tabelas->setContentsMargins(0, 0, 0, 0);
        pilha_tabelas->setSpacing(0);

        QVariantMap views_por_idioma;
        QVariantMap modelos_por_idioma;

        for (const QString& idioma : idiomas_tabela_suportados()) {
            auto* modelo = new source::services::GerenciamentoTabela::EventTableModel(this, idioma, this);
            auto* view = new QTableView(container_tabelas);
            view->setObjectName(QStringLiteral("tabela_dados_%1").arg(idioma));
            view->setProperty("idioma_tabela", idioma);
            view->setProperty("interface_monitor", QVariant::fromValue(static_cast<QObject*>(this)));
            view->setModel(modelo);

            configurar_view_modelo(view, modelo, gerenciador_colunas);
            if (!snapshot_existente.isEmpty()) {
                modelo->replace_events(snapshot_existente);
            }

            pilha_tabelas->addWidget(view);
            views_por_idioma.insert(idioma, QVariant::fromValue(static_cast<QObject*>(view)));
            modelos_por_idioma.insert(idioma, QVariant::fromValue(static_cast<QObject*>(modelo)));
        }

        if (parent_layout != nullptr) {
            if (QLayoutItem* item_antigo = parent_layout->replaceWidget(widget_atual, container_tabelas)) {
                delete item_antigo;
            } else {
                parent_layout->addWidget(container_tabelas);
            }
        } else if (splitter_parent != nullptr) {
            splitter_parent->addWidget(container_tabelas);
        } else {
            container_tabelas->setParent(parent_widget);
            container_tabelas->show();
        }

        widget_atual->hide();

        setProperty(kTableLanguageViewsProperty, views_por_idioma);
        setProperty(kTableLanguageModelsProperty, modelos_por_idioma);
        setProperty(kTableLanguageStackProperty,
                    QVariant::fromValue(static_cast<QObject*>(pilha_tabelas)));
        setProperty(kTableLanguageContainerProperty,
                    QVariant::fromValue(static_cast<QObject*>(container_tabelas)));
        setProperty(kTableLanguageEnabledProperty, true);

        alternar_tabela_idioma();
    } catch (...) {
        source::utils::LogManager::get_logger().warning("Falha ao ativar Model/View");
    }
}

void InterfaceMonitor::inserir_evento_streaming(const QVariantMap& evento) {
    try {
        if (evento.isEmpty()) {
            return;
        }
        inserir_eventos_streaming(QVariantList{QVariant(evento)});
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inserir evento em streaming", true);
    }
}

void InterfaceMonitor::inserir_eventos_streaming(const QVariantList& eventos) {
    try {
        if (eventos.isEmpty()) {
            return;
        }

        int inseridos = 0;
        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_dados);
            tabela_view != nullptr && property(kTableLanguageEnabledProperty).toBool()) {
            adicionar_eventos_tabelas_idioma(eventos);
            inseridos = eventos.size();
        } else if (gerenciador_tabela != nullptr) {
            for (const QVariant& evento_var : eventos) {
                const QVariantMap evento = evento_var.toMap();
                if (evento.isEmpty()) {
                    continue;
                }
                gerenciador_tabela->atualizar_linha_mais_recente(tabela_dados, evento);
                ++inseridos;
            }
        }

        if (inseridos <= 0) {
            return;
        }

        const int eventos_desde_ultimo_status = property("contador_eventos").toInt() + inseridos;
        setProperty("contador_eventos", eventos_desde_ultimo_status);
        const qint64 agora_ms = QDateTime::currentMSecsSinceEpoch();
        const qint64 ultimo_status_ms = property("ultimo_update_status").toLongLong();
        const bool deve_atualizar_status =
            (eventos_desde_ultimo_status >= 64) ||
            ((agora_ms - ultimo_status_ms) >= 120);
        if (deve_atualizar_status) {
            setProperty("ultimo_update_status", QVariant::fromValue(agora_ms));
            setProperty("contador_eventos", 0);
            atualizar_status();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inserir eventos em streaming", true);
    }
}

void InterfaceMonitor::verificar_movimentacao(const QVariantMap& evento) {
    if (gerenciador_eventos_arquivo != nullptr) {
        gerenciador_eventos_arquivo->verificar_movimentacao(evento);
    }
}

void InterfaceMonitor::adicionar_evento(const QVariantMap& evento) {
    if (gerenciador_eventos_arquivo != nullptr) {
        gerenciador_eventos_arquivo->adicionar_evento(evento);
    }
}

void InterfaceMonitor::selecionar_diretorio() {
    if (gerenciador_botoes != nullptr) {
        gerenciador_botoes->selecionar_diretorio();
    }
}

void InterfaceMonitor::alternar_analise_diretorio() {
    if (gerenciador_botoes != nullptr) {
        gerenciador_botoes->alternar_analise_diretorio();
    }
}

void InterfaceMonitor::pausar_monitoramento_ou_escaneamento() {
    if (gerenciador_botoes != nullptr) {
        gerenciador_botoes->pausar_monitoramento_ou_escaneamento();
    }
}

void InterfaceMonitor::salvar_dados() {
    if (gerenciador_dados != nullptr) {
        gerenciador_dados->salvar_dados();
    }
}

void InterfaceMonitor::abrir_salvar_como() {
    if (gerenciador_dados != nullptr) {
        gerenciador_dados->abrir_salvar_como();
    }
}

void InterfaceMonitor::mostrar_estatisticas() {
    if (gerenciador_estatisticas_ui != nullptr) {
        gerenciador_estatisticas_ui->mostrar_estatisticas();
    }
}

void InterfaceMonitor::limpar_dados() {
    if (gerenciador_limpeza != nullptr) {
        gerenciador_limpeza->limpar_dados();
    }
}

void InterfaceMonitor::sair_aplicacao() {
    setProperty("_permitir_fechamento_programatico", true);
    close();
}

void InterfaceMonitor::alternar_filtro() {
    if (gerenciador_eventos_ui == nullptr) {
        return;
    }
    if (auto* acao = qobject_cast<QAction*>(sender())) {
        setProperty("sender_action", QVariant::fromValue(acao));
    }
    gerenciador_eventos_ui->alternar_filtro(QString());
    setProperty("sender_action", QVariant());
}

void InterfaceMonitor::alternar_visibilidade_coluna() {
    if (gerenciador_eventos_ui == nullptr) {
        return;
    }
    if (auto* acao = qobject_cast<QAction*>(sender())) {
        setProperty("sender_action", QVariant::fromValue(acao));
    }
    gerenciador_eventos_ui->alternar_visibilidade_coluna(QString(), true);
    setProperty("sender_action", QVariant());
}

void InterfaceMonitor::resetar_colunas() {
    if (gerenciador_eventos_ui != nullptr) {
        gerenciador_eventos_ui->resetar_colunas();
    }
}

void InterfaceMonitor::resetar_cores() {
    if (gerenciador_eventos_ui != nullptr) {
        gerenciador_eventos_ui->resetar_cores();
    }
}

void InterfaceMonitor::redefinir_todas_colunas_cores() {
    if (gerenciador_eventos_ui != nullptr) {
        gerenciador_eventos_ui->redefinir_todas_colunas_cores();
    }
}

void InterfaceMonitor::criar_menu_principal() {
    if (gerenciador_menus_ui != nullptr) {
        gerenciador_menus_ui->criar_menu_principal();
    }
}

void InterfaceMonitor::alterar_idioma() {
    if (gerenciador_eventos_ui == nullptr) {
        return;
    }
    if (auto* acao = qobject_cast<QAction*>(sender())) {
        setProperty("sender_action", QVariant::fromValue(acao));
    }
    gerenciador_eventos_ui->alterar_idioma(QString());
    setProperty("sender_action", QVariant());
}

void InterfaceMonitor::atualizar_visibilidade_colunas() {
    ManipuladorTabela::atualizar_visibilidade_colunas(this);
}

void InterfaceMonitor::reiniciar_sistema_monitoramento() {
    if (gerenciador_monitoramento != nullptr) {
        gerenciador_monitoramento->reiniciar_sistema_monitoramento();
    }
}

void InterfaceMonitor::atualizar_tradutor_qt(const QString& idioma) {
    atualizar_tipos_operacao_interface(this, loc);

    if (security_manager != nullptr) {
        security_manager->sync_runtime_language(idioma);
    }

    sincronizar_idioma_observador(observador, idioma);

    const qulonglong obs_ptr = property("_observador_cpp_ptr").toULongLong();
    if (obs_ptr != 0) {
        sincronizar_idioma_observador(reinterpret_cast<source::services::Observador*>(obs_ptr), idioma);
    }

    QObject* observador_qt = property("observador").value<QObject*>();
    if (observador_qt != nullptr) {
        QObject* obs_loc_obj = observador_qt->property("loc").value<QObject*>();
        if (auto* obs_loc = qobject_cast<source::ui::LocalizadorQt*>(obs_loc_obj)) {
            if (obs_loc->property("idioma_atual").toString() != idioma) {
                obs_loc->set_idioma(idioma);
            }
        }
    }
}

void InterfaceMonitor::alternar_tabela_idioma(const QString& idioma) {
    try {
        const QString idioma_alvo = resolver_idioma_tabela(this, idioma);
        auto* tabela_view = obter_tabela_idioma(this, idioma_alvo);
        auto* modelo = obter_modelo_idioma(this, idioma_alvo);
        auto* pilha = qobject_cast<QStackedLayout*>(
            property(kTableLanguageStackProperty).value<QObject*>());

        if (tabela_view == nullptr || modelo == nullptr) {
            return;
        }

        if (pilha != nullptr) {
            pilha->setCurrentWidget(tabela_view);
        }

        tabela_dados = tabela_view;
        event_table_model = modelo;
        setProperty("tabela_dados", QVariant::fromValue(static_cast<QObject*>(tabela_view)));
        setProperty(kTableLanguageActiveProperty, idioma_alvo);

        reaplicar_filtros_tabela_ativa(this);

        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->atualizar_visibilidade_colunas(true);
            gerenciador_tabela->atualizar_estilo_tema(gerenciador_tabela->detectar_tema_windows());
            gerenciador_tabela->atualizar_cores_colunas(true);
            gerenciador_tabela->atualizacao_pendente = true;
        }

        aplicar_estado_ordenacao_view(tabela_view, this);

        if (auto* header = tabela_view->horizontalHeader()) {
            header->viewport()->update();
        }
        if (tabela_view->viewport() != nullptr) {
            tabela_view->viewport()->update();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar tabela por idioma", true);
    }
}

void InterfaceMonitor::substituir_eventos_tabelas_idioma(const QVariantList& eventos) {
    try {
        const QVariantMap modelos = property(kTableLanguageModelsProperty).toMap();
        if (modelos.isEmpty()) {
            if (event_table_model != nullptr) {
                event_table_model->replace_events(eventos);
            }
            return;
        }

        for (auto it = modelos.constBegin(); it != modelos.constEnd(); ++it) {
            if (auto* modelo = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(
                    it.value().value<QObject*>())) {
                modelo->replace_events(eventos);
            }
        }

        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_dados);
            tabela_view != nullptr && tabela_view->viewport() != nullptr) {
            tabela_view->viewport()->update();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao substituir eventos das tabelas por idioma", true);
    }
}

void InterfaceMonitor::adicionar_eventos_tabelas_idioma(const QVariantList& eventos) {
    try {
        if (eventos.isEmpty()) {
            return;
        }

        const QVariantMap modelos = property(kTableLanguageModelsProperty).toMap();
        if (modelos.isEmpty()) {
            if (event_table_model != nullptr) {
                event_table_model->append_events(eventos);
            }
            return;
        }

        for (auto it = modelos.constBegin(); it != modelos.constEnd(); ++it) {
            if (auto* modelo = qobject_cast<source::services::GerenciamentoTabela::EventTableModel*>(
                    it.value().value<QObject*>())) {
                modelo->append_events(eventos);
            }
        }

        if (auto* tabela_view = qobject_cast<QTableView*>(tabela_dados);
            tabela_view != nullptr && tabela_view->viewport() != nullptr) {
            tabela_view->viewport()->update();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar eventos nas tabelas por idioma", true);
    }
}

void InterfaceMonitor::limpar_tabelas_idioma() {
    try {
        substituir_eventos_tabelas_idioma(QVariantList());

        const QVariantMap views = property(kTableLanguageViewsProperty).toMap();
        for (auto it = views.constBegin(); it != views.constEnd(); ++it) {
            if (auto* tabela_view = qobject_cast<QTableView*>(it.value().value<QObject*>())) {
                tabela_view->clearSelection();
                if (tabela_view->viewport() != nullptr) {
                    tabela_view->viewport()->update();
                }
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar tabelas por idioma", true);
    }
}

QObject* InterfaceMonitor::criar_observador(const QString& diretorio) {
    Q_UNUSED(diretorio)
    return nullptr;
}

bool InterfaceMonitor::adicionar_diretorio_monitorado(const QString& diretorio) {
    const bool adicionado = gerenciador_monitoramento != nullptr
                                ? gerenciador_monitoramento->adicionar_diretorio_monitorado(diretorio)
                                : (security_manager != nullptr &&
                                   security_manager->add_monitored_directory(diretorio));
    if (adicionado) {
        atualizar_rotulo_diretorios_monitorados();
        atualizar_status();
    }
    return adicionado;
}

bool InterfaceMonitor::remover_diretorio_monitorado(const QString& diretorio) {
    const bool removido = gerenciador_monitoramento != nullptr
                              ? gerenciador_monitoramento->remover_diretorio_monitorado(diretorio)
                              : (security_manager != nullptr &&
                                 security_manager->remove_monitored_directory(diretorio));
    if (removido) {
        atualizar_rotulo_diretorios_monitorados();
        atualizar_status();
    }
    return removido;
}

QStringList InterfaceMonitor::obter_diretorios_monitorados() const {
    return gerenciador_monitoramento != nullptr
               ? gerenciador_monitoramento->obter_diretorios_monitorados()
               : (security_manager != nullptr ? security_manager->monitored_directories() : QStringList());
}

QVariantMap InterfaceMonitor::obter_resumo_seguranca() const {
    return gerenciador_monitoramento != nullptr
               ? gerenciador_monitoramento->obter_resumo_seguranca()
               : (security_manager != nullptr ? security_manager->profile_summary() : QVariantMap());
}

void InterfaceMonitor::atualizar_rotulo_diretorios_monitorados() {
    QObject* rotulo_obj = property("rotulo_diretorio").value<QObject*>();
    if (rotulo_obj == nullptr) {
        return;
    }

    const QStringList diretorios = diretorios_monitorados_ativos(this);
    const QString texto = diretorios.isEmpty()
                              ? get_text(loc, QStringLiteral("no_dir"), QStringLiteral("Nenhum diretório selecionado"))
                              : format_with_one_arg(
                                    get_text(loc, QStringLiteral("dir_selected"),
                                             QStringLiteral("Diretório selecionado: %1")),
                                    diretorios.join(QStringLiteral(" | ")));

    if (auto* rotulo = qobject_cast<QLabel*>(rotulo_obj)) {
        rotulo->setText(texto);
        return;
    }

    QMetaObject::invokeMethod(rotulo_obj, "setText", Q_ARG(QString, texto));
}

QString InterfaceMonitor::exportar_pacote_evidencia(const QString& destino) {
    return gerenciador_monitoramento != nullptr
               ? gerenciador_monitoramento->exportar_pacote_evidencia(destino)
               : (security_manager != nullptr && event_table_model != nullptr
                      ? security_manager->export_evidence_package(event_table_model->snapshot_events(), destino)
                      : QString());
}

void InterfaceMonitor::criar_barra_progresso() { GerenciadorProgresso::criar_barra_progresso(this); }

void InterfaceMonitor::atualizar_progresso_scan(int progresso, int contador, int total) {
    if (property("_atualizando_progresso_scan").toBool()) {
        return;
    }
    setProperty("_atualizando_progresso_scan", true);
    try {
        GerenciadorProgresso::atualizar_progresso_scan(this, progresso, contador, total);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em InterfaceMonitor::atualizar_progresso_scan", true);
    }
    setProperty("_atualizando_progresso_scan", false);
}

void InterfaceMonitor::atualizar_cores_tabela(const QVariantMap& resultado_cores) {
    try {
        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->_atualizar_cores_na_interface(resultado_cores);
            gerenciador_tabela->atualizar_interface_pos_processamento(resultado_cores);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar cores da tabela", true);
    }
}

void InterfaceMonitor::changeEvent(QEvent* event) {
    QMainWindow::changeEvent(event);

    if (event == nullptr) {
        return;
    }

    switch (event->type()) {
        case QEvent::ActivationChange:
            if (isActiveWindow()) {
                promover_janela_na_ordem(this, this);
                agendar_sincronizacao_janelas_associadas(this);
            }
            break;
        case QEvent::WindowStateChange:
            if (!isMinimized()) {
                agendar_sincronizacao_janelas_associadas(this);
            }
            break;
        default:
            break;
    }
}

void InterfaceMonitor::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    preaquecer_superficie_grafica_interface(this);
    if (isActiveWindow()) {
        promover_janela_na_ordem(this, this);
        agendar_sincronizacao_janelas_associadas(this);
    }
}

void InterfaceMonitor::hideEvent(QHideEvent* event) {
    QMainWindow::hideEvent(event);
}

bool InterfaceMonitor::eventFilter(QObject* watched, QEvent* event) {
    if (event != nullptr) {
        if (QWidget* widget = qobject_cast<QWidget*>(watched);
            widget != nullptr && janela_associada_a_interface(widget, this)) {
            switch (event->type()) {
                case QEvent::WindowActivate:
                    promover_janela_na_ordem(this, widget);
                    agendar_sincronizacao_janelas_associadas(this);
                    break;
                case QEvent::Show:
                    if (widget->isActiveWindow()) {
                        promover_janela_na_ordem(this, widget);
                        agendar_sincronizacao_janelas_associadas(this);
                    }
                    break;
                case QEvent::WindowStateChange:
                    if (!widget->isMinimized() && widget->isActiveWindow()) {
                        promover_janela_na_ordem(this, widget);
                        agendar_sincronizacao_janelas_associadas(this);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void InterfaceMonitor::closeEvent(QCloseEvent* event) {
    const bool fechamento_programatico_permitido =
        property("_permitir_fechamento_programatico").toBool();
    if (event != nullptr && !event->spontaneous() && !fechamento_programatico_permitido) {
        source::utils::LogManager::get_logger().warning(
            "Tentativa de fechamento programatico da InterfaceMonitor foi bloqueada");
        event->ignore();
        return;
    }

    setProperty("_encerrando_interface", true);

    try {
        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->shutdown_executors();
            gerenciador_tabela->timer_atualizacao.stop();
        }

        if (evento_base != nullptr) {
            QString db_path;
            if (!QMetaObject::invokeMethod(evento_base, "db_path", Q_RETURN_ARG(QString, db_path))) {
                db_path = evento_base->property("db_path").toString();
            }
            if (!db_path.isEmpty()) {
                source::services::GerenciamentoBaseEvento::DatabaseWriter::get_instance(db_path)->stop();
            }
        }

        if (observador == nullptr) {
            observador = security_manager != nullptr ? security_manager->primary_observer()
                                                     : obter_observador_cpp(this);
        }
        if (observador != nullptr && observador->ativo.load()) {
            observador->parar();
        }
        if (security_manager != nullptr) {
            security_manager->stop_runtime_observers();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao fechar recursos da aplicação", true);
    }

    if (painel_filtros != nullptr) {
        painel_filtros->close();
    }

    fechar_janelas_associadas_a_interface(this);
    fechar_dialogo_se_existir(this, "dialog_estatisticas");
    fechar_dialogo_se_existir(this, "_sobre_dialog");
    fechar_dialogo_se_existir(this, "_manual_dialog");
    fechar_dialogo_se_existir(this, "_ajuda_dialog");
    fechar_dialogo_se_existir(this, "_security_multidirectory_dialog");
    fechar_dialogo_se_existir(this, "_security_alert_center_dialog");
    fechar_dialogo_se_existir(this, "_security_session_center_dialog");
    fechar_dialogo_se_existir(this, "_security_recovery_dialog");
    fechar_dialogo_se_existir(this, "_security_evidence_dialog");

    setProperty("_permitir_fechamento_programatico", false);
    QMainWindow::closeEvent(event);
}

}  // namespace source::gui
