#include "ob_10_GerenciadorTabela.h"

#include <QAbstractItemView>
#include <QMetaObject>
#include <QTableWidget>

#include "source/services/GerenciamentoTabela/gtab_01_DetectarTemaWindows.h"
#include "source/services/GerenciamentoTabela/gtab_02_CalcularCorTextoIdeal.h"
#include "source/services/GerenciamentoTabela/gtab_03_ConfigurarTabela.h"
#include "source/services/GerenciamentoTabela/gtab_04_AjustarLargurasColunas.h"
#include "source/services/GerenciamentoTabela/gtab_05_AplicarQuebraLinhaCabecalho.h"
#include "source/services/GerenciamentoTabela/gtab_06_AplicarQuebraLinhaTodosCabecalhos.h"
#include "source/services/GerenciamentoTabela/gtab_07_AjustarAlturaCabecalho.h"
#include "source/services/GerenciamentoTabela/gtab_08_RedimensionarCabecalho.h"
#include "source/services/GerenciamentoTabela/gtab_09_AtualizarCabecalhos.h"
#include "source/services/GerenciamentoTabela/gtab_10_AtualizarDadosTabela.h"
#include "source/services/GerenciamentoTabela/gtab_11_AtualizarLinhaMaisRecente.h"
#include "source/services/GerenciamentoTabela/gtab_12_AtualizarVisualizacaoTabela.h"
#include "source/services/GerenciamentoTabela/gtab_13_AtualizarVisibilidadeColunas.h"
#include "source/services/GerenciamentoTabela/gtab_14_InvalidarCacheCores.h"
#include "source/services/GerenciamentoTabela/gtab_15_ObterCoresOperacao.h"
#include "source/services/GerenciamentoTabela/gtab_16_ObterIndicesColunas.h"
#include "source/services/GerenciamentoTabela/gtab_17_AtivarCores.h"
#include "source/services/GerenciamentoTabela/gtab_18_OcultarCores.h"
#include "source/services/GerenciamentoTabela/gtab_19_AplicarCoresLinhaEspecifica.h"
#include "source/services/GerenciamentoTabela/gtab_20_AplicarCoresTodasColunas.h"
#include "source/services/GerenciamentoTabela/gtab_21_RedefinirCoresTodasColunas.h"
#include "source/services/GerenciamentoTabela/gtab_22_RemoverCorColuna.h"
#include "source/services/GerenciamentoTabela/gtab_23_AtualizarCoresColunas.h"
#include "source/services/GerenciamentoTabela/gtab_24_SetColunaColorir.h"
#include "source/services/GerenciamentoTabela/gtab_25_SetColunasColorirEmMassa.h"
#include "source/services/GerenciamentoTabela/gtab_26_RemoverTodasCoresColunas.h"
#include "source/services/GerenciamentoTabela/gtab_27_SalvarConfiguracoesCores.h"
#include "source/services/GerenciamentoTabela/gtab_28_CarregarConfiguracoesCores.h"
#include "source/services/GerenciamentoTabela/gtab_29_EhColunaPersonalizadaColorida.h"
#include "source/services/GerenciamentoTabela/gtab_30_MostrarDialogoConfiguracao.h"
#include "source/services/GerenciamentoTabela/gtab_31_AjustarCorSelecao.h"
#include "source/services/GerenciamentoTabela/gtab_33_AtualizarEstiloTema.h"
#include "source/services/GerenciamentoTabela/gtab_34_MonitorTemaWindows.h"
#include "source/services/GerenciamentoTabela/gtab_35_IniciarProcessamentoPesado.h"
#include "source/services/GerenciamentoTabela/gtab_36_AplicarCoresTodasColunasProcessamento.h"
#include "source/services/GerenciamentoTabela/gtab_37_ProcessarCoresEmBackground.h"
#include "source/services/GerenciamentoTabela/gtab_38_OnCoresProcessadas.h"
#include "source/services/GerenciamentoTabela/gtab_39_AtualizarCoresNaInterface.h"
#include "source/services/GerenciamentoTabela/gtab_40_AtualizarInterfacePosProcessamento.h"
#include "source/services/GerenciamentoTabela/gtab_41_ProcessarSelecaoBackground.h"
#include "source/services/GerenciamentoTabela/gtab_42_ShutdownExecutors.h"
#include "source/services/GerenciamentoTabela/gtab_43_AtualizarCabecalhosBridge.h"
#include "source/services/GerenciamentoTabela/gtab_44_OnIdiomaAlterado.h"
#include "source/services/GerenciamentoTabela/gtab_45_RetraduzirDadosExistentes.h"
#include "source/services/GerenciamentoTabela/gtab_46_ExecutarRetraducaoAgendada.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::services {

