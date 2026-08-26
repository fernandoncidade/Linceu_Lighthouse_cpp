#include "ob_03_DiretorioScanner.h"

#include <QMetaObject>

#include "ob_01_Observador.h"
#include "ob_02_BaseEvento.h"
#include "ob_09_GerenciadorColunas.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_01_process_batch.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_02_scan_directory.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_03_processar_fila.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_04_get_file_type.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_05_processar_item.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_06_atualizar_progresso.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_07_atualizar_interface.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_08_finalizar_scan.h"
#include "source/services/GerenciamentoDiretorioScanner/gscanner_09_scan_worker_run.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services {

DiretorioScanner::DiretorioScanner(source::services::Observador* observador_)
    : QObject(nullptr),
      observador(observador_),
      db_path(observador_ && observador_->evento_base ? observador_->evento_base->db_path : QString()),
      gerenciador_colunas(observador_ ? observador_->gerenciador_colunas.get() : nullptr),
      contador_processados(0),
      total_arquivos(0),
      ultimo_progresso(0.0),
      ultimo_update_ui_ms(0),
      intervalo_atualizacao(1),
      pausado(false),
      pause_event_set(true) {
    try {
        if (observador && observador->interface) {
            QMetaObject::invokeMethod(observador->interface, "criar_barra_progresso", Qt::QueuedConnection);
            QObject::connect(this, &DiretorioScanner::progresso_atualizado, this, &DiretorioScanner::_atualizar_interface);
            QObject::connect(this, &DiretorioScanner::scan_finalizado, this, &DiretorioScanner::_finalizar_scan);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar DiretorioScanner", true);
    }
}

void DiretorioScanner::pausar() {
    try {
        pausado = !pausado.load();
        pause_event_set = !pausado.load();

        if (observador && observador->interface) {
            const QString status = (observador->loc && pausado)
                                       ? observador->loc->get_text("paused")
                                       : (observador->loc ? observador->loc->get_text("resumed") : QStringLiteral("resumed"));
            QObject* rotulo_resultado = observador->interface->property("rotulo_resultado").value<QObject*>();
            if (rotulo_resultado) {
                QMetaObject::invokeMethod(rotulo_resultado, "setText", Qt::QueuedConnection, Q_ARG(QString, status));
            } else {
                QMetaObject::invokeMethod(observador->interface, "set_status_text", Qt::QueuedConnection,
                                          Q_ARG(QString, status));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao pausar/resumir scan", true);
    }
}

void DiretorioScanner::_process_batch(const QList<QVariantList>& batch) { GerenciamentoDiretorioScanner::_process_batch(this, batch); }
void DiretorioScanner::scan_directory(const QString& directory) { GerenciamentoDiretorioScanner::scan_directory(this, directory); }
void DiretorioScanner::_processar_fila() { GerenciamentoDiretorioScanner::_processar_fila(this); }
QString DiretorioScanner::get_file_type(const QString& caminho) { return GerenciamentoDiretorioScanner::get_file_type(this, caminho); }
void DiretorioScanner::_processar_item(const QString& nome, const QString& caminho, const QString& tipo) { GerenciamentoDiretorioScanner::_processar_item(this, nome, caminho, tipo); }
void DiretorioScanner::_atualizar_progresso() { GerenciamentoDiretorioScanner::_atualizar_progresso(this); }
void DiretorioScanner::_atualizar_interface(int progresso, int contador, int total) { GerenciamentoDiretorioScanner::_atualizar_interface(this, progresso, contador, total); }
void DiretorioScanner::_finalizar_scan() { GerenciamentoDiretorioScanner::_finalizar_scan(this); }

ScanWorker::ScanWorker(source::services::DiretorioScanner* scanner, const QString& directory)
    : QObject(nullptr), scanner_(scanner), directory_(directory) {}

void ScanWorker::run() { GerenciamentoDiretorioScanner::scan_worker_run(this); }
source::services::DiretorioScanner* ScanWorker::scanner_obj() const { return scanner_.data(); }
QString ScanWorker::directory() const { return directory_; }

}  // namespace source::services
