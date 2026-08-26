#include "gevmov_04_atualizar_linha_recente.h"

#include <QMetaObject>
#include <QVariant>

#include "gevmov_05_atualizar_tabela_completa.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoMovido {

void atualizar_linha_recente(QObject* interfaceMonitor) {
    try {
        if (!interfaceMonitor) return;

        QObject* gerenciador_tabela = interfaceMonitor->property("gerenciador_tabela").value<QObject*>();
        QObject* tabela_dados = interfaceMonitor->property("tabela_dados").value<QObject*>();

        if (gerenciador_tabela && tabela_dados) {
            QMetaObject::invokeMethod(gerenciador_tabela, "atualizar_linha_mais_recente", Qt::DirectConnection,
                                      Q_ARG(QObject*, tabela_dados));
            QMetaObject::invokeMethod(interfaceMonitor, "atualizar_status", Qt::QueuedConnection);
            return;
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar linha recente", true);
    }

    atualizar_tabela_completa(interfaceMonitor);
}

}  // namespace source::services::GerenciamentoEventoMovido
