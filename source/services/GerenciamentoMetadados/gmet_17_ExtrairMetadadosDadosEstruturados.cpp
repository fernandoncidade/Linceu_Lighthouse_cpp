#include "gmet_17_ExtrairMetadadosDadosEstruturados.h"

#include <QFile>
#include <QFileInfo>

#include "gmet_21_GetFormataTamanho.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_dados_estruturados(const QString& caminho, source::ui::LocalizadorQt* loc) {
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return m;
        m.insert("tamanho", get_formata_tamanho(fi.size()));

        QFile f(caminho);
        if (!f.open(QIODevice::ReadOnly)) return m;
        const QByteArray sample = f.read(4096);
        int non_ascii = 0;
        for (char c : sample) if (static_cast<unsigned char>(c) == 0 || static_cast<unsigned char>(c) > 127) ++non_ascii;
        const bool binario = !sample.isEmpty() && (static_cast<double>(non_ascii) / static_cast<double>(sample.size()) > 0.1);

        if (binario) {
            m.insert("formato", QStringLiteral("Arquivo de dados binário"));
            m.insert("binario", QStringLiteral("%1 %2").arg(fi.size()).arg(loc ? loc->get_text("bytes") : QStringLiteral("bytes")));
        } else {
            m.insert("formato", QStringLiteral("Arquivo de dados"));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados de arquivo de dados", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
