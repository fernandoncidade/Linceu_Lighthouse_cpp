#include "gmet_23_GetAutorArquivo.h"

#include <QFileInfo>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QString get_autor_arquivo(const QVariantMap& item, source::ui::LocalizadorQt* loc) {
    try {
        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
        QString autor = item.value("autor").toString();

        if (!caminho.isEmpty() && QFileInfo::exists(caminho)) {
            QFileInfo fi(caminho);
            const QString owner = fi.owner();
            if (!owner.trimmed().isEmpty()) autor = owner.trimmed();
        }

        if (autor.trimmed().isEmpty()) autor = qEnvironmentVariable("USERNAME");
        if (autor.trimmed().isEmpty()) autor = qEnvironmentVariable("USER");
        if (autor.trimmed().isEmpty() && loc) autor = loc->get_text("unknown_author");

        if (loc) return loc->traduzir_metadados(autor);
        return autor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter autor do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
