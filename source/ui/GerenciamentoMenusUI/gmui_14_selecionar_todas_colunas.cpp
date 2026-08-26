#include "gmui_14_selecionar_todas_colunas.h"

#include <QAction>
#include <QMessageBox>
#include <QMetaObject>
#include <QVariantMap>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

void selecionar_todas_colunas(source::ui::GerenciadorMenusUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        QVariantMap mapa = iface->property("acoes_colunas").toMap();
        for (auto it = mapa.begin(); it != mapa.end(); ++it) {
            if (auto* acao = it.value().value<QAction*>()) {
                acao->setChecked(true);
            }
        }

        auto* gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(
            iface->property("gerenciador_colunas").value<QObject*>());
        if (gerenciador_colunas != nullptr) {
            for (auto it = mapa.cbegin(); it != mapa.cend(); ++it) {
                if (gerenciador_colunas->COLUNAS_DISPONIVEIS.contains(it.key())) {
                    QVariantMap dados = gerenciador_colunas->COLUNAS_DISPONIVEIS.value(it.key());
                    dados.insert("visivel", true);
                    gerenciador_colunas->COLUNAS_DISPONIVEIS.insert(it.key(), dados);
                }
            }
            gerenciador_colunas->salvar_configuracoes();
        }

        auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
            iface->property("gerenciador_tabela").value<QObject*>());
        if (gerenciador_tabela != nullptr) {
            gerenciador_tabela->atualizar_visibilidade_colunas(true);
        } else {
            QMetaObject::invokeMethod(iface, "atualizar_visibilidade_colunas");
        }

        QString titulo = QStringLiteral("Sucesso");
        QString mensagem = QStringLiteral("Colunas selecionadas com sucesso");
        if (loc != nullptr) {
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, titulo), Q_ARG(QString, QStringLiteral("success")));
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, mensagem), Q_ARG(QString, QStringLiteral("columns_select_success")));
        }

        QMessageBox::information(qobject_cast<QWidget*>(iface), titulo, mensagem);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao selecionar todas as colunas", true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self != nullptr ? self->interface_obj() : nullptr),
            QStringLiteral("Erro"),
            QStringLiteral("Erro ao selecionar todas as colunas"));
    }
}

}
