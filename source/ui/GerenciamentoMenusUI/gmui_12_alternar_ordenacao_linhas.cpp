#include "gmui_12_alternar_ordenacao_linhas.h"

#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QMetaObject>
#include <QTableView>
#include <QTableWidget>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace {
QString gt(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString t = fallback;
    if (loc) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty()) {
            t = translated;
        }
    }
    return t;
}

void aplicar_estado_ordenacao_view(QObject* iface, QTableView* tabela) {
    if (iface == nullptr || tabela == nullptr || tabela->model() == nullptr) {
        return;
    }

    auto* header = tabela->horizontalHeader();
    if (header == nullptr) {
        return;
    }

    const bool habilitada = iface->property("_ordenacao_linhas_habilitada").toBool();
    const int coluna = iface->property("_ordenacao_linhas_coluna").toInt();
    const Qt::SortOrder ordem =
        static_cast<Qt::SortOrder>(iface->property("_ordenacao_linhas_ordem").toInt());

    if (!habilitada) {
        header->setSortIndicatorShown(false);
        tabela->viewport()->update();
        return;
    }

    if (coluna >= 0 && coluna < tabela->model()->columnCount()) {
        header->setSortIndicatorShown(true);
        header->setSortIndicator(coluna, ordem);
        tabela->model()->sort(coluna, ordem);
    } else {
        header->setSortIndicatorShown(false);
    }

    tabela->viewport()->update();
}
}

namespace source::ui::GerenciamentoMenusUI {

void alternar_ordenacao_linhas(source::ui::GerenciadorMenusUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* acao = iface->property("acao_ordenar_linhas").value<QAction*>();
        if (!acao) return;

        const bool habilitada = acao->isChecked();
        iface->setProperty("_ordenacao_linhas_habilitada", habilitada);

        if (auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
                iface->property("gerenciador_tabela").value<QObject*>())) {
            gerenciador_tabela->ordenacao_habilitada = habilitada;
        }

        if (auto* tabela_view = qobject_cast<QTableView*>(iface->property("tabela_dados").value<QObject*>())) {
            aplicar_estado_ordenacao_view(iface, tabela_view);
        } else if (auto* tabela = qobject_cast<QTableWidget*>(iface->property("tabela_dados").value<QObject*>())) {
            tabela->setSortingEnabled(habilitada);
        }

        const QString estado = habilitada ? gt(self->loc_obj(), "enabled", "habilitado") : gt(self->loc_obj(), "disabled", "desabilitado");
        QMessageBox::information(qobject_cast<QWidget*>(iface), gt(self->loc_obj(), "success", "Sucesso"), gt(self->loc_obj(), "sort_rows", "Ordenar linhas") + QStringLiteral(" ") + estado);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar ordenação de linhas", true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self != nullptr ? self->interface_obj() : nullptr),
            gt(self != nullptr ? self->loc_obj() : nullptr, "error", "Erro"),
            gt(self != nullptr ? self->loc_obj() : nullptr, "error", "Erro"));
    }
}

}
