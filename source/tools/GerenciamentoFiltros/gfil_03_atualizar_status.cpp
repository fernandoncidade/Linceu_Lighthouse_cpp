#include "gfil_03_atualizar_status.h"

#include <QLabel>

#include "gfil_08_atualizar_contagem.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void atualizar_status(source::tools::Filtros* self, const QString&) {
    try {
        if (!self) return;

        if (self->parent() && self->parent()->metaObject()->indexOfMethod("atualizar_status()") >= 0) {
            QMetaObject::invokeMethod(self->parent(), "atualizar_status");
        } else {
            if (auto* label = self->findChild<QLabel*>(QStringLiteral("label_contagem"))) {
                label->setText(atualizar_contagem(self));
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar status dos filtros", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
