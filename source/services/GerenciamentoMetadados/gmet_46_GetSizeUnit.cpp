#include "gmet_46_GetSizeUnit.h"

#include "gmet_47_GetTamanhoBytes.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_size_unit(source::services::GerenciadorColunas* gc, const QVariantMap& item, const QString& unidade) {
    try {
        const double tamanho_bytes = static_cast<double>(get_tamanho_bytes(gc, item));
        if (unidade == QStringLiteral("B")) return QString::number(static_cast<qint64>(tamanho_bytes));
        if (unidade == QStringLiteral("KB")) return QString::number(tamanho_bytes / 1024.0, 'f', 2);
        if (unidade == QStringLiteral("MB")) return QString::number(tamanho_bytes / (1024.0 * 1024.0), 'f', 2);
        if (unidade == QStringLiteral("GB")) return QString::number(tamanho_bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2);
        if (unidade == QStringLiteral("TB")) return QString::number(tamanho_bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2);
        return QStringLiteral("0");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter unidade de tamanho", true);
        return QStringLiteral("0");
    }
}

}  // namespace source::services::GerenciamentoMetadados
