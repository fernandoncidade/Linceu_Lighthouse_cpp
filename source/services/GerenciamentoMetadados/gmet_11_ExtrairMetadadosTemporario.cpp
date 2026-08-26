#include "gmet_11_ExtrairMetadadosTemporario.h"

#include <QDateTime>
#include <QFileInfo>

#include "gmet_21_GetFormataTamanho.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_temporario(const QString& caminho, source::ui::LocalizadorQt* loc) {
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return m;
        m.insert("tamanho", formata_tamanho(fi.size()));
        const qint64 horas = fi.lastModified().secsTo(QDateTime::currentDateTime()) / 3600;
        if (horas < 1) m.insert("idade", QStringLiteral("%1 %2").arg(fi.lastModified().secsTo(QDateTime::currentDateTime()) / 60).arg(loc ? loc->get_text("minutes") : QStringLiteral("minutes")));
        else if (horas < 24) m.insert("idade", QStringLiteral("%1 %2").arg(horas).arg(loc ? loc->get_text("hours") : QStringLiteral("hours")));
        else m.insert("idade", QStringLiteral("%1 %2").arg(horas / 24).arg(loc ? loc->get_text("days") : QStringLiteral("days")));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados do arquivo temporário", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
