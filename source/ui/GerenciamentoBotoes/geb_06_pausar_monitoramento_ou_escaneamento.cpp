#include "geb_06_pausar_monitoramento_ou_escaneamento.h"

#include <exception>
#include <stdexcept>

#include <QMessageBox>
#include <QMetaObject>
#include <QThread>
#include <QVariant>
#include <QWidget>

#include "geb_02_alternar_analise_diretorio.h"
#include "source/services/ob_01_Observador.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text.isEmpty() ? fallback : text;
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
}

namespace source::ui::GerenciamentoBotoes {

void pausar_monitoramento_ou_escaneamento(source::ui::GerenciadorBotoes* self) {
    try {
        if (self == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        if (interface_obj == nullptr) {
            return;
        }

        auto* loc = self->loc_obj();
        auto* parent = qobject_cast<QWidget*>(interface_obj);

        QObject* obs_qt = interface_obj->property("observador").value<QObject*>();
        source::services::Observador* obs_cpp = nullptr;
        if (obs_qt == nullptr) {
            obs_cpp = obter_observador_cpp(interface_obj);
        }

        if (obs_qt != nullptr || obs_cpp != nullptr) {
            bool scan_running = false;

            if (obs_qt != nullptr) {
                scan_running = interface_obj->property("_scan_em_execucao").toBool();

                const bool ativo = obs_qt->property("ativo").toBool();
                if (ativo || scan_running) {
                    try {
                        if (!QMetaObject::invokeMethod(obs_qt, "pausar_monitoramento_ou_escaneamento")) {
                            throw std::runtime_error("Método pausar_monitoramento_ou_escaneamento indisponível");
                        }
                    } catch (const std::exception& e) {
                        source::utils::LogManager::get_logger().error(
                            QString("Erro ao pausar monitoramento ou escaneamento: %1")
                                .arg(QString::fromUtf8(e.what())),
                            true);
                    } catch (...) {
                        source::utils::LogManager::get_logger().error(
                            "Erro ao pausar monitoramento ou escaneamento", true);
                    }
                    return;
                }
            } else {
                if (obs_cpp->thread_scan != nullptr) {
                    scan_running = is_thread_valid_and_running(obs_cpp->thread_scan.data());
                }

                if (obs_cpp->ativo.load() || scan_running) {
                    try {
                        obs_cpp->pausar_monitoramento_ou_escaneamento();
                    } catch (const std::exception& e) {
                        source::utils::LogManager::get_logger().error(
                            QString("Erro ao pausar monitoramento ou escaneamento: %1")
                                .arg(QString::fromUtf8(e.what())),
                            true);
                    } catch (...) {
                        source::utils::LogManager::get_logger().error(
                            "Erro ao pausar monitoramento ou escaneamento", true);
                    }
                    return;
                }
            }

            QMessageBox::information(parent,
                                     get_text(loc, "warning", "Aviso"),
                                     get_text(loc, "no_scan_or_monitor_to_pause",
                                              "Não há escaneamento nem monitoramento em andamento para pausar."));
            return;
        }

        QObject* scanner = interface_obj->property("scanner").value<QObject*>();
        if (scanner != nullptr) {
            try {
                if (!QMetaObject::invokeMethod(scanner, "pausar")) {
                    throw std::runtime_error("Método pausar indisponível no scanner");
                }
                return;
            } catch (const std::exception& e) {
                source::utils::LogManager::get_logger().error(
                    QString("Erro ao pausar scanner: %1").arg(QString::fromUtf8(e.what())), true);
            } catch (...) {
                source::utils::LogManager::get_logger().error("Erro ao pausar scanner", true);
            }
        }

        QMessageBox::information(parent,
                                 get_text(loc, "warning", "Aviso"),
                                 get_text(loc, "no_scan_or_monitor_to_pause",
                                          "Não há escaneamento nem monitoramento em andamento para pausar."));
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao pausar: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao pausar", true);
    }
}

}  // namespace source::ui::GerenciamentoBotoes
