#include "gst_06_GraficoHistograma.h"

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoHistograma::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("hour_histogram", QStringLiteral("Distribuição de Operações por Hora"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVector<int> contagem(24, 0);
        for (const QVariant& v : dados) {
            const QDateTime dt = extrair_timestamp(v.toMap());
            if (dt.isValid()) contagem[dt.time().hour()] += 1;
        }

        QVariantList horas;
        QVariantList valores;
        for (int h = 0; h < 24; ++h) {
            horas.append(h);
            valores.append(contagem[h]);
        }

        QVariantMap out;
        out.insert("kind", "histograma");
        out.insert("title", titulo);
        out.insert("labels", horas);
        out.insert("values", valores);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar histograma", true);
        return _criar_grafico_sem_dados(QStringLiteral("Distribuição de Operações por Hora"));
    }
}

}  // namespace source::data
