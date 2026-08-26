#pragma once

#include <QObject>
#include <QTimer>

namespace source::services::GerenciamentoTabela {

class MonitorTemaWindows : public QObject {
    Q_OBJECT
public:
    explicit MonitorTemaWindows(QObject* parent = nullptr);
    QString obter_tema_atual() const;
    void iniciar_monitoramento();
    void parar_monitoramento();

signals:
    void tema_alterado(const QString& tema);

private slots:
    void verificar_mudanca_tema();

private:
    QString detectar_tema_atual() const;
    QString tema_atual_;
    bool monitorando_;
    QTimer timer_verificacao_;
};

}  // namespace source::services::GerenciamentoTabela
