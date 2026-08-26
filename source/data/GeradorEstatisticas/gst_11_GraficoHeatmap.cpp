#include "gst_11_GraficoHeatmap.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QMap>
#include <QSet>
#include <QVector>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoHeatmap::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("temporal_distribution", QStringLiteral("Distribuição Temporal"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Evento {
            QDateTime dt;
            QString op;
            QString tipo;
            QString arquivo;
            qint64 bucket = 0;
        };
        QVector<Evento> eventos;
        eventos.reserve(dados.size());

        const QString op_unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
        const QString tipo_unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QDateTime dt = extrair_timestamp(row);
            if (!dt.isValid()) continue;
            QString op = row.value(QStringLiteral("tipo_operacao")).toString().trimmed();
            if (op.isEmpty()) op = op_unknown;
            QString tipo = row.value(QStringLiteral("tipo")).toString().trimmed();
            if (tipo.isEmpty()) tipo = tipo_unknown;
            QString arquivo = row.value(QStringLiteral("nome")).toString().trimmed();
            const QString dir = row.value(QStringLiteral("dir_atual")).toString().trimmed();
            if (!dir.isEmpty() && !arquivo.isEmpty()) arquivo = normalizar_caminho(dir, arquivo);
            if (arquivo.isEmpty()) arquivo = row.value(QStringLiteral("nome_anterior")).toString().trimmed();
            if (arquivo.isEmpty()) arquivo = QStringLiteral("-");
            eventos.push_back({dt, op, tipo, arquivo, 0});
        }
        if (eventos.isEmpty()) return _criar_grafico_sem_dados(titulo);

        std::sort(eventos.begin(), eventos.end(),
                  [](const Evento& a, const Evento& b) { return a.dt < b.dt; });

        const QDateTime data_min = eventos.first().dt;
        const QDateTime data_max = eventos.last().dt;
        const double total_minutos = qMax(1.0, data_min.secsTo(data_max) / 60.0);

        int bucket_minutos = 60;
        if (total_minutos <= 120.0) bucket_minutos = 5;
        else if (total_minutos <= 720.0) bucket_minutos = 15;
        else if (total_minutos <= 2880.0) bucket_minutos = 30;
        else if (total_minutos <= 10080.0) bucket_minutos = 60;
        else bucket_minutos = 180;
        const qint64 bucket_ms = static_cast<qint64>(bucket_minutos) * 60 * 1000;

        struct BucketInfo {
            QHash<QString, int> por_operacao;
            int total = 0;
        };
        QMap<qint64, BucketInfo> buckets;
        QHash<QString, int> total_por_operacao;
        for (Evento& e : eventos) {
            const qint64 key = (e.dt.toMSecsSinceEpoch() / bucket_ms) * bucket_ms;
            e.bucket = key;
            BucketInfo& b = buckets[key];
            b.por_operacao[e.op] += 1;
            b.total += 1;
            total_por_operacao[e.op] += 1;
        }

        if (buckets.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QPair<QString, int>> ops_ordenadas;
        ops_ordenadas.reserve(total_por_operacao.size());
        for (auto it = total_por_operacao.constBegin(); it != total_por_operacao.constEnd(); ++it) {
            ops_ordenadas.append({it.key(), it.value()});
        }
        std::sort(ops_ordenadas.begin(), ops_ordenadas.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) { return a.second > b.second; });

        const QList<QString> fallback_colors = {
            QStringLiteral("#0000ff"), QStringLiteral("#ff0000"), QStringLiteral("#808080"),
            QStringLiteral("#ff6200"), QStringLiteral("#ff00ff"), QStringLiteral("#00cc00")};

        QVariantList operation_order;
        QVariantMap op_colors;
        for (int i = 0; i < ops_ordenadas.size(); ++i) {
            const QString op = ops_ordenadas.at(i).first;
            operation_order.append(op);
            op_colors.insert(op, cores_operacoes_.value(op, fallback_colors.at(i % fallback_colors.size())));
        }

        QHash<QString, qint64> bucket_pico_por_op;
        QHash<QString, int> quantidade_pico_por_op;
        for (const auto& op_item : ops_ordenadas) {
            const QString& op = op_item.first;
            int melhor_qtd = 0;
            qint64 melhor_bucket = 0;
            for (auto it = buckets.constBegin(); it != buckets.constEnd(); ++it) {
                const int c = it.value().por_operacao.value(op, 0);
                if (c > melhor_qtd) {
                    melhor_qtd = c;
                    melhor_bucket = it.key();
                }
            }
            if (melhor_qtd > 0) {
                bucket_pico_por_op.insert(op, melhor_bucket);
                quantidade_pico_por_op.insert(op, melhor_qtd);
            }
        }

        if (bucket_pico_por_op.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantList points;
        qint64 x_min = std::numeric_limits<qint64>::max();
        qint64 x_max = std::numeric_limits<qint64>::min();
        int y_max = 1;

        const bool multiplos_dias = data_min.date() != data_max.date();
        const QString faixa_format = multiplos_dias ? QStringLiteral("dd/MM HH:mm") : QStringLiteral("HH:mm");
        const QString instante_format = QStringLiteral("dd/MM/yyyy HH:mm:ss");

        for (const auto& op_item : ops_ordenadas) {
            const QString& op = op_item.first;
            if (!bucket_pico_por_op.contains(op)) continue;
            const qint64 bucket_pico = bucket_pico_por_op.value(op);
            const int qtd_pico = quantidade_pico_por_op.value(op, 0);
            if (qtd_pico <= 0) continue;

            const QDateTime faixa_inicio = QDateTime::fromMSecsSinceEpoch(bucket_pico);
            const QDateTime faixa_fim = faixa_inicio.addMSecs(bucket_ms);
            const double jitter_x_span = static_cast<double>(bucket_ms) * 0.08;
            const double jitter_y_span = qMax(0.20, static_cast<double>(qtd_pico) * 0.06);

            for (int idx = 0; idx < eventos.size(); ++idx) {
                const Evento& e = eventos.at(idx);
                if (e.op != op || e.bucket != bucket_pico) continue;

                const QString seed = QStringLiteral("%1|%2|%3|%4")
                                         .arg(e.arquivo)
                                         .arg(e.dt.toMSecsSinceEpoch())
                                         .arg(op)
                                         .arg(idx);
                const uint h1 = qHash(seed);
                const uint h2 = qHash(seed + QStringLiteral("#y"));
                const double rx = (static_cast<double>(h1 % 2001) - 1000.0) / 1000.0;
                const double ry = (static_cast<double>(h2 % 2001) - 1000.0) / 1000.0;

                double x = static_cast<double>(e.dt.toMSecsSinceEpoch()) + (rx * jitter_x_span);
                x = qBound(static_cast<double>(bucket_pico) + 1.0, x,
                           static_cast<double>(bucket_pico + bucket_ms) - 1.0);

                double y = static_cast<double>(qtd_pico) + (ry * jitter_y_span);
                y = qMax(0.1, y);

                QVariantMap p;
                p.insert(QStringLiteral("tipo_operacao"), op);
                p.insert(QStringLiteral("x"), x);
                p.insert(QStringLiteral("y"), y);
                p.insert(QStringLiteral("bucket_label"), faixa_inicio.toString(faixa_format));
                p.insert(QStringLiteral("bucket_end_label"), faixa_fim.toString(faixa_format));
                p.insert(QStringLiteral("timestamp_label"), e.dt.toString(instante_format));
                p.insert(QStringLiteral("arquivo"), e.arquivo);
                p.insert(QStringLiteral("tipo"), e.tipo);
                p.insert(QStringLiteral("group_count"), qtd_pico);
                points.append(p);

                const qint64 xll = static_cast<qint64>(std::llround(x));
                x_min = qMin(x_min, xll);
                x_max = qMax(x_max, xll);
                y_max = qMax(y_max, qtd_pico);
            }
        }

        if (points.isEmpty()) return _criar_grafico_sem_dados(titulo);
        if (x_min >= x_max) {
            const qint64 any_peak = bucket_pico_por_op.constBegin().value();
            x_min = any_peak - (bucket_ms / 2);
            x_max = any_peak + (bucket_ms / 2);
        }

        QVariantMap out;
        out.insert(QStringLiteral("kind"), QStringLiteral("temporal_scatter"));
        out.insert(
            QStringLiteral("title"),
            obter_texto("temporal_peak_cloud_title", QStringLiteral("%1 (nuvem de pico por operação)")).arg(titulo));
        out.insert(QStringLiteral("points"), points);
        out.insert(QStringLiteral("operation_order"), operation_order);
        out.insert(QStringLiteral("op_colors"), op_colors);
        out.insert(QStringLiteral("x_min"), static_cast<double>(x_min));
        out.insert(QStringLiteral("x_max"), static_cast<double>(x_max));
        out.insert(QStringLiteral("y_max"), static_cast<double>(y_max));
        out.insert(QStringLiteral("x_label"), obter_texto("time_minutes", QStringLiteral("Tempo")));
        out.insert(QStringLiteral("y_label"), obter_texto("quantity", QStringLiteral("Quantidade")));
        out.insert(QStringLiteral("bucket_minutes"), bucket_minutos);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar heatmap", true);
        return _criar_grafico_sem_dados(
            obter_texto("temporal_distribution", QStringLiteral("Distribuição Temporal")));
    }
}

}  // namespace source::data
