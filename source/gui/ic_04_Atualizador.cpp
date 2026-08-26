#include "ic_04_Atualizador.h"

#include <algorithm>

#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QTableView>
#include <QTableWidget>
#include <QVariant>

#include "ic_01_InterfaceMonitor.h"
#include "source/security/secui_00_helpers.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_02_GerenciadorBotoesUI.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

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
    return pattern;
}

void set_text(QObject* obj, const QString& text) {
    if (obj == nullptr) {
        return;
    }
    if (auto* label = qobject_cast<QLabel*>(obj)) {
        label->setText(text);
        return;
    }
    obj->setProperty("text", text);
}

void atualizar_feedback_traducao(QObject* interface_obj, source::ui::LocalizadorQt* loc) {
    if (interface_obj == nullptr || loc == nullptr) {
        return;
    }
    if (!interface_obj->property("_traducao_em_execucao").toBool()) {
        return;
    }

    const int total = std::max(1, interface_obj->property("_traducao_total_unidades").toInt());
    int concluido = interface_obj->property("_traducao_concluidas_unidades").toInt();
    if (concluido < 0) concluido = 0;
    if (concluido > total) concluido = total;
    const int progresso = (concluido * 100) / total;

    if (QObject* barra_obj = interface_obj->property("barra_progresso").value<QObject*>()) {
        if (auto* barra = qobject_cast<QProgressBar*>(barra_obj)) {
            if (!barra->isVisible()) {
                barra->show();
            }
            barra->setValue(progresso);
        } else {
            QMetaObject::invokeMethod(barra_obj, "show");
            QMetaObject::invokeMethod(barra_obj, "setValue", Q_ARG(int, progresso));
        }
    }

    const QString prefix = get_text(loc, "translating_table", "Translating...");
    const QString texto = QStringLiteral("%1 %2% (%3/%4)")
                              .arg(prefix)
                              .arg(progresso)
                              .arg(concluido)
                              .arg(total);
    set_text(interface_obj->property("rotulo_resultado").value<QObject*>(), texto);
}

struct ProgressoTraducaoUI {
    bool ativo = false;
    int total_unidades_ui = 0;
    int unidades_aplicadas = 0;
    int total_fases = 6;
};

ProgressoTraducaoUI iniciar_progresso_ui(InterfaceMonitor* interface_monitor) {
    ProgressoTraducaoUI progresso;
    if (interface_monitor == nullptr || interface_monitor->loc == nullptr) {
        return progresso;
    }
    if (!interface_monitor->property("_traducao_em_execucao").toBool()) {
        return progresso;
    }
    if (interface_monitor->property("_traducao_ui_contabilizada").toBool()) {
        return progresso;
    }

    progresso.ativo = true;
    progresso.total_unidades_ui =
        std::max(1, interface_monitor->property("_traducao_unidades_ui_total").toInt());
    return progresso;
}

void aplicar_fase_progresso_ui(InterfaceMonitor* interface_monitor,
                               ProgressoTraducaoUI& progresso,
                               int fase_atual) {
    if (!progresso.ativo || interface_monitor == nullptr || interface_monitor->loc == nullptr) {
        return;
    }

    if (fase_atual < 1) {
        fase_atual = 1;
    } else if (fase_atual > progresso.total_fases) {
        fase_atual = progresso.total_fases;
    }

    const int alvo_unidades =
        (progresso.total_unidades_ui * fase_atual) / std::max(1, progresso.total_fases);
    if (alvo_unidades <= progresso.unidades_aplicadas) {
        return;
    }

    const int delta = alvo_unidades - progresso.unidades_aplicadas;
    const int total_global = std::max(1, interface_monitor->property("_traducao_total_unidades").toInt());
    int concluido = interface_monitor->property("_traducao_concluidas_unidades").toInt();
    concluido += delta;
    if (concluido < 0) concluido = 0;
    if (concluido > total_global) concluido = total_global;

    interface_monitor->setProperty("_traducao_concluidas_unidades", concluido);
    progresso.unidades_aplicadas = alvo_unidades;
    atualizar_feedback_traducao(interface_monitor, interface_monitor->loc);
}

