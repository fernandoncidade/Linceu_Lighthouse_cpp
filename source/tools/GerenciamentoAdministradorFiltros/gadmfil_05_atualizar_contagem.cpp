#include "gadmfil_05_atualizar_contagem.h"

#include <QPair>
#include <QTableView>
#include <QTableWidget>

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

namespace {
QPair<int, int> obter_visiveis_total(QWidget* tabela_widget) {
    if (auto* tabela = qobject_cast<QTableWidget*>(tabela_widget)) {
        const int total = tabela->rowCount();
        int visiveis = 0;
        for (int row = 0; row < total; ++row) {
            if (!tabela->isRowHidden(row)) ++visiveis;
        }
        return {visiveis, total};
    }

    if (auto* view = qobject_cast<QTableView*>(tabela_widget)) {
        const int total = (view->model() != nullptr) ? view->model()->rowCount() : 0;
        int visiveis = 0;
        for (int row = 0; row < total; ++row) {
            if (!view->isRowHidden(row)) ++visiveis;
        }
        return {visiveis, total};
    }

    return {0, 0};
}
}  // namespace

QString atualizar_contagem(source::tools::AdministradorFiltros* self) {
    try {
        if (!self) return QStringLiteral("0 / 0 | 0%");

        source::ui::LocalizadorQt fallback_loc;
        source::ui::LocalizadorQt* loc = (self->parent_obj() && self->parent_obj()->localizador()) ? self->parent_obj()->localizador() : &fallback_loc;

        const QWidget* tabela = self->parent_obj() ? self->parent_obj()->tabela_dados() : nullptr;
        const QPair<int, int> contagem = obter_visiveis_total(const_cast<QWidget*>(tabela));
        const int visiveis = contagem.first;
        const int total = contagem.second;

        if (total == 0) {
            return QStringLiteral("%1: 0 / %2: 0 | %3: 0%")
                .arg(loc->get_text("visible_filters"), loc->get_text("total_monitored"), loc->get_text("filtered_rate"));
        }

        const double taxa = 100.0 - ((static_cast<double>(visiveis) / static_cast<double>(total)) * 100.0);
        const QString taxa_txt = QString::number(taxa, 'f', 2);
        return QStringLiteral("%1: %2 / %3: %4 | %5: %6%")
            .arg(loc->get_text("visible_filters"))
            .arg(visiveis)
            .arg(loc->get_text("total_monitored"))
            .arg(total)
            .arg(loc->get_text("filtered_rate"))
            .arg(taxa_txt);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar contagem", true);
        return QStringLiteral("0 / 0 | 0%");
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
