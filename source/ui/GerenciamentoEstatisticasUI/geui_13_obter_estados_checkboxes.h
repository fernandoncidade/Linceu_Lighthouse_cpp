#pragma once

#include <QHash>
#include <QString>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
QHash<QString, bool> obter_estados_checkboxes(source::ui::GerenciadorEstatisticasUI* self);
}
