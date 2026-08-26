#include "gtmqt_04_traduzir_metadados.h"

#include "gtmqt_01_verificar_idioma_cache.h"
#include "gtmqt_05_traduzir_tipo_arquivo.h"
#include "gtmqt_06_traduzir_atributos.h"
#include "gtmqt_07_traduzir_autor.h"
#include "gtmqt_08_traduzir_protegido.h"
#include "gtmqt_09_traduzir_dimensoes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_metadados(source::ui::TradutorMetadadosQt* self, const QString& valor,
                           const QString& campo) {
    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        verificar_idioma_cache(self);
        if (valor.isEmpty()) {
            return valor;
        }

        QString campo_lower = campo.toLower();
        const QString cache_key =
            valor + QStringLiteral("_") + campo_lower + QStringLiteral("_") +
            self->loc_->property("idioma_atual").toString();

        if (campo_lower == QStringLiteral("tipo") || campo_lower == QStringLiteral("type")) {
            if (self->cache_traducoes_tipo_.contains(cache_key)) {
                return self->cache_traducoes_tipo_.value(cache_key);
            }

            const QString resultado = traduzir_tipo_arquivo(self, valor);
            self->cache_traducoes_tipo_.insert(cache_key, resultado);
            return resultado;
        }

        if (campo_lower == QStringLiteral("atributos") || campo_lower == QStringLiteral("attributes")) {
            if (self->cache_traducoes_atributos_.contains(cache_key)) {
                return self->cache_traducoes_atributos_.value(cache_key);
            }

            const QString resultado = traduzir_atributos(self, valor);
            self->cache_traducoes_atributos_.insert(cache_key, resultado);
            return resultado;
        }

        if (campo_lower == QStringLiteral("protegido") || campo_lower == QStringLiteral("protected")) {
            if (self->cache_traducoes_protegido_.contains(cache_key)) {
                return self->cache_traducoes_protegido_.value(cache_key);
            }

            const QString resultado = traduzir_protegido(self, valor);
            self->cache_traducoes_protegido_.insert(cache_key, resultado);
            return resultado;
        }

        campo_lower = campo.toLower();
        if (campo_lower == QStringLiteral("autor") || campo_lower == QStringLiteral("author")) {
            return traduzir_autor(self, valor);
        }

        if (campo_lower == QStringLiteral("dimensoes") || campo_lower == QStringLiteral("dimensions") ||
            campo_lower == QStringLiteral("tamanho") || campo_lower == QStringLiteral("size")) {
            return traduzir_dimensoes(self, valor);
        }

        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir metadados", true);
        return QString();
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
