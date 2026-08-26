#include "ob_01_Observador.h"

#include <chrono>

#include "GerenciamentoObservador/gob_01_DetectarOperacaoMassiva.h"
#include "GerenciamentoObservador/gob_02_Iniciar.h"
#include "GerenciamentoObservador/gob_03_HandleScanError.h"
#include "GerenciamentoObservador/gob_04_IniciarMonitoramento.h"
#include "GerenciamentoObservador/gob_05_ProcessarBufferEventos.h"
#include "GerenciamentoObservador/gob_06_PararScan.h"
#include "GerenciamentoObservador/gob_07_Parar.h"
#include "GerenciamentoObservador/gob_08_LimparEstado.h"
#include "GerenciamentoObservador/gob_09_Monitorar.h"
#include "GerenciamentoObservador/gob_10_ProcessarEventoInterno.h"
#include "GerenciamentoObservador/gob_11_ProcessarEvento.h"
#include "GerenciamentoObservador/gob_12_ReiniciarMonitoramento.h"
#include "GerenciamentoObservador/gob_13_PausarMonitoramentoEscaneamento.h"
#include "ob_02_BaseEvento.h"
#include "ob_04_EventoAdicionado.h"
#include "ob_05_EventoExcluido.h"
#include "ob_06_EventoModificado.h"
#include "ob_07_EventoRenomeado.h"
#include "ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services {

Observador::Observador(const QString& diretorio_, std::function<void(const QVariantMap&)> callback_)
    : diretorio(diretorio_),
      callback(std::move(callback_)),
      ativo(false),
      desligando(false),
      pausado(false),
      pause_event_set(true),
      loc(new source::ui::LocalizadorQt()),
      falhas_consecutivas(0),
      max_falhas(3),
      operacao_massiva_detectada(false),
      contador_eventos_segundo(0),
      ultimo_reset_contador(std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count()),
      max_threads(4),
      total_eventos_recebidos(0),
      total_eventos_processados(0),
      eventos_perdidos_estimados(0),
      interface(nullptr),
    scanner(nullptr),
      thread_scan(nullptr),
      scan_worker(nullptr),
    handle_dir(0),
      owns_loc_(true) {
    try {
        gerenciador_colunas = std::make_unique<source::services::GerenciadorColunas>(this);
        evento_base = std::make_unique<source::services::BaseEvento>(this);
        evento_adicionado = std::make_unique<source::services::EventoAdicionado>(this);
        evento_excluido = std::make_unique<source::services::EventoExcluido>(this);
        evento_modificado = std::make_unique<source::services::EventoModificado>(this);
        evento_renomeado = std::make_unique<source::services::EventoRenomeado>(this);

        ACOES = {
            {1, QStringLiteral("op_added")},
            {2, QStringLiteral("op_deleted")},
            {3, QStringLiteral("op_modified")},
            {4, QStringLiteral("op_renamed")},
            {5, QStringLiteral("op_renamed")},
        };
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar Observador", true);
    }
}

Observador::~Observador() {
    try {
        const bool possui_runtime_ativo =
            ativo.load() || thread_scan != nullptr || scanner != nullptr ||
            (thread != nullptr && thread->joinable()) ||
            (thread_buffer != nullptr && thread_buffer->joinable()) ||
            handle_dir != 0;

        if (possui_runtime_ativo) {
            GerenciamentoObservador::parar(this);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao destruir Observador", true);
    }

    if (owns_loc_ && loc != nullptr) {
        delete loc;
        loc = nullptr;
    }
}

bool Observador::detectar_operacao_massiva() { return GerenciamentoObservador::detectar_operacao_massiva(this); }
void Observador::iniciar() { GerenciamentoObservador::iniciar(this); }
void Observador::handle_scan_error(const QString& erro) { GerenciamentoObservador::handle_scan_error(this, erro); }
void Observador::iniciar_monitoramento() { GerenciamentoObservador::iniciar_monitoramento(this); }
void Observador::processar_buffer_eventos() { GerenciamentoObservador::processar_buffer_eventos(this); }
void Observador::parar_scan() { GerenciamentoObservador::parar_scan(this); }
void Observador::parar() { GerenciamentoObservador::parar(this); }
void Observador::limpar_estado() { GerenciamentoObservador::limpar_estado(this); }
void Observador::monitorar() { GerenciamentoObservador::monitorar(this); }
void Observador::_processar_evento_interno(const QVariantMap& evento) { GerenciamentoObservador::_processar_evento_interno(this, evento); }
void Observador::processar_evento(const QVariantMap& evento) { GerenciamentoObservador::processar_evento(this, evento); }
void Observador::reiniciar_monitoramento() { GerenciamentoObservador::reiniciar_monitoramento(this); }
void Observador::pausar_monitoramento_ou_escaneamento() { GerenciamentoObservador::pausar_monitoramento_ou_escaneamento(this); }

}  // namespace source::services
