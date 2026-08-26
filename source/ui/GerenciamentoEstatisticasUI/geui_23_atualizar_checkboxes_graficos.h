#pragma once

#include <QHash>
#include <QString>
#include <QVariantHash>
#include <QVariantList>

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
void atualizar_checkboxes_graficos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos_atualizados = QVariantList(), const QHash<QString, bool>& estados_checkboxes = QHash<QString, bool>(), const QVariantHash& mapeamento_funcoes = QVariantHash());
}
