#pragma once

#include <QVariantHash>
#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
QVariantHash criar_mapeamento_funcoes(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos_atualizados = QVariantList());
}
