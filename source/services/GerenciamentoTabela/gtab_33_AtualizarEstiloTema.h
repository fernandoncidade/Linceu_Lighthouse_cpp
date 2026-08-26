#pragma once

#include <QColor>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVariant>

namespace source::services { class GerenciadorTabela; }

namespace source::services::GerenciamentoTabela {

void atualizar_estilo_tema(source::services::GerenciadorTabela* self, const QString& novo_tema);
void atualizar_cores_celulas_incolores(source::services::GerenciadorTabela* self, const QString& novo_tema);
bool eh_cor_padrao_qualquer_tema(source::services::GerenciadorTabela* self, const QColor& cor);
void forcar_reconstrucao_todas_celulas(source::services::GerenciadorTabela* self, const QString& novo_tema);

}  // namespace source::services::GerenciamentoTabela
