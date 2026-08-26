#pragma once

#include <functional>

#include <QObject>
#include <QMutex>
#include <QThreadPool>
#include <QTimer>
#include <QVariantMap>

namespace source::services {

class EventoRunnable : public QObject {
    Q_OBJECT

public:
    using ProcessFunc = std::function<QVariantMap(const QVariantMap&)>;

    explicit EventoRunnable(const QVariantMap& evento, ProcessFunc func_processar);
    void run();

signals:
    void concluido(const QVariantMap& resultado);

private:
    QVariantMap evento_;
    ProcessFunc func_processar_;
};

class EventoMovido : public QObject {
    Q_OBJECT

public:
    explicit EventoMovido(QObject* parent = nullptr);

    void processar_evento(const QVariantMap& evento);
    void adicionar_evento_para_lote(const QVariantMap& evento);
    void processar_lote();
    void shutdown();

signals:
    void evento_processado(const QVariantMap& evento);
    void eventos_em_lote_processados(const QList<QVariantMap>& eventos);

private:
    QList<QVariantMap> eventos_pendentes_;
    QMutex eventos_lock_;
    QTimer processamento_timer_;
    QThreadPool thread_pool_;
};

class EventoBuffer : public QObject {
    Q_OBJECT

public:
    explicit EventoBuffer(QObject* interface_monitor, QObject* parent = nullptr);

    void adicionar_evento(const QVariantMap& evento);
    void adicionar_eventos_lote(const QList<QVariantMap>& eventos);
    void _adicionar_evento_lote(const QVariantMap& evento);
    void _processar_evento_imediato(const QVariantMap& evento);
    void shutdown();

public slots:
    void atualizar_interface();

private:
    QObject* interface_;
    QMutex lock_;
    qint64 ultimo_update_;
    QTimer timer_;
    QList<QVariantMap> eventos_pendentes_;
};

QVariant verificar_movimentacao(QObject* self, const QVariant& evento = QVariant());
void _remover_exclusao(QObject* self, const QString& nome = QString(), const QString& dir_anterior = QString(), const QString& exclusao_id = QString());
void _atualizar_linha_recente(QObject* self);
void _atualizar_tabela_completa(QObject* self);
void _inicializar_sistema_evento(QObject* self);
void _processar_exclusoes_pendentes(QObject* self);
void _adicionar_item_tabela(QObject* self, const QVariantMap& evento = QVariantMap(), bool atualizar_interface = true);
void adicionar_evento(QObject* self, const QVariant& evento = QVariant());
void parar_sistema_evento(QObject* self, int timeout_ms = 5000);

}  // namespace source::services
