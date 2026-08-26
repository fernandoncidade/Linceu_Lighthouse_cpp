#include "gadmfil_09_notificar_alteracao_idioma.h"

#include <QApplication>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QWidget>

#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace {
void safe_invoke(QObject* obj, const char* method) {
    if (!obj || !method) return;
    if (obj->metaObject()->indexOfMethod(
            QMetaObject::normalizedSignature((QString(method) + "(QVariantMap)").toUtf8().constData())) >=
        0) {
        QMetaObject::invokeMethod(obj, method, Qt::DirectConnection,
                                  Q_ARG(QVariantMap, QVariantMap{}));
        return;
    }
    if (obj->metaObject()->indexOfMethod(QMetaObject::normalizedSignature((QString(method) + "()").toUtf8().constData())) >= 0) {
        QMetaObject::invokeMethod(obj, method, Qt::DirectConnection);
        return;
    }
    if (obj->metaObject()->indexOfMethod(QMetaObject::normalizedSignature((QString(method) + "(QString)").toUtf8().constData())) >= 0) {
        QMetaObject::invokeMethod(obj, method, Qt::DirectConnection, Q_ARG(QString, QString()));
        return;
    }
}
}

namespace source::tools::GerenciamentoAdministradorFiltros {

void notificar_alteracao_idioma(source::tools::AdministradorFiltros* self) {
    try {
        Q_UNUSED(self)
        for (QWidget* widget : QApplication::topLevelWidgets()) {
            if (!widget) continue;
            safe_invoke(widget, "atualizar_status");
            safe_invoke(widget, "atualizar_interface");

            QObject* geui = widget->property("gerenciador_eventos_ui").value<QObject*>();
            if (geui) safe_invoke(geui, "atualizar_interface");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao notificar alteração de idioma", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