void finalizar_progresso_ui(InterfaceMonitor* interface_monitor, ProgressoTraducaoUI& progresso) {
    if (!progresso.ativo || interface_monitor == nullptr) {
        return;
    }
    aplicar_fase_progresso_ui(interface_monitor, progresso, progresso.total_fases);
    interface_monitor->setProperty("_traducao_ui_contabilizada", true);
    progresso.ativo = false;
}

}  // namespace

void Atualizador::atualizar_interface(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        if (interface_monitor->loc != nullptr) {
            interface_monitor->alternar_tabela_idioma(
                interface_monitor->loc->property("idioma_atual").toString());
        }

        QObject* tabela_obj = interface_monitor->tabela_dados;
        source::tools::Filtros* painel_filtros = interface_monitor->painel_filtros;
        ProgressoTraducaoUI progresso_ui = iniciar_progresso_ui(interface_monitor);

        if (auto* tabela_widget = qobject_cast<QTableWidget*>(tabela_obj)) {
            tabela_widget->blockSignals(true);
        } else if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
            tabela_view->blockSignals(true);
        }

        if (painel_filtros != nullptr) {
            painel_filtros->blockSignals(true);
        }

        interface_monitor->setWindowTitle(get_text(interface_monitor->loc, "window_title", "Linceu Lighthouse"));

        interface_monitor->atualizar_rotulo_diretorios_monitorados();
        source::security::ui::retranslate_active_security_feedback(interface_monitor);
        aplicar_fase_progresso_ui(interface_monitor, progresso_ui, 1);

        QObject* rotulo_resultado = interface_monitor->property("rotulo_resultado").value<QObject*>();
        source::services::Observador* observador_cpp = interface_monitor->observador;
        if (observador_cpp == nullptr) {
            const qulonglong ptr = interface_monitor->property("_observador_cpp_ptr").toULongLong();
            observador_cpp = reinterpret_cast<source::services::Observador*>(ptr);
        }

        if (observador_cpp != nullptr && observador_cpp->ativo.load()) {
            set_text(rotulo_resultado, get_text(interface_monitor->loc, "monitoring_started"));
        } else if (observador_cpp != nullptr) {
            set_text(rotulo_resultado, get_text(interface_monitor->loc, "monitoring_stopped"));
        } else {
            set_text(rotulo_resultado, get_text(interface_monitor->loc, "select_to_start"));
        }
        aplicar_fase_progresso_ui(interface_monitor, progresso_ui, 2);

        if (interface_monitor->gerenciador_menus_ui != nullptr) {
            interface_monitor->gerenciador_menus_ui->criar_menu_principal();
        }
        aplicar_fase_progresso_ui(interface_monitor, progresso_ui, 3);
        if (interface_monitor->gerenciador_botoes_ui != nullptr) {
            interface_monitor->gerenciador_botoes_ui->update_buttons_text(interface_monitor->loc);
        }
        if (painel_filtros != nullptr) {
            painel_filtros->atualizar_interface();
        }
        aplicar_fase_progresso_ui(interface_monitor, progresso_ui, 4);
        if (interface_monitor->gerenciador_tabela != nullptr) {
            const bool trocando_idioma = interface_monitor->property("_trocando_idioma").toBool() ||
                                         (interface_monitor->loc != nullptr &&
                                          interface_monitor->loc->property("_set_idioma_in_progress").toBool());

            if (trocando_idioma) {
                // Em troca de idioma, evite recarregar a tabela inteira do banco.
                interface_monitor->gerenciador_tabela->atualizar_cabecalhos();
            } else {
                interface_monitor->gerenciador_tabela->configurar_tabela(interface_monitor->tabela_dados);
                interface_monitor->gerenciador_tabela->aplicar_cores_todas_colunas_processamento(
                    QVariantMap{{"reprocessar", true}});
            }
        }
        aplicar_fase_progresso_ui(interface_monitor, progresso_ui, 5);

        if (auto* tabela_widget = qobject_cast<QTableWidget*>(tabela_obj)) {
            tabela_widget->blockSignals(false);
            tabela_widget->viewport()->update();
        } else if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
            tabela_view->blockSignals(false);
            tabela_view->viewport()->update();
        }

        if (painel_filtros != nullptr) {
            painel_filtros->blockSignals(false);
        }

        finalizar_progresso_ui(interface_monitor, progresso_ui);
        atualizar_status(interface_monitor);
        interface_monitor->update();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface", true);
    }
}

