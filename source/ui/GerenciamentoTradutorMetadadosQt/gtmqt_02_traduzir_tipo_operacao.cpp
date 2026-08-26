#include "gtmqt_02_traduzir_tipo_operacao.h"

#include <QVector>

#include "gtmqt_01_verificar_idioma_cache.h"
#include "gtmqt_03_obter_chave_traducao_reversa.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/ui_13_TradutorMetadadosQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoTradutorMetadadosQt {

QString traduzir_tipo_operacao(source::ui::TradutorMetadadosQt* self, const QString& valor,
                               const QString& idioma_origem) {
    QString cache_key;

    try {
        if (self == nullptr || self->loc_ == nullptr) {
            return valor;
        }

        verificar_idioma_cache(self);

        const QString valor_normalizado = valor.toLower().trimmed();
        const QString idioma_cache = !idioma_origem.isEmpty()
                                         ? idioma_origem
                                         : self->loc_->property("idioma_atual").toString();
        cache_key = valor_normalizado + QStringLiteral("_") + idioma_cache;

        if (self->cache_traducoes_tipo_operacao_.contains(cache_key)) {
            return self->cache_traducoes_tipo_operacao_.value(cache_key);
        }

        const QString valor_traduzido_reverso = obter_chave_traducao_reversa(self, valor_normalizado);
        if (!valor_traduzido_reverso.isEmpty()) {
            const QString traducao = self->loc_->get_text(valor_traduzido_reverso);
            self->cache_traducoes_tipo_operacao_.insert(cache_key, traducao);
            return traducao;
        }

        const QVector<QPair<QString, QString>> mapeamento_operacoes = {
            {QStringLiteral("moved"), QStringLiteral("op_moved")},
            {QStringLiteral("renamed"), QStringLiteral("op_renamed")},
            {QStringLiteral("added"), QStringLiteral("op_added")},
            {QStringLiteral("deleted"), QStringLiteral("op_deleted")},
            {QStringLiteral("modified"), QStringLiteral("op_modified")},
            {QStringLiteral("scanned"), QStringLiteral("op_scanned")},

            {QStringLiteral("movido"), QStringLiteral("op_moved")},
            {QStringLiteral("renomeado"), QStringLiteral("op_renamed")},
            {QStringLiteral("adicionado"), QStringLiteral("op_added")},
            {QStringLiteral("excluído"), QStringLiteral("op_deleted")},
            {QStringLiteral("modificado"), QStringLiteral("op_modified")},
            {QStringLiteral("escaneado"), QStringLiteral("op_scanned")},

            {QStringLiteral("movido"), QStringLiteral("op_moved")},
            {QStringLiteral("renombrado"), QStringLiteral("op_renamed")},
            {QStringLiteral("añadido"), QStringLiteral("op_added")},
            {QStringLiteral("eliminado"), QStringLiteral("op_deleted")},
            {QStringLiteral("modificado"), QStringLiteral("op_modified")},
            {QStringLiteral("escanneado"), QStringLiteral("op_scanned")},

            {QStringLiteral("déplacé"), QStringLiteral("op_moved")},
            {QStringLiteral("renommé"), QStringLiteral("op_renamed")},
            {QStringLiteral("ajouté"), QStringLiteral("op_added")},
            {QStringLiteral("supprimé"), QStringLiteral("op_deleted")},
            {QStringLiteral("modifié"), QStringLiteral("op_modified")},
            {QStringLiteral("numérisé"), QStringLiteral("op_scanned")},

            {QStringLiteral("spostato"), QStringLiteral("op_moved")},
            {QStringLiteral("rinominato"), QStringLiteral("op_renamed")},
            {QStringLiteral("aggiunto"), QStringLiteral("op_added")},
            {QStringLiteral("eliminato"), QStringLiteral("op_deleted")},
            {QStringLiteral("modificato"), QStringLiteral("op_modified")},
            {QStringLiteral("scansionato"), QStringLiteral("op_scanned")},

            {QStringLiteral("verschoben"), QStringLiteral("op_moved")},
            {QStringLiteral("umbenannt"), QStringLiteral("op_renamed")},
            {QStringLiteral("hinzugefügt"), QStringLiteral("op_added")},
            {QStringLiteral("gelöscht"), QStringLiteral("op_deleted")},
            {QStringLiteral("geändert"), QStringLiteral("op_modified")},
            {QStringLiteral("gescannt"), QStringLiteral("op_scanned")},
        };

        for (const auto& item : mapeamento_operacoes) {
            if (item.first == valor_normalizado) {
                const QString traducao = self->loc_->get_text(item.second);
                self->cache_traducoes_tipo_operacao_.insert(cache_key, traducao);
                return traducao;
            }
        }

        if (valor_normalizado.startsWith(QStringLiteral("op_"))) {
            const QString traducao = self->loc_->get_text(valor_normalizado);
            self->cache_traducoes_tipo_operacao_.insert(cache_key, traducao);
            return traducao;
        }

        for (const auto& item : mapeamento_operacoes) {
            if (item.first.contains(valor_normalizado) || valor_normalizado.contains(item.first)) {
                const QString traducao = self->loc_->get_text(item.second);
                self->cache_traducoes_tipo_operacao_.insert(cache_key, traducao);
                return traducao;
            }
        }

        self->cache_traducoes_tipo_operacao_.insert(cache_key, valor);
        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir tipo de operação", true);
        if (!cache_key.isEmpty() && self != nullptr) {
            self->cache_traducoes_tipo_operacao_.insert(cache_key, valor);
        }
        return QString();
    }
}

}  // namespace source::ui::GerenciamentoTradutorMetadadosQt
