#include "gevmod_06_processar_massivo.h"

#include <QFileInfo>

#include "gevmod_04_limpar_cache_metadados.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

bool processar_massivo(source::services::EventoModificado* self, const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) {
    try {
        if (!self || !self->observador) return false;

        if (self->observador->ultima_modificacao.contains(nome_arquivo)) {
            const double ultimo_tempo = self->observador->ultima_modificacao.value(nome_arquivo);
            if ((tempo_atual - ultimo_tempo) < self->min_intervalo) {
                return false;
            }
        }

        self->modificacoes_recentes[nome_arquivo] = tempo_atual;
        self->count_modificacoes[nome_arquivo] = self->count_modificacoes.value(nome_arquivo, 0) + 1;
        self->observador->ultima_modificacao[nome_arquivo] = tempo_atual;

        const QString ext = QFileInfo(nome_arquivo).suffix().isEmpty()
                                ? QString()
                                : QStringLiteral(".") + QFileInfo(nome_arquivo).suffix().toLower();
        limpar_cache_metadados(self, caminho_completo, ext);

        self->notificar_evento(QStringLiteral("op_modified"), nome_arquivo, caminho_completo, caminho_completo);
        return true;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar modificação massiva", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
