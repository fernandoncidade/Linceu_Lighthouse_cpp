#pragma once

#include <QHash>
#include <QString>

class QObject;

namespace source::ui {

class GerenciadorMenusUI {
public:
    explicit GerenciadorMenusUI(QObject* interface_principal);

    QObject* interface_obj() const;
    QObject* loc_obj() const;

    void criar_menu_principal();
    void configurar_menu_arquivo();
    void configurar_menu_configuracoes();
    void configurar_menu_opcoes();
    void criar_submenu_cores();
    void criar_submenu_colunas();
    void alternar_ordenacao_linhas();
    void criar_submenu_colunas_coloridas();
    void selecionar_todas_colunas();
    void selecionar_todas_cores_colunas();
    void criar_submenu_exportacao();
    void resetar_opcoes_exportacao();
    void criar_submenu_idiomas();
    QString get_texto_traduzido_para_idioma(const QString& chave, const QString& idioma);
    void confirmar_alteracao_idioma(const QString& codigo_idioma);
    void on_traducoes_carregadas(const QString& idioma);
    void exibir_sobre();
    void exibir_manual();
    void exibir_ajuda();

private:
    QObject* interface_;
    QObject* loc_;
    QHash<QString, QObject*> acoes_colunas_;
    QObject* gerenciador_cores_;
    QHash<QString, QObject*> acoes_idioma_;
    bool trocando_idioma_;
    bool aguardando_conclusao_traducao_;
};

}  // namespace source::ui
