#include "gevmov_05_atualizar_tabela_completa.h"

#include <QMetaObject>
#include <QTableView>
#include <QTableWidget>
#include <QWidget>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

void atualizar_tabela_completa(QObject* interfaceMonitor) {
    try {
        if (!interfaceMonitor) return;

        QObject* gerenciador_tabela = interfaceMonitor->property("gerenciador_tabela").value<QObject*>();
        QObject* tabelaObj = interfaceMonitor->property("tabela_dados").value<QObject*>();
        if (!gerenciador_tabela || !tabelaObj) return;

        bool sorting_enabled = false;
        QMetaObject::invokeMethod(tabelaObj, "isSortingEnabled", Qt::DirectConnection,
                                  Q_RETURN_ARG(bool, sorting_enabled));
        QMetaObject::invokeMethod(tabelaObj, "setSortingEnabled", Qt::DirectConnection,
                                  Q_ARG(bool, false));

        QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_dados_tabela", Qt::DirectConnection,
                                  Q_ARG(QObject*, tabelaObj),
                                  Q_ARG(int, -1));
        QMetaObject::invokeMethod(interfaceMonitor, "atualizar_status", Qt::QueuedConnection);
        QMetaObject::invokeMethod(tabelaObj, "setSortingEnabled", Qt::DirectConnection,
                                  Q_ARG(bool, sorting_enabled));

        if (auto* tw = qobject_cast<QTableWidget*>(tabelaObj)) {
            if (tw->viewport()) tw->viewport()->update();
        } else if (auto* tv = qobject_cast<QTableView*>(tabelaObj)) {
            if (tv->viewport()) tv->viewport()->update();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar tabela completa", true);
    }
}

}  // namespace source::services::GerenciamentoEventoMovido
