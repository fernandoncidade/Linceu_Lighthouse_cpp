#include "ob_09_GerenciadorColunas.h"

#include <QDir>
#include <QMetaObject>

#include "ob_01_Observador.h"
#include "ob_10_GerenciadorTabela.h"
#include "GerenciamentoMetadados/gmet_28_AtualizarInterface.h"
#include "GerenciamentoMetadados/gmet_29_SalvarEstadoTabela.h"
#include "GerenciamentoMetadados/gmet_30_RestaurarEstadoTabela.h"
#include "GerenciamentoMetadados/gmet_31_CarregarConfiguracoes.h"
#include "GerenciamentoMetadados/gmet_32_SalvarConfiguracoes.h"
#include "GerenciamentoMetadados/gmet_33_ProcessarFilaMetadados.h"
#include "GerenciamentoMetadados/gmet_34_CallbackMetadados.h"
#include "GerenciamentoMetadados/gmet_35_ConfigurarTabela.h"
#include "GerenciamentoMetadados/gmet_36_MostrarDialogoConfiguracao.h"
#include "GerenciamentoMetadados/gmet_37_InvalidarCacheDiretoriosRelacionados.h"
#include "GerenciamentoMetadados/gmet_38_GetMetadados.h"
#include "GerenciamentoMetadados/gmet_39_IdentificarTipoArquivo.h"
#include "GerenciamentoMetadados/gmet_40_AdicionarMetadadosAoItem.h"
#include "GerenciamentoMetadados/gmet_41_ProcessarColunaThread.h"
#include "GerenciamentoMetadados/gmet_42_AdicionarItemParaColuna.h"
#include "GerenciamentoMetadados/gmet_43_AtualizarColunaInterface.h"
#include "GerenciamentoMetadados/gmet_44_ExtrairMetadadosEmLote.h"
#include "GerenciamentoMetadados/gmet_45_MetadadosExtraidosCallback.h"
#include "GerenciamentoMetadados/gmet_46_GetSizeUnit.h"
#include "GerenciamentoMetadados/gmet_47_GetTamanhoBytes.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/CaminhoPersistenteUtils.h"
#include "source/utils/LogManager.h"

namespace source::services::GerenciamentoMetadados {
void atualizar_interface(source::services::GerenciadorColunas* self, const QString& idioma);
QVariantMap salvar_estado_tabela(source::services::GerenciadorColunas* self, QObject* tabela);
void restaurar_estado_tabela(source::services::GerenciadorColunas* self, QObject* tabela, const QList<QVariantMap>& eventos);
void carregar_configuracoes(source::services::GerenciadorColunas* self);
void salvar_configuracoes(source::services::GerenciadorColunas* self);
void processar_fila_metadados(source::services::GerenciadorColunas* self);
void callback_metadados(source::services::GerenciadorColunas* self, const QVariant& futuro);
void configurar_tabela(source::services::GerenciadorColunas* self, QObject* tabela);
void mostrar_dialogo_configuracao(source::services::GerenciadorColunas* self, const QPoint& pos);
void invalidar_cache_diretorios_relacionados(source::services::GerenciadorColunas* self, const QString& caminho);
QVariantMap get_metadados_gc(source::services::GerenciadorColunas* self, const QVariantMap& item);
QString identificar_tipo_arquivo_gc(source::services::GerenciadorColunas* self, const QString& caminho);
QVariantMap adicionar_metadados_ao_item(source::services::GerenciadorColunas* self, QVariantMap item);
void processar_coluna_thread(source::services::GerenciadorColunas* self, const QString& coluna_key);
void adicionar_item_para_coluna(source::services::GerenciadorColunas* self, const QString& coluna_key, const QVariantMap& item);
void atualizar_coluna_interface(source::services::GerenciadorColunas* self, const QString& coluna_key);
void extrair_metadados_em_lote(source::services::GerenciadorColunas* self, const QList<QVariantMap>& lista_itens);
void metadados_extraidos_callback(source::services::GerenciadorColunas* self, const QVariant& futuro);
QString get_size_unit(source::services::GerenciadorColunas* self, const QVariantMap& item, const QString& unidade);
qint64 get_tamanho_bytes(source::services::GerenciadorColunas* self, const QVariantMap& item);
}

