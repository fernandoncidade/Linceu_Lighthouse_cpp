#pragma once

#include <atomic>
#include <memory>

#include <QObject>
#include <QMutex>
#include <QPointer>
#include <QQueue>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace source::services {
class Observador;

class DiretorioScanner : public QObject {
    Q_OBJECT

public:
    explicit DiretorioScanner(source::services::Observador* observador);

    void pausar();

    void _process_batch(const QList<QVariantList>& batch = {});
    void scan_directory(const QString& directory = QString());
    void _processar_fila();
    QString get_file_type(const QString& caminho = QString());
    void _processar_item(const QString& nome = QString(), const QString& caminho = QString(), const QString& tipo = QString());
    void _atualizar_progresso();
    void _atualizar_interface(int progresso, int contador, int total);
    void _finalizar_scan();

signals:
    void progresso_atualizado(int progresso, int contador, int total);
    void scan_finalizado();

public:
    source::services::Observador* observador;
    QString db_path;
    QObject* gerenciador_colunas;
    QVariantMap current_item;
    QQueue<QVariantList> fila_processamento;
    int contador_processados;
    int total_arquivos;
    QMutex lock_db;
    double ultimo_progresso;
    qint64 ultimo_update_ui_ms;
    int intervalo_atualizacao;
    std::atomic<bool> pausado;
    std::atomic<bool> pause_event_set;
};

class ScanWorker : public QObject {
    Q_OBJECT

public:
    explicit ScanWorker(source::services::DiretorioScanner* scanner, const QString& directory);

    void run();
    source::services::DiretorioScanner* scanner_obj() const;
    QString directory() const;

signals:
    void finished();
    void error(const QString& mensagem);

private:
    QPointer<source::services::DiretorioScanner> scanner_;
    QString directory_;
};

}  // namespace source::services
