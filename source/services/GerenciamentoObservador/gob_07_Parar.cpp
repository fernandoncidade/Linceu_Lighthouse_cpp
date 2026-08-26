#include "gob_07_Parar.h"

#include <QMetaObject>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#ifdef interface
#undef interface
#endif
#endif

#include "gob_06_PararScan.h"
#include "gob_09_Monitorar.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {
namespace {

void join_thread_safely(std::unique_ptr<std::thread>& worker, const QString& nome_thread) {
    if (!worker) {
        return;
    }

    try {
        if (worker->joinable()) {
            if (worker->get_id() == std::this_thread::get_id()) {
                source::utils::LogManager::get_logger().warning(
                    QStringLiteral("Observador tentou aguardar a propria thread (%1); aplicando detach de seguranca.")
                        .arg(nome_thread));
                worker->detach();
            } else {
                worker->join();
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QStringLiteral("Erro ao aguardar thread do observador: %1").arg(nome_thread), true);
    }

    worker.reset();
}

}  // namespace

void parar(source::services::Observador* self) {
    try {
        if (!self) return;
        bool precisava_limpar_estado = false;
        {
            std::lock_guard<std::mutex> guard(self->lock);
            precisava_limpar_estado = self->ativo.load() || (self->thread && self->thread->joinable()) ||
                                      (self->thread_buffer && self->thread_buffer->joinable()) ||
                                      self->thread_scan != nullptr || self->scanner != nullptr ||
                                      self->handle_dir != 0;

            self->desligando = true;
            self->ativo = false;
            self->pausado = false;
            self->pause_event_set = true;
            if (self->interface != nullptr) {
                self->interface->setProperty("_aceitar_eventos_monitoramento", false);
            }
        }

        parar_scan(self);

    #ifdef _WIN32
        HANDLE handle = reinterpret_cast<HANDLE>(self->handle_dir);
        if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
            CancelIoEx(handle, nullptr);
        }
    #endif

        join_thread_safely(self->thread, QStringLiteral("monitor"));
        join_thread_safely(self->thread_buffer, QStringLiteral("buffer"));
        close_handle_dir_safely(self);

        if (precisava_limpar_estado) {
            self->limpar_estado();
        }
        self->desligando = false;

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
        source::utils::LogManager::get_logger().error("Erro ao parar Observador", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
