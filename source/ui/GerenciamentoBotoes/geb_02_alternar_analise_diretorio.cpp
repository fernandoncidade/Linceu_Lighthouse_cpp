#include "geb_02_alternar_analise_diretorio.h"

#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QPointer>
#include <QThread>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

#include "source/security/secore_02_SecurityManager.h"
#include "source/security/secui_00_helpers.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_08_EventoMovido.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QLabel* get_label_property(QObject* interface_obj, const char* property_name) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    const QVariant prop = interface_obj->property(property_name);
    if (!prop.isValid()) {
        return nullptr;
    }

    QLabel* label = prop.value<QLabel*>();
    if (label == nullptr) {
        label = qobject_cast<QLabel*>(prop.value<QObject*>());
    }
    return label;
}

QString idioma_atual_interface(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return QString();
    }

    QObject* loc = interface_obj->property("loc").value<QObject*>();
    return loc != nullptr ? loc->property("idioma_atual").toString() : QString();
}

void sincronizar_idioma_observador(source::services::Observador* observador, QObject* interface_obj) {
    if (observador == nullptr || observador->loc == nullptr) {
        return;
    }

    const QString idioma = idioma_atual_interface(interface_obj);
    if (idioma.isEmpty() || observador->loc->property("idioma_atual").toString() == idioma) {
        return;
    }
    observador->loc->set_idioma(idioma);
}

void drenar_exclusoes_pendentes(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    try {
        source::services::_processar_exclusoes_pendentes(interface_obj);
    } catch (...) {
    }
    interface_obj->setProperty("excluidos_recentemente", QVariantMap());
}

using ObservadorMap = std::unordered_map<QObject*, std::unique_ptr<source::services::Observador>>;
ObservadorMap g_observadores_cpp;
std::mutex g_observadores_cpp_mutex;

source::services::Observador* obter_observador_cpp_mapa(QObject* interface_obj) {
    std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
    const auto it = g_observadores_cpp.find(interface_obj);
    if (it == g_observadores_cpp.end()) {
        return nullptr;
    }
    return it->second.get();
}

source::services::Observador* obter_observador_cpp_prop(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    const qulonglong ptr = interface_obj->property("_observador_cpp_ptr").toULongLong();
    if (ptr == 0) {
        return nullptr;
    }

    return reinterpret_cast<source::services::Observador*>(ptr);
}

source::services::Observador* obter_observador_cpp(QObject* interface_obj) {
    if (interface_obj != nullptr) {
        auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        if (security_manager != nullptr) {
            return security_manager->primary_observer();
        }
    }

    source::services::Observador* obs = obter_observador_cpp_mapa(interface_obj);
    if (obs != nullptr) {
        return obs;
    }
    return obter_observador_cpp_prop(interface_obj);
}

void remover_observador_cpp(QObject* interface_obj) {
    if (interface_obj == nullptr) {
        return;
    }

    if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        security_manager != nullptr) {
        security_manager->stop_runtime_observers();
        interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
        return;
    }

    std::unique_ptr<source::services::Observador> removido;
    {
        std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
        const auto it = g_observadores_cpp.find(interface_obj);
        if (it != g_observadores_cpp.end()) {
            removido = std::move(it->second);
            g_observadores_cpp.erase(it);
        }
    }

    if (removido != nullptr) {
        try {
            source::services::parar_sistema_evento(interface_obj);
            if (removido->ativo.load()) {
                removido->parar();
            }
        } catch (...) {
        }
    }

    if (interface_obj->property("_observador_cpp_ptr").toULongLong() != 0) {
        interface_obj->setProperty("_observador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
    }
}

source::services::Observador* criar_observador_cpp(QObject* interface_obj, const QString& diretorio) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    if (auto* security_manager = source::security::ui::get_security_manager(interface_obj);
        security_manager != nullptr) {
        security_manager->configure_primary_directory(diretorio);
        return security_manager->configure_runtime_observers();
    }

    auto callback = [interface_guard = QPointer<QObject>(interface_obj)](const QVariantMap& evento) {
        QObject* interface_viva = interface_guard.data();
        if (interface_viva == nullptr) {
            return;
        }

        try {
            const QVariant evento_var(evento);
            source::services::adicionar_evento(interface_viva, evento_var);
        } catch (...) {
            source::utils::LogManager::get_logger().error("Erro ao encaminhar evento para a interface", true);
        }
    };

    auto novo_observador = std::make_unique<source::services::Observador>(diretorio, std::move(callback));
    novo_observador->interface = interface_obj;
    sincronizar_idioma_observador(novo_observador.get(), interface_obj);

    source::services::Observador* observador_raw = novo_observador.get();
    {
        std::lock_guard<std::mutex> lock(g_observadores_cpp_mutex);
        g_observadores_cpp[interface_obj] = std::move(novo_observador);
    }

    interface_obj->setProperty("_observador_cpp_ptr",
                               QVariant::fromValue<qulonglong>(reinterpret_cast<qulonglong>(observador_raw)));

    if (!interface_obj->property("_observador_cpp_cleanup_connected").toBool()) {
        QObject::connect(interface_obj, &QObject::destroyed, [](QObject* obj) { remover_observador_cpp(obj); });
        interface_obj->setProperty("_observador_cpp_cleanup_connected", true);
    }

    return observador_raw;
}

