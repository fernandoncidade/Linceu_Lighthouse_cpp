#include "gmui_13_criar_submenu_colunas_coloridas.h"

#include <QAction>
#include <QMenu>
#include <QMetaObject>
#include <QSet>
#include <QVariantMap>

#include <algorithm>
#include <vector>

#include "gmui_01_MenuPersistente.h"
#include "gmui_15_selecionar_todas_cores_colunas.h"
#include "source/services/ob_09_GerenciadorColunas.h"
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

QString get_column_text(QObject* loc, const QVariantMap& dados_coluna, const QString& key) {
    const QString fallback = dados_coluna.value("nome", key).toString();
    const QString translation_key = dados_coluna.value("translation_key").toString();
    return translation_key.isEmpty() ? fallback : gt(loc, translation_key, fallback);
}
}

namespace source::ui::GerenciamentoMenusUI {

void criar_submenu_colunas_coloridas(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (!self || !menu_configuracoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(
            iface->property("gerenciador_colunas").value<QObject*>());
        auto* gerenciador_tabela = qobject_cast<source::services::GerenciadorTabela*>(
            iface->property("gerenciador_tabela").value<QObject*>());
        QObject* gerenciador_tabela_obj = iface->property("gerenciador_tabela").value<QObject*>();

        auto* submenu = new MenuPersistente(gt(self->loc_obj(), "color_columns", "Colorir colunas"), qobject_cast<QWidget*>(iface));
        menu_configuracoes->addMenu(submenu);

        auto* intern = new MenuPersistente(gt(self->loc_obj(), "columns", "Colunas"), qobject_cast<QWidget*>(iface));
        submenu->addMenu(intern);

        QSet<QString> colunas_coloridas;
        if (gerenciador_tabela != nullptr) {
            colunas_coloridas = gerenciador_tabela->colunas_para_colorir;
        }

        std::vector<QPair<QString, QVariantMap>> colunas_ordenadas;
        if (gerenciador_colunas != nullptr) {
            for (auto it = gerenciador_colunas->COLUNAS_DISPONIVEIS.cbegin();
                 it != gerenciador_colunas->COLUNAS_DISPONIVEIS.cend(); ++it) {
                colunas_ordenadas.emplace_back(it.key(), it.value());
            }
        }

        std::sort(colunas_ordenadas.begin(), colunas_ordenadas.end(),
                  [](const auto& a, const auto& b) {
                      return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
                  });

        QVariantMap acoes_colunas_coloridas;

        for (const auto& coluna : colunas_ordenadas) {
            const QString key = coluna.first;
            const QVariantMap dados_coluna = coluna.second;
            auto* a = new QAction(get_column_text(self->loc_obj(), dados_coluna, key), iface);
            a->setCheckable(true);
            if (key == QStringLiteral("tipo_operacao")) {
                a->setChecked(true);
            } else {
                a->setChecked(colunas_coloridas.contains(key));
            }
            a->setData(key);
            QObject::connect(a, &QAction::triggered, iface, [gerenciador_tabela, gerenciador_tabela_obj, key](bool checked) {
                if (gerenciador_tabela != nullptr) {
                    gerenciador_tabela->set_coluna_colorir(key, checked);
                } else if (gerenciador_tabela_obj != nullptr) {
                    QMetaObject::invokeMethod(gerenciador_tabela_obj, "set_coluna_colorir", Q_ARG(QString, key), Q_ARG(bool, checked));
                }
            });
            intern->addAction(a);
            acoes_colunas_coloridas.insert(key, QVariant::fromValue(a));
        }

        iface->setProperty("acoes_colunas_coloridas", acoes_colunas_coloridas);

        submenu->addSeparator();
        auto* selecionar_todas = new QAction(gt(self->loc_obj(), "select_all_columns", "Selecionar todas colunas"), iface);
        selecionar_todas->setShortcut(QStringLiteral("Ctrl+Shift+A"));
        QObject::connect(selecionar_todas, &QAction::triggered, iface, [self]() { selecionar_todas_cores_colunas(self); });
        submenu->addAction(selecionar_todas);

        auto* reset = new QAction(gt(self->loc_obj(), "reset_column_colors", "Resetar cores de colunas"), iface);
        QObject::connect(reset, &QAction::triggered, iface, [iface]() {
            QMetaObject::invokeMethod(iface, "redefinir_todas_colunas_cores");
        });
        submenu->addAction(reset);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu colunas coloridas", true);
    }
}

}
