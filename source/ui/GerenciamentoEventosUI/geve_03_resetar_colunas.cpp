#include "geve_03_resetar_colunas.h"

#include <QAction>
#include <exception>

#include <QHash>
#include <QMessageBox>
#include <QMetaObject>
#include <QStringList>
#include <QVariantMap>

#include "source/services/ob_09_GerenciadorColunas.h"
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

void resetar_colunas(source::ui::GerenciadorEventosUI* self) {
    try {
        auto* interface_obj = self->interface_obj();
        auto* loc = self->loc_obj();

        const QHash<QString, bool> colunas_padrao = {
            {"tipo_operacao", true}, {"nome", true}, {"dir_anterior", true}, {"dir_atual", true},
            {"data_criacao", false}, {"data_modificacao", true}, {"data_acesso", false}, {"tipo", true},
            {"size_b", false}, {"size_kb", true}, {"size_mb", false}, {"size_gb", false}, {"size_tb", false},
            {"atributos", false}, {"autor", false}, {"dimensoes", false}, {"duracao", false}, {"taxa_bits", false},
            {"protegido", false}, {"paginas", false}, {"linhas", false}, {"palavras", false}, {"paginas_estimadas", false},
            {"linhas_codigo", false}, {"total_linhas", false}, {"slides_estimados", false}, {"arquivos", false},
            {"unzipped_b", false}, {"unzipped_kb", false}, {"unzipped_mb", false}, {"unzipped_gb", false},
            {"unzipped_tb", false}, {"slides", false}, {"binary_file_b", false}, {"binary_file_kb", false},
            {"binary_file_mb", false}, {"binary_file_gb", false}, {"binary_file_tb", false}, {"planilhas", false},
            {"colunas", false}, {"registros", false}, {"tabelas", false}, {"timestamp", true},
            {"monitor_profile_name", false}, {"sha256_atual", false}, {"baseline_status", false},
            {"alerta_resumo", true}, {"session_label", false}, {"diff_resumo", false},
            {"risk_score", false}, {"risk_level", true}, {"actor_user", false}, {"actor_process", false},
            {"publisher", false}, {"quarantine_status", false}, {"evidence_id", false}
        };

        if (auto* gc = qobject_cast<source::services::GerenciadorColunas*>(interface_obj->property("gerenciador_colunas").value<QObject*>())) {
            for (auto it = colunas_padrao.begin(); it != colunas_padrao.end(); ++it) {
                if (!gc->COLUNAS_DISPONIVEIS.contains(it.key())) {
                    continue;
                }
                QVariantMap coluna_data = gc->COLUNAS_DISPONIVEIS.value(it.key());
                coluna_data.insert("visivel", it.value());
                gc->COLUNAS_DISPONIVEIS.insert(it.key(), coluna_data);
            }
            gc->salvar_configuracoes();
        }

        const QVariantMap acoes_colunas = interface_obj->property("acoes_colunas").toMap();
        for (auto it = acoes_colunas.cbegin(); it != acoes_colunas.cend(); ++it) {
            auto* acao = it.value().value<QAction*>();
            if (acao == nullptr) {
                acao = qobject_cast<QAction*>(it.value().value<QObject*>());
            }
            if (acao == nullptr) {
                continue;
            }
            acao->setChecked(colunas_padrao.value(it.key(), false));
        }

        if (auto* gt = qobject_cast<source::services::GerenciadorTabela*>(interface_obj->property("gerenciador_tabela").value<QObject*>())) {
            gt->atualizar_visibilidade_colunas(true);
        } else {
            QMetaObject::invokeMethod(interface_obj, "atualizar_visibilidade_colunas");
        }

        QMessageBox::information(qobject_cast<QWidget*>(interface_obj), get_text(loc, "success"), get_text(loc, "columns_reset_success"));
    } catch (const std::exception& e) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao resetar colunas: %1").arg(QString::fromUtf8(e.what())), true);
        QMessageBox::warning(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            format_text(get_text(self->loc_obj(), "columns_reset_error"), {QString::fromUtf8(e.what())}));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao resetar colunas", true);
        QMessageBox::warning(
            qobject_cast<QWidget*>(self->interface_obj()),
            get_text(self->loc_obj(), "error"),
            get_text(self->loc_obj(), "columns_reset_error"));
    }
}

}
