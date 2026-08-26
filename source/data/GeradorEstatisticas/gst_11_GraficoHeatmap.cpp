#include "gst_11_GraficoHeatmap.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QMap>
#include <QSet>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVector>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
constexpr int kMaxTemporalScatterPointsTotal = 18000;
constexpr int kMaxTemporalScatterPointsPerOperation = 3500;

QDateTime parse_temporal_timestamp(const QVariant& value) {
    if (value.canConvert<QDateTime>()) {
        const QDateTime dt = value.toDateTime();
        if (dt.isValid()) return dt;
    }

    const QString text = value.toString().trimmed();
    if (text.isEmpty()) return {};

    bool epochOk = false;
    const qlonglong epoch = text.toLongLong(&epochOk);
    if (epochOk) {
        const int digits = text.startsWith(QLatin1Char('-')) ? text.size() - 1 : text.size();
        if (digits >= 16) return QDateTime::fromMSecsSinceEpoch(epoch / 1000);
        if (digits >= 13) return QDateTime::fromMSecsSinceEpoch(epoch);
        return QDateTime::fromSecsSinceEpoch(epoch);
    }

    QDateTime dt = QDateTime::fromString(text, Qt::ISODateWithMs);
    if (dt.isValid()) return dt;
    dt = QDateTime::fromString(text, Qt::ISODate);
    if (dt.isValid()) return dt;

    const QString isoGuess = (text.contains(QLatin1Char(' ')) && !text.contains(QLatin1Char('T')))
                                 ? QString(text).replace(QLatin1Char(' '), QLatin1Char('T'))
                                 : text;
    if (isoGuess != text) {
        dt = QDateTime::fromString(isoGuess, Qt::ISODateWithMs);
        if (dt.isValid()) return dt;
        dt = QDateTime::fromString(isoGuess, Qt::ISODate);
        if (dt.isValid()) return dt;
    }

    const QStringList formats = {
        QStringLiteral("yyyy-MM-dd HH:mm:ss"),
        QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss"),
        QStringLiteral("yyyy/MM/dd HH:mm:ss.zzz"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss"),
        QStringLiteral("dd/MM/yyyy HH:mm:ss.zzz"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss"),
        QStringLiteral("MM/dd/yyyy HH:mm:ss.zzz"),
    };
    for (const QString& format : formats) {
        dt = QDateTime::fromString(text, format);
        if (dt.isValid()) return dt;
    }
    return {};
}

QString traduzir_valor_monitorado(const QHash<QString, QString>& mapa,
                                  const QString& value,
                                  const QString& unknown) {
    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) return unknown;
    return mapa.value(trimmed, trimmed);
}

struct TemporalPeakPoint {
    qint64 timestampMs = 0;
    QString operacao;
    QString operacaoRaw;
    QString tipo;
    QString tipoRaw;
    QString arquivo;
    int ordinal = 0;
};

struct TemporalPeakSampler {
    int limit = kMaxTemporalScatterPointsPerOperation;
    int stride = 1;
    int seen = 0;
    QVector<TemporalPeakPoint> points;

    void consider(const TemporalPeakPoint& point) {
        if ((seen % stride) == 0) {
            points.append(point);
            if (points.size() > limit) {
                QVector<TemporalPeakPoint> reduced;
                reduced.reserve((points.size() + 1) / 2);
                for (int i = 0; i < points.size(); i += 2) {
                    reduced.append(points.at(i));
                }
                points = std::move(reduced);
                stride *= 2;
            }
        }
        ++seen;
    }
};
}  // namespace

