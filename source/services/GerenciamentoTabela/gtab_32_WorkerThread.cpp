#include "gtab_32_WorkerThread.h"

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

WorkerThread::WorkerThread(const QVariant& dados, std::function<QVariant(const QVariant&)> processamento_pesado, QObject* parent)
    : QThread(parent), dados_(dados), processamento_pesado_(std::move(processamento_pesado)) {}

void WorkerThread::run() {
    try {
        QVariant resultado;
        if (processamento_pesado_) {
            resultado = processamento_pesado_(dados_);
        }
        emit finished(resultado);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao executar WorkerThread", true);
    }
}

}  // namespace source::services::GerenciamentoTabela
