#pragma once

#include <QPushButton>

namespace source::ui::GerenciamentoEstatisticasUI {

class BotaoRotacionado : public QPushButton {
public:
    explicit BotaoRotacionado(const QString& texto, QWidget* parent = nullptr);
    void setText(const QString& texto);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void calcular_tamanho();
};

}