QObject* obter_ou_criar_observador_qt(QObject* interface_obj, const QString& diretorio) {
    if (interface_obj == nullptr) {
        return nullptr;
    }

    QObject* observador = interface_obj->property("observador").value<QObject*>();
    if (observador != nullptr) {
        return observador;
    }

    QObject* novo_objeto = nullptr;
    bool ok = QMetaObject::invokeMethod(interface_obj, "criar_observador",
                                        Q_RETURN_ARG(QObject*, novo_objeto),
                                        Q_ARG(QString, diretorio));
    if (ok && novo_objeto != nullptr) {
        observador = novo_objeto;
    }

    if (observador != nullptr) {
        observador->setProperty("interface", QVariant::fromValue(interface_obj));
        interface_obj->setProperty("observador", QVariant::fromValue(observador));
    }

    return observador;
}
}

namespace source::ui::GerenciamentoBotoes {

bool is_thread_valid_and_running(QThread* thread) {
    return thread != nullptr && thread->isRunning();
}

void alternar_analise_diretorio(source::ui::GerenciadorBotoes* self) {
    try {
        if (self == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        if (interface_obj == nullptr) {
            return;
        }

        if (interface_obj->property("_alternando_analise").toBool()) {
            return;
        }
        interface_obj->setProperty("_alternando_analise", true);

        auto* loc = self->loc_obj();
        auto* parent = qobject_cast<QWidget*>(interface_obj);
        auto* security_manager = source::security::ui::get_security_manager(interface_obj);

        const QString diretorio = interface_obj->property("diretorio_atual").toString();
        if (diretorio.isEmpty()) {
            QMessageBox::warning(parent, get_text(loc, "warning"), get_text(loc, "select_first"));
            interface_obj->setProperty("_alternando_analise", false);
            return;
        }

        QObject* observador_qt = obter_ou_criar_observador_qt(interface_obj, diretorio);
        source::services::Observador* observador_cpp = nullptr;

        if (observador_qt == nullptr) {
            observador_cpp = obter_observador_cpp(interface_obj);
            if (observador_cpp == nullptr) {
                observador_cpp = criar_observador_cpp(interface_obj, diretorio);
            }
        }

        if (observador_qt == nullptr && observador_cpp == nullptr) {
            interface_obj->setProperty("_alternando_analise", false);
            return;
        }

        auto* rotulo = get_label_property(interface_obj, "rotulo_resultado");

        if (observador_qt != nullptr) {
            const bool scan_running = interface_obj->property("_scan_em_execucao").toBool();

            if (scan_running) {
                try {
                    drenar_exclusoes_pendentes(interface_obj);
                    if (!QMetaObject::invokeMethod(observador_qt, "parar_scan")) {
                        throw std::runtime_error("Método parar_scan indisponível");
                    }
                    if (rotulo != nullptr) {
                        rotulo->setText(get_text(loc, "monitoring_stopped"));
                    }
                    interface_obj->setProperty("_alternando_analise", false);
                    return;
                } catch (const std::exception& e) {
                    source::utils::LogManager::get_logger().error(
                        QString("Erro ao parar escaneamento: %1").arg(QString::fromUtf8(e.what())), true);
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao parar escaneamento", true);
                }
            }
            const bool ativo = observador_qt->property("ativo").toBool();
            if (!ativo) {
                QMetaObject::invokeMethod(observador_qt, "iniciar");
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_started"));
                }
            } else {
                drenar_exclusoes_pendentes(interface_obj);
                QMetaObject::invokeMethod(observador_qt, "parar");
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_stopped"));
                }
            }
            interface_obj->setProperty("_alternando_analise", false);
            return;
        }

        if (security_manager != nullptr) {
            if (security_manager->any_runtime_scan_running()) {
                try {
                    drenar_exclusoes_pendentes(interface_obj);
                    security_manager->stop_runtime_scans();
                    if (rotulo != nullptr) {
                        rotulo->setText(get_text(loc, "monitoring_stopped"));
                    }
                    interface_obj->setProperty("_alternando_analise", false);
                    return;
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao parar escaneamento", true);
                }
            }

            if (!security_manager->any_runtime_active()) {
                security_manager->configure_runtime_observers();
                security_manager->start_runtime_observers();
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_started"));
                }
            } else {
                drenar_exclusoes_pendentes(interface_obj);
                security_manager->stop_runtime_observers();
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_stopped"));
                }
            }
        } else {
            QThread* thread_scan = observador_cpp->thread_scan.data();
            if (is_thread_valid_and_running(thread_scan)) {
                try {
                    drenar_exclusoes_pendentes(interface_obj);
                    observador_cpp->parar_scan();
                    if (rotulo != nullptr) {
                        rotulo->setText(get_text(loc, "monitoring_stopped"));
                    }
                    interface_obj->setProperty("_alternando_analise", false);
                    return;
                } catch (const std::exception& e) {
                    source::utils::LogManager::get_logger().error(
                        QString("Erro ao parar escaneamento: %1").arg(QString::fromUtf8(e.what())), true);
                } catch (...) {
                    source::utils::LogManager::get_logger().error("Erro ao parar escaneamento", true);
                }
            }

            if (!observador_cpp->ativo.load()) {
                sincronizar_idioma_observador(observador_cpp, interface_obj);
                observador_cpp->iniciar();
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_started"));
                }
            } else {
                drenar_exclusoes_pendentes(interface_obj);
                source::services::parar_sistema_evento(interface_obj);
                observador_cpp->parar();
                if (rotulo != nullptr) {
                    rotulo->setText(get_text(loc, "monitoring_stopped"));
                }
            }
        }
        interface_obj->setProperty("_alternando_analise", false);
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao alternar análise: %1").arg(QString::fromUtf8(e.what())), true);
        if (self != nullptr && self->interface_obj() != nullptr) {
            self->interface_obj()->setProperty("_alternando_analise", false);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar análise", true);
        if (self != nullptr && self->interface_obj() != nullptr) {
            self->interface_obj()->setProperty("_alternando_analise", false);
        }
    }
}

}  // namespace source::ui::GerenciamentoBotoes
