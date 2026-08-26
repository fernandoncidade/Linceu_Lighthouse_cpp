#include "gmet_21_GetFormataTamanho.h"

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString formata_tamanho(double tamanho) {
    try {
        if (tamanho < 0.0) return QStringLiteral("0 B");
        if (tamanho < 1024.0) return QString::number(tamanho, 'f', 0) + QStringLiteral(" B");
        if (tamanho < 1024.0 * 1024.0) return QString::number(tamanho / 1024.0, 'f', 2) + QStringLiteral(" KB");
        if (tamanho < 1024.0 * 1024.0 * 1024.0) return QString::number(tamanho / (1024.0 * 1024.0), 'f', 2) + QStringLiteral(" MB");
        if (tamanho < 1024.0 * 1024.0 * 1024.0 * 1024.0) return QString::number(tamanho / (1024.0 * 1024.0 * 1024.0), 'f', 2) + QStringLiteral(" GB");
        return QString::number(tamanho / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2) + QStringLiteral(" TB");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao formatar tamanho", true);
        return QStringLiteral("0 B");
    }
}

QString get_formata_tamanho(double tamanho) {
    try {
        return formata_tamanho(tamanho);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter tamanho formatado", true);
        return QStringLiteral("0 B");
    }
}

}  // namespace source::services::GerenciamentoMetadados
