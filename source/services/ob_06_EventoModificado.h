#pragma once

#include <QHash>
#include <QSet>
#include <QString>

#include "ob_02_BaseEvento.h"

namespace source::services {

class EventoModificado : public BaseEvento {
public:
    explicit EventoModificado(source::services::Observador* observador);

    double calcular_intervalo(const QString& caminho = QString());
    double _calcular_intervalo_original(const QString& caminho = QString());
    bool is_arquivo_codigo_grande(const QString& caminho = QString());
    void _limpar_cache_metadados(const QString& caminho_completo = QString(), const QString& ext = QString());
    bool processar(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);
    bool _processar_massivo(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);
    bool _processar_normal(const QString& nome_arquivo, const QString& caminho_completo, double tempo_atual);

    double intervalo_base;
    qint64 tamanho_threshold;
    double max_intervalo;
    double exclusao_threshold;
    double min_intervalo;
    QHash<QString, double> modificacoes_recentes;
    QHash<QString, int> count_modificacoes;
    QHash<QString, QVariant> eventos_grandes;
    qint64 grande_arquivo_threshold;
    double tempo_espera_grande_arquivo;
    QSet<QString> arquivos_em_processamento;

    QSet<QString> EXTENSOES_VIDEO;
    QSet<QString> EXTENSOES_RAPIDAS;
    QSet<QString> EXTENSOES_CODIGO;

    int CODIGO_LINHA_THRESHOLD;
};

}  // namespace source::services
