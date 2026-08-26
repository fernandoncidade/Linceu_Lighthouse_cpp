#include "gevmod_03_is_arquivo_codigo_grande.h"

#include <QFile>
#include <QFileInfo>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoEventoModificado {

bool is_arquivo_codigo_grande(source::services::EventoModificado* self, const QString& caminho) {
    try {
        if (!self) return false;

        const QString ext = QFileInfo(caminho).suffix().isEmpty()
                                ? QString()
                                : QStringLiteral(".") + QFileInfo(caminho).suffix().toLower();
        if (!self->EXTENSOES_CODIGO.contains(ext)) {
            return false;
        }

        if (self->observador && self->observador->operacao_massiva_detectada) {
            return false;
        }

        QFile arquivo(caminho);
        if (!arquivo.open(QIODevice::ReadOnly)) {
            source::utils::LogManager::get_logger().error(
                QStringLiteral("Erro ao verificar arquivo de código grande: falha ao abrir '%1'").arg(caminho),
                true);
            return false;
        }

        int linhas = 0;
        while (!arquivo.atEnd()) {
            arquivo.readLine();
            ++linhas;
            if (linhas > self->CODIGO_LINHA_THRESHOLD) {
                return true;
            }
        }

        return false;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao verificar arquivo de código grande", true);
        return false;
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
