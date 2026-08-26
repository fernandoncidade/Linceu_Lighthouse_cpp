#include "geve_02_alternar_visibilidade_coluna.h"

#include <QAction>
#include <QMetaObject>

#include "source/services/ob_09_GerenciadorColunas.h"
#include "source/services/ob_10_GerenciadorTabela.h"
#include "source/ui/ui_04_GerenciadorEventosUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEventosUI {

void alternar_visibilidade_coluna(source::ui::GerenciadorEventosUI* self, const QString& coluna, bool visivel) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        auto* interface_obj = self->interface_obj();
        QAction* acao = interface_obj->property("sender_action").value<QAction*>();

        QString chave = coluna;
        bool checked = visivel;
        if (acao != nullptr) {
            chave = acao->data().toString();
            checked = acao->isChecked();
        } else if (chave.isEmpty()) {
            return;
        }

        QObject* gerenciador_colunas = interface_obj->property("gerenciador_colunas").value<QObject*>();
        if (auto* gc = qobject_cast<source::services::GerenciadorColunas*>(gerenciador_colunas)) {
            if (gc->COLUNAS_DISPONIVEIS.contains(chave)) {
                QVariantMap coluna_data = gc->COLUNAS_DISPONIVEIS.value(chave);
                coluna_data.insert("visivel", checked);
                gc->COLUNAS_DISPONIVEIS.insert(chave, coluna_data);
                gc->salvar_configuracoes();
            }
        } else if (gerenciador_colunas != nullptr) {
            QMetaObject::invokeMethod(gerenciador_colunas, "salvar_configuracoes");
        }

        if (auto* gt = qobject_cast<source::services::GerenciadorTabela*>(
                interface_obj->property("gerenciador_tabela").value<QObject*>())) {
            gt->atualizar_visibilidade_colunas(true);
        } else {
            QMetaObject::invokeMethod(interface_obj, "atualizar_visibilidade_colunas");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao alternar visibilidade da coluna", true);
    }
}

}
