#include "ui_04_GerenciadorEventosUI.h"

#include <QVariant>

#include "source/ui/GerenciamentoEventosUI/geve_01_alternar_filtro.h"
#include "source/ui/GerenciamentoEventosUI/geve_02_alternar_visibilidade_coluna.h"
#include "source/ui/GerenciamentoEventosUI/geve_03_resetar_colunas.h"
#include "source/ui/GerenciamentoEventosUI/geve_04_resetar_cores.h"
#include "source/ui/GerenciamentoEventosUI/geve_05_redefinir_todas_colunas_cores.h"
#include "source/ui/GerenciamentoEventosUI/geve_06_alterar_idioma.h"
#include "source/ui/GerenciamentoEventosUI/geve_07_confirmar_mudanca_idioma.h"
#include "source/ui/GerenciamentoEventosUI/geve_08_limpar_dados_monitorados.h"
#include "source/ui/GerenciamentoEventosUI/geve_09_limpar_dados_interface.h"
#include "source/ui/GerenciamentoEventosUI/geve_10_reiniciar_aplicativo.h"
#include "source/ui/GerenciamentoEventosUI/geve_11_atualizar_interface.h"
#include "source/utils/LogManager.h"

namespace source::ui {

GerenciadorEventosUI::GerenciadorEventosUI(QObject* interface_principal)
    : interface_(interface_principal), loc_(nullptr) {
    try {
        if (interface_ != nullptr) {
            loc_ = interface_->property("loc").value<QObject*>();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorEventosUI", true);
    }
}

QObject* GerenciadorEventosUI::interface_obj() const {
    return interface_;
}

QObject* GerenciadorEventosUI::loc_obj() const {
    return loc_;
}

void GerenciadorEventosUI::alternar_filtro(const QString& filtro) { GerenciamentoEventosUI::alternar_filtro(this, filtro); }
void GerenciadorEventosUI::alternar_visibilidade_coluna(const QString& coluna, bool visivel) { GerenciamentoEventosUI::alternar_visibilidade_coluna(this, coluna, visivel); }
void GerenciadorEventosUI::resetar_colunas() { GerenciamentoEventosUI::resetar_colunas(this); }
void GerenciadorEventosUI::resetar_cores() { GerenciamentoEventosUI::resetar_cores(this); }
void GerenciadorEventosUI::redefinir_todas_colunas_cores() { GerenciamentoEventosUI::redefinir_todas_colunas_cores(this); }
void GerenciadorEventosUI::alterar_idioma(const QString& idioma) { GerenciamentoEventosUI::alterar_idioma(this, idioma); }
bool GerenciadorEventosUI::confirmar_mudanca_idioma(const QString& idioma) { return GerenciamentoEventosUI::confirmar_mudanca_idioma(this, idioma); }
void GerenciadorEventosUI::limpar_dados_monitorados() { GerenciamentoEventosUI::limpar_dados_monitorados(this); }
void GerenciadorEventosUI::limpar_dados_interface() { GerenciamentoEventosUI::limpar_dados_interface(this); }
void GerenciadorEventosUI::reiniciar_aplicativo() { GerenciamentoEventosUI::reiniciar_aplicativo(this); }
void GerenciadorEventosUI::atualizar_interface() { GerenciamentoEventosUI::atualizar_interface(this); }

}  // namespace source::ui
