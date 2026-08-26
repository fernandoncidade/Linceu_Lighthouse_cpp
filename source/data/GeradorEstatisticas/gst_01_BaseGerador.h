#pragma once

#include <atomic>
#include <memory>

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

namespace source::data {

class BaseGerador {
public:
    explicit BaseGerador(const QString& db_path, QObject* localizador = nullptr, QObject* interface_principal = nullptr);
    virtual ~BaseGerador();

    virtual QVariant gerar();
    void atualizar_textos_traduzidos();

protected:
    QString db_path_;
    QObject* loc_;
    QObject* interface_;
    QHash<QString, QString> cores_operacoes_;
    std::shared_ptr<std::atomic_bool> alive_flag_;

    void _atualizar_textos_traduzidos();
    QVariantList _obter_dados() const;
    QVariantList _traduzir_dados(const QVariantList& dados) const;
    QHash<QString, QString> _obter_mapeamento_operacoes() const;
    QHash<QString, QString> _obter_mapeamento_tipos() const;
    QVariant _criar_grafico_sem_dados(const QString& titulo) const;
    QString _obter_cor_hex_operacao(const QString& op_key, const QString& fallback_hex) const;

    QString obter_texto(const QString& chave, const QString& fallback = QString()) const;
    QVariantMap criar_resumo_por_coluna(const QVariantList& dados, const QString& coluna, int top_n = -1) const;
    double extrair_tamanho_bytes(const QVariantMap& row) const;
    QDateTime extrair_timestamp(const QVariantMap& row) const;
    QString normalizar_caminho(const QString& dir_val, const QString& nome_val) const;
};

}  // namespace source::data