GerenciadorTabela::GerenciadorTabela(QObject* interface_monitor)
    : QObject(interface_monitor),
      interface(interface_monitor),
      loc(interface_monitor ? interface_monitor->property("loc").value<QObject*>() : nullptr),
      monitor_tema(nullptr),
      atualizacao_pendente(false),
      cores_visiveis(true),
      ordenacao_habilitada(false),
      retraducao_realizada_para_idioma(false),
      retraducao_em_andamento(false),
      retraducao_agendada(false) {
    try {
        if (loc) {
            idioma_ultima_retraducao = loc->property("idioma_atual").toString();
            idioma_alvo_retraducao = idioma_ultima_retraducao;
        }

        if (auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(loc)) {
            QObject::connect(loc_qt, &source::ui::LocalizadorQt::idioma_alterado, this,
                             [this](const QString&) { atualizar_cabecalhos(); });
            QObject::connect(loc_qt, &source::ui::LocalizadorQt::idioma_alterado, this,
                             [this](const QString& idioma) { _on_idioma_alterado(idioma); });
        }

        if (interface) {
            QObject* tabela_obj = interface->property("tabela_dados").value<QObject*>();
            if (auto* tabela = qobject_cast<QTableWidget*>(tabela_obj)) {
                tabela->setSelectionBehavior(QAbstractItemView::SelectItems);
                tabela->setSelectionMode(QAbstractItemView::ExtendedSelection);
                QObject::connect(tabela, &QTableWidget::itemSelectionChanged, this,
                                 &GerenciadorTabela::ajustar_cor_selecao);
            }
        }

        timer_tema.setSingleShot(true);
        timer_tema.setInterval(90);
        QObject::connect(&timer_tema, &QTimer::timeout, this, [this]() {
            const QString tema = tema_pendente;
            tema_pendente.clear();
            atualizar_estilo_tema(tema);
        });

        monitor_tema = new GerenciamentoTabela::MonitorTemaWindows(this);
        QObject::connect(static_cast<GerenciamentoTabela::MonitorTemaWindows*>(monitor_tema),
                         &GerenciamentoTabela::MonitorTemaWindows::tema_alterado,
                         this,
                         [this](const QString& tema) {
                             tema_pendente = tema;
                             timer_tema.start();
                         },
                         Qt::QueuedConnection);
        static_cast<GerenciamentoTabela::MonitorTemaWindows*>(monitor_tema)->iniciar_monitoramento();
        tema_pendente = static_cast<GerenciamentoTabela::MonitorTemaWindows*>(monitor_tema)->obter_tema_atual();
        timer_tema.start(0);

        QObject::connect(&timer_atualizacao, &QTimer::timeout, this, [this]() { atualizar_visualizacao_tabela(); });
        timer_atualizacao.start(100);

        timer_retraducao.setSingleShot(true);
        QObject::connect(&timer_retraducao, &QTimer::timeout, this, [this]() { _executar_retraducao_agendada(); });

        QTimer::singleShot(0, this, [this]() { _carregar_configuracoes_cores(); });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorTabela", true);
    }
}

