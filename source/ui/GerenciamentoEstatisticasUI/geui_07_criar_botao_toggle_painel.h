#pragma once

#include <QString>

namespace source::ui { class GerenciadorEstatisticasUI; }
class QWidget;

namespace source::ui::GerenciamentoEstatisticasUI {
QWidget* criar_botao_toggle_painel(source::ui::GerenciadorEstatisticasUI* self, const QString& texto);
}
