#include "ui_08_GerenciadorEventosArquivo.h"

#include <QMetaObject>

#include "source/utils/LogManager.h"

namespace source::services {
QVariant verificar_movimentacao(QObject* interface_obj, const QVariant& evento);
void adicionar_evento(QObject* interface_obj, const QVariant& evento);
}

namespace source::ui {

GerenciadorEventosArquivo::GerenciadorEventosArquivo(QObject* interface_principal) : interface_(interface_principal) {
    try {
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorEventosArquivo", true);
    }
}

QVariant GerenciadorEventosArquivo::verificar_movimentacao(const QVariant& evento) {
    try {
        return source::services::verificar_movimentacao(interface_, evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar movimentação", true);
        return QVariant();
    }
}

void GerenciadorEventosArquivo::adicionar_evento(const QVariant& evento) {
    try {
        source::services::adicionar_evento(interface_, evento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar evento", true);
    }
}

void GerenciadorEventosArquivo::limpar_eventos() {
    try {
        auto* evento_base = interface_->property("evento_base").value<QObject*>();
        if (evento_base != nullptr) {
            QMetaObject::invokeMethod(evento_base, "limpar_registros");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar eventos", true);
    }
}

}  // namespace source::ui
