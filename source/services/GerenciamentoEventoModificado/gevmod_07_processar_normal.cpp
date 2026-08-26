#include "gevmod_07_processar_normal.h"

#include <algorithm>

#include <QFileInfo>

#include "gevmod_01_calcular_intervalo.h"
#include "gevmod_03_is_arquivo_codigo_grande.h"
#include "gevmod_04_limpar_cache_metadados.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

bool processar_normal(source::services::EventoModificado* self, const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual) {
    try {
        if (!self || !self->observador) return false;

        const QString ext = QFileInfo(nome_arquivo).suffix().isEmpty()
                                ? QString()
                                : QStringLiteral(".") + QFileInfo(nome_arquivo).suffix().toLower();

        if (self->observador->arquivos_recem_renomeados.contains(nome_arquivo)) {
            const double tempo_renomeacao = self->observador->arquivos_recem_renomeados.value(nome_arquivo);
            try {
                const qint64 tamanho = QFileInfo(caminho_completo).size();
                const double tempo_espera = self->EXTENSOES_VIDEO.contains(ext)
                                                ? std::min(60.0, std::max(5.0, static_cast<double>(tamanho) / (20.0 * 1024.0 * 1024.0)))
                                                : self->tempo_espera_grande_arquivo;
                if ((tempo_atual - tempo_renomeacao) < tempo_espera) {
                    return false;
                }
                self->observador->arquivos_recem_renomeados.remove(nome_arquivo);
            } catch (...) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro ao processar arquivo renomeado: %1").arg(caminho_completo), true);
                self->observador->arquivos_recem_renomeados.remove(nome_arquivo);
            }
        }

        const qint64 tamanho = QFileInfo(caminho_completo).size();

        if (ext == QStringLiteral(".log")) {
            if (self->observador->arquivos_recem_adicionados.contains(nome_arquivo)) {
                const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
                if ((tempo_atual - tempo_adicao) < 2.0) {
                    return false;
                }
            }

            const double intervalo_log = std::min(self->min_intervalo, 0.5);
            if (self->observador->ultima_modificacao.contains(nome_arquivo)) {
                const double ultimo_tempo = self->observador->ultima_modificacao.value(nome_arquivo);
                if ((tempo_atual - ultimo_tempo) < intervalo_log) {
                    return false;
                }
            }

            self->modificacoes_recentes[nome_arquivo] = tempo_atual;
            self->count_modificacoes[nome_arquivo] = self->count_modificacoes.value(nome_arquivo, 0) + 1;
            self->observador->ultima_modificacao[nome_arquivo] = tempo_atual;
            limpar_cache_metadados(self, caminho_completo, ext);
            self->notificar_evento(QStringLiteral("op_modified"), nome_arquivo, caminho_completo, caminho_completo);
            return true;
        }

        if (self->EXTENSOES_VIDEO.contains(ext) && self->observador->arquivos_recem_adicionados.contains(nome_arquivo)) {
            const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
            const double tempo_espera_video = std::min(120.0, std::max(10.0, static_cast<double>(tamanho) / (10.0 * 1024.0 * 1024.0)));
            if ((tempo_atual - tempo_adicao) < tempo_espera_video) {
                return false;
            }
        }

        bool codigo_grande = is_arquivo_codigo_grande(self, caminho_completo);
        double intervalo = 0.0;

        if (codigo_grande) {
            self->arquivos_em_processamento.insert(caminho_completo);
            intervalo = std::max(self->max_intervalo, self->tempo_espera_grande_arquivo * 2.0);
            if (self->observador->ultima_modificacao.contains(nome_arquivo)) {
                const double ultimo_tempo = self->observador->ultima_modificacao.value(nome_arquivo);
                if ((tempo_atual - ultimo_tempo) < intervalo) {
                    return false;
                }
            }
        } else {
            if (tamanho > self->grande_arquivo_threshold) {
                if (self->observador->arquivos_recem_adicionados.contains(nome_arquivo)) {
                    const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
                    const double tempo_extra = self->EXTENSOES_VIDEO.contains(ext)
                                                   ? std::min(120.0, std::max(10.0, static_cast<double>(tamanho) / (10.0 * 1024.0 * 1024.0)))
                                                   : self->tempo_espera_grande_arquivo;
                    if ((tempo_atual - tempo_adicao) < tempo_extra) {
                        return false;
                    }
                }

                if (self->eventos_grandes.contains(nome_arquivo)) {
                    const double ultimo_evento = self->eventos_grandes.value(nome_arquivo).toDouble();
                    if ((tempo_atual - ultimo_evento) < self->tempo_espera_grande_arquivo) {
                        return false;
                    }
                }

                self->eventos_grandes[nome_arquivo] = tempo_atual;
            }

            intervalo = calcular_intervalo(self, caminho_completo);
        }

        if (self->observador->arquivos_recem_excluidos.contains(nome_arquivo)) {
            const double ultima_exclusao = self->observador->arquivos_recem_excluidos.value(nome_arquivo);
            if ((tempo_atual - ultima_exclusao) < self->exclusao_threshold) {
                return false;
            }
        }

        if (self->observador->arquivos_recem_adicionados.contains(nome_arquivo) && !self->EXTENSOES_VIDEO.contains(ext)) {
            const double tempo_adicao = self->observador->arquivos_recem_adicionados.value(nome_arquivo);
            if ((tempo_atual - tempo_adicao) < intervalo) {
                return false;
            }
        }

        if (!codigo_grande && self->observador->ultima_modificacao.contains(nome_arquivo)) {
            const double ultimo_tempo = self->observador->ultima_modificacao.value(nome_arquivo);
            if ((tempo_atual - ultimo_tempo) < intervalo) {
                return false;
            }
        }

        self->modificacoes_recentes[nome_arquivo] = tempo_atual;
        self->count_modificacoes[nome_arquivo] = self->count_modificacoes.value(nome_arquivo, 0) + 1;
        self->observador->ultima_modificacao[nome_arquivo] = tempo_atual;
        limpar_cache_metadados(self, caminho_completo, ext);

        self->notificar_evento(QStringLiteral("op_modified"), nome_arquivo, caminho_completo, caminho_completo);

        if (codigo_grande) {
            self->arquivos_em_processamento.remove(caminho_completo);
        }

        return true;
    } catch (...) {
        if (self) {
            self->arquivos_em_processamento.remove(caminho_completo);
        }
        source::utils::LogManager::get_logger().error("Erro ao processar modificação", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