QString GerenciadorTabela::detectar_tema_windows() { return GerenciamentoTabela::detectar_tema_windows(this); }
QColor GerenciadorTabela::calcular_cor_texto_ideal(const QColor& cor_fundo, bool eh_coluna_personalizada) {
    return GerenciamentoTabela::calcular_cor_texto_ideal(this, cor_fundo, eh_coluna_personalizada);
}
bool GerenciadorTabela::_eh_cor_fundo_padrao(const QColor& cor) { return GerenciamentoTabela::eh_cor_fundo_padrao(this, cor); }
void GerenciadorTabela::configurar_tabela(QObject* tabela) { GerenciamentoTabela::configurar_tabela(this, tabela); }
void GerenciadorTabela::ajustar_larguras_colunas(QObject* tabela, const QVariantList& colunas_visiveis) {
    GerenciamentoTabela::ajustar_larguras_colunas(this, tabela, colunas_visiveis);
}
void GerenciadorTabela::aplicar_quebra_linha_cabecalho(QObject* tabela, int coluna_index) {
    GerenciamentoTabela::aplicar_quebra_linha_cabecalho(this, tabela, coluna_index);
}
void GerenciadorTabela::aplicar_quebra_linha_todos_cabecalhos(QObject* tabela) {
    GerenciamentoTabela::aplicar_quebra_linha_todos_cabecalhos(this, tabela);
}
void GerenciadorTabela::ajustar_altura_cabecalho(QObject* tabela) { GerenciamentoTabela::ajustar_altura_cabecalho(this, tabela); }
void GerenciadorTabela::redimensionar_cabecalho(int logicalIndex, int oldSize, int newSize) {
    GerenciamentoTabela::redimensionar_cabecalho(this, logicalIndex, oldSize, newSize);
}
void GerenciadorTabela::atualizar_cabecalhos() { GerenciamentoTabela::atualizar_cabecalhos(this); }
void GerenciadorTabela::atualizar_dados_tabela(QObject* tabela_dados, int row_especifico) {
    GerenciamentoTabela::atualizar_dados_tabela(this, tabela_dados, row_especifico);
}
void GerenciadorTabela::atualizar_linha_mais_recente() {
    QObject* tabela_dados = interface ? interface->property("tabela_dados").value<QObject*>() : nullptr;
    atualizar_linha_mais_recente(tabela_dados, QVariantMap{});
}

void GerenciadorTabela::atualizar_linha_mais_recente(QObject* tabela_dados) {
    atualizar_linha_mais_recente(tabela_dados, QVariantMap{});
}

