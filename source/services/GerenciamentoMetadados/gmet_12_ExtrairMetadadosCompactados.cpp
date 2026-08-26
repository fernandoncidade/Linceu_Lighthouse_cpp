#include "gmet_12_ExtrairMetadadosCompactados.h"

#include <QtEndian>
#include <QFile>
#include <QFileInfo>

#include "gmet_21_GetFormataTamanho.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_compactados(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap m;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return m;

        const QString ext = QStringLiteral(".") + fi.suffix().toLower();
        if (ext == QStringLiteral(".zip")) {
            QFile f(caminho);
            if (f.open(QIODevice::ReadOnly)) {
                const QByteArray bytes = f.readAll();
                int arquivos = 0;
                qint64 tamanho_descompactado = 0;

                for (int i = 0; i + 46 <= bytes.size(); ++i) {
                    if (static_cast<uchar>(bytes[i + 0]) == 0x50 && static_cast<uchar>(bytes[i + 1]) == 0x4B &&
                        static_cast<uchar>(bytes[i + 2]) == 0x01 && static_cast<uchar>(bytes[i + 3]) == 0x02) {
                        const quint32 uncomp = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(bytes.constData() + i + 24));
                        tamanho_descompactado += static_cast<qint64>(uncomp);
                        ++arquivos;
                    }
                }

                if (arquivos > 0) {
                    m.insert("arquivos", arquivos);
                    m.insert("tamanho_descompactado", formata_tamanho(static_cast<double>(tamanho_descompactado)));
                    m.insert("tamanho_descompactado_bytes", tamanho_descompactado);
                    m.insert("descompactados", formata_tamanho(static_cast<double>(tamanho_descompactado)));
                    m.insert("unzipped_b", tamanho_descompactado);
                    m.insert("unzipped_kb", QString::number(tamanho_descompactado / 1024.0, 'f', 2));
                    m.insert("unzipped_mb", QString::number(tamanho_descompactado / (1024.0 * 1024.0), 'f', 2));
                    m.insert("unzipped_gb", QString::number(tamanho_descompactado / (1024.0 * 1024.0 * 1024.0), 'f', 2));
                    m.insert("unzipped_tb", QString::number(tamanho_descompactado / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2));

                    if (tamanho_descompactado > 0) {
                        const double taxa = (1.0 - (static_cast<double>(fi.size()) / static_cast<double>(tamanho_descompactado))) * 100.0;
                        m.insert("taxa_compressao", QString::number(taxa, 'f', 1) + QStringLiteral("%"));
                    }
                }
            }
        }

        if (!m.contains("descompactados")) {
            m.insert("tamanho_descompactado", formata_tamanho(fi.size()));
            m.insert("descompactados", formata_tamanho(fi.size()));
            m.insert("unzipped_b", fi.size());
            m.insert("unzipped_kb", QString::number(fi.size() / 1024.0, 'f', 2));
            m.insert("unzipped_mb", QString::number(fi.size() / (1024.0 * 1024.0), 'f', 2));
            m.insert("unzipped_gb", QString::number(fi.size() / (1024.0 * 1024.0 * 1024.0), 'f', 2));
            m.insert("unzipped_tb", QString::number(fi.size() / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro geral ao extrair metadados do arquivo compactado", true);
    }
    return m;
}

}  // namespace source::services::GerenciamentoMetadados
