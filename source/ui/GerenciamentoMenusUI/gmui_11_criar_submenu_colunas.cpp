#include "gmui_11_criar_submenu_colunas.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMetaObject>
#include <QVariant>
#include <QVariantMap>

#include <algorithm>
#include <vector>

#include "gmui_01_MenuPersistente.h"
#include "gmui_12_alternar_ordenacao_linhas.h"
#include "gmui_14_selecionar_todas_colunas.h"
#include "source/services/ob_09_GerenciadorColunas.h"
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

void criar_submenu_colunas(source::ui::GerenciadorMenusUI* self, QMenu* menu_configuracoes) {
    try {
        if (!self || !menu_configuracoes || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        auto* gerenciador_colunas = qobject_cast<source::services::GerenciadorColunas*>(
            iface->property("gerenciador_colunas").value<QObject*>());

        auto* submenu = new MenuPersistente(gt(self->loc_obj(), "configure_columns", "Configurar colunas"), qobject_cast<QWidget*>(iface));
        menu_configuracoes->addMenu(submenu);

        auto* intern = new MenuPersistente(gt(self->loc_obj(), "columns", "Colunas"), qobject_cast<QWidget*>(iface));
        submenu->addMenu(intern);

        auto* grupo = new QActionGroup(iface);
        grupo->setExclusive(false);

        QVariantMap acoes_colunas;
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

        for (const auto& coluna : colunas_ordenadas) {
            const QString key = coluna.first;
            const QVariantMap dados_coluna = coluna.second;

            auto* a = new QAction(get_column_text(self->loc_obj(), dados_coluna, key), iface);
            a->setCheckable(true);
            a->setChecked(dados_coluna.value("visivel").toBool());
            a->setData(key);
            QObject::connect(a, &QAction::triggered, iface, [iface, a]() {
                iface->setProperty("sender_action", QVariant::fromValue(a));
                QMetaObject::invokeMethod(iface, "alternar_visibilidade_coluna");
                iface->setProperty("sender_action", QVariant());
            });
            grupo->addAction(a);
            intern->addAction(a);
            acoes_colunas.insert(key, QVariant::fromValue(a));
        }
        iface->setProperty("acoes_colunas", acoes_colunas);

        submenu->addSeparator();
        auto* selecionar_todas = new QAction(gt(self->loc_obj(), "select_all_columns", "Selecionar todas colunas"), iface);
        selecionar_todas->setShortcut(QStringLiteral("Ctrl+A"));
        QObject::connect(selecionar_todas, &QAction::triggered, iface, [self]() { selecionar_todas_colunas(self); });
        submenu->addAction(selecionar_todas);

        auto* resetar = new QAction(gt(self->loc_obj(), "reset_columns", "Resetar colunas"), iface);
        QObject::connect(resetar, &QAction::triggered, iface, [iface]() { QMetaObject::invokeMethod(iface, "resetar_colunas"); });
        submenu->addAction(resetar);

        submenu->addSeparator();
        auto* ordenar = new QAction(gt(self->loc_obj(), "sort_rows", "Ordenar linhas"), iface);
        ordenar->setCheckable(true);
        ordenar->setChecked(iface->property("_ordenacao_linhas_habilitada").toBool());
        ordenar->setShortcut(QStringLiteral("Ctrl+O"));
        QObject::connect(ordenar, &QAction::triggered, iface, [self]() { alternar_ordenacao_linhas(self); });
        submenu->addAction(ordenar);
        iface->setProperty("acao_ordenar_linhas", QVariant::fromValue(ordenar));
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar submenu colunas", true);
    }
}

}
