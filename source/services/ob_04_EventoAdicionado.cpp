#include "ob_04_EventoAdicionado.h"

#include "ob_01_Observador.h"
#include "source/ui/ui_12_LocalizadorQt.h"

namespace source::services {

EventoAdicionado::EventoAdicionado(source::services::Observador* observador)
    : BaseEvento(observador) {}

bool EventoAdicionado::processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) {
    if (!observador) return false;

    if (observador->registros_anteriores.contains(nome_arquivo)) {
        return false;
    }

    observador->arquivos_recem_adicionados[nome_arquivo] = tempo_atual;
    notificar_evento(QStringLiteral("op_added"), nome_arquivo, QString(), caminho_completo);
    return true;
}

}  // namespace source::services
