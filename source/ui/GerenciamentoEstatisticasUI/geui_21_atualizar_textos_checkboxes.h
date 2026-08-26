#pragma once

#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
void atualizar_textos_checkboxes(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos = QVariantList());
}
