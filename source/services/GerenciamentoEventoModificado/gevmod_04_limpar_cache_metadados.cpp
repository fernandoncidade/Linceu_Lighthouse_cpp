#include "gevmod_04_limpar_cache_metadados.h"

#include <QMutexLocker>
#include <QVariant>
#include <QVariantMap>

#include "source/services/ob_01_Observador.h"
#include "source/services/ob_06_EventoModificado.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/utils/LogManager.h"

namespace {

void remover_cache_metadados(source::services::GerenciadorColunas* gc, const QString& caminho_completo) {
    if (!gc) return;
    QMutexLocker locker(&gc->lock_cache);
    gc->cache_metadados.remove(caminho_completo);
}

void remover_metadados_cache_dinamico(source::services::GerenciadorColunas* gc, const QString& caminho_completo) {
    if (!gc) return;
    const QVariant cache_var = gc->property("metadados_cache");
    if (!cache_var.isValid() || !cache_var.canConvert<QVariantMap>()) return;
    QVariantMap cache = cache_var.toMap();
    if (!cache.contains(caminho_completo)) return;
    cache.remove(caminho_completo);
    gc->setProperty("metadados_cache", cache);
}

void limpar_uma_passada(source::services::GerenciadorColunas* gc, const QString& caminho_completo) {
    remover_cache_metadados(gc, caminho_completo);
    remover_metadados_cache_dinamico(gc, caminho_completo);
}

}  // namespace

namespace source::services::GerenciamentoEventoModificado {

void limpar_cache_metadados(source::services::EventoModificado* self, const QString& caminho_completo, const QString& ext) {
    try {
        if (!self || !self->observador || !self->observador->gerenciador_colunas) return;

        auto* gc = self->observador->gerenciador_colunas.get();
        if (ext == QStringLiteral(".txt")) {
            limpar_uma_passada(gc, caminho_completo);
        }

        // Mantém o mesmo fluxo do Python: limpeza adicional sempre executada.
        limpar_uma_passada(gc, caminho_completo);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar cache de metadados", true);
    }
}

}  // namespace source::services::GerenciamentoEventoModificado
