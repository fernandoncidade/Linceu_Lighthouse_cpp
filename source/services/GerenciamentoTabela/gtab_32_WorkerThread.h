#pragma once

#include <QThread>
#include <QVariant>
#include <functional>

namespace source::services::GerenciamentoTabela {

class WorkerThread : public QThread {
    Q_OBJECT
public:
    explicit WorkerThread(const QVariant& dados, std::function<QVariant(const QVariant&)> processamento_pesado, QObject* parent = nullptr);

signals:
    void finished(const QVariant& resultado);

protected:
    void run() override;

private:
    QVariant dados_;
    std::function<QVariant(const QVariant&)> processamento_pesado_;
};

}  // namespace source::services::GerenciamentoTabela