void GerenciadorTabela::atualizar_linha_mais_recente(QObject* tabela_dados, const QVariantMap& evento) {
    GerenciamentoTabela::atualizar_linha_mais_recente(this, tabela_dados, evento);
}
void GerenciadorTabela::atualizar_visualizacao_tabela() { GerenciamentoTabela::atualizar_visualizacao_tabela(this); }
void GerenciadorTabela::atualizar_visibilidade_colunas(bool atualizar_em_massa) {
    GerenciamentoTabela::atualizar_visibilidade_colunas(this, atualizar_em_massa);
}
void GerenciadorTabela::_invalidar_cache_cores() { GerenciamentoTabela::invalidar_cache_cores(this); }
QVariantMap GerenciadorTabela::_obter_cores_operacao() { return GerenciamentoTabela::obter_cores_operacao(this); }
QVariantMap GerenciadorTabela::_obter_indices_colunas(QObject* tabela) {
    return GerenciamentoTabela::obter_indices_colunas(this, tabela);
}
void GerenciadorTabela::ativar_cores() { GerenciamentoTabela::ativar_cores(this); }
void GerenciadorTabela::ocultar_cores() { GerenciamentoTabela::ocultar_cores(this); }
void GerenciadorTabela::aplicar_cores_linha_especifica(QObject* tabela, int row, const QString& tipo_operacao_valor) {
    GerenciamentoTabela::aplicar_cores_linha_especifica(this, tabela, row, tipo_operacao_valor);
}
void GerenciadorTabela::aplicar_cores_todas_colunas() { GerenciamentoTabela::aplicar_cores_todas_colunas(this); }
void GerenciadorTabela::redefinir_cores_todas_colunas() { GerenciamentoTabela::redefinir_cores_todas_colunas(this); }
void GerenciadorTabela::remover_cor_coluna(const QString& coluna) { GerenciamentoTabela::remover_cor_coluna(this, coluna); }
void GerenciadorTabela::atualizar_cores_colunas(bool aplicar_em_massa, const QSet<QString>& colunas_afetadas) {
    GerenciamentoTabela::atualizar_cores_colunas(this, aplicar_em_massa, colunas_afetadas);
}
void GerenciadorTabela::set_coluna_colorir(const QString& coluna, bool ativo) {
    GerenciamentoTabela::set_coluna_colorir(this, coluna, ativo);
}
void GerenciadorTabela::set_colunas_colorir_em_massa(const QSet<QString>& colunas) {
    GerenciamentoTabela::set_colunas_colorir_em_massa(this, colunas);
}
void GerenciadorTabela::remover_todas_cores_colunas() { GerenciamentoTabela::remover_todas_cores_colunas(this); }
void GerenciadorTabela::salvar_configuracoes_cores() { GerenciamentoTabela::salvar_configuracoes_cores(this); }
void GerenciadorTabela::_carregar_configuracoes_cores() { GerenciamentoTabela::carregar_configuracoes_cores(this); }
bool GerenciadorTabela::eh_coluna_personalizada_colorida(const QString& coluna) {
    return GerenciamentoTabela::eh_coluna_personalizada_colorida(this, coluna);
}
void GerenciadorTabela::mostrar_dialogo_configuracao(const QPoint& pos) {
    GerenciamentoTabela::mostrar_dialogo_configuracao(this, pos);
}
void GerenciadorTabela::ajustar_cor_selecao() { GerenciamentoTabela::ajustar_cor_selecao(this); }
void GerenciadorTabela::atualizar_estilo_tema(const QString& novo_tema) {
    GerenciamentoTabela::atualizar_estilo_tema(this, novo_tema);
}
void GerenciadorTabela::_atualizar_cores_celulas_incolores(const QString& novo_tema) {
    GerenciamentoTabela::atualizar_cores_celulas_incolores(this, novo_tema);
}
bool GerenciadorTabela::_eh_cor_padrao_qualquer_tema(const QColor& cor) {
    return GerenciamentoTabela::eh_cor_padrao_qualquer_tema(this, cor);
}
void GerenciadorTabela::_forcar_reconstrucao_todas_celulas(const QString& novo_tema) {
    GerenciamentoTabela::forcar_reconstrucao_todas_celulas(this, novo_tema);
}
bool GerenciadorTabela::_processar_selecao_background() {
    return GerenciamentoTabela::processar_selecao_background(this);
}
void GerenciadorTabela::iniciar_processamento_pesado(const QVariant& dados) {
    GerenciamentoTabela::iniciar_processamento_pesado(this, dados);
}
void GerenciadorTabela::aplicar_cores_todas_colunas_processamento(const QVariant& dados) {
    GerenciamentoTabela::aplicar_cores_todas_colunas_processamento(this, dados);
}
QVariantMap GerenciadorTabela::_processar_cores_em_background(const QVariant& dados) {
    return GerenciamentoTabela::processar_cores_em_background(this, dados);
}
void GerenciadorTabela::_on_cores_processadas(const QVariantMap& cores) { GerenciamentoTabela::on_cores_processadas(this, cores); }
void GerenciadorTabela::_atualizar_cores_na_interface(const QVariantMap& resultado) {
    GerenciamentoTabela::atualizar_cores_na_interface(this, resultado);
}
void GerenciadorTabela::atualizar_interface_pos_processamento(const QVariantMap& resultado) {
    GerenciamentoTabela::atualizar_interface_pos_processamento(this, resultado);
}
void GerenciadorTabela::shutdown_executors() { GerenciamentoTabela::shutdown_executors(this); }
void GerenciadorTabela::_on_idioma_alterado(const QString& idioma) { GerenciamentoTabela::on_idioma_alterado(this, idioma); }
void GerenciadorTabela::retraduzir_dados_existentes() { GerenciamentoTabela::retraduzir_dados_existentes(this); }
void GerenciadorTabela::_executar_retraducao_agendada() { GerenciamentoTabela::executar_retraducao_agendada(this); }

}  // namespace source::services
