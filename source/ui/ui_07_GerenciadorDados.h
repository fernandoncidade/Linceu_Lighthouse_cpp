#pragma once

class QObject;

namespace source::ui {

class GerenciadorDados {
public:
    explicit GerenciadorDados(QObject* interface_principal);

    void salvar_dados();
    void abrir_salvar_como();

private:
    QObject* interface_;
};

}  // namespace source::ui
