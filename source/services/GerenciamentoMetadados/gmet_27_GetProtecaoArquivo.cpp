#include "gmet_27_GetProtecaoArquivo.h"

#include <QFileInfo>
#include <QMetaType>

#include "gmet_10_ExtrairMetadadosExecutavel.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace source::services::GerenciamentoMetadados {

QString get_protecao_arquivo(source::services::GerenciadorColunas* gerenciador, const QVariantMap& item, source::ui::LocalizadorQt* loc) {
    try {
        const auto texto_protecao = [loc](bool protegido) {
            if (loc) return protegido ? loc->get_text("yes") : loc->get_text("no");
            return protegido ? QStringLiteral("yes") : QStringLiteral("no");
        };
        const auto interpretar_protecao = [loc](const QVariant& valor, bool& ok) -> bool {
            ok = true;
            if (valor.metaType().id() == QMetaType::Bool) return valor.toBool();

            const QString texto = valor.toString().trimmed().toLower();
            const QString yes_loc = loc ? loc->get_text("yes").trimmed().toLower() : QStringLiteral("yes");
            const QString no_loc = loc ? loc->get_text("no").trimmed().toLower() : QStringLiteral("no");

            if (texto.isEmpty()) {
                ok = false;
                return false;
            }
            if (texto == yes_loc || texto == QStringLiteral("yes") || texto == QStringLiteral("true") || texto == QStringLiteral("1")) return true;
            if (texto == no_loc || texto == QStringLiteral("no") || texto == QStringLiteral("false") || texto == QStringLiteral("0")) return false;

            ok = false;
            return false;
        };

        const QString caminho = item.value("dir_atual").toString().isEmpty() ? item.value("dir_anterior").toString() : item.value("dir_atual").toString();
        if (caminho.isEmpty() || !QFileInfo::exists(caminho)) {
            const QString protegido_item = item.value("protegido").toString().trimmed();
            if (!protegido_item.isEmpty()) return protegido_item;
            return texto_protecao(false);
        }

        if (gerenciador) {
            QMutexLocker locker(&gerenciador->lock_cache);
            if (gerenciador->cache_metadados.contains(caminho)) {
                const QVariantMap cache = gerenciador->cache_metadados.value(caminho).toMap();
                if (cache.contains("protegido")) {
                    bool ok_cache = false;
                    const bool cached = interpretar_protecao(cache.value("protegido"), ok_cache);
                    if (ok_cache) return texto_protecao(cached);
                }
            }
        }

        QFileInfo fi(caminho);
        bool protegido = !fi.isWritable() || fi.isHidden();

#ifdef _WIN32
        const std::wstring caminho_w = caminho.toStdWString();
        const DWORD attrs = GetFileAttributesW(caminho_w.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES) {
            protegido = protegido || ((attrs & FILE_ATTRIBUTE_READONLY) != 0) || ((attrs & FILE_ATTRIBUTE_HIDDEN) != 0) ||
                        ((attrs & FILE_ATTRIBUTE_SYSTEM) != 0) || ((attrs & FILE_ATTRIBUTE_ENCRYPTED) != 0) ||
                        ((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0);
        }
#endif

        if (!protegido && fi.isFile()) {
            const QString ext = QStringLiteral(".") + fi.suffix().toLower();
            if (ext == QStringLiteral(".exe") || ext == QStringLiteral(".dll") || ext == QStringLiteral(".msi")) {
                protegido = verificar_assinatura_executavel(caminho);
            }
        }

        if (gerenciador) {
            QMutexLocker locker(&gerenciador->lock_cache);
            QVariantMap cache = gerenciador->cache_metadados.value(caminho).toMap();
            cache.insert("protegido", protegido);
            gerenciador->cache_metadados[caminho] = cache;
        }

        return texto_protecao(protegido);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao obter proteção do arquivo", true);
        return QString();
    }
}

}  // namespace source::services::GerenciamentoMetadados
