#include "gmet_22_GetAtributosArquivo.h"

#include <QFileInfo>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_atributos_arquivo(const QVariantMap& item, source::ui::LocalizadorQt* loc) {
    try {
        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
        QFileInfo fi(caminho);
        if (!fi.exists()) return item.value("atributos").toString();

        QStringList attrs;
        if (fi.isDir()) attrs << (loc ? loc->get_text("folder") : QStringLiteral("folder"));
        else attrs << (loc ? loc->get_text("file") : QStringLiteral("file"));
        if (!fi.isWritable()) attrs << (loc ? loc->get_text("readonly") : QStringLiteral("readonly"));
        if (fi.isHidden()) attrs << (loc ? loc->get_text("hidden") : QStringLiteral("hidden"));
        const QString atributos = attrs.join(QStringLiteral(", "));
        if (!atributos.trimmed().isEmpty()) return atributos;
        return loc ? loc->get_text("file") : QStringLiteral("file");
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter atributos do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
