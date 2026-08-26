#pragma once

#include <functional>

#include <QHash>
#include <QList>
#include <QString>
#include <QVariantMap>

namespace source::services {
class Observador;

class BaseEvento {
public:
    explicit BaseEvento(source::services::Observador* observador);
    virtual ~BaseEvento() = default;

    void set_callback(const QVariant& callback = QVariant());
    void criar_banco_de_dados();
    void processar_exclusao(const QVariantMap& evento = QVariantMap());
    void registrar_evento_especifico(const QString& tabela = QString(), const QVariantMap& evento = QVariantMap());
    QVariantMap obter_metadados_arquivo_excluido(const QString& nome = QString(), const QString& caminho = QString());
    void registrar_evento_no_banco(const QVariantMap& evento = QVariantMap());
    void _atualizar_interface_apos_evento(const QVariantMap& evento = QVariantMap());
    void scan_directory(const QString& diretorio = QString());
    QString get_tipo_from_snapshot(const QString& nome = QString());
    bool is_directory(const QString& caminho = QString());
    void limpar_registros();
    QString obter_tipo_anterior(const QString& nome_base = QString());
    void notificar_evento(const QString& tipo_operacao = QString(), const QString& nome = QString(), const QString& dir_anterior = QString(), const QString& dir_atual = QString());
    void _remover_exclusao_temporaria(const QString& nome = QString(), const QString& dir_anterior = QString(), const QString& exclusao_id = QString());
    QVariantMap _criar_evento_exclusao(const QString& nome_base = QString(), const QString& dir_anterior = QString());
    QVariantMap _criar_evento_padrao(const QString& tipo_operacao = QString(), const QString& nome_base = QString(), const QString& dir_anterior = QString(), const QString& dir_atual = QString());
    void _atualizar_interface_apos_exclusao(const QVariantMap& evento = QVariantMap());
    void processar_eventos_movimentacao(const QList<QVariantMap>& eventos_movidos = {}, const std::function<void()>& callback = {});
    bool _inserir_evento_movido(const QVariantMap& evento = QVariantMap());

    source::services::Observador* observador;
    QString db_path;
    QHash<QString, QString> operacoes;
    int eventos_excluidos;
    QVariant callback;
};

}  // namespace source::services
