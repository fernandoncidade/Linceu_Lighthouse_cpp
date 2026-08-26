#pragma once

#include <QVariantHash>
#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
void atualizar_dados_graficos_com_novos_titulos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos_atualizados = QVariantList(), const QVariantHash& mapeamento_funcoes = QVariantHash());
}
