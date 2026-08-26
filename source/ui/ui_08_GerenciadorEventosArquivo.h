#pragma once

#include <QVariant>

class QObject;

namespace source::ui {

class GerenciadorEventosArquivo {
public:
    explicit GerenciadorEventosArquivo(QObject* interface_principal);

    QVariant verificar_movimentacao(const QVariant& evento);
    void adicionar_evento(const QVariant& evento);
    void limpar_eventos();

private:
    QObject* interface_;
};

}  // namespace source::ui
