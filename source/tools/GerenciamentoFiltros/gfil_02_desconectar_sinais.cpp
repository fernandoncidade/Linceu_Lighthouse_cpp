#include "gfil_02_desconectar_sinais.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

void desconectar_sinais(source::tools::Filtros* self) {
    try {
        if (!self || !self->localizador()) return;
        QObject::disconnect(self->localizador(), &source::ui::LocalizadorQt::idioma_alterado,
                            self, &source::tools::Filtros::on_idioma_alterado);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao desconectar sinais", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
