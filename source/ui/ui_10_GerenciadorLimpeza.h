#pragma once

class QObject;

namespace source::ui {

class GerenciadorLimpeza {
public:
    explicit GerenciadorLimpeza(QObject* interface_principal);

    void limpar_dados();

private:
    QObject* interface_;
};

}  // namespace source::ui
