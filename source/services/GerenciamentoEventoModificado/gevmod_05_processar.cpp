#include "gevmod_05_processar.h"

#include <algorithm>

#include <QFileInfo>

#include "gevmod_06_processar_massivo.h"
#include "gevmod_07_processar_normal.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

bool processar(source::services::EventoModificado* self, const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) {
    try {
        if (!self) return false;

        if (!QFileInfo::exists(caminho_completo)) {
            return false;
        }

        if (nome_arquivo.startsWith("~$") || nome_arquivo.startsWith('.')) {
            return false;
        }

        if (self->arquivos_em_processamento.contains(caminho_completo)) {
            return false;
        }

        const QString ext = QFileInfo(nome_arquivo).suffix().isEmpty()
                                ? QString()
                                : QStringLiteral(".") + QFileInfo(nome_arquivo).suffix().toLower();

        if (self->EXTENSOES_VIDEO.contains(ext) && self->observador && self->observador->arquivos_recem_adicionados.contains(nome_arquivo)) {
            const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
            try {
                const qint64 tamanho = QFileInfo(caminho_completo).size();
                const double tempo_espera_video = std::min(120.0, std::max(10.0, static_cast<double>(tamanho) / (10.0 * 1024.0 * 1024.0)));

                if ((tempo_atual - tempo_adicao) < tempo_espera_video) {
                    return false;
                }

                self->observador->arquivos_recem_adicionados.remove(nome_arquivo);
            } catch (...) {
                if ((tempo_atual - tempo_adicao) < 30.0) {
                    return false;
                }
                self->observador->arquivos_recem_adicionados.remove(nome_arquivo);
            }
        }

        if (self->observador && self->observador->arquivos_recem_renomeados.contains(nome_arquivo)) {
            const double tempo_renomeacao = self->observador->arquivos_recem_renomeados.value(nome_arquivo);
            const double tempo_espera = self->observador->operacao_massiva_detectada ? 2.0 : 1.0;
            if ((tempo_atual - tempo_renomeacao) < tempo_espera) {
                return false;
            }
            self->observador->arquivos_recem_renomeados.remove(nome_arquivo);
        }

        if (self->observador && self->observador->arquivos_recem_adicionados.contains(nome_arquivo) && !self->EXTENSOES_VIDEO.contains(ext)) {
            const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
            if (self->observador->operacao_massiva_detectada && (tempo_atual - tempo_adicao) < 1.5) {
                return false;
            }
        }

        if (self->observador && self->observador->operacao_massiva_detectada) {
            return processar_massivo(self, nome_arquivo, caminho_completo, tempo_atual);
        }

        return processar_normal(self, nome_arquivo, caminho_completo, tempo_atual);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao processar arquivo", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
