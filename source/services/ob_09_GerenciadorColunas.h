#pragma once

#include <memory>

#include <QHash>
#include <QMutex>
#include <QObject>
#include <QPoint>
#include <QQueue>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace source::services {
class GerenciadorTabela;
class Observador;

class GerenciadorColunas : public QObject {
    Q_OBJECT

public:
    explicit GerenciadorColunas(QObject* interface_monitor);
    explicit GerenciadorColunas(source::services::Observador* observador);
    ~GerenciadorColunas() override;

    void atualizar_interface(const QString& idioma);
    QVariantMap salvar_estado_tabela(QObject* tabela);
    void restaurar_estado_tabela(QObject* tabela, const QList<QVariantMap>& eventos);
    void carregar_configuracoes();
    void salvar_configuracoes();
    void processar_fila_metadados();
    void callback_metadados(const QVariant& futuro = QVariant());
    void configurar_tabela(QObject* tabela);
    void mostrar_dialogo_configuracao(const QPoint& pos = QPoint());
    void _invalidar_cache_diretorios_relacionados(const QString& caminho);
    QVariantMap get_metadados(const QVariantMap& item);
    QString identificar_tipo_arquivo(const QString& caminho);
    void adicionar_metadados_ao_item(const QVariantMap& item);
    void processar_coluna_thread(const QString& coluna_key);
    void adicionar_item_para_coluna(const QString& coluna_key, const QVariantMap& item);
    void atualizar_coluna_interface(const QString& coluna_key = QString());
    void extrair_metadados_em_lote(const QList<QVariantMap>& lista_itens);
    void _metadados_extraidos_callback(const QVariant& futuro = QVariant());
    QString _get_size_unit(const QVariantMap& item, const QString& unidade);
    qint64 _get_tamanho_bytes(const QVariantMap& item);

    QObject* interface;
    QObject* loc;
    QString config_path;
    QHash<QString, QVariant> cache_metadados;
    QQueue<QVariantMap> fila_metadados;
    QMutex lock_cache;
    bool processando_metadados;
    std::unique_ptr<source::services::GerenciadorTabela> gerenciador_tabela;
    QHash<QString, QVariantMap> COLUNAS_DISPONIVEIS;
    QHash<QString, QQueue<QVariantMap>> filas_colunas;
    QHash<QString, QVariant> resultados_colunas;
    QMutex lock_resultados;

private:
    void inicializar_base(QObject* interface_monitor, source::services::Observador* observador);
};

}  // namespace source::services
