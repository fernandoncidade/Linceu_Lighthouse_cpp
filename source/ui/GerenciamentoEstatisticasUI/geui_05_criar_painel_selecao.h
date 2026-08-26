#pragma once

#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

class QWidget;

namespace source::ui::GerenciamentoEstatisticasUI {
QWidget* criar_painel_selecao(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos = QVariantList());
}
