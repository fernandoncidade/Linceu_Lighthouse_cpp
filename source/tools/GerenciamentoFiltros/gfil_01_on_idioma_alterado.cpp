#include "gfil_01_on_idioma_alterado.h"

#include <QDateTimeEdit>
#include <QLocale>

#include "gfil_03_atualizar_status.h"
#include "gfil_09_atualizar_interface.h"
#include "source/tools/fil_01_Filtros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void on_idioma_alterado(source::tools::Filtros* self, const QString& idioma) {
    try {
        if (!self) return;

        atualizar_interface(self, idioma);
        atualizar_status(self, idioma);

        const QString fmt = QLocale().dateTimeFormat(QLocale::ShortFormat);
        if (auto* data_inicial = self->findChild<QDateTimeEdit*>(QStringLiteral("data_inicial"))) {
            data_inicial->setDisplayFormat(fmt);
        }
        if (auto* data_final = self->findChild<QDateTimeEdit*>(QStringLiteral("data_final"))) {
            data_final->setDisplayFormat(fmt);
        }

        emit self->filtroAplicado();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro no slot de idioma_alterado", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
