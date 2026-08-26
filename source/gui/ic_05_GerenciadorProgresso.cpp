#include "ic_05_GerenciadorProgresso.h"

#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QString>

#include "ic_01_InterfaceMonitor.h"
#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_05_GerenciadorProgressoUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

source::services::Observador* obter_observador_cpp(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return nullptr;
    }
    if (interface_monitor->observador != nullptr) {
        return interface_monitor->observador;
    }
    const qulonglong ptr = interface_monitor->property("_observador_cpp_ptr").toULongLong();
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

void set_rotulo_resultado(InterfaceMonitor* interface_monitor, const QString& text) {
    if (interface_monitor == nullptr) {
        return;
    }
    QObject* rotulo_obj = interface_monitor->property("rotulo_resultado").value<QObject*>();
    auto* rotulo = qobject_cast<QLabel*>(rotulo_obj);
    if (rotulo != nullptr) {
        rotulo->setText(text);
    }
}

void set_label_text(QObject* label_obj, const QString& text) {
    if (auto* label = qobject_cast<QLabel*>(label_obj)) {
        label->setText(text);
    } else if (label_obj != nullptr) {
        label_obj->setProperty("text", text);
    }
}

void atualizar_contadores_rodape_scan(InterfaceMonitor* interface_monitor, int contador, int total) {
    if (interface_monitor == nullptr || interface_monitor->loc == nullptr) {
        return;
    }

    const QString visiveis = get_text(interface_monitor->loc, "visible_filters", "Visible");
    const QString total_monitorado = get_text(interface_monitor->loc, "total_monitored", "Total monitored");
    const QString taxa_filtrada = get_text(interface_monitor->loc, "filtered_rate", "Filtered rate");
    const QString eventos_monitorados = get_text(interface_monitor->loc, "events_monitored", "Events monitored");

    const QString texto_contagem = QStringLiteral("%1: %2 / %3: %4 | %5: 0.00%")
                                       .arg(visiveis)
                                       .arg(contador)
                                       .arg(total_monitorado)
                                       .arg(total)
                                       .arg(taxa_filtrada);
    const QString texto_eventos = QStringLiteral("%1: %2").arg(eventos_monitorados).arg(contador);

    set_label_text(interface_monitor->property("label_contagem").value<QObject*>(), texto_contagem);
    set_label_text(interface_monitor->property("rotulo_contador_eventos").value<QObject*>(), texto_eventos);
}

QProgressBar* get_barra(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return nullptr;
    }
    return interface_monitor->property("barra_progresso").value<QProgressBar*>();
}

QVariantMap resumo_scan_multidiretorio(InterfaceMonitor* interface_monitor) {
    if (interface_monitor == nullptr) {
        return {};
    }

    auto* security_manager = source::security::ui::get_security_manager(interface_monitor);
    return security_manager != nullptr ? security_manager->runtime_scan_summary() : QVariantMap();
}

void aplicar_resumo_scan_multidiretorio(InterfaceMonitor* interface_monitor,
                                        int& progresso,
                                        int& contador,
                                        int& total,
                                        QVariantMap* resumo_saida = nullptr) {
    const QVariantMap resumo = resumo_scan_multidiretorio(interface_monitor);
    if (resumo_saida != nullptr) {
        *resumo_saida = resumo;
    }

    const int total_agregado = resumo.value(QStringLiteral("scan_total")).toInt();
    const int contador_agregado = resumo.value(QStringLiteral("scan_processed")).toInt();
    const bool scan_em_execucao = resumo.value(QStringLiteral("scan_running")).toBool();

    if (total_agregado <= 0 && contador_agregado <= 0 && !scan_em_execucao) {
        return;
    }

    total = total_agregado;
    contador = contador_agregado;
    progresso = resumo.value(QStringLiteral("scan_progress")).toInt();
}

}  // namespace

void GerenciadorProgresso::criar_barra_progresso(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr || interface_monitor->gerenciador_progresso_ui == nullptr) {
            return;
        }
        interface_monitor->gerenciador_progresso_ui->criar_barra_progresso();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar barra de progresso", true);
    }
}

