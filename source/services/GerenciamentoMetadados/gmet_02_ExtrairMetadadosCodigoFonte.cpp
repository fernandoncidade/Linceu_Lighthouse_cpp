#include "gmet_02_ExtrairMetadadosCodigoFonte.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "gmet_21_GetFormataTamanho.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

QVariantMap extrair_metadados_codigo_fonte(const QString& caminho, source::ui::LocalizadorQt* loc) {
    Q_UNUSED(loc)
    QVariantMap metadados;
    try {
        QFileInfo fi(caminho);
        if (!fi.exists()) return metadados;

        int total = 0;
        int codigo = 0;
        int comentarios = 0;
        int vazias = 0;

        QFile f(caminho);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            in.setAutoDetectUnicode(true);
            while (!in.atEnd()) {
                const QString l = in.readLine();
                ++total;
                const QString t = l.trimmed();
                if (t.isEmpty()) ++vazias;
                else if (t.startsWith("#") || t.startsWith("//") || t.startsWith("/*") || t.startsWith("*")) ++comentarios;
                else ++codigo;
            }
        }

        metadados.insert("total_linhas", total);
        metadados.insert("linhas_codigo", codigo);
        metadados.insert("linhas_comentario", comentarios);
        metadados.insert("linhas_vazias", vazias);
        metadados.insert("tamanho", get_formata_tamanho(fi.size()));
        metadados.insert("data_criacao", fi.birthTime().toString("yyyy-MM-dd HH:mm:ss"));
        metadados.insert("data_modificacao", fi.lastModified().toString("yyyy-MM-dd HH:mm:ss"));
        metadados.insert("data_acesso", fi.lastRead().toString("yyyy-MM-dd HH:mm:ss"));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao extrair metadados do código fonte", true);
    }
    return metadados;
}

}  // namespace source::services::GerenciamentoMetadados
