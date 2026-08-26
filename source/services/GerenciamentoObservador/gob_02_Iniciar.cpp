#include "gob_02_Iniciar.h"

#include <QObject>
#include <QThread>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void iniciar(source::services::Observador* self) {
    try {
        if (!self) return;
        std::lock_guard<std::mutex> guard(self->lock);
        if (self->ativo) return;

        self->ativo = true;
        self->desligando = false;
        self->pausado = false;
        self->pause_event_set = true;
        if (self->interface != nullptr) {
            self->interface->setProperty("_aceitar_eventos_monitoramento", true);
        }

        auto* scanner = new source::services::DiretorioScanner(self);
        scanner->pausado = false;
        scanner->pause_event_set = true;
        self->scanner = scanner;

        auto* thread_scan = new QThread();
        auto* worker = new source::services::ScanWorker(scanner, self->diretorio);
        worker->moveToThread(thread_scan);

        QObject::connect(thread_scan, &QThread::started, worker, &source::services::ScanWorker::run);
        QObject::connect(worker, &source::services::ScanWorker::finished, thread_scan, [self, thread_scan]() {
            if (self != nullptr && self->thread_scan == thread_scan && self->ativo.load() &&
                !self->desligando.load()) {
                self->iniciar_monitoramento();
            }
        });
        QObject::connect(worker, &source::services::ScanWorker::error, thread_scan, [self, thread_scan](const QString& erro) {
            if (self != nullptr && self->thread_scan == thread_scan && !self->desligando.load()) {
                self->handle_scan_error(erro);
            }
        });
        QObject::connect(worker, &source::services::ScanWorker::error, thread_scan, &QThread::quit);
        QObject::connect(worker, &source::services::ScanWorker::finished, thread_scan, &QThread::quit);
        QObject::connect(worker, &source::services::ScanWorker::error, worker, &QObject::deleteLater);
        QObject::connect(worker, &source::services::ScanWorker::finished, worker, &QObject::deleteLater);
        QObject::connect(thread_scan, &QThread::finished, thread_scan, [self, thread_scan, worker, scanner]() {
            if (self == nullptr) {
                return;
            }

            if (self->thread_scan == thread_scan) {
                self->thread_scan = nullptr;
            }
            if (self->scan_worker == worker) {
                self->scan_worker = nullptr;
            }
            if (self->scanner == scanner) {
                self->scanner = nullptr;
            }
        });
        QObject::connect(thread_scan, &QThread::finished, scanner, &QObject::deleteLater);
        QObject::connect(thread_scan, &QThread::finished, thread_scan, &QObject::deleteLater);

        self->thread_scan = thread_scan;
        self->scan_worker = worker;
        thread_scan->start();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao iniciar Observador", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
