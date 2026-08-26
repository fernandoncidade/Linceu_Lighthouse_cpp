#pragma once

#include <QHash>
#include <QString>
#include <QVariant>

class QObject;

namespace source::ui {

class GerenciadorEstatisticasUI {
public:
    explicit GerenciadorEstatisticasUI(QObject* interface_principal);

    QObject* interface_obj() const;
    QObject* loc_obj() const;
    QObject* evento_base_obj() const;

    void mostrar_estatisticas();
    void salvar_todos_graficos();
    void limpar_referencia_dialog();
    void atualizar_graficos_apos_mudanca_idioma();
    void criar_painel_selecao();
    void criar_painel_graficos();
    void criar_botao_toggle_painel();
    void criar_lista_graficos();
    void criar_mapeamento_funcoes();
    void popular_checkboxes();
    void verificar_estado_checkbox_todos();
    void alternar_todos_checkboxes(bool checked);
    QHash<QString, bool> obter_estados_checkboxes() const;
    void gerar_todos_graficos();
    void atualizar_graficos();
    void atualizar_graficos_sem_fechar();
    void regenerar_graficos_existentes();
    void salvar_graficos_selecionados();
    void toggle_painel_selecao();
    void atualizar_textos_painel_selecao();
    void atualizar_textos_checkboxes();
    void atualizar_layout_apos_mudanca_botao();
    void atualizar_checkboxes_graficos();
    void atualizar_abas_graficos();
    void atualizar_dados_graficos_com_novos_titulos();
    int calcular_largura_ideal() const;
    void ajustar_largura_painel_selecao();
    void traduzir_botoes_detalhes();
    void abrir_diretorio(const QString& caminho);

private:
    QObject* interface_;
    QObject* loc_;
    QObject* evento_base_;
};

}  // namespace source::ui
