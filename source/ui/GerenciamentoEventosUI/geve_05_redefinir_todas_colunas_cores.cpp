#include "geve_05_redefinir_todas_colunas_cores.h"

#include <QAction>
#include <exception>

#include <QMessageBox>
#include <QMetaObject>
#include <QStringList>
#include <QVariantMap>

#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text;
}

QString format_text(QString pattern, const QStringList& args) {
    for (const QString& value : args) {
        const int brace_pos = pattern.indexOf(QStringLiteral("{}"));
        if (brace_pos >= 0) {
            pattern.replace(brace_pos, 2, value);
        } else if (pattern.contains(QLatin1Char('%'))) {
            pattern = pattern.arg(value);
        }
    }
    return pattern;
}
}

namespace source::ui::GerenciamentoEventosUI {

void redefinir_todas_colunas_cores(source::ui::GerenciadorEventosUI* self) {
    try {
        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();
        const int resposta = QMessageBox::question(qobject_cast<QWidget*>(interface_obj), get_text(loc, "confirm"), get_text(loc, "reset_column_color_confirm"), QMessageBox::Yes | QMessageBox::No);
        if (resposta != QMessageBox::Yes) {
            return;
        }

        auto* acao_exportar_colunas_ativas = interface_obj->property("acao_exportar_colunas_ativas").value<QAction*>();
        auto* acao_exportar_filtros_ativos = interface_obj->property("acao_exportar_filtros_ativos").value<QAction*>();
        auto* acao_exportar_selecao = interface_obj->property("acao_exportar_selecao").value<QAction*>();
        const bool exportar_colunas_ativas = (acao_exportar_colunas_ativas != nullptr) ? acao_exportar_colunas_ativas->isChecked() : false;
        const bool exportar_filtros_ativos = (acao_exportar_filtros_ativos != nullptr) ? acao_exportar_filtros_ativos->isChecked() : false;
        const bool exportar_selecao = (acao_exportar_selecao != nullptr) ? acao_exportar_selecao->isChecked() : false;

        if (auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(interface_obj->property("gerenciador_tabela").value<QObject*>())) {
            gerenciador_tabela->colunas_para_colorir.clear();
            gerenciador_tabela->colunas_para_colorir.insert(QStringLiteral("tipo_operacao"));
            gerenciador_tabela->salvar_configuracoes_cores();
            gerenciador_tabela->redefinir_cores_todas_colunas();
        } else if (QObject* gerenciador_tabela_obj = interface_obj->property("gerenciador_tabela").value<QObject*>()) {
            QMetaObject::invokeMethod(gerenciador_tabela_obj, "redefinir_cores_todas_colunas");
            QMetaObject::invokeMethod(gerenciador_tabela_obj, "salvar_configuracoes_cores");
        }

        const QVariantMap acoes_colunas_coloridas = interface_obj->property("acoes_colunas_coloridas").toMap();
        for (auto it = acoes_colunas_coloridas.cbegin(); it != acoes_colunas_coloridas.cend(); ++it) {
            if (auto* acao = it.value().value<QAction*>()) {
                acao->setChecked(it.key() == QStringLiteral("tipo_operacao"));
            }
        }

        QObject* gerenciador_menus_ui = interface_obj->property("gerenciador_menus_ui").value<QObject*>();
        if (gerenciador_menus_ui != nullptr) {
            QMetaObject::invokeMethod(gerenciador_menus_ui, "criar_menu_principal");
        } else {
            QMetaObject::invokeMethod(interface_obj, "criar_menu_principal");
        }
        if (acao_exportar_colunas_ativas != nullptr) acao_exportar_colunas_ativas->setChecked(exportar_colunas_ativas);
        if (acao_exportar_filtros_ativos != nullptr) acao_exportar_filtros_ativos->setChecked(exportar_filtros_ativos);
        if (acao_exportar_selecao != nullptr) acao_exportar_selecao->setChecked(exportar_selecao);

        QMessageBox::information(qobject_cast<QWidget*>(interface_obj), get_text(loc, "success"), get_text(loc, "column_colors_reset_success"));
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao redefinir cores das colunas: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            format_text(QString("%1: {}").arg(get_text(self->loc_obj(), "error")), {QString::fromUtf8(e.what())}));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao redefinir cores das colunas", true);
        QMessageBox::critical(qobject_cast<QWidget*>(self->interface_obj()), get_text(self->loc_obj(), "error"), get_text(self->loc_obj(), "error"));
    }
}

}
