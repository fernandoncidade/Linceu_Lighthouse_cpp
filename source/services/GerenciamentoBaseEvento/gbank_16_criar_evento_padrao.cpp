#include "gbank_16_criar_evento_padrao.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>

#include "source/services/GerenciamentoMetadados/gmet_19_GetTipoArquivo.h"
#include "source/services/ob_01_Observador.h"
#include "source/services/ob_02_BaseEvento.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
qint64 directory_size_and_count(const QString& dirPath, qint64& filesCount) {
    qint64 total = 0;
    filesCount = 0;
    QDir dir(dirPath);
    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo& fi : list) {
        if (fi.isDir()) {
            qint64 c = 0;
            total += directory_size_and_count(fi.absoluteFilePath(), c);
            filesCount += c;
        } else {
            total += fi.size();
            ++filesCount;
        }
    }
    return total;
}

QDateTime primeira_data_valida(const QDateTime& principal,
                               const QDateTime& fallback_1 = QDateTime(),
                               const QDateTime& fallback_2 = QDateTime()) {
    if (principal.isValid()) return principal;
    if (fallback_1.isValid()) return fallback_1;
    if (fallback_2.isValid()) return fallback_2;
    return {};
}

QString formatar_data_metadado(const QDateTime& data) {
    return data.isValid() ? data.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) : QString();
}
}

