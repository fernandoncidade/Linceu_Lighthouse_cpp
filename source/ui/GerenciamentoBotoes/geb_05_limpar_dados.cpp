#include "geb_05_limpar_dados.h"

#include <exception>

#include <QMetaObject>
#include <QVariant>
#include <QVariantMap>
#include <Qt>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_01_GerenciadorBotoes.h"
#include "source/utils/LogManager.h"

namespace {
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

void limpar_dados(source::ui::GerenciadorBotoes* self) {
    try {
        if (self == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        if (interface_obj == nullptr) {
            return;
        }

        QObject* observador_qt = interface_obj->property("observador").value<QObject*>();
        if (observador_qt != nullptr) {
            QMetaObject::invokeMethod(observador_qt, "limpar_estado");
        } else {
            source::services::Observador* observador_cpp = obter_observador_cpp(interface_obj);
            if (observador_cpp != nullptr) {
                observador_cpp->limpar_estado();
            }
        }

        if (interface_obj->property("excluidos_recentemente").isValid()) {
            interface_obj->setProperty("excluidos_recentemente", QVariantMap());
        } else {
            QObject* excluidos_obj = interface_obj->property("excluidos_recentemente_obj").value<QObject*>();
            if (excluidos_obj != nullptr) {
                QMetaObject::invokeMethod(excluidos_obj, "clear");
            }
        }

        QObject* gerenciador_colunas_obj = interface_obj->property("gerenciador_colunas").value<QObject*>();
        if (gerenciador_colunas_obj != nullptr) {
            if (auto* gerenciador_colunas =
                    qobject_cast<source::services::GerenciadorColunas*>(gerenciador_colunas_obj)) {
                gerenciador_colunas->cache_metadados.clear();
            } else {
                QMetaObject::invokeMethod(gerenciador_colunas_obj, "limpar_cache_metadados");
            }
        }

        QMetaObject::invokeMethod(interface_obj, "atualizar_status", Qt::QueuedConnection);
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao limpar dados no gerenciador: %1").arg(QString::fromUtf8(e.what())), true);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar dados no gerenciador", true);
    }
}

}  // namespace source::ui::GerenciamentoBotoes
