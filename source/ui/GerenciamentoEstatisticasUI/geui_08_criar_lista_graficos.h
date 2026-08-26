#pragma once

#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
QVariantList criar_lista_graficos(source::ui::GerenciadorEstatisticasUI* self);
}