void GerenciadorProgresso::atualizar_progresso_scan(InterfaceMonitor* interface_monitor, int progresso,
                                                     int contador, int total) {
    QVariantMap resumo_scan;
    aplicar_resumo_scan_multidiretorio(interface_monitor, progresso, contador, total, &resumo_scan);

    try {
        if (interface_monitor == nullptr) {
            return;
        }

        source::services::Observador* obs_cpp = obter_observador_cpp(interface_monitor);
        QObject* obs_qt = interface_monitor->property("observador").value<QObject*>();
        auto* security_manager = source::security::ui::get_security_manager(interface_monitor);
        const bool ativo_cpp = (obs_cpp != nullptr && obs_cpp->ativo.load());
        const bool ativo_qt = (obs_qt != nullptr && obs_qt->property("ativo").toBool());
        const bool ativo_security = (security_manager != nullptr && security_manager->any_runtime_active());

        if (!ativo_cpp && !ativo_qt && !ativo_security) {
            interface_monitor->setProperty("_scan_em_execucao", false);
            interface_monitor->setProperty("_scan_contador", 0);
            interface_monitor->setProperty("_scan_total", 0);
            if (QProgressBar* barra = get_barra(interface_monitor)) {
                barra->hide();
                barra->setValue(0);
            }
            set_rotulo_resultado(interface_monitor,
                                 get_text(interface_monitor->loc, "monitoring_stopped"));
            return;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            "Erro ao atualizar progresso (monitoramento parado)", true);
    }

    try {
        source::services::Observador* obs_cpp = obter_observador_cpp(interface_monitor);
        QObject* obs_qt = interface_monitor->property("observador").value<QObject*>();

        bool scanner_pausado = false;
        if (obs_cpp != nullptr) {
            if (auto* scanner = obs_cpp->scanner.data(); scanner != nullptr) {
                scanner_pausado = scanner->pausado.load();
            }
        } else if (obs_qt != nullptr) {
            QObject* scan_worker = obs_qt->property("scan_worker").value<QObject*>();
            QObject* scanner = (scan_worker != nullptr) ? scan_worker->property("scanner").value<QObject*>()
                                                        : nullptr;
            if (scanner != nullptr) {
                scanner_pausado = scanner->property("pausado").toBool();
            }
        }
        scanner_pausado = scanner_pausado || resumo_scan.value(QStringLiteral("scan_paused")).toBool();

        if (scanner_pausado) {
            interface_monitor->setProperty("_scan_em_execucao", (total > 0 && progresso < 100));
            interface_monitor->setProperty("_scan_contador", contador);
            interface_monitor->setProperty("_scan_total", total);
            if (QProgressBar* barra = get_barra(interface_monitor)) {
                if (!barra->isVisible()) {
                    barra->show();
                }
                barra->setValue(progresso);
            }

            const QString texto =
                QString("%1 %2% (%3/%4)")
                    .arg(get_text(interface_monitor->loc, "paused", "Paused"))
                    .arg(progresso)
                    .arg(contador)
                    .arg(total);
            set_rotulo_resultado(interface_monitor, texto);
            return;
        }

        const bool pausado_cpp = (obs_cpp != nullptr && obs_cpp->pausado.load());
        const bool pausado_qt = (obs_qt != nullptr && obs_qt->property("pausado").toBool());
        if ((pausado_cpp || pausado_qt) && !scanner_pausado) {
            interface_monitor->setProperty("_scan_em_execucao", false);
            interface_monitor->setProperty("_scan_contador", 0);
            interface_monitor->setProperty("_scan_total", 0);
            if (QProgressBar* barra = get_barra(interface_monitor)) {
                barra->hide();
                barra->setValue(0);
            }

            set_rotulo_resultado(interface_monitor,
                                 get_text(interface_monitor->loc, "monitoring_paused"));
            return;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar progresso (pausado)", true);
    }

    try {
        interface_monitor->setProperty("_scan_em_execucao", (total > 0 && progresso < 100));
        interface_monitor->setProperty("_scan_contador", contador);
        interface_monitor->setProperty("_scan_total", total);
        if (interface_monitor->gerenciador_progresso_ui != nullptr) {
            interface_monitor->gerenciador_progresso_ui->atualizar_progresso_scan(progresso, contador,
                                                                                   total);
        }
        if (total >= 0 && contador >= 0) {
            atualizar_contadores_rodape_scan(interface_monitor, contador, total);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar barra de progresso", true);
    }

    try {
        if (progresso >= 100) {
            QTimer::singleShot(800, interface_monitor, [interface_monitor]() {
                try {
                    source::services::Observador* obs_cpp = obter_observador_cpp(interface_monitor);
                    QObject* obs_qt = interface_monitor->property("observador").value<QObject*>();
                    const bool ativo_cpp = (obs_cpp != nullptr && obs_cpp->ativo.load());
                    const bool ativo_qt = (obs_qt != nullptr && obs_qt->property("ativo").toBool());

                    if (ativo_cpp || ativo_qt) {
                        set_rotulo_resultado(interface_monitor,
                                             get_text(interface_monitor->loc, "monitoring_started"));
                    }
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao finalizar pós-scan", true);
                }
            });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao agendar pós-scan", true);
    }
}

void GerenciadorProgresso::esconder_barra_progresso(InterfaceMonitor* interface_monitor) {
    try {
        if (QProgressBar* barra = get_barra(interface_monitor)) {
            QTimer::singleShot(1000, barra, &QProgressBar::hide);
            barra->setValue(0);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao esconder barra de progresso", true);
    }
}

}  // namespace source::gui
