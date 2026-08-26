#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

QColor calcular_cor_texto_ideal(source::services::GerenciadorTabela* self, const QColor& cor_fundo, bool eh_coluna_personalizada);
bool eh_cor_fundo_padrao(source::services::GerenciadorTabela* self, const QColor& cor);

}  // namespace source::services::GerenciamentoTabela
