#include "gob_13_PausarMonitoramentoEscaneamento.h"

#include <QMetaObject>
#include <QThread>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_03_DiretorioScanner.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoObservador {

void pausar_monitoramento_ou_escaneamento(source::services::Observador* self) {
    try {
        if (!self) return;
        self->pausado = !self->pausado.load();
        self->pause_event_set = !self->pausado.load();

        auto* worker = self->scan_worker.data();
        if (worker && worker->scanner_obj()) {
            worker->scanner_obj()->pausado.store(self->pausado.load());
            worker->scanner_obj()->pause_event_set.store(!self->pausado.load());
        }

        auto* scanner = self->scanner.data();
        if (scanner) {
            scanner->pausado.store(self->pausado.load());
            scanner->pause_event_set.store(!self->pausado.load());
        }

        if (!self->interface || !self->loc) return;

        QObject* rotulo = self->interface->property("rotulo_resultado").value<QObject*>();
        QObject* barra = self->interface->property("barra_progresso").value<QObject*>();

        auto* thread_scan = self->thread_scan.data();
        const bool scan_running = thread_scan && thread_scan->isRunning();
        auto* scanner_ativo = scanner;
        if (!scanner_ativo && worker) scanner_ativo = worker->scanner_obj();

        if (scan_running && scanner_ativo) {
            const int progresso = static_cast<int>(scanner_ativo->ultimo_progresso);
            const int contador = scanner_ativo->contador_processados;
            const int total = scanner_ativo->total_arquivos;
            const QString prefixo = self->pausado ? self->loc->get_text("paused") : self->loc->get_text("scanning");
            const QString texto = QStringLiteral("%1 %2% (%3/%4)").arg(prefixo).arg(progresso).arg(contador).arg(total);

            if (rotulo) QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, texto));
            if (barra) {
                QMetaObject::invokeMethod(barra, "show", Qt::QueuedConnection);
                QMetaObject::invokeMethod(barra, "setValue", Qt::QueuedConnection, Q_ARG(int, progresso));
            }
            return;
        }

        if (self->pausado) {
            if (rotulo) QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, self->loc->get_text("monitoring_paused")));
            if (barra) {
                QMetaObject::invokeMethod(barra, "hide", Qt::QueuedConnection);
                QMetaObject::invokeMethod(barra, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
            }
        } else {
            if (rotulo) QMetaObject::invokeMethod(rotulo, "setText", Qt::QueuedConnection, Q_ARG(QString, self->loc->get_text("monitoring_started")));
            if (barra) {
                QMetaObject::invokeMethod(barra, "hide", Qt::QueuedConnection);
                QMetaObject::invokeMethod(barra, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface de pausa", true);
    }
}

}  // namespace source::services::GerenciamentoObservador
