#include "gevmod_02_calcular_intervalo_original.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <QFileInfo>

#include "source/services/ob_06_EventoModificado.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

double calcular_intervalo_original(source::services::EventoModificado* self, const QString& caminho_completo) {
    try {
        if (!self) return 5.0;
        if (!QFileInfo::exists(caminho_completo)) return self->max_intervalo;

        const double PESO_TAMANHO = 0.6;
        const double PESO_FREQUENCIA = 0.4;

        const qint64 tamanho = QFileInfo(caminho_completo).size();
        const double threshold = static_cast<double>(self->tamanho_threshold);
        if (threshold <= 0.0) {
            throw std::runtime_error("tamanho_threshold invalido");
        }

        double fator_tamanho;
        if (tamanho > self->grande_arquivo_threshold) {
            fator_tamanho = std::pow(static_cast<double>(tamanho) / threshold, 0.75);
        } else {
            fator_tamanho = std::pow(static_cast<double>(tamanho) / threshold, 0.5);
        }

        const QString nome_arquivo = QFileInfo(caminho_completo).fileName();
        const int freq_mod = self->count_modificacoes.value(nome_arquivo, 0);
        const double fator_freq = std::max(0.2, 1.0 / (1.0 + static_cast<double>(freq_mod) * 0.1));

        double intervalo = self->intervalo_base * (fator_tamanho * PESO_TAMANHO + fator_freq * PESO_FREQUENCIA);
        if (tamanho > self->tamanho_threshold) {
            intervalo *= 1.5;
        }

        return std::min(std::max(intervalo, self->min_intervalo), self->max_intervalo);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao calcular intervalo original", true);
        return self ? self->max_intervalo : 5.0;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
