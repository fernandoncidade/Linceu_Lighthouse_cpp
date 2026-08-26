#pragma once

#include <atomic>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include <QHash>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QVariant>
#include <QVariantMap>

class QThread;

namespace source::ui {
class LocalizadorQt;
}

namespace source::services {
class BaseEvento;
class DiretorioScanner;
class EventoAdicionado;
class EventoExcluido;
class EventoModificado;
class EventoRenomeado;
class GerenciadorColunas;
class ScanWorker;

class Observador {
public:
    explicit Observador(const QString& diretorio, std::function<void(const QVariantMap&)> callback = {});
    ~Observador();

    bool detectar_operacao_massiva();
    void iniciar();
    void handle_scan_error(const QString& erro = QString());
    void iniciar_monitoramento();
    void processar_buffer_eventos();
    void parar_scan();
    void parar();
    void limpar_estado();
    void monitorar();
    void _processar_evento_interno(const QVariantMap& evento);
    void processar_evento(const QVariantMap& evento);
    void reiniciar_monitoramento();
    void pausar_monitoramento_ou_escaneamento();

    QString diretorio;
    std::function<void(const QVariantMap&)> callback;
    std::atomic<bool> ativo;
    std::atomic<bool> desligando;
    std::atomic<bool> pausado;
    std::atomic<bool> pause_event_set;

    QVariant ultimo_evento;
    QHash<QString, QVariant> eventos_pendentes;
    QHash<QString, QString> registros_anteriores;
    std::deque<std::pair<QString, QString>> renomeacoes_pendentes;
    QSet<QString> eventos_ignorados;
    QHash<QString, double> arquivos_recem_adicionados;
    QHash<QString, double> arquivos_recem_excluidos;
    QHash<QString, double> arquivos_recem_renomeados;
    QHash<QString, double> arquivos_recem_modificados;
    QHash<QString, double> ultima_modificacao;

    std::mutex lock;
    std::mutex evento_lock;
    std::mutex registros_lock;

    source::ui::LocalizadorQt* loc;
    int falhas_consecutivas;
    int max_falhas;
    QString ultimo_erro;
    std::deque<QVariantMap> buffer_eventos;
    bool operacao_massiva_detectada;
    int contador_eventos_segundo;
    double ultimo_reset_contador;
    int max_threads;
    qint64 total_eventos_recebidos;
    qint64 total_eventos_processados;
    qint64 eventos_perdidos_estimados;

    QObject* interface;

    std::unique_ptr<source::services::GerenciadorColunas> gerenciador_colunas;
    std::unique_ptr<source::services::BaseEvento> evento_base;
    std::unique_ptr<source::services::EventoAdicionado> evento_adicionado;
    std::unique_ptr<source::services::EventoExcluido> evento_excluido;
    std::unique_ptr<source::services::EventoModificado> evento_modificado;
    std::unique_ptr<source::services::EventoRenomeado> evento_renomeado;

    QHash<int, QString> ACOES;
    QPointer<source::services::DiretorioScanner> scanner;
    QPointer<QThread> thread_scan;
    QPointer<source::services::ScanWorker> scan_worker;
    std::unique_ptr<std::thread> thread;
    std::unique_ptr<std::thread> thread_buffer;
    quintptr handle_dir;

private:
    bool owns_loc_;
};

}  // namespace source::services
