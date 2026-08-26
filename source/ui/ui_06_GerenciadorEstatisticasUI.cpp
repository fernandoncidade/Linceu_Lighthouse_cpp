#include "ui_06_GerenciadorEstatisticasUI.h"

#include <QMetaObject>

#include "source/ui/GerenciamentoEstatisticasUI/geui_01_mostrar_estatisticas.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_02_salvar_todos_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_03_limpar_referencia_dialog.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_04_atualizar_graficos_apos_mudanca_idioma.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_05_criar_painel_selecao.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_06_criar_painel_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_07_criar_botao_toggle_painel.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_08_criar_lista_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_09_criar_mapeamento_funcoes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_10_popular_checkboxes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_11_verificar_estado_checkbox_todos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_12_alternar_todos_checkboxes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_13_obter_estados_checkboxes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_14_gerar_todos_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_15_atualizar_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_16_atualizar_graficos_sem_fechar.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_17_regenerar_graficos_existentes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_18_salvar_graficos_selecionados.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_19_toggle_painel_selecao.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_20_atualizar_textos_painel_selecao.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_21_atualizar_textos_checkboxes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_22_atualizar_layout_apos_mudanca_botao.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_23_atualizar_checkboxes_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_24_atualizar_abas_graficos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_25_atualizar_dados_graficos_com_novos_titulos.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_26_calcular_largura_ideal.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_27_ajustar_largura_painel_selecao.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_28_traduzir_botoes_detalhes.h"
#include "source/ui/GerenciamentoEstatisticasUI/geui_29_abrir_diretorio.h"

#include "source/ui/ui_12_LocalizadorQt.h"

#include "source/utils/LogManager.h"

namespace source::ui {

GerenciadorEstatisticasUI::GerenciadorEstatisticasUI(QObject* interface_principal)
    : interface_(interface_principal),
      loc_(nullptr),
    evento_base_(nullptr) {
    try {
        if (interface_ != nullptr) {
            loc_ = interface_->property("loc").value<QObject*>();
            evento_base_ = interface_->property("evento_base").value<QObject*>();
        }

        if (loc_ != nullptr) {
            if (auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(loc_)) {
                QObject::connect(loc_qt, &source::ui::LocalizadorQt::idioma_alterado, interface_,
                                 [this](const QString&) { atualizar_graficos_apos_mudanca_idioma(); });
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorEstatisticasUI", true);
    }
}

QObject* GerenciadorEstatisticasUI::interface_obj() const { return interface_; }
QObject* GerenciadorEstatisticasUI::loc_obj() const { return loc_; }
QObject* GerenciadorEstatisticasUI::evento_base_obj() const {
    if (interface_ != nullptr) {
        QObject* atual = interface_->property("evento_base").value<QObject*>();
        if (atual != nullptr) {
            return atual;
        }
    }
    return evento_base_;
}

void GerenciadorEstatisticasUI::mostrar_estatisticas() { GerenciamentoEstatisticasUI::mostrar_estatisticas(this); }
void GerenciadorEstatisticasUI::salvar_todos_graficos() { GerenciamentoEstatisticasUI::salvar_todos_graficos(this); }
void GerenciadorEstatisticasUI::limpar_referencia_dialog() { GerenciamentoEstatisticasUI::limpar_referencia_dialog(this); }
void GerenciadorEstatisticasUI::atualizar_graficos_apos_mudanca_idioma() { GerenciamentoEstatisticasUI::atualizar_graficos_apos_mudanca_idioma(this); }
void GerenciadorEstatisticasUI::criar_painel_selecao() { GerenciamentoEstatisticasUI::criar_painel_selecao(this); }
void GerenciadorEstatisticasUI::criar_painel_graficos() { GerenciamentoEstatisticasUI::criar_painel_graficos(this); }
void GerenciadorEstatisticasUI::criar_botao_toggle_painel() { (void)GerenciamentoEstatisticasUI::criar_botao_toggle_painel(this, QString()); }
void GerenciadorEstatisticasUI::criar_lista_graficos() { GerenciamentoEstatisticasUI::criar_lista_graficos(this); }
void GerenciadorEstatisticasUI::criar_mapeamento_funcoes() { GerenciamentoEstatisticasUI::criar_mapeamento_funcoes(this); }
void GerenciadorEstatisticasUI::popular_checkboxes() { GerenciamentoEstatisticasUI::popular_checkboxes(this); }
void GerenciadorEstatisticasUI::verificar_estado_checkbox_todos() { GerenciamentoEstatisticasUI::verificar_estado_checkbox_todos(this); }
void GerenciadorEstatisticasUI::alternar_todos_checkboxes(bool) { GerenciamentoEstatisticasUI::alternar_todos_checkboxes(this); }
QHash<QString, bool> GerenciadorEstatisticasUI::obter_estados_checkboxes() const { return GerenciamentoEstatisticasUI::obter_estados_checkboxes(const_cast<GerenciadorEstatisticasUI*>(this)); }
void GerenciadorEstatisticasUI::gerar_todos_graficos() { GerenciamentoEstatisticasUI::gerar_todos_graficos(this); }
void GerenciadorEstatisticasUI::atualizar_graficos() { GerenciamentoEstatisticasUI::atualizar_graficos(this); }
void GerenciadorEstatisticasUI::atualizar_graficos_sem_fechar() { GerenciamentoEstatisticasUI::atualizar_graficos_sem_fechar(this); }
void GerenciadorEstatisticasUI::regenerar_graficos_existentes() { GerenciamentoEstatisticasUI::regenerar_graficos_existentes(this); }
void GerenciadorEstatisticasUI::salvar_graficos_selecionados() { GerenciamentoEstatisticasUI::salvar_graficos_selecionados(this); }
void GerenciadorEstatisticasUI::toggle_painel_selecao() { GerenciamentoEstatisticasUI::toggle_painel_selecao(this); }
void GerenciadorEstatisticasUI::atualizar_textos_painel_selecao() { GerenciamentoEstatisticasUI::atualizar_textos_painel_selecao(this); }
void GerenciadorEstatisticasUI::atualizar_textos_checkboxes() { GerenciamentoEstatisticasUI::atualizar_textos_checkboxes(this); }
void GerenciadorEstatisticasUI::atualizar_layout_apos_mudanca_botao() { GerenciamentoEstatisticasUI::atualizar_layout_apos_mudanca_botao(this); }
void GerenciadorEstatisticasUI::atualizar_checkboxes_graficos() { GerenciamentoEstatisticasUI::atualizar_checkboxes_graficos(this); }
void GerenciadorEstatisticasUI::atualizar_abas_graficos() { GerenciamentoEstatisticasUI::atualizar_abas_graficos(this); }
void GerenciadorEstatisticasUI::atualizar_dados_graficos_com_novos_titulos() { GerenciamentoEstatisticasUI::atualizar_dados_graficos_com_novos_titulos(this); }
int GerenciadorEstatisticasUI::calcular_largura_ideal() const { return GerenciamentoEstatisticasUI::calcular_largura_ideal(const_cast<GerenciadorEstatisticasUI*>(this)); }
void GerenciadorEstatisticasUI::ajustar_largura_painel_selecao() { GerenciamentoEstatisticasUI::ajustar_largura_painel_selecao(this); }
void GerenciadorEstatisticasUI::traduzir_botoes_detalhes() { GerenciamentoEstatisticasUI::traduzir_botoes_detalhes(this, nullptr); }
void GerenciadorEstatisticasUI::abrir_diretorio(const QString& caminho) { GerenciamentoEstatisticasUI::abrir_diretorio(this, caminho); }

}  // namespace source::ui