QVariant GraficoHeatmap::gerar() {
    try {
        const QString titulo = obter_texto("temporal_distribution", QStringLiteral("Distribuição Temporal"));
        const QString op_unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
        const QString tipo_unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
        const QHash<QString, QString> mapa_ops = _obter_mapeamento_operacoes();
        const QHash<QString, QString> mapa_tipos = _obter_mapeamento_tipos();

        QDateTime data_min;
        QDateTime data_max;
        qint64 total_validos = 0;
        QHash<QString, QHash<qint64, int>> buckets_por_operacao;
        QHash<QString, int> total_por_operacao;

        const QString conn_name = QStringLiteral("temporal_scatter_conn_%1_%2")
                                      .arg(reinterpret_cast<quintptr>(this))
                                      .arg(QDateTime::currentMSecsSinceEpoch());
        bool ok = true;

        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para dispersão temporal: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(QStringLiteral(
                        "SELECT timestamp FROM monitoramento WHERE timestamp IS NOT NULL"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar faixa temporal: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QDateTime dt = parse_temporal_timestamp(query.value(0));
                        if (!dt.isValid()) continue;
                        if (!data_min.isValid() || dt < data_min) data_min = dt;
                        if (!data_max.isValid() || dt > data_max) data_max = dt;
                        ++total_validos;
                    }
                }

                if (ok && total_validos <= 0) {
                    db.close();
                }
            }
        }
        QSqlDatabase::removeDatabase(conn_name);
        if (!ok || total_validos <= 0 || !data_min.isValid() || !data_max.isValid()) {
            return _criar_grafico_sem_dados(titulo);
        }

        const double total_minutos = qMax(1.0, data_min.secsTo(data_max) / 60.0);

        int bucket_minutos = 60;
        if (total_minutos <= 120.0) bucket_minutos = 5;
        else if (total_minutos <= 720.0) bucket_minutos = 15;
        else if (total_minutos <= 2880.0) bucket_minutos = 30;
        else if (total_minutos <= 10080.0) bucket_minutos = 60;
        else bucket_minutos = 180;
        const qint64 bucket_ms = static_cast<qint64>(bucket_minutos) * 60 * 1000;

        const QString conn_name_buckets = QStringLiteral("temporal_scatter_buckets_%1_%2")
                                              .arg(reinterpret_cast<quintptr>(this))
                                              .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_buckets);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para buckets temporais: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(QStringLiteral(
                        "SELECT tipo_operacao, timestamp FROM monitoramento WHERE timestamp IS NOT NULL"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar buckets temporais: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QDateTime dt = parse_temporal_timestamp(query.value(1));
                        if (!dt.isValid()) continue;
                        const QString op =
                            traduzir_valor_monitorado(mapa_ops, query.value(0).toString(), op_unknown);
                        const qint64 bucket = (dt.toMSecsSinceEpoch() / bucket_ms) * bucket_ms;
                        buckets_por_operacao[op][bucket] += 1;
                        total_por_operacao[op] += 1;
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_buckets);
        if (!ok || buckets_por_operacao.isEmpty()) {
            return _criar_grafico_sem_dados(titulo);
        }

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
            const QHash<qint64, int> buckets_op = buckets_por_operacao.value(op);
            for (auto it = buckets_op.constBegin(); it != buckets_op.constEnd(); ++it) {
                const int c = it.value();
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

        QHash<QString, TemporalPeakSampler> samplers;
        const int per_operation_limit =
            qBound(800, kMaxTemporalScatterPointsTotal / qMax(1, ops_ordenadas.size()),
                   kMaxTemporalScatterPointsPerOperation);
        for (const auto& op_item : ops_ordenadas) {
            const QString& op = op_item.first;
            if (!bucket_pico_por_op.contains(op)) continue;
            TemporalPeakSampler sampler;
            sampler.limit = per_operation_limit;
            sampler.points.reserve(qMin(per_operation_limit, quantidade_pico_por_op.value(op, 0)));
            samplers.insert(op, sampler);
        }

        const QString conn_name_points = QStringLiteral("temporal_scatter_points_%1_%2")
                                             .arg(reinterpret_cast<quintptr>(this))
                                             .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_points);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para pontos temporais: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(QStringLiteral(
                        "SELECT tipo_operacao, tipo, timestamp, nome, dir_atual "
                        "FROM monitoramento WHERE timestamp IS NOT NULL"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar pontos temporais: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    int ordinal = 0;
                    while (query.next()) {
                        const QDateTime dt = parse_temporal_timestamp(query.value(2));
                        if (!dt.isValid()) continue;
                        const QString rawOp = query.value(0).toString().trimmed();
                        const QString op = traduzir_valor_monitorado(mapa_ops, rawOp, op_unknown);
                        if (!bucket_pico_por_op.contains(op)) continue;

                        const qint64 timestamp_ms = dt.toMSecsSinceEpoch();
                        const qint64 bucket = (timestamp_ms / bucket_ms) * bucket_ms;
                        if (bucket != bucket_pico_por_op.value(op)) continue;

                        QString arquivo = query.value(3).toString().trimmed();
                        const QString dir = query.value(4).toString().trimmed();
                        if (!dir.isEmpty() && !arquivo.isEmpty()) {
                            arquivo = normalizar_caminho(dir, arquivo);
                        }
                        if (arquivo.isEmpty()) arquivo = QStringLiteral("-");

                        TemporalPeakPoint point;
                        point.timestampMs = timestamp_ms;
                        point.operacao = op;
                        point.operacaoRaw = rawOp;
                        point.tipoRaw = query.value(1).toString().trimmed();
                        point.tipo = traduzir_valor_monitorado(mapa_tipos, point.tipoRaw, tipo_unknown);
                        point.arquivo = arquivo;
                        point.ordinal = ordinal++;
                        samplers[op].consider(point);
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_points);
        if (!ok) {
            return _criar_grafico_sem_dados(titulo);
        }

        QVariantList points;
        qint64 x_min = std::numeric_limits<qint64>::max();
        qint64 x_max = std::numeric_limits<qint64>::min();
        int y_max = 1;

        const bool multiplos_dias = data_min.date() != data_max.date();
        const QString faixa_format = multiplos_dias ? QStringLiteral("dd/MM HH:mm") : QStringLiteral("HH:mm");
        const QString instante_format = QStringLiteral("dd/MM/yyyy HH:mm:ss");

        for (const auto& op_item : ops_ordenadas) {
            const QString& op = op_item.first;
            if (!bucket_pico_por_op.contains(op) || !samplers.contains(op)) continue;
            const qint64 bucket_pico = bucket_pico_por_op.value(op);
            const int qtd_pico = quantidade_pico_por_op.value(op, 0);
            if (qtd_pico <= 0) continue;

            const QDateTime faixa_inicio = QDateTime::fromMSecsSinceEpoch(bucket_pico);
            const QDateTime faixa_fim = faixa_inicio.addMSecs(bucket_ms);
            const double jitter_x_span = static_cast<double>(bucket_ms) * 0.08;
            const double jitter_y_span = qMax(0.20, static_cast<double>(qtd_pico) * 0.06);

            const QVector<TemporalPeakPoint> sampled_points = samplers.value(op).points;
            for (const TemporalPeakPoint& e : sampled_points) {
                const QString seed = QStringLiteral("%1|%2|%3|%4")
                                         .arg(e.arquivo)
                                         .arg(e.timestampMs)
                                         .arg(op)
                                         .arg(e.ordinal);
                const uint h1 = qHash(seed);
                const uint h2 = qHash(seed + QStringLiteral("#y"));
                const double rx = (static_cast<double>(h1 % 2001) - 1000.0) / 1000.0;
                const double ry = (static_cast<double>(h2 % 2001) - 1000.0) / 1000.0;

                double x = static_cast<double>(e.timestampMs) + (rx * jitter_x_span);
                x = qBound(static_cast<double>(bucket_pico) + 1.0, x,
                           static_cast<double>(bucket_pico + bucket_ms) - 1.0);

                double y = static_cast<double>(qtd_pico) + (ry * jitter_y_span);
                y = qMax(0.1, y);

                QVariantMap p;
                p.insert(QStringLiteral("tipo_operacao"), op);
                p.insert(QStringLiteral("tipo_operacao_raw"), e.operacaoRaw);
                p.insert(QStringLiteral("x"), x);
                p.insert(QStringLiteral("y"), y);
                p.insert(QStringLiteral("bucket_label"), faixa_inicio.toString(faixa_format));
                p.insert(QStringLiteral("bucket_end_label"), faixa_fim.toString(faixa_format));
                p.insert(QStringLiteral("timestamp_label"),
                         QDateTime::fromMSecsSinceEpoch(e.timestampMs).toString(instante_format));
                p.insert(QStringLiteral("arquivo"), e.arquivo);
                p.insert(QStringLiteral("tipo"), e.tipo);
                p.insert(QStringLiteral("tipo_raw"), e.tipoRaw);
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
        out.insert(QStringLiteral("grid_overlay"), true);
        out.insert(QStringLiteral("fast_point_cloud"), true);
        out.insert(QStringLiteral("source_points"), total_validos);
        out.insert(QStringLiteral("display_points"), points.size());
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar heatmap", true);
        return _criar_grafico_sem_dados(
            obter_texto("temporal_distribution", QStringLiteral("Distribuição Temporal")));
    }
}

}  // namespace source::data
