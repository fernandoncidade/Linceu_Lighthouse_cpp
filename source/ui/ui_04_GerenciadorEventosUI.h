#pragma once

#include <QString>

class QObject;

namespace source::ui {

class GerenciadorEventosUI {
public:
    explicit GerenciadorEventosUI(QObject* interface_principal);

    QObject* interface_obj() const;
    QObject* loc_obj() const;

    void alternar_filtro(const QString& filtro);
    void alternar_visibilidade_coluna(const QString& coluna, bool visivel);
    void resetar_colunas();
    void resetar_cores();
    void redefinir_todas_colunas_cores();
    void alterar_idioma(const QString& idioma);
    bool confirmar_mudanca_idioma(const QString& idioma);
    void limpar_dados_monitorados();
    void limpar_dados_interface();
    void reiniciar_aplicativo();
    void atualizar_interface();

private:
    QObject* interface_;
    QObject* loc_;
};

}  // namespace source::ui
