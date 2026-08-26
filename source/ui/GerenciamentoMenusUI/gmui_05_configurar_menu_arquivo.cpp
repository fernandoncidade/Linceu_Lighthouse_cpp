#include "gmui_05_configurar_menu_arquivo.h"

#include <QAction>
#include <QMenu>
#include <QMetaObject>
#include <QVector>

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
}

namespace source::ui::GerenciamentoMenusUI {

void configurar_menu_arquivo(source::ui::GerenciadorMenusUI* self, QMenu* menu_arquivo) {
    try {
        if (!self || !menu_arquivo || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();

        struct Acao { QString key; const char* slot; QString shortcut; };
        const QVector<Acao> acoes = {
            {"select_dir", "selecionar_diretorio", "Ctrl+Shift+O"},
            {"start_stop", "alternar_analise_diretorio", "Ctrl+R"},
            {"pause_analysis", "pausar_monitoramento_ou_escaneamento", "Ctrl+P"},
            {"save_as", "abrir_salvar_como", "Ctrl+Shift+S"},
            {"save", "salvar_dados", "Ctrl+S"},
            {"statistics", "mostrar_estatisticas", "Ctrl+G"},
            {"clear_data", "limpar_dados", "Ctrl+L"},
            {"exit", "sair_aplicacao", "Ctrl+Q"}
        };

        for (const auto& a : acoes) {
            auto* item = new QAction(gt(self->loc_obj(), a.key, a.key), iface);
            if (!a.shortcut.isEmpty()) item->setShortcut(a.shortcut);
            QObject::connect(item, &QAction::triggered, iface, [iface, a]() {
                if (a.key == QStringLiteral("pause_analysis")) {
                    QObject* gerenciador_botoes = iface->property("gerenciador_botoes").value<QObject*>();
                    if (gerenciador_botoes != nullptr) {
                        QMetaObject::invokeMethod(gerenciador_botoes, "pausar_monitoramento_ou_escaneamento");
                        return;
                    }
                }
                QMetaObject::invokeMethod(iface, a.slot);
            });
            menu_arquivo->addAction(item);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar menu arquivo", true);
    }
}

}
