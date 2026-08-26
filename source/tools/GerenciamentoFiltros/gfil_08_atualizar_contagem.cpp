#include "gfil_08_atualizar_contagem.h"

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoFiltros {

namespace {
QString fallback_contagem(source::tools::Filtros* self) {
    QString padrao = QStringLiteral("Itens: {}/{}");
    if (self && self->localizador()) {
        const QString translated = self->localizador()->get_text("items_count");
        if (!translated.trimmed().isEmpty()) padrao = translated;
    }

    const int first_brace = padrao.indexOf(QStringLiteral("{}"));
    if (first_brace >= 0) {
        padrao.replace(first_brace, 2, QStringLiteral("0"));
        const int second_brace = padrao.indexOf(QStringLiteral("{}"));
        if (second_brace >= 0) padrao.replace(second_brace, 2, QStringLiteral("0"));
        return padrao;
    }
    return padrao.arg(0).arg(0);
}
}  // namespace

QString atualizar_contagem(source::tools::Filtros* self) {
    try {
        if (!self || !self->administrador_filtros_obj()) return fallback_contagem(self);
        return self->administrador_filtros_obj()->atualizar_contagem();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar contagem", true);
        return fallback_contagem(self);
    }
}

} // namespace source::tools::GerenciamentoFiltros
