#include "ic_06_GerenciadorMensagens.h"

#include <QLabel>

#include "ic_01_InterfaceMonitor.h"
#include "source/services/ob_01_Observador.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::gui {
namespace {

void set_label_text(QObject* label_obj, const QString& text) {
    if (auto* label = qobject_cast<QLabel*>(label_obj)) {
        label->setText(text);
    }
}

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

}  // namespace

void GerenciadorMensagens::atualizar_rotulos(InterfaceMonitor* interface_monitor) {
    try {
        if (interface_monitor == nullptr) {
            return;
        }

        QObject* rotulo_resultado = interface_monitor->property("rotulo_resultado").value<QObject*>();
        interface_monitor->atualizar_rotulo_diretorios_monitorados();

        source::services::Observador* obs = obter_observador_cpp(interface_monitor);
        if (obs != nullptr && obs->ativo.load()) {
            set_label_text(rotulo_resultado, get_text(interface_monitor->loc, "monitoring_started"));
        } else if (obs != nullptr) {
            set_label_text(rotulo_resultado, get_text(interface_monitor->loc, "monitoring_stopped"));
        } else {
            set_label_text(rotulo_resultado, get_text(interface_monitor->loc, "select_to_start"));
        }

        interface_monitor->atualizar_status();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar rótulos", true);
    }
}

void GerenciadorMensagens::notificar_erro(InterfaceMonitor* interface_monitor, const QString& mensagem) {
    try {
        source::utils::LogManager::get_logger().error(QString("Notificando erro: %1").arg(mensagem));
        if (interface_monitor != nullptr) {
            QObject* rotulo_resultado = interface_monitor->property("rotulo_resultado").value<QObject*>();
            set_label_text(rotulo_resultado, QString("Erro: %1").arg(mensagem));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao notificar erro", true);
    }
}

void GerenciadorMensagens::notificar_sucesso(InterfaceMonitor* interface_monitor, const QString& mensagem) {
    try {
        source::utils::LogManager::get_logger().info(QString("Notificando sucesso: %1").arg(mensagem));
        if (interface_monitor != nullptr) {
            QObject* rotulo_resultado = interface_monitor->property("rotulo_resultado").value<QObject*>();
            set_label_text(rotulo_resultado, mensagem);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao notificar sucesso", true);
    }
}

}  // namespace source::gui
