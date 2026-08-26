#include "ui_03_GerenciadorMenusUI.h"

#include <QMetaObject>
#include <QVariant>

#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/ui/GerenciamentoMenusUI/gmui_02_GerenciadorCores.h"
#include "source/ui/GerenciamentoMenusUI/gmui_04_criar_menu_principal.h"
#include "source/ui/GerenciamentoMenusUI/gmui_05_configurar_menu_arquivo.h"
#include "source/ui/GerenciamentoMenusUI/gmui_06_configurar_menu_configuracoes.h"
#include "source/ui/GerenciamentoMenusUI/gmui_07_configurar_menu_opcoes.h"
#include "source/ui/GerenciamentoMenusUI/gmui_08_criar_submenu_cores.h"
#include "source/ui/GerenciamentoMenusUI/gmui_11_criar_submenu_colunas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_12_alternar_ordenacao_linhas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_13_criar_submenu_colunas_coloridas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_14_selecionar_todas_colunas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_15_selecionar_todas_cores_colunas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_16_criar_submenu_exportacao.h"
#include "source/ui/GerenciamentoMenusUI/gmui_17_resetar_opcoes_exportacao.h"
#include "source/ui/GerenciamentoMenusUI/gmui_18_criar_submenu_idiomas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_19_get_texto_traduzido_para_idioma.h"
#include "source/ui/GerenciamentoMenusUI/gmui_20_confirmar_alteracao_idioma.h"
#include "source/ui/GerenciamentoMenusUI/gmui_21_on_traducoes_carregadas.h"
#include "source/ui/GerenciamentoMenusUI/gmui_22_exibir_sobre.h"
#include "source/ui/GerenciamentoMenusUI/gmui_24_exibir_manual.h"
#include "source/ui/GerenciamentoMenusUI/gmui_25_exibir_ajuda.h"

#include "source/utils/LogManager.h"

namespace source::ui {

GerenciadorMenusUI::GerenciadorMenusUI(QObject* interface_principal)
    : interface_(interface_principal),
      loc_(nullptr),
      gerenciador_cores_(nullptr),
      trocando_idioma_(false),
      aguardando_conclusao_traducao_(false) {
    try {
        if (interface_ != nullptr) {
            loc_ = interface_->property("loc").value<QObject*>();
            interface_->setProperty("_ordenacao_linhas_habilitada", false);
            interface_->setProperty("_ordenacao_linhas_coluna", -1);
            interface_->setProperty("_ordenacao_linhas_ordem", static_cast<int>(Qt::AscendingOrder));
            interface_->setProperty("_trocando_idioma", false);
            interface_->setProperty("_aguardando_conclusao_traducao", false);

            auto* gerenciador_cores = new source::ui::GerenciamentoMenusUI::GerenciadorCores(interface_, interface_);
            gerenciador_cores_ = gerenciador_cores;
            interface_->setProperty("gerenciador_cores", QVariant::fromValue(static_cast<QObject*>(gerenciador_cores)));
        }

        if (auto* loc_tipado = qobject_cast<source::ui::LocalizadorQt*>(loc_); loc_tipado != nullptr) {
            QObject::connect(loc_tipado, &source::ui::LocalizadorQt::traducoes_carregadas, interface_,
                             [this](const QString& idioma) { on_traducoes_carregadas(idioma); });
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Falha ao conectar sinal de traduções");
    }
}

QObject* GerenciadorMenusUI::interface_obj() const { return interface_; }
QObject* GerenciadorMenusUI::loc_obj() const { return loc_; }

void GerenciadorMenusUI::criar_menu_principal() { GerenciamentoMenusUI::criar_menu_principal(this); }
void GerenciadorMenusUI::configurar_menu_arquivo() { GerenciamentoMenusUI::configurar_menu_arquivo(this); }
void GerenciadorMenusUI::configurar_menu_configuracoes() { GerenciamentoMenusUI::configurar_menu_configuracoes(this); }
void GerenciadorMenusUI::configurar_menu_opcoes() { GerenciamentoMenusUI::configurar_menu_opcoes(this); }
void GerenciadorMenusUI::criar_submenu_cores() { GerenciamentoMenusUI::criar_submenu_cores(this); }
void GerenciadorMenusUI::criar_submenu_colunas() { GerenciamentoMenusUI::criar_submenu_colunas(this); }
void GerenciadorMenusUI::alternar_ordenacao_linhas() { GerenciamentoMenusUI::alternar_ordenacao_linhas(this); }
void GerenciadorMenusUI::criar_submenu_colunas_coloridas() { GerenciamentoMenusUI::criar_submenu_colunas_coloridas(this); }
void GerenciadorMenusUI::selecionar_todas_colunas() { GerenciamentoMenusUI::selecionar_todas_colunas(this); }
void GerenciadorMenusUI::selecionar_todas_cores_colunas() { GerenciamentoMenusUI::selecionar_todas_cores_colunas(this); }
void GerenciadorMenusUI::criar_submenu_exportacao() { GerenciamentoMenusUI::criar_submenu_exportacao(this); }
void GerenciadorMenusUI::resetar_opcoes_exportacao() { GerenciamentoMenusUI::resetar_opcoes_exportacao(this); }
void GerenciadorMenusUI::criar_submenu_idiomas() { GerenciamentoMenusUI::criar_submenu_idiomas(this); }
QString GerenciadorMenusUI::get_texto_traduzido_para_idioma(const QString& chave, const QString& idioma) { return GerenciamentoMenusUI::get_texto_traduzido_para_idioma(this, chave, idioma); }
void GerenciadorMenusUI::confirmar_alteracao_idioma(const QString& codigo_idioma) { GerenciamentoMenusUI::confirmar_alteracao_idioma(this, codigo_idioma); }
void GerenciadorMenusUI::on_traducoes_carregadas(const QString& idioma) { GerenciamentoMenusUI::on_traducoes_carregadas(this, idioma); }
void GerenciadorMenusUI::exibir_sobre() { GerenciamentoMenusUI::exibir_sobre(this); }
void GerenciadorMenusUI::exibir_manual() { GerenciamentoMenusUI::exibir_manual(this); }
void GerenciadorMenusUI::exibir_ajuda() { GerenciamentoMenusUI::exibir_ajuda(this); }

}  // namespace source::ui
