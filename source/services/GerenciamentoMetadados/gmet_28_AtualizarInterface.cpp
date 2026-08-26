#include "gmet_28_AtualizarInterface.h"

#include <QMetaObject>
#include <QMutexLocker>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {

void atualizar_interface(source::services::GerenciadorColunas* gc, const QString& idioma) {
    Q_UNUSED(idioma)
    try {
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(gc ? gc->loc : nullptr);
        if (!gc || !loc) return;

        for (auto it = gc->COLUNAS_DISPONIVEIS.begin(); it != gc->COLUNAS_DISPONIVEIS.end(); ++it) {
            QVariantMap coluna = it.value();
            const QString tk = coluna.value("translation_key", it.key()).toString();
            coluna["nome"] = loc->get_text(tk);
            it.value() = coluna;
        }

        {
            QMutexLocker locker(&gc->lock_cache);
            gc->cache_metadados.clear();
        }

        if (gc->interface) {
            QObject* gt = gc->interface->property("gerenciador_tabela").value<QObject*>();
            if (gt != nullptr) {
                // Evita recarregamento completo síncrono da tabela durante troca de idioma.
                if (gt->metaObject()->indexOfMethod("atualizar_cabecalhos()") >= 0) {
                    QMetaObject::invokeMethod(gt, "atualizar_cabecalhos", Qt::QueuedConnection);
                }
                if (gt->metaObject()->indexOfMethod("retraduzir_dados_existentes()") >= 0) {
                    QMetaObject::invokeMethod(gt, "retraduzir_dados_existentes", Qt::QueuedConnection);
                }
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface", true);
    }
}

}  // namespace source::services::GerenciamentoMetadados
