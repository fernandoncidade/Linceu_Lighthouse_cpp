#pragma once

#include <QMainWindow>
#include <QMutex>
#include <QStringList>
#include <QVariantMap>
#include <memory>

class QObject;
class QEvent;
class QHideEvent;
class QModelIndex;
class QShowEvent;

namespace source::services {
class GerenciadorColunas;
class GerenciadorTabela;
class Observador;
namespace GerenciamentoTabela {
class EventTableModel;
}
}  // namespace source::services

namespace source::tools {
class Filtros;
}

namespace source::security {
class SecurityManager;
}

namespace source::ui {
class GerenciadorBotoes;
class GerenciadorBotoesUI;
class GerenciadorDados;
class GerenciadorEstatisticasUI;
class GerenciadorEventosArquivo;
class GerenciadorEventosUI;
class GerenciadorLimpeza;
class GerenciadorMenusUI;
class GerenciadorMonitoramento;
class GerenciadorProgressoUI;
class LocalizadorQt;
}  // namespace source::ui

namespace source::gui {
class InterfaceMonitor : public QMainWindow {
    Q_OBJECT
public:
    explicit InterfaceMonitor(QWidget* parent = nullptr);
    ~InterfaceMonitor() override;

    void setup_ui();
    void setup_menu_bar();
    void atualizar_interface();
    Q_INVOKABLE void abrir_janela_filtros();
    Q_INVOKABLE void atualizar_status();
    void configurar_tabela();
    void verificar_movimentacao(const QVariantMap& evento);
    void adicionar_evento(const QVariantMap& evento);
    Q_INVOKABLE void selecionar_diretorio();
    Q_INVOKABLE void alternar_analise_diretorio();
    Q_INVOKABLE void pausar_monitoramento_ou_escaneamento();
    Q_INVOKABLE void salvar_dados();
    Q_INVOKABLE void abrir_salvar_como();
    Q_INVOKABLE void mostrar_estatisticas();
    Q_INVOKABLE void limpar_dados();
    Q_INVOKABLE void sair_aplicacao();
    Q_INVOKABLE void alternar_filtro();
    Q_INVOKABLE void alternar_visibilidade_coluna();
    Q_INVOKABLE void resetar_colunas();
    Q_INVOKABLE void resetar_cores();
    Q_INVOKABLE void redefinir_todas_colunas_cores();
    Q_INVOKABLE void criar_menu_principal();
    void alterar_idioma();
    Q_INVOKABLE void atualizar_visibilidade_colunas();
    void reiniciar_sistema_monitoramento();
    void atualizar_tradutor_qt(const QString& idioma);
    Q_INVOKABLE void alternar_tabela_idioma(const QString& idioma = QString());
    Q_INVOKABLE void substituir_eventos_tabelas_idioma(const QVariantList& eventos);
    Q_INVOKABLE void adicionar_eventos_tabelas_idioma(const QVariantList& eventos);
    Q_INVOKABLE void limpar_tabelas_idioma();
    Q_INVOKABLE QObject* criar_observador(const QString& diretorio);
    Q_INVOKABLE bool adicionar_diretorio_monitorado(const QString& diretorio);
    Q_INVOKABLE bool remover_diretorio_monitorado(const QString& diretorio);
    Q_INVOKABLE QStringList obter_diretorios_monitorados() const;
    Q_INVOKABLE QVariantMap obter_resumo_seguranca() const;
    Q_INVOKABLE void atualizar_rotulo_diretorios_monitorados();
    Q_INVOKABLE QString exportar_pacote_evidencia(const QString& destino = QString());

public slots:
    void criar_barra_progresso();
    void atualizar_progresso_scan(int progresso, int contador, int total);
    void inserir_evento_streaming(const QVariantMap& evento);
    void inserir_eventos_streaming(const QVariantList& eventos);
    void atualizar_cores_tabela(const QVariantMap& resultado_cores);
    void abrir_arquivo_reportado(const QModelIndex& index);

protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void ativar_model_view_streaming();

public:
    source::ui::LocalizadorQt* loc;
    source::services::GerenciadorTabela* gerenciador_tabela;
    source::services::GerenciadorColunas* gerenciador_colunas;
    source::ui::GerenciadorEventosArquivo* gerenciador_eventos_arquivo;
    source::ui::GerenciadorDados* gerenciador_dados;
    source::ui::GerenciadorMonitoramento* gerenciador_monitoramento;
    source::ui::GerenciadorLimpeza* gerenciador_limpeza;
    source::ui::GerenciadorBotoes* gerenciador_botoes;
    source::ui::GerenciadorBotoesUI* gerenciador_botoes_ui;
    source::ui::GerenciadorMenusUI* gerenciador_menus_ui;
    source::ui::GerenciadorEventosUI* gerenciador_eventos_ui;
    source::ui::GerenciadorProgressoUI* gerenciador_progresso_ui;
    source::ui::GerenciadorEstatisticasUI* gerenciador_estatisticas_ui;
    source::tools::Filtros* painel_filtros;
    QObject* evento_base;
    source::services::Observador* observador;
    QObject* tabela_dados;
    source::services::GerenciamentoTabela::EventTableModel* event_table_model;
    source::security::SecurityManager* security_manager;
    std::unique_ptr<QMutex> mutex_interface;
};

}  // namespace source::gui
