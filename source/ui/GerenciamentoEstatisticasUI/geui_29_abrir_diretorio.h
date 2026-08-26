#pragma once

#include <QString>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
void abrir_diretorio(source::ui::GerenciadorEstatisticasUI* self, const QString& caminho);
}
