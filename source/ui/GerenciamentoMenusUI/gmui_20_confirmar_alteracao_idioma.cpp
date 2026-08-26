#include "gmui_20_confirmar_alteracao_idioma.h"

#include <algorithm>
#include <limits>

#include <QAction>
#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QLabel>
#include <QMessageBox>
#include <QMetaObject>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QTableWidget>
#include <QVariantMap>

#include "gmui_04_criar_menu_principal.h"
#include "gmui_19_get_texto_traduzido_para_idioma.h"
#include "gmui_21_on_traducoes_carregadas.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {
namespace {

bool objeto_tem_texto_traduzivel(QObject* obj) {
    if (obj == nullptr) {
        return false;
    }

    static const char* props_texto[] = {
        "text",
        "windowTitle",
        "title",
        "placeholderText",
        "toolTip",
        "statusTip",
        "whatsThis"
    };

    for (const char* prop : props_texto) {
        const int prop_idx = obj->metaObject()->indexOfProperty(prop);
        if (prop_idx < 0) {
            continue;
        }
        const QString valor = obj->property(prop).toString().trimmed();
        if (!valor.isEmpty()) {
            return true;
        }
    }

    return false;
}

int calcular_unidades_traducao_ui(QObject* iface) {
    if (iface == nullptr) {
        return 1;
    }

    int total = 0;

    auto contabilizar_objeto = [&total](QObject* obj) {
        if (objeto_tem_texto_traduzivel(obj)) {
            ++total;
        }
    };

    contabilizar_objeto(iface);
    for (QObject* obj : iface->findChildren<QObject*>()) {
        contabilizar_objeto(obj);
    }

    QObject* tabela_obj = iface->property("tabela_dados").value<QObject*>();
    if (auto* tabela_widget = qobject_cast<QTableWidget*>(tabela_obj)) {
        total += std::max(1, tabela_widget->columnCount());
    } else if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
        if (tabela_view->model() != nullptr) {
            total += std::max(1, tabela_view->model()->columnCount());
        }
    }

    // Base mínima para evitar barra "travada" em interfaces menores.
    return std::max(32, total);
}

int estimar_unidades_traducao_tabela(QObject* iface) {
    if (iface == nullptr) {
        return 0;
    }

    QObject* tabela_obj = iface->property("tabela_dados").value<QObject*>();
    int total_linhas = 0;
    int colunas = 0;

    if (auto* tabela_widget = qobject_cast<QTableWidget*>(tabela_obj)) {
        total_linhas = tabela_widget->rowCount();
        colunas = tabela_widget->columnCount();
    } else if (auto* tabela_view = qobject_cast<QTableView*>(tabela_obj)) {
        if (auto* model = tabela_view->model()) {
            total_linhas = model->rowCount();
            colunas = model->columnCount();
        }
    }

    if (total_linhas <= 0) {
        return 0;
    }

    // tipo_operacao + tipo + atributos + autor + protegido + dimensoes (estimativa).
    const int unidades_por_linha = std::max(1, std::min(6, colunas > 0 ? colunas : 6));
    const qint64 total_estimado = static_cast<qint64>(total_linhas) * static_cast<qint64>(unidades_por_linha);
    return static_cast<int>(std::min<qint64>(total_estimado, std::numeric_limits<int>::max()));
}

void atualizar_feedback_traducao_inicial(QObject* iface, source::ui::LocalizadorQt* loc, int total) {
    if (iface == nullptr || loc == nullptr) {
        return;
    }

    if (QObject* barra_obj = iface->property("barra_progresso").value<QObject*>()) {
        if (auto* barra = qobject_cast<QProgressBar*>(barra_obj)) {
            barra->setValue(0);
            if (!barra->isVisible()) {
                barra->show();
            }
        } else {
            QMetaObject::invokeMethod(barra_obj, "setValue", Q_ARG(int, 0));
            QMetaObject::invokeMethod(barra_obj, "show");
        }
    }

    const QString prefix = loc->get_text("translating_table");
    const QString texto = QStringLiteral("%1 0% (0/%2)").arg(prefix).arg(total);
    if (QObject* rotulo_obj = iface->property("rotulo_resultado").value<QObject*>()) {
        if (auto* rotulo = qobject_cast<QLabel*>(rotulo_obj)) {
            rotulo->setText(texto);
        } else {
            QMetaObject::invokeMethod(rotulo_obj, "setText", Q_ARG(QString, texto));
        }
    }
}

}  // namespace

