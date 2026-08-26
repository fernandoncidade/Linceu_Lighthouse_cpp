#include "gtab_16_ObterIndicesColunas.h"

#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

QVariantMap obter_indices_colunas(source::services::GerenciadorTabela* self, QObject* tabela) {
    try {
        if (!self) return QVariantMap{};
        auto* tw = qobject_cast<QTableWidget*>(tabela);
        if (!tw) return QVariantMap{};

        if (self->cache_indices_colunas.isEmpty()) {
            for (int i = 0; i < tw->columnCount(); ++i) {
                if (auto* item = tw->horizontalHeaderItem(i)) {
                    const QString nome = item->text().replace('\n', ' ').trimmed();
                    self->cache_indices_colunas.insert(nome, i);
                }
            }
        }

        QVariantMap out;
        for (auto it = self->cache_indices_colunas.constBegin(); it != self->cache_indices_colunas.constEnd(); ++it) {
            out.insert(it.key(), it.value());
        }
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em obter_indices_colunas", true);
    }
    return QVariantMap{};
}
}  // namespace source::services::GerenciamentoTabela

