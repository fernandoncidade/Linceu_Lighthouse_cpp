#include "glqt_13_traduzir_tipo_operacao_fallback.h"

#include <QHash>

#include "glqt_05_get_text.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

QString traduzir_tipo_operacao_fallback(source::ui::LocalizadorQt* self, const QString& valor) {
    try {
        const QHash<QString, QString> mapeamento = {
            {"moved", "op_moved"}, {"renamed", "op_renamed"}, {"added", "op_added"},
            {"deleted", "op_deleted"}, {"modified", "op_modified"}, {"scanned", "op_scanned"},
            {"movido", "op_moved"}, {"renomeado", "op_renamed"}, {"adicionado", "op_added"},
            {"excluido", "op_deleted"}, {"excluído", "op_deleted"}, {"modificado", "op_modified"}, {"escaneado", "op_scanned"},
            {"renombrado", "op_renamed"}, {"añadido", "op_added"}, {"eliminado", "op_deleted"},
            {"déplacé", "op_moved"}, {"renommé", "op_renamed"}, {"ajouté", "op_added"},
            {"supprimé", "op_deleted"}, {"modifié", "op_modified"}, {"numérisé", "op_scanned"},
            {"spostato", "op_moved"}, {"rinominato", "op_renamed"}, {"aggiunto", "op_added"},
            {"eliminato", "op_deleted"}, {"modificato", "op_modified"}, {"scansionato", "op_scanned"},
            {"verschoben", "op_moved"}, {"umbenannt", "op_renamed"}, {"hinzugefügt", "op_added"},
            {"gelöscht", "op_deleted"}, {"geändert", "op_modified"}, {"gescannt", "op_scanned"}
        };

        const QString valor_lower = valor.toLower().trimmed();
        const QString chave = mapeamento.value(valor_lower, valor_lower);
        if (chave.startsWith("op_")) {
            return get_text(self, chave);
        }
        return valor;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir tipo de operação (fallback)", true);
        return valor;
    }
}

}
