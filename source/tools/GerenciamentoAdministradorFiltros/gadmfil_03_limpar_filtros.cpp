#include "gadmfil_03_limpar_filtros.h"

#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QTableView>
#include <QTableWidget>
#include <QVariantMap>

#include "source/tools/fil_01_Filtros.h"
#include "source/tools/fil_03_AdministradorFiltros.h"
#include "source/utils/LogManager.h"

namespace source::tools::GerenciamentoAdministradorFiltros {

void limpar_filtros(source::tools::AdministradorFiltros* self) {
    try {
        if (!self || !self->parent_obj()) return;
        auto* parent = self->parent_obj();

        const QVariantMap map = parent->property("checkboxes_operacao").toMap();
        for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
            auto* cb = qobject_cast<QCheckBox*>(it.value().value<QObject*>());
            if (cb) cb->setChecked(true);
        }

        if (auto* busca = parent->findChild<QLineEdit*>(QStringLiteral("campo_busca"))) busca->clear();
        if (auto* ext = parent->findChild<QLineEdit*>(QStringLiteral("campo_extensao"))) ext->clear();
        if (auto* di = parent->findChild<QDateTimeEdit*>(QStringLiteral("data_inicial"))) di->setDateTime(QDateTime::currentDateTime().addDays(-30));
        if (auto* df = parent->findChild<QDateTimeEdit*>(QStringLiteral("data_final"))) df->setDateTime(QDateTime::currentDateTime());

        auto setIgnore = [](source::tools::Filtros* p, const QString& name, const QString& key) {
            if (auto* cb = p->findChild<QCheckBox*>(name)) {
                cb->setChecked(true);
                source::tools::AdministradorFiltros::filtros_estado[key] = true;
            }
        };

        setIgnore(parent, "ignorar_mover", "ignorar_mover");
        setIgnore(parent, "ignorar_renomeados", "ignorar_renomeados");
        setIgnore(parent, "ignorar_adicionados", "ignorar_adicionados");
        setIgnore(parent, "ignorar_excluidos", "ignorar_excluidos");
        setIgnore(parent, "ignorar_data_modificados", "ignorar_data_modificados");
        setIgnore(parent, "ignorar_escaneados", "ignorar_escaneados");

        if (auto* tabela = qobject_cast<QTableWidget*>(parent->tabela_dados())) {
            for (int row = 0; row < tabela->rowCount(); ++row) tabela->setRowHidden(row, false);
        } else if (auto* view = qobject_cast<QTableView*>(parent->tabela_dados())) {
            const int total = view->model() != nullptr ? view->model()->rowCount() : 0;
            for (int row = 0; row < total; ++row) view->setRowHidden(row, false);
            if (view->viewport() != nullptr) {
                view->viewport()->update();
            }
        }

        self->sincronizar_menu_principal_com_filtros();
        emit parent->filtroAplicado();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar filtros", true);
    }
}

}  // namespace source::tools::GerenciamentoAdministradorFiltros
