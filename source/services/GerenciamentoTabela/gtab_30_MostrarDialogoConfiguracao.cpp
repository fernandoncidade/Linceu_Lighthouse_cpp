#include "gtab_30_MostrarDialogoConfiguracao.h"

#include <QAction>
#include <QCursor>
#include <QMenu>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoTabela {

void mostrar_dialogo_configuracao(source::services::GerenciadorTabela* self, const QPoint& pos) {
    try {
        if (!self || !self->interface) return;

        auto* gc = qobject_cast<source::services::GerenciadorColunas*>(self->interface->property("gerenciador_colunas").value<QObject*>());
        if (!gc) return;

        QMenu menu;
        menu.setToolTipsVisible(true);

        QAction* titulo = menu.addAction(QStringLiteral("Configurar colunas"));
        titulo->setEnabled(false);
        menu.addSeparator();

        QList<QPair<QString, QVariantMap>> cols;
        for (auto it = gc->COLUNAS_DISPONIVEIS.constBegin(); it != gc->COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            cols.append({it.key(), it.value()});
        }
        std::sort(cols.begin(), cols.end(), [](const auto& a, const auto& b) {
            return a.second.value("ordem").toInt() < b.second.value("ordem").toInt();
        });

        for (const auto& c : cols) {
            const QString key = c.first;
            const QVariantMap col = c.second;
            QAction* a = menu.addAction(col.value("nome", key).toString());
            a->setCheckable(true);
            a->setChecked(col.value("visivel", true).toBool());
            QObject::connect(a, &QAction::triggered, self, [self, gc, key](bool checked) {
                QVariantMap col = gc->COLUNAS_DISPONIVEIS.value(key);
                col.insert("visivel", checked);
                gc->COLUNAS_DISPONIVEIS[key] = col;
                gc->salvar_configuracoes();
                self->atualizar_visibilidade_colunas(true);
            });
        }

        menu.addSeparator();
        QAction* acao_cores = menu.addAction(QStringLiteral("Mostrar cores nas colunas"));
        acao_cores->setCheckable(true);
        acao_cores->setChecked(self->cores_visiveis);
        QObject::connect(acao_cores, &QAction::triggered, self, [self](bool checked) {
            if (checked) self->ativar_cores();
            else self->ocultar_cores();
        });

        const QPoint global_pos = pos.isNull() ? QCursor::pos() : pos;
        menu.exec(global_pos);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro em mostrar_dialogo_configuracao", true);
    }
    
}
}  // namespace source::services::GerenciamentoTabela

