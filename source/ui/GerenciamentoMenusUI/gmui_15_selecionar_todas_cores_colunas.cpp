#include "gmui_15_selecionar_todas_cores_colunas.h"

#include <QAction>
#include <QMessageBox>
#include <QMetaObject>
#include <QSet>
#include <QVariantMap>

#include "gmui_04_criar_menu_principal.h"
#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

void selecionar_todas_cores_colunas(source::ui::GerenciadorMenusUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        auto* gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(
            iface->property("gerenciador_colunas").value<QObject*>());
        auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
            iface->property("gerenciador_tabela").value<QObject*>());

        QSet<QString> novas_colunas;
        if (gerenciador_colunas != nullptr) {
            for (auto it = gerenciador_colunas->COLUNAS_DISPONIVEIS.cbegin();
                 it != gerenciador_colunas->COLUNAS_DISPONIVEIS.cend(); ++it) {
                novas_colunas.insert(it.key());
            }
        }

        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->set_colunas_colorir_em_massa(novas_colunas);
        }

        auto* acao_exportar_colunas_ativas = iface->property("acao_exportar_colunas_ativas").value<QAction*>();
        auto* acao_exportar_filtros_ativos = iface->property("acao_exportar_filtros_ativos").value<QAction*>();
        auto* acao_exportar_selecao = iface->property("acao_exportar_selecao").value<QAction*>();

        const bool exportar_colunas_ativas = (acao_exportar_colunas_ativas != nullptr) ? acao_exportar_colunas_ativas->isChecked() : false;
        const bool exportar_filtros_ativos = (acao_exportar_filtros_ativos != nullptr) ? acao_exportar_filtros_ativos->isChecked() : false;
        const bool exportar_selecao = (acao_exportar_selecao != nullptr) ? acao_exportar_selecao->isChecked() : false;

        criar_menu_principal(self);

        acao_exportar_colunas_ativas = iface->property("acao_exportar_colunas_ativas").value<QAction*>();
        acao_exportar_filtros_ativos = iface->property("acao_exportar_filtros_ativos").value<QAction*>();
        acao_exportar_selecao = iface->property("acao_exportar_selecao").value<QAction*>();

        if (acao_exportar_colunas_ativas != nullptr) acao_exportar_colunas_ativas->setChecked(exportar_colunas_ativas);
        if (acao_exportar_filtros_ativos != nullptr) acao_exportar_filtros_ativos->setChecked(exportar_filtros_ativos);
        if (acao_exportar_selecao != nullptr) acao_exportar_selecao->setChecked(exportar_selecao);

        QString titulo = QStringLiteral("Sucesso");
        QString mensagem = QStringLiteral("Cores aplicadas com sucesso");
        if (loc != nullptr) {
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, titulo), Q_ARG(QString, QStringLiteral("success")));
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, mensagem), Q_ARG(QString, QStringLiteral("colors_applied_success")));
        }

        QMessageBox::information(qobject_cast<QWidget*>(iface), titulo, mensagem);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao iniciar coloração de todas as colunas", true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self != nullptr ? self->interface_obj() : nullptr),
            QStringLiteral("Erro"),
            QStringLiteral("Erro ao iniciar coloração de todas as colunas"));
    }
}

}
