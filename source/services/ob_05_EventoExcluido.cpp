#include "ob_05_EventoExcluido.h"

#include "ob_01_Observador.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::services {

EventoExcluido::EventoExcluido(source::services::Observador* observador)
    : BaseEvento(observador) {}

bool EventoExcluido::processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) {
    Q_UNUSED(tempo_atual)
    if (!observador) return false;

    observador->arquivos_recem_excluidos[nome_arquivo] = tempo_atual;

    if (observador->registros_anteriores.contains(nome_arquivo)) {
        observador->registros_anteriores.remove(nome_arquivo);
    }

    notificar_evento(QStringLiteral("op_deleted"), nome_arquivo, caminho_completo, QString());
    return true;
}

}  // namespace source::services