void Atualizador::atualizar_status(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr || interface_monitor->painel_filtros == nullptr) {
            return;
        }

        QObject* label_contagem = interface_monitor->property("label_contagem").value<QObject*>();
        QObject* rotulo_contador_eventos =
            interface_monitor->property("rotulo_contador_eventos").value<QObject*>();

        // While scan is running, keep footer counters aligned with scan progress counters.
        if (interface_monitor->property("_scan_em_execucao").toBool()) {
            const int total = interface_monitor->property("_scan_total").toInt();
            int contador = interface_monitor->property("_scan_contador").toInt();
            if (contador < 0) contador = 0;
            if (total > 0 && contador > total) contador = total;

            const QString visible = get_text(interface_monitor->loc, "visible_filters", "Visible");
            const QString total_monitored =
                get_text(interface_monitor->loc, "total_monitored", "Total monitored");
            const QString filtered_rate =
                get_text(interface_monitor->loc, "filtered_rate", "Filtered rate");
            const QString events_monitored =
                get_text(interface_monitor->loc, "events_monitored", "Events monitored");

            set_text(label_contagem,
                     QStringLiteral("%1: %2 / %3: %4 | %5: 0.00%")
                         .arg(visible)
                         .arg(contador)
                         .arg(total_monitored)
                         .arg(total)
                         .arg(filtered_rate));
            set_text(rotulo_contador_eventos,
                     QStringLiteral("%1: %2").arg(events_monitored).arg(contador));
            return;
        }

        set_text(label_contagem, interface_monitor->painel_filtros->atualizar_contagem());
        auto* adm = interface_monitor->painel_filtros->administrador_filtros_obj();
        if (adm != nullptr) {
            set_text(rotulo_contador_eventos, adm->atualizar_contagem_eventos_monitorados());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar status", true);
    }
}

void Atualizador::abrir_janela_filtros(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        auto* tabela = qobject_cast<QWidget*>(interface_monitor->tabela_dados);
        if (tabela == nullptr) {
            return;
        }

        if (interface_monitor->painel_filtros != nullptr) {
            interface_monitor->painel_filtros->set_tabela_dados(tabela);
            interface_monitor->painel_filtros->setWindowModality(Qt::NonModal);
            interface_monitor->painel_filtros->show();
            interface_monitor->painel_filtros->raise();
            interface_monitor->painel_filtros->activateWindow();
            interface_monitor->setProperty(
                "painel_filtros",
                QVariant::fromValue(static_cast<QObject*>(interface_monitor->painel_filtros)));
            return;
        }

        interface_monitor->painel_filtros =
            new source::tools::Filtros(tabela, interface_monitor->loc, interface_monitor);
        interface_monitor->painel_filtros->setWindowModality(Qt::NonModal);
        QObject::connect(interface_monitor->painel_filtros, &source::tools::Filtros::filtroAplicado,
                         interface_monitor, &source::gui::InterfaceMonitor::atualizar_status);
        interface_monitor->painel_filtros->show();
        interface_monitor->setProperty(
            "painel_filtros",
            QVariant::fromValue(static_cast<QObject*>(interface_monitor->painel_filtros)));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao abrir janela de filtros", true);
    }
}

}  // namespace source::gui
