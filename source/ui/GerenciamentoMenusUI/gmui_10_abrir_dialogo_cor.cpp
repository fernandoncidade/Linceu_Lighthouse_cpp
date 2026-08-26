#include "gmui_10_abrir_dialogo_cor.h"

#include <QAction>
#include <QColor>
#include <QMessageBox>
#include <QMetaObject>
#include <QPointer>

#include "gmui_02_GerenciadorCores.h"
#include "gmui_04_criar_menu_principal.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

void abrir_dialogo_cor(source::ui::GerenciadorMenusUI* self, const QString& tipo_operacao) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();
        auto* gerenciador_cores = qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(
            iface->property("gerenciador_cores").value<QObject*>());

        QString nome = tipo_operacao;
        if (loc) {
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, nome), Q_ARG(QString, tipo_operacao));
        }

        QString prefix = QStringLiteral("Select color for");
        if (loc) {
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, prefix), Q_ARG(QString, QStringLiteral("select_color_for")));
        }
        const QString titulo = prefix + QStringLiteral(" ") + nome;

        const QColor cor_atual = QColor((gerenciador_cores != nullptr)
                                            ? gerenciador_cores->obter_cor_hex(tipo_operacao)
                                            : QStringLiteral("#333333"));

        auto* dialogo = new source::ui::DialogoPaletaCores(cor_atual, iface, titulo, nullptr, tipo_operacao);
        source::ui::GerenciamentoMenusUI::GerenciadorCores::aplicar_icone_paleta(dialogo, QStringLiteral("paleta"));
        dialogo->show();
        QPointer<source::ui::DialogoPaletaCores> dialogo_ptr(dialogo);

        QObject::connect(dialogo, &source::ui::DialogoPaletaCores::corSelecionada, iface, [self, iface, loc, tipo_operacao, gerenciador_cores, dialogo_ptr](const QColor& nova_cor) {
            if (!nova_cor.isValid()) {
                return;
            }

            if (gerenciador_cores != nullptr) {
                gerenciador_cores->definir_cor(tipo_operacao, nova_cor.name());
                gerenciador_cores->salvar_cores();
                gerenciador_cores->atualizar_cores_no_sistema();
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

            QString titulo_msg = QStringLiteral("Success");
            QString mensagem = QStringLiteral("Color changed successfully");
            if (loc != nullptr) {
                QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, titulo_msg), Q_ARG(QString, QStringLiteral("success")));
                QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, mensagem), Q_ARG(QString, QStringLiteral("color_changed_success")));
            }

            QMessageBox::information(qobject_cast<QWidget*>(iface), titulo_msg, mensagem);
            if (!dialogo_ptr.isNull()) {
                dialogo_ptr->close();
            }
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao abrir diálogo de cor", true);
        if (self != nullptr && self->interface_obj() != nullptr) {
            QString titulo = QStringLiteral("Error");
            QString mensagem = QStringLiteral("Error opening color dialog");
            if (self->loc_obj() != nullptr) {
                QMetaObject::invokeMethod(self->loc_obj(), "get_text", Q_RETURN_ARG(QString, titulo), Q_ARG(QString, QStringLiteral("error")));
                QMetaObject::invokeMethod(self->loc_obj(), "get_text", Q_RETURN_ARG(QString, mensagem), Q_ARG(QString, QStringLiteral("error")));
            }
            QMessageBox::critical(qobject_cast<QWidget*>(self->interface_obj()), titulo, mensagem);
        }
    }
}

}
