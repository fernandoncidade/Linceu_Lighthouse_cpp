#include "gmet_16_ExtrairMetadadosOlefile.h"

#include <QFileInfo>

#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_olefile(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return m;
        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        if (ext == QStringLiteral(".doc")) {
            const int paginas = std::max(1, static_cast<int>(fi.size() / 20000));
            m.insert("paginas", QString::number(paginas));
            m.insert("paginas_estimadas", QString::number(paginas));
            m.insert("palavras", QString::number(paginas * 300));
            m.insert("linhas", QString::number(paginas * 40));
        } else if (ext == QStringLiteral(".ppt")) {
            m.insert("slides", QString::number(std::max(1, static_cast<int>(fi.size() / 100000))));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados OLE", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