namespace source::services::GerenciamentoBaseEvento {

QVariantMap _criar_evento_padrao(source::services::BaseEvento* self, const QString& tipo_operacao, const QString& nome_base, const QString& dir_anterior, const QString& dir_atual) {
    try {
        if (!self || !self->observador) return {};

        QString caminho = QFileInfo::exists(dir_atual) ? dir_atual : dir_anterior;
        QVariantMap metadados;
        QString tipo_arquivo;
        qint64 size_b = 0;
        qint64 arquivos = 0;
        const QString tipo_preferencial =
            source::services::GerenciamentoMetadados::extensao_tipo_preferencial(nome_base);

        QDateTime dc = QDateTime::currentDateTime();
        QDateTime dm = dc;
        QDateTime da = dc;

        QVariantMap item_data{{"nome", nome_base}, {"dir_atual", dir_atual}, {"dir_anterior", dir_anterior}};
        if (self->observador->gerenciador_colunas) {
            metadados = self->observador->gerenciador_colunas->get_metadados(item_data);
        }
        if (!tipo_preferencial.isEmpty()) {
            metadados["tipo"] = tipo_preferencial;
        }

        if (QFileInfo::exists(caminho)) {
            const bool e_pasta = self->is_directory(caminho);
            if (e_pasta) {
                tipo_arquivo = self->observador->loc ? self->observador->loc->get_text("folder") : QStringLiteral("folder");
                if (metadados.contains(QStringLiteral("size_b"))) {
                    size_b = metadados.value(QStringLiteral("size_b")).toLongLong();
                    arquivos = metadados.value(QStringLiteral("arquivos")).toLongLong();
                } else {
                    size_b = directory_size_and_count(caminho, arquivos);
                }
            } else {
                const QString ext = QFileInfo(nome_base).suffix().toLower();
                tipo_arquivo = ext.isEmpty() ? (self->observador->loc ? self->observador->loc->get_text("unknown") : QStringLiteral("unknown")) : ext;
                size_b = QFileInfo(caminho).size();
            }

            QFileInfo st(caminho);
            const QDateTime agora = QDateTime::currentDateTime();
            dm = primeira_data_valida(st.lastModified(), st.metadataChangeTime(), agora);
            dc = primeira_data_valida(st.birthTime(), st.metadataChangeTime(), dm);
            da = primeira_data_valida(st.lastRead(), dm, dc);
        } else {
            tipo_arquivo = source::services::GerenciamentoMetadados::identificar_tipo_arquivo(
                nome_base, self->observador->loc, nome_base);
            if (tipo_arquivo.isEmpty()) {
                tipo_arquivo = self->get_tipo_from_snapshot(nome_base);
                if (tipo_arquivo.isEmpty()) {
                    const QString ext = QFileInfo(nome_base).suffix().toLower();
                    tipo_arquivo = ext.isEmpty() ? (self->observador->loc ? self->observador->loc->get_text("unknown")
                                                                           : QStringLiteral("unknown"))
                                                 : ext;
                }
            }

            if (!dir_anterior.isEmpty() && self->is_directory(dir_anterior)) {
                tipo_arquivo = self->observador->loc ? self->observador->loc->get_text("folder") : QStringLiteral("folder");
                size_b = directory_size_and_count(dir_anterior, arquivos);
            }
        }
        if (!tipo_preferencial.isEmpty()) {
            tipo_arquivo = tipo_preferencial;
        }

        const QString extDot = QStringLiteral(".") + QFileInfo(nome_base).suffix().toLower();
        const QSet<QString> tipos_temporarios = {".tmp", ".temp", ".~", ".swp", ".swo", ".$$", ".old", ".part", ".cache", ".crdownload", ".download", ".partial", ".lock", ".thumb", ".TMP", ".TEMP"};
        const QString lower = nome_base.toLower();
        const bool eh_temporario = tipos_temporarios.contains(extDot) || lower.startsWith("~") || lower.startsWith("._") || lower.startsWith(".#") || lower.startsWith("~$") || lower.endsWith("~") || lower.endsWith(".lock") || lower.contains("temp-index") || lower.contains("~index") || lower.contains("thumb");
        if (!tipo_preferencial.isEmpty()) {
            metadados["tipo"] = tipo_preferencial;
        }

        if (self->observador->loc && tipo_arquivo == self->observador->loc->get_text("folder")) {
            metadados["size_b"] = size_b;
            metadados["size_kb"] = QString::number(size_b / 1024.0, 'f', 2);
            metadados["size_mb"] = QString::number(size_b / (1024.0 * 1024.0), 'f', 2);
            metadados["size_gb"] = QString::number(size_b / (1024.0 * 1024.0 * 1024.0), 'f', 2);
            metadados["size_tb"] = QString::number(size_b / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2);
            metadados["arquivos"] = arquivos;
            metadados["atributos"] = self->observador->loc->get_text("folder");
            metadados["autor"] = qEnvironmentVariable("USERNAME");
            metadados["protegido"] = self->observador->loc->get_text("No");
        } else if (eh_temporario && self->observador->loc) {
            metadados["atributos"] = self->observador->loc->get_text("file");
        }

        return {
            {"tipo_operacao", tipo_operacao}, {"nome", nome_base}, {"dir_anterior", dir_anterior}, {"dir_atual", dir_atual},
            {"data_criacao", formatar_data_metadado(dc)}, {"data_modificacao", formatar_data_metadado(dm)}, {"data_acesso", formatar_data_metadado(da)},
            {"tipo", tipo_arquivo},
            {"mime", metadados.value("mime")},
            {"size_b", metadados.value("size_b", size_b)}, {"size_kb", metadados.value("size_kb", QString::number(size_b / 1024.0, 'f', 2))},
            {"size_mb", metadados.value("size_mb", QString::number(size_b / (1024.0 * 1024.0), 'f', 2))}, {"size_gb", metadados.value("size_gb", QString::number(size_b / (1024.0 * 1024.0 * 1024.0), 'f', 2))},
            {"size_tb", metadados.value("size_tb", QString::number(size_b / (1024.0 * 1024.0 * 1024.0 * 1024.0), 'f', 2))},
            {"atributos", metadados.value("atributos", (self->observador->loc && eh_temporario) ? self->observador->loc->get_text("file") : QString())},
            {"autor", metadados.value("autor", qEnvironmentVariable("USERNAME"))}, {"dimensoes", metadados.value("dimensoes")}, {"duracao", metadados.value("duracao")}, {"taxa_bits", metadados.value("taxa_bits")},
            {"protegido", metadados.value("protegido", self->observador->loc ? self->observador->loc->get_text("No") : QStringLiteral("No"))},
            {"paginas", metadados.value("paginas")}, {"linhas", metadados.value("linhas")}, {"palavras", metadados.value("palavras")}, {"paginas_estimadas", metadados.value("paginas_estimadas")},
            {"linhas_codigo", metadados.value("linhas_codigo")}, {"total_linhas", metadados.value("total_linhas")}, {"slides_estimados", metadados.value("slides_estimados")}, {"arquivos", metadados.value("arquivos", arquivos)},
            {"unzipped_b", metadados.value("unzipped_b")}, {"unzipped_kb", metadados.value("unzipped_kb")}, {"unzipped_mb", metadados.value("unzipped_mb")}, {"unzipped_gb", metadados.value("unzipped_gb")}, {"unzipped_tb", metadados.value("unzipped_tb")},
            {"slides", metadados.value("slides")}, {"binary_file_b", metadados.value("binary_file_b")}, {"binary_file_kb", metadados.value("binary_file_kb")}, {"binary_file_mb", metadados.value("binary_file_mb")}, {"binary_file_gb", metadados.value("binary_file_gb")},
            {"binary_file_tb", metadados.value("binary_file_tb")}, {"planilhas", metadados.value("planilhas")}, {"colunas", metadados.value("colunas")}, {"registros", metadados.value("registros")}, {"tabelas", metadados.value("tabelas")}
        };
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar evento padrão", true);
        return {};
    }
}

}  // namespace source::services::GerenciamentoBaseEvento
