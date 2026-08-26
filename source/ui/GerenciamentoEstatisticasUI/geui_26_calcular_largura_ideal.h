#pragma once

#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
int calcular_largura_ideal(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos = QVariantList());
}
