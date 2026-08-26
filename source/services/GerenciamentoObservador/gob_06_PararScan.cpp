#include "gob_06_PararScan.h"

#include <QMetaObject>
#include <QThread>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void parar_scan(source::services::Observador* self) {
    try {
        if (!self) return;
        auto* thread_scan = self->thread_scan.data();
        auto* worker = self->scan_worker.data();
        auto* scanner = self->scanner.data();

        if (thread_scan && thread_scan->isRunning()) {
            self->desligando = true;
            self->ativo = false;
            self->pausado = false;
            self->pause_event_set = true;

            if (worker && worker->scanner_obj()) {
                worker->scanner_obj()->pausado = false;
                worker->scanner_obj()->pause_event_set = true;
            }

            thread_scan->requestInterruption();
            thread_scan->quit();

            int ciclos_espera = 0;
            while (thread_scan->isRunning()) {
                if (thread_scan->wait(250)) {
                    break;
                }

                ++ciclos_espera;
                if ((ciclos_espera % 20) == 0) {
                    source::utils::LogManager::get_logger().warning(
                        "Aguardando finalizacao segura do escaneamento antes de liberar observador");
                }
            }
        }

        if (worker != nullptr) {
            QObject::disconnect(worker, nullptr, nullptr, nullptr);
        }
        if (thread_scan != nullptr) {
            QObject::disconnect(thread_scan, nullptr, nullptr, nullptr);
        }

        self->thread_scan = nullptr;
        self->scan_worker = nullptr;
        self->scanner = nullptr;

        if (worker != nullptr) {
            worker->deleteLater();
        }
        if (scanner != nullptr) {
            scanner->deleteLater();
        }
        if (thread_scan != nullptr && !thread_scan->isRunning()) {
            thread_scan->deleteLater();
        }

        if (self->interface && self->loc) {
            QObject* rotulo = self->interface->property("rotulo_resultado").value<QObject*>();
            if (rotulo) {
                QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, self->loc->get_text("monitoring_stopped")));
            }

            QObject* barra = self->interface->property("barra_progresso").value<QObject*>();
            if (barra) {
                QMetaObject::invokeMethod(barra, "hide", Qt::QueuedConnection);
                QMetaObject::invokeMethod(barra, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao parar escaneamento", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
