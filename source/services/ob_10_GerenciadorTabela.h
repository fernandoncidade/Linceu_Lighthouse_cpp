#pragma once

#include <QHash>
#include <QColor>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QTimer>
#include <QVariant>

namespace source::services {

class GerenciadorTabela : public QObject {
    Q_OBJECT

public:
    explicit GerenciadorTabela(QObject* interface_monitor);

    QString detectar_tema_windows();
    QColor calcular_cor_texto_ideal(const QColor& cor_fundo = QColor(), bool eh_coluna_personalizada = false);
    bool _eh_cor_fundo_padrao(const QColor& cor = QColor());
    void configurar_tabela(QObject* tabela = nullptr);
    void ajustar_larguras_colunas(QObject* tabela = nullptr, const QVariantList& colunas_visiveis = {});
    void aplicar_quebra_linha_cabecalho(QObject* tabela = nullptr, int coluna_index = 0);
    void aplicar_quebra_linha_todos_cabecalhos(QObject* tabela = nullptr);
    void ajustar_altura_cabecalho(QObject* tabela = nullptr);
    void redimensionar_cabecalho(int logicalIndex = 0, int oldSize = 0, int newSize = 0);
    void atualizar_cabecalhos();
    Q_INVOKABLE void atualizar_dados_tabela(QObject* tabela_dados = nullptr, int row_especifico = -1);
    Q_INVOKABLE void atualizar_linha_mais_recente();
    Q_INVOKABLE void atualizar_linha_mais_recente(QObject* tabela_dados);
    Q_INVOKABLE void atualizar_linha_mais_recente(QObject* tabela_dados, const QVariantMap& evento);
    void atualizar_visualizacao_tabela();
    void atualizar_visibilidade_colunas(bool atualizar_em_massa = false);
    void _invalidar_cache_cores();
    QVariantMap _obter_cores_operacao();
    QVariantMap _obter_indices_colunas(QObject* tabela = nullptr);
    void ativar_cores();
    void ocultar_cores();
    void aplicar_cores_linha_especifica(QObject* tabela = nullptr, int row = -1, const QString& tipo_operacao_valor = QString());
    void aplicar_cores_todas_colunas();
    void redefinir_cores_todas_colunas();
    void remover_cor_coluna(const QString& coluna = QString());
    void atualizar_cores_colunas(bool aplicar_em_massa = false, const QSet<QString>& colunas_afetadas = {});
    void set_coluna_colorir(const QString& coluna = QString(), bool ativo = true);
    void set_colunas_colorir_em_massa(const QSet<QString>& colunas = {});
    void remover_todas_cores_colunas();
    void salvar_configuracoes_cores();
    void _carregar_configuracoes_cores();
    bool eh_coluna_personalizada_colorida(const QString& coluna = QString());
    void mostrar_dialogo_configuracao(const QPoint& pos = QPoint());
    void ajustar_cor_selecao();
    void atualizar_estilo_tema(const QString& novo_tema = QString());
    void _atualizar_cores_celulas_incolores(const QString& novo_tema = QString());
    bool _eh_cor_padrao_qualquer_tema(const QColor& cor = QColor());
    void _forcar_reconstrucao_todas_celulas(const QString& novo_tema = QString());
    bool _processar_selecao_background();
    void iniciar_processamento_pesado(const QVariant& dados = QVariant());
    void aplicar_cores_todas_colunas_processamento(const QVariant& dados = QVariant());
    QVariantMap _processar_cores_em_background(const QVariant& dados = QVariant());
    void _on_cores_processadas(const QVariantMap& cores = QVariantMap());
    void _atualizar_cores_na_interface(const QVariantMap& resultado = QVariantMap());
    void atualizar_interface_pos_processamento(const QVariantMap& resultado = QVariantMap());
    void shutdown_executors();
    void _on_idioma_alterado(const QString& idioma = QString());
    void retraduzir_dados_existentes();
    void _executar_retraducao_agendada();

signals:
    void cores_processadas(const QVariantMap& cores);

public:
    QObject* interface;
    QObject* loc;
    QObject* monitor_tema;
    QTimer timer_tema;
    QString tema_pendente;
    QTimer timer_atualizacao;
    bool atualizacao_pendente;
    QHash<QString, QString> texto_original_cabecalhos;
    QSet<QString> colunas_para_colorir;
    QHash<QString, QVariant> cache_cores;
    QHash<QString, int> cache_indices_colunas;
    bool cores_visiveis;
    bool ordenacao_habilitada;
    QString idioma_ultima_retraducao;
    bool retraducao_realizada_para_idioma;
    QString idioma_alvo_retraducao;
    QTimer timer_retraducao;
    bool retraducao_em_andamento;
    bool retraducao_agendada;
};

}  // namespace source::services
