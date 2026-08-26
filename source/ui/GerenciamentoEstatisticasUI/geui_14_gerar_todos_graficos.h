#pragma once

#include <QVariantList>
#include <QSize>
#include <QString>
#include <QVariant>

class QObject;
class QWidget;

namespace source::ui { class GerenciadorEstatisticasUI; }

namespace source::ui::GerenciamentoEstatisticasUI {
QWidget* criar_widget_grafico_localizado(const QString& titulo, const QVariant& fig, QObject* loc = nullptr);
void gerar_todos_graficos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList& graficos = QVariantList());
void cancelar_geracao_graficos(source::ui::GerenciadorEstatisticasUI* self);
bool renderizar_grafico_para_imagem(
    const QString& titulo,
    const QVariant& fig,
    const QString& arquivo_destino,
    const QSize& tamanho = QSize(1920, 1200),
    QObject* loc = nullptr);
}