namespace source::services {

GerenciadorColunas::GerenciadorColunas(QObject* interface_monitor)
    : QObject(interface_monitor), interface(nullptr), loc(nullptr), processando_metadados(false) {
    inicializar_base(interface_monitor, nullptr);
}

GerenciadorColunas::GerenciadorColunas(source::services::Observador* observador)
    : QObject(nullptr), interface(nullptr), loc(nullptr), processando_metadados(false) {
    inicializar_base(nullptr, observador);
}

void GerenciadorColunas::inicializar_base(QObject* interface_monitor, source::services::Observador* observador) {
    try {
        interface = interface_monitor;

        if (interface && interface->property("loc").isValid()) {
            loc = interface->property("loc").value<QObject*>();
        }
        if (!loc && observador) {
            loc = observador->loc;
        }

        if (auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(loc)) {
            QObject::connect(loc_qt, &source::ui::LocalizadorQt::idioma_alterado, this,
                             [this](const QString& idioma) { atualizar_interface(idioma); });
        }

        config_path = QDir(source::utils::obter_caminho_persistente()).filePath(QStringLiteral("colunas_config.json"));

        if (interface && interface->property("tabela_dados").isValid()) {
            gerenciador_tabela = std::make_unique<source::services::GerenciadorTabela>(interface);
        }

        auto add_col = [&](const QString& key, const QString& translation_key, bool visivel, int ordem) {
            QVariantMap col;
            col.insert("translation_key", translation_key);
            if (auto* loc_qt = qobject_cast<source::ui::LocalizadorQt*>(loc)) {
                col.insert("nome", loc_qt->get_text(translation_key));
            } else {
                col.insert("nome", translation_key);
            }
            col.insert("visivel", visivel);
            col.insert("ordem", ordem);
            COLUNAS_DISPONIVEIS.insert(key, col);
        };

        add_col("tipo_operacao", "operation_type", true, 0);
        add_col("nome", "name", true, 1);
        add_col("dir_anterior", "prev_dir", true, 2);
        add_col("dir_atual", "curr_dir", true, 3);
        add_col("data_criacao", "creation_date", false, 4);
        add_col("data_modificacao", "modification_date", true, 6);
        add_col("data_acesso", "access_date", false, 7);
        add_col("tipo", "type", true, 8);
        add_col("mime", "mime", true, 9);
        add_col("size_b", "size_b", false, 10);
        add_col("size_kb", "size_kb", true, 11);
        add_col("size_mb", "size_mb", false, 12);
        add_col("size_gb", "size_gb", false, 13);
        add_col("size_tb", "size_tb", false, 14);
        add_col("atributos", "attributes", false, 15);
        add_col("autor", "author", false, 16);
        add_col("dimensoes", "dimensions", false, 17);
        add_col("duracao", "duration", false, 18);
        add_col("taxa_bits", "bit_rate", false, 19);
        add_col("protegido", "protected", false, 20);
        add_col("paginas", "pages", false, 21);
        add_col("linhas", "lines", false, 22);
        add_col("palavras", "words", false, 23);
        add_col("paginas_estimadas", "pages_estimated", false, 24);
        add_col("linhas_codigo", "lines_code", false, 25);
        add_col("total_linhas", "total_lines", false, 26);
        add_col("slides_estimados", "slides_estimated", false, 27);
        add_col("arquivos", "files", false, 28);
        add_col("unzipped_b", "unzipped_b", false, 29);
        add_col("unzipped_kb", "unzipped_kb", false, 30);
        add_col("unzipped_mb", "unzipped_mb", false, 31);
        add_col("unzipped_gb", "unzipped_gb", false, 32);
        add_col("unzipped_tb", "unzipped_tb", false, 33);
        add_col("slides", "slides", false, 34);
        add_col("binary_file_b", "binary_file_b", false, 35);
        add_col("binary_file_kb", "binary_file_kb", false, 36);
        add_col("binary_file_mb", "binary_file_mb", false, 37);
        add_col("binary_file_gb", "binary_file_gb", false, 38);
        add_col("binary_file_tb", "binary_file_tb", false, 39);
        add_col("planilhas", "spreadsheets", false, 40);
        add_col("colunas", "columns", false, 41);
        add_col("registros", "records", false, 42);
        add_col("tabelas", "tables", false, 43);
        add_col("timestamp", "timestamp", true, 44);
        add_col("monitor_profile_name", "security_monitoring_profile", false, 45);
        add_col("sha256_atual", "security_sha256_current", false, 46);
        add_col("baseline_status", "security_baseline_status", false, 47);
        add_col("alerta_resumo", "security_alert_summary", true, 48);
        add_col("session_label", "security_session_label", false, 49);
        add_col("diff_resumo", "security_diff_summary", false, 50);
        add_col("risk_score", "security_risk_score", false, 51);
        add_col("risk_level", "security_risk_level", true, 52);
        add_col("actor_user", "security_actor_user", false, 53);
        add_col("actor_process", "security_actor_process", false, 54);
        add_col("publisher", "security_publisher_name", false, 55);
        add_col("quarantine_status", "security_quarantine_status", false, 56);
        add_col("evidence_id", "security_evidence_id", false, 57);

        carregar_configuracoes();
        for (auto it = COLUNAS_DISPONIVEIS.constBegin(); it != COLUNAS_DISPONIVEIS.constEnd(); ++it) {
            filas_colunas.insert(it.key(), QQueue<QVariantMap>());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar GerenciadorColunas", true);
    }
}

GerenciadorColunas::~GerenciadorColunas() = default;

void GerenciadorColunas::atualizar_interface(const QString& idioma) { GerenciamentoMetadados::atualizar_interface(this, idioma); }
QVariantMap GerenciadorColunas::salvar_estado_tabela(QObject* tabela) { return GerenciamentoMetadados::salvar_estado_tabela(this, tabela); }
void GerenciadorColunas::restaurar_estado_tabela(QObject* tabela, const QList<QVariantMap>& eventos) { GerenciamentoMetadados::restaurar_estado_tabela(this, tabela, eventos); }
void GerenciadorColunas::carregar_configuracoes() { GerenciamentoMetadados::carregar_configuracoes(this); }
void GerenciadorColunas::salvar_configuracoes() { GerenciamentoMetadados::salvar_configuracoes(this); }
void GerenciadorColunas::processar_fila_metadados() { GerenciamentoMetadados::processar_fila_metadados(this); }
void GerenciadorColunas::callback_metadados(const QVariant& futuro) { GerenciamentoMetadados::callback_metadados(this, futuro); }
void GerenciadorColunas::configurar_tabela(QObject* tabela) { GerenciamentoMetadados::configurar_tabela(this, tabela); }
void GerenciadorColunas::mostrar_dialogo_configuracao(const QPoint& pos) { GerenciamentoMetadados::mostrar_dialogo_configuracao(this, pos); }
void GerenciadorColunas::_invalidar_cache_diretorios_relacionados(const QString& caminho) { GerenciamentoMetadados::invalidar_cache_diretorios_relacionados(this, caminho); }
QVariantMap GerenciadorColunas::get_metadados(const QVariantMap& item) { return GerenciamentoMetadados::get_metadados_gc(this, item); }
QString GerenciadorColunas::identificar_tipo_arquivo(const QString& caminho) { return GerenciamentoMetadados::identificar_tipo_arquivo_gc(this, caminho); }
void GerenciadorColunas::adicionar_metadados_ao_item(const QVariantMap& item) { Q_UNUSED(GerenciamentoMetadados::adicionar_metadados_ao_item(this, item)); }
void GerenciadorColunas::processar_coluna_thread(const QString& coluna_key) { GerenciamentoMetadados::processar_coluna_thread(this, coluna_key); }
void GerenciadorColunas::adicionar_item_para_coluna(const QString& coluna_key, const QVariantMap& item) { GerenciamentoMetadados::adicionar_item_para_coluna(this, coluna_key, item); }
void GerenciadorColunas::atualizar_coluna_interface(const QString& coluna_key) { GerenciamentoMetadados::atualizar_coluna_interface(this, coluna_key); }
void GerenciadorColunas::extrair_metadados_em_lote(const QList<QVariantMap>& lista_itens) { GerenciamentoMetadados::extrair_metadados_em_lote(this, lista_itens); }
void GerenciadorColunas::_metadados_extraidos_callback(const QVariant& futuro) { GerenciamentoMetadados::metadados_extraidos_callback(this, futuro); }
QString GerenciadorColunas::_get_size_unit(const QVariantMap& item, const QString& unidade) { return GerenciamentoMetadados::get_size_unit(this, item, unidade); }
qint64 GerenciadorColunas::_get_tamanho_bytes(const QVariantMap& item) { return GerenciamentoMetadados::get_tamanho_bytes(this, item); }

}  // namespace source::services
