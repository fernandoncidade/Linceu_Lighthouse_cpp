#include "ob_02_BaseEvento.h"

#include <QDir>

#include "ob_01_Observador.h"
#include "source/services/GerenciamentoBaseEvento/gbank_01_set_callback.h"
#include "source/services/GerenciamentoBaseEvento/gbank_02_criar_banco_de_dados.h"
#include "source/services/GerenciamentoBaseEvento/gbank_03_processar_exclusao.h"
#include "source/services/GerenciamentoBaseEvento/gbank_04_registrar_evento_especifico.h"
#include "source/services/GerenciamentoBaseEvento/gbank_05_obter_metadados_arquivo_excluido.h"
#include "source/services/GerenciamentoBaseEvento/gbank_06_registrar_evento_no_banco.h"
#include "source/services/GerenciamentoBaseEvento/gbank_07_atualizar_interface_apos_evento.h"
#include "source/services/GerenciamentoBaseEvento/gbank_08_scan_directory.h"
#include "source/services/GerenciamentoBaseEvento/gbank_09_get_tipo_from_snapshot.h"
#include "source/services/GerenciamentoBaseEvento/gbank_10_is_directory.h"
#include "source/services/GerenciamentoBaseEvento/gbank_11_limpar_registros.h"
#include "source/services/GerenciamentoBaseEvento/gbank_12_obter_tipo_anterior.h"
#include "source/services/GerenciamentoBaseEvento/gbank_13_notificar_evento.h"
#include "source/services/GerenciamentoBaseEvento/gbank_14_remover_exclusao_temporaria.h"
#include "source/services/GerenciamentoBaseEvento/gbank_15_criar_evento_exclusao.h"
#include "source/services/GerenciamentoBaseEvento/gbank_16_criar_evento_padrao.h"
#include "source/services/GerenciamentoBaseEvento/gbank_17_atualizar_interface_apos_exclusao.h"
#include "source/services/GerenciamentoBaseEvento/gbank_18_processar_eventos_movimentacao.h"
#include "source/services/GerenciamentoBaseEvento/gbank_19_inserir_evento_movido.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::services {

BaseEvento::BaseEvento(source::services::Observador* observador_)
    : observador(observador_),
      db_path(QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("monitoramento.db"))),
      eventos_excluidos(0) {
    try {
        criar_banco_de_dados();

        operacoes = {
            {QStringLiteral("op_renamed"), QStringLiteral("op_renamed")},
            {QStringLiteral("op_added"), QStringLiteral("op_added")},
            {QStringLiteral("op_deleted"), QStringLiteral("op_deleted")},
            {QStringLiteral("op_modified"), QStringLiteral("op_modified")},
            {QStringLiteral("op_moved"), QStringLiteral("op_moved")},
            {QStringLiteral("op_scanned"), QStringLiteral("op_scanned")},
        };
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar BaseEvento", true);
    }
}

void BaseEvento::set_callback(const QVariant& callback_obj) { GerenciamentoBaseEvento::set_callback(this, callback_obj); }
void BaseEvento::criar_banco_de_dados() { GerenciamentoBaseEvento::criar_banco_de_dados(this); }
void BaseEvento::processar_exclusao(const QVariantMap& evento) { GerenciamentoBaseEvento::processar_exclusao(this, evento); }
void BaseEvento::registrar_evento_especifico(const QString& tabela, const QVariantMap& evento) { GerenciamentoBaseEvento::registrar_evento_especifico(this, tabela, evento); }
QVariantMap BaseEvento::obter_metadados_arquivo_excluido(const QString& nome, const QString& caminho) {
    return GerenciamentoBaseEvento::obter_metadados_arquivo_excluido(this, nome, caminho);
}
void BaseEvento::registrar_evento_no_banco(const QVariantMap& evento) { GerenciamentoBaseEvento::registrar_evento_no_banco(this, evento); }
void BaseEvento::_atualizar_interface_apos_evento(const QVariantMap& evento) { GerenciamentoBaseEvento::_atualizar_interface_apos_evento(this, evento); }
void BaseEvento::scan_directory(const QString& diretorio) { GerenciamentoBaseEvento::scan_directory(this, diretorio); }
QString BaseEvento::get_tipo_from_snapshot(const QString& nome) { return GerenciamentoBaseEvento::get_tipo_from_snapshot(this, nome); }
bool BaseEvento::is_directory(const QString& caminho) { return GerenciamentoBaseEvento::is_directory(this, caminho); }
void BaseEvento::limpar_registros() { GerenciamentoBaseEvento::limpar_registros(this); }
QString BaseEvento::obter_tipo_anterior(const QString& nome_base) { return GerenciamentoBaseEvento::obter_tipo_anterior(this, nome_base); }
void BaseEvento::notificar_evento(const QString& tipo_operacao, const QString& nome, const QString& dir_anterior, const QString& dir_atual) { GerenciamentoBaseEvento::notificar_evento(this, tipo_operacao, nome, dir_anterior, dir_atual); }
void BaseEvento::_remover_exclusao_temporaria(const QString& nome, const QString& dir_anterior, const QString& exclusao_id) { GerenciamentoBaseEvento::_remover_exclusao_temporaria(this, nome, dir_anterior, exclusao_id); }
QVariantMap BaseEvento::_criar_evento_exclusao(const QString& nome_base, const QString& dir_anterior) { return GerenciamentoBaseEvento::_criar_evento_exclusao(this, nome_base, dir_anterior); }
QVariantMap BaseEvento::_criar_evento_padrao(const QString& tipo_operacao, const QString& nome_base, const QString& dir_anterior, const QString& dir_atual) { return GerenciamentoBaseEvento::_criar_evento_padrao(this, tipo_operacao, nome_base, dir_anterior, dir_atual); }
void BaseEvento::_atualizar_interface_apos_exclusao(const QVariantMap& evento) { GerenciamentoBaseEvento::_atualizar_interface_apos_exclusao(this, evento); }
void BaseEvento::processar_eventos_movimentacao(const QList<QVariantMap>& eventos_movidos, const std::function<void()>& callback_fn) { GerenciamentoBaseEvento::processar_eventos_movimentacao(this, eventos_movidos, callback_fn); }
bool BaseEvento::_inserir_evento_movido(const QVariantMap& evento) { return GerenciamentoBaseEvento::_inserir_evento_movido(this, evento); }

}  // namespace source::services
