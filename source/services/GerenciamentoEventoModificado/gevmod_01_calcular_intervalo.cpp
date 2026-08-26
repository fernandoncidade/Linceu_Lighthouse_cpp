#include "gevmod_01_calcular_intervalo.h"

#include <algorithm>

#include <QFileInfo>

#include "gevmod_02_calcular_intervalo_original.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

double calcular_intervalo(source::services::EventoModificado* self, const QString& caminho_completo) {
    try {
        if (!self) return 0.01;
        if (!QFileInfo::exists(caminho_completo)) return self->min_intervalo;

        const QString ext = QFileInfo(caminho_completo).suffix().isEmpty()
                                ? QString()
                                : QStringLiteral(".") + QFileInfo(caminho_completo).suffix().toLower();

        if (self->EXTENSOES_RAPIDAS.contains(ext)) {
            return self->min_intervalo;
        }

        if (self->observador && self->observador->operacao_massiva_detectada) {
            return self->min_intervalo;
        }

        const qint64 tamanho = QFileInfo(caminho_completo).size();
        if (tamanho > self->grande_arquivo_threshold) {
            return std::min(self->tempo_espera_grande_arquivo, 1.0);
        }

        return calcular_intervalo_original(self, caminho_completo);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao calcular intervalo", true);
        return self ? self->min_intervalo : 0.01;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
