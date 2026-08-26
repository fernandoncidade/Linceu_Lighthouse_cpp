#pragma once

#include <atomic>

#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <QVariantMap>

namespace source::services::GerenciamentoEventoMovido {

class MovimentacaoWorker : public QObject {
    Q_OBJECT

public:
    explicit MovimentacaoWorker(QObject* interface, QObject* parent = nullptr);
    void adicionar_evento(const QVariantMap& evento);
    void shutdown(int timeout_ms = 5000);
    void reactivate();

signals:
    void processamento_concluido();
    void atualizacao_progresso(int atual, int total);

private:
    void _processar_evento_individual(const QVariantMap& evento);
    void _concluir_processamento();

    QObject* interface_;
    QObject* evento_base_;
    QMutex mutex_;
    QMutex state_mutex_;
    QWaitCondition idle_cv_;
    std::atomic<bool> shutting_down_;
    std::atomic<int> active_tasks_;
};

}  // namespace source::services::GerenciamentoEventoMovido
