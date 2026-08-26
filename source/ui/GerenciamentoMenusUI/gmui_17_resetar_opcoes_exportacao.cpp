#include "gmui_17_resetar_opcoes_exportacao.h"

#include <QAction>
#include <QMessageBox>
#include <QMetaObject>

#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

void resetar_opcoes_exportacao(source::ui::GerenciadorMenusUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();
        if (auto* a = iface->property("acao_exportar_colunas_ativas").value<QAction*>()) a->setChecked(false);
        if (auto* a = iface->property("acao_exportar_filtros_ativos").value<QAction*>()) a->setChecked(false);
        if (auto* a = iface->property("acao_exportar_selecao").value<QAction*>()) a->setChecked(false);

        QString titulo = QStringLiteral("Sucesso");
        QString mensagem = QStringLiteral("Opções de exportação restauradas");
        if (loc != nullptr) {
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, titulo), Q_ARG(QString, QStringLiteral("success")));
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, mensagem), Q_ARG(QString, QStringLiteral("export_options_reset_success")));
        }

        QMessageBox::information(qobject_cast<QWidget*>(iface), titulo, mensagem);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao restaurar opções de exportação", true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self != nullptr ? self->interface_obj() : nullptr),
            QStringLiteral("Erro"),
            QStringLiteral("Erro ao restaurar opções de exportação"));
    }
}

}
