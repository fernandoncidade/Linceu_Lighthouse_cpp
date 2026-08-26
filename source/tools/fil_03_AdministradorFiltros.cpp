#include "fil_03_AdministradorFiltros.h"

#include <QApplication>
#include <QWidget>

#include "fil_01_Filtros.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_01_desconectar_sinais.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_02_aplicar_filtros.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_03_limpar_filtros.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_04_sincronizar_menu_principal_com_filtros.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_05_atualizar_contagem.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_06_atualizar_contagem_eventos_monitorados.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_07_verificar_filtro_operacao.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_08_atualizar_contagem_apos_idioma.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_09_notificar_alteracao_idioma.h"
#include "source/tools/GerenciamentoAdministradorFiltros/gadmfil_10_salvar_estado_checkboxes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::tools {

QHash<QString, bool> AdministradorFiltros::filtros_estado = {
    {"ignorar_mover", true},
    {"ignorar_renomeados", true},
    {"ignorar_adicionados", true},
    {"ignorar_excluidos", true},
    {"ignorar_data_modificados", true},
    {"ignorar_escaneados", true},
};

AdministradorFiltros::AdministradorFiltros(source::tools::Filtros* parent)
    : parent_(parent),
      contadores_({{"op_moved", 0}, {"op_renamed", 0}, {"op_added", 0}, {"op_deleted", 0}, {"op_modified", 0}, {"op_scanned", 0}}),
      contadores_originais_({{"op_moved", 0}, {"op_renamed", 0}, {"op_added", 0}, {"op_deleted", 0}, {"op_modified", 0}, {"op_scanned", 0}}) {
    try {
        for (QWidget* widget : QApplication::topLevelWidgets()) {
            auto* locObj = widget ? widget->property("loc").value<source::ui::LocalizadorQt*>() : nullptr;
            if (locObj != nullptr) {
                conexoes_sinais_.append(
                    QObject::connect(locObj, &source::ui::LocalizadorQt::idioma_alterado,
                                     parent_, [this](const QString& idioma) { atualizar_contagem_apos_idioma(idioma); }));
                break;
            }
        }

        if (parent_ != nullptr && parent_->localizador() != nullptr) {
            conexoes_sinais_.append(
                QObject::connect(parent_->localizador(), &source::ui::LocalizadorQt::idioma_alterado,
                                 parent_, [this](const QString& idioma) { atualizar_contagem_apos_idioma(idioma); }));
        }

        if (parent_ != nullptr) {
            conexoes_sinais_.append(QObject::connect(parent_, &QObject::destroyed, parent_,
                                                     [this]() { desconectar_sinais(); }));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar o AdministradorFiltros", true);
    }
}

AdministradorFiltros::~AdministradorFiltros() { desconectar_sinais(); }

source::tools::Filtros* AdministradorFiltros::parent_obj() const { return parent_; }
void AdministradorFiltros::set_parent_obj(source::tools::Filtros* parent) { parent_ = parent; }
QVector<QMetaObject::Connection>& AdministradorFiltros::conexoes_sinais_ref() { return conexoes_sinais_; }
QHash<QString, int>& AdministradorFiltros::contadores_ref() { return contadores_; }
QHash<QString, int>& AdministradorFiltros::contadores_originais_ref() { return contadores_originais_; }

void AdministradorFiltros::desconectar_sinais() { GerenciamentoAdministradorFiltros::desconectar_sinais(this); }
void AdministradorFiltros::aplicar_filtros() { GerenciamentoAdministradorFiltros::aplicar_filtros(this); }
void AdministradorFiltros::limpar_filtros() { GerenciamentoAdministradorFiltros::limpar_filtros(this); }
void AdministradorFiltros::sincronizar_menu_principal_com_filtros() { GerenciamentoAdministradorFiltros::sincronizar_menu_principal_com_filtros(this); }
QString AdministradorFiltros::atualizar_contagem() { return GerenciamentoAdministradorFiltros::atualizar_contagem(this); }
QString AdministradorFiltros::atualizar_contagem_eventos_monitorados() { return GerenciamentoAdministradorFiltros::atualizar_contagem_eventos_monitorados(this); }
bool AdministradorFiltros::verificar_filtro_operacao(const QString& operacao) const { return GerenciamentoAdministradorFiltros::verificar_filtro_operacao(const_cast<AdministradorFiltros*>(this), operacao); }
void AdministradorFiltros::atualizar_contagem_apos_idioma(const QString& idioma) { GerenciamentoAdministradorFiltros::atualizar_contagem_apos_idioma(this, idioma); }
void AdministradorFiltros::notificar_alteracao_idioma() { GerenciamentoAdministradorFiltros::notificar_alteracao_idioma(this); }
void AdministradorFiltros::salvar_estado_checkboxes() { GerenciamentoAdministradorFiltros::salvar_estado_checkboxes(this); }

}  // namespace source::tools