void confirmar_alteracao_idioma(source::ui::GerenciadorMenusUI* self, const QString& codigo_idioma) {
    try {
        if (!self || !self->interface_obj() || !self->loc_obj()) return;
        QObject* iface = self->interface_obj();
        auto* loc = qobject_cast<source::ui::LocalizadorQt*>(self->loc_obj());
        if (loc == nullptr) return;

        const QString idioma_atual = loc->property("idioma_atual").toString();
        if (codigo_idioma.isEmpty() || codigo_idioma == idioma_atual) return;

        const QString titulo_atual = get_texto_traduzido_para_idioma(self, QStringLiteral("warning"), idioma_atual);
        const QString titulo_alvo = get_texto_traduzido_para_idioma(self, QStringLiteral("warning"), codigo_idioma);

        QMessageBox caixa(qobject_cast<QWidget*>(iface));
        caixa.setIcon(QMessageBox::Warning);
        caixa.setWindowTitle((titulo_alvo != titulo_atual && !titulo_alvo.isEmpty()) ? titulo_atual + " / " + titulo_alvo : titulo_atual);

        QString msgAtual;
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, msgAtual), Q_ARG(QString, QStringLiteral("language_change_performance_warning")));
        caixa.setText(msgAtual);

        const QString msgAlvo = get_texto_traduzido_para_idioma(self, QStringLiteral("language_change_performance_warning"), codigo_idioma);
        if (!msgAlvo.isEmpty() && msgAlvo != msgAtual) caixa.setInformativeText(msgAlvo);

        caixa.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        caixa.setDefaultButton(QMessageBox::No);

        QString yes_atual;
        QString no_atual;
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, yes_atual), Q_ARG(QString, QStringLiteral("yes")));
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, no_atual), Q_ARG(QString, QStringLiteral("no")));

        if (yes_atual.isEmpty()) yes_atual = QStringLiteral("Yes");
        if (no_atual.isEmpty()) no_atual = QStringLiteral("No");

        const QString yes_alvo = get_texto_traduzido_para_idioma(self, QStringLiteral("yes"), codigo_idioma);
        const QString no_alvo = get_texto_traduzido_para_idioma(self, QStringLiteral("no"), codigo_idioma);

        const QString label_yes = (!yes_alvo.isEmpty() && yes_alvo.compare(yes_atual, Qt::CaseInsensitive) != 0)
                                      ? (yes_atual + QStringLiteral("/") + yes_alvo)
                                      : yes_atual;
        const QString label_no = (!no_alvo.isEmpty() && no_alvo.compare(no_atual, Qt::CaseInsensitive) != 0)
                                     ? (no_atual + QStringLiteral("/") + no_alvo)
                                     : no_atual;

        if (QAbstractButton* btn_yes = caixa.button(QMessageBox::Yes)) {
            btn_yes->setText(label_yes);
        }
        if (QAbstractButton* btn_no = caixa.button(QMessageBox::No)) {
            btn_no->setText(label_no);
        }

        const int resposta = caixa.exec();
        QVariantMap acoes = iface->property("_acoes_idioma").toMap();
        auto* acaoSelecionada = acoes.value(codigo_idioma).value<QAction*>();

        if (resposta == QMessageBox::Yes) {
            if (acaoSelecionada) acaoSelecionada->setChecked(true);

            const int total_ui = calcular_unidades_traducao_ui(iface);
            const int total_tabela_estimado = estimar_unidades_traducao_tabela(iface);
            const int total_geral = std::max(total_ui, total_ui + total_tabela_estimado);
            iface->setProperty("_traducao_em_execucao", true);
            iface->setProperty("_traducao_total_unidades", total_geral);
            iface->setProperty("_traducao_concluidas_unidades", 0);
            iface->setProperty("_traducao_unidades_ui_total", total_ui);
            iface->setProperty("_traducao_unidades_tabela_total", total_tabela_estimado);
            iface->setProperty("_traducao_ui_contabilizada", false);
            iface->setProperty("_trocando_idioma", true);
            iface->setProperty("_aguardando_conclusao_traducao", true);
            atualizar_feedback_traducao_inicial(iface, loc, total_geral);

            QMetaObject::invokeMethod(loc, "set_idioma", Qt::QueuedConnection, Q_ARG(QString, codigo_idioma));
        } else {
            auto* acaoAtual = acoes.value(idioma_atual).value<QAction*>();
            if (acaoAtual) acaoAtual->setChecked(true);
            if (acaoSelecionada) acaoSelecionada->setChecked(false);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao confirmar alteração de idioma", true);
    }
}

}
