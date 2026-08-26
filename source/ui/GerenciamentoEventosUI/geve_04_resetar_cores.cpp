#include "geve_04_resetar_cores.h"

#include <QAction>
#include <exception>

#include <QHash>
#include <QMessageBox>
#include <QMetaObject>
#include <QStringList>

#include "source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h"
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

void resetar_cores(source::ui::GerenciadorEventosUI* self) {
    try {
        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();
        const int resposta = QMessageBox::question(qobject_cast<QWidget*>(interface_obj), get_text(loc, "confirm"), get_text(loc, "reset_colors_confirm"), QMessageBox::Yes | QMessageBox::No);
        if (resposta != QMessageBox::Yes) {
            return;
        }

        auto* acao_exportar_colunas_ativas = interface_obj->property("acao_exportar_colunas_ativas").value<QAction*>();
        auto* acao_exportar_filtros_ativos = interface_obj->property("acao_exportar_filtros_ativos").value<QAction*>();
        auto* acao_exportar_selecao = interface_obj->property("acao_exportar_selecao").value<QAction*>();
        const bool exportar_colunas_ativas = (acao_exportar_colunas_ativas != nullptr) ? acao_exportar_colunas_ativas->isChecked() : false;
        const bool exportar_filtros_ativos = (acao_exportar_filtros_ativos != nullptr) ? acao_exportar_filtros_ativos->isChecked() : false;
        const bool exportar_selecao = (acao_exportar_selecao != nullptr) ? acao_exportar_selecao->isChecked() : false;

        QObject* gerenciador_cores_obj = interface_obj->property("gerenciador_cores").value<QObject*>();

        if (auto* gerenciador_cores = qobject_cast<source::ui::GerenciamentoMenusUI::GerenciadorCores*>(gerenciador_cores_obj)) {
            const QHash<QString, QString> cores = {
                {"op_renamed", "#00ff00"}, {"op_added", "#0000ff"}, {"op_deleted", "#ff0000"},
                {"op_modified", "#ff6200"}, {"op_moved", "#ff00ff"}, {"op_scanned", "#808080"}
            };
            for (auto it = cores.begin(); it != cores.end(); ++it) {
                gerenciador_cores->definir_cor(it.key(), it.value());
            }
            gerenciador_cores->salvar_cores();
            gerenciador_cores->atualizar_cores_no_sistema();
        } else if (gerenciador_cores_obj != nullptr) {
            const QHash<QString, QString> cores = {
                {"op_renamed", "#00ff00"}, {"op_added", "#0000ff"}, {"op_deleted", "#ff0000"},
                {"op_modified", "#ff6200"}, {"op_moved", "#ff00ff"}, {"op_scanned", "#808080"}
            };
            for (auto it = cores.begin(); it != cores.end(); ++it) {
                QMetaObject::invokeMethod(gerenciador_cores_obj, "definir_cor", Q_ARG(QString, it.key()), Q_ARG(QString, it.value()));
            }
            QMetaObject::invokeMethod(gerenciador_cores_obj, "salvar_cores");
            QMetaObject::invokeMethod(gerenciador_cores_obj, "atualizar_cores_no_sistema");
        }

        QMetaObject::invokeMethod(interface_obj, "criar_menu_principal");

        if (acao_exportar_colunas_ativas != nullptr) acao_exportar_colunas_ativas->setChecked(exportar_colunas_ativas);
        if (acao_exportar_filtros_ativos != nullptr) acao_exportar_filtros_ativos->setChecked(exportar_filtros_ativos);
        if (acao_exportar_selecao != nullptr) acao_exportar_selecao->setChecked(exportar_selecao);

        QMessageBox::information(qobject_cast<QWidget*>(interface_obj), get_text(loc, "success"), get_text(loc, "colors_reset_success"));
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao restaurar cores padrão: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::critical(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            format_text(QString("%1: {}").arg(get_text(self->loc_obj(), "error")), {QString::fromUtf8(e.what())}));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao restaurar cores padrão", true);
        QMessageBox::critical(qobject_cast<QWidget*>(self->interface_obj()), get_text(self->loc_obj(), "error"), get_text(self->loc_obj(), "error"));
    }
}

}
