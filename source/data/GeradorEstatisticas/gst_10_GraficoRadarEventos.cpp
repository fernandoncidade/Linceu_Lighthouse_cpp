#include "gst_10_GraficoRadarEventos.h"

#include <QVector>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoRadarEventos::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("events_monitored", QStringLiteral("Eventos Monitorados"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantMap out;
        out.insert("kind", "radar_eventos");
        out.insert("title", titulo);
        out.insert("top_types", criar_resumo_por_coluna(dados, QStringLiteral("tipo"), 8));
        out.insert("top_operations", criar_resumo_por_coluna(dados, QStringLiteral("tipo_operacao"), 8));

        QVector<int> dias(7, 0);
        QVector<int> horas(24, 0);
        for (const QVariant& v : dados) {
            const QDateTime dt = extrair_timestamp(v.toMap());
            if (!dt.isValid()) continue;
            dias[dt.date().dayOfWeek() - 1] += 1;
            horas[dt.time().hour()] += 1;
        }

        QVariantList dias_values;
        QVariantList horas_values;
        for (int d : dias) dias_values.append(d);
        for (int h : horas) horas_values.append(h);
        out.insert("by_weekday", dias_values);
        out.insert("by_hour", horas_values);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar radar de eventos", true);
        return _criar_grafico_sem_dados(QStringLiteral("Eventos Monitorados"));
    }
}

}  // namespace source::data
