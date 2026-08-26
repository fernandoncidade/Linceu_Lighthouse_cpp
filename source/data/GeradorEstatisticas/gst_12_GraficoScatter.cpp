#include "gst_12_GraficoScatter.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QVector>
#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
constexpr int kMaxFileSizeScatterPoints = 20000;

double to_double_locale(const QVariant& value) {
    bool ok = false;
    double parsed = value.toDouble(&ok);
    if (ok) return parsed;

    QString text = value.toString().trimmed();
    if (text.isEmpty()) return 0.0;
    text.replace(QLatin1Char(','), QLatin1Char('.'));
    parsed = text.toDouble(&ok);
    return ok ? parsed : 0.0;
}

double bytes_from_query(const QSqlQuery& query,
                        int idx_size_b,
                        int idx_size_kb,
                        int idx_size_mb,
                        int idx_size_gb,
                        int idx_size_tb) {
    const double sizeB = to_double_locale(query.value(idx_size_b));
    if (sizeB > 0.0) return sizeB;
    const double sizeKb = to_double_locale(query.value(idx_size_kb));
    if (sizeKb > 0.0) return sizeKb * 1024.0;
    const double sizeMb = to_double_locale(query.value(idx_size_mb));
    if (sizeMb > 0.0) return sizeMb * 1024.0 * 1024.0;
    const double sizeGb = to_double_locale(query.value(idx_size_gb));
    if (sizeGb > 0.0) return sizeGb * 1024.0 * 1024.0 * 1024.0;
    const double sizeTb = to_double_locale(query.value(idx_size_tb));
    if (sizeTb > 0.0) return sizeTb * 1024.0 * 1024.0 * 1024.0 * 1024.0;
    return 0.0;
}

QDateTime parse_scatter_timestamp(const QVariant& value) {
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

double percentile(QVector<double> values, const double q) {
    if (values.isEmpty()) return 0.0;
    if (values.size() == 1) return values.first();

    const double idx = (values.size() - 1) * q;
    const int lo = static_cast<int>(std::floor(idx));
    const int hi = static_cast<int>(std::ceil(idx));
    const double frac = idx - lo;
    auto nthLo = values.begin() + lo;
    std::nth_element(values.begin(), nthLo, values.end());
    const double loValue = values.at(lo);
    if (lo == hi) return loValue;
    auto nthHi = values.begin() + hi;
    std::nth_element(values.begin(), nthHi, values.end());
    return loValue + (values.at(hi) - loValue) * frac;
}

QString translated_value(const QHash<QString, QString>& map, const QString& raw, const QString& fallback) {
    const QString trimmed = raw.trimmed();
    if (trimmed.isEmpty()) return fallback;
    return map.value(trimmed, trimmed);
}

struct ScatterSamplePoint {
    qint64 timestampMs = 0;
    double sizeMb = 0.0;
    QString operacao;
    QString operacaoRaw;
    QString tipo;
    QString tipoRaw;
    QString arquivo;
    QString timestampText;
};

struct ScatterSampler {
    int limit = kMaxFileSizeScatterPoints;
    int stride = 1;
    int seen = 0;
    QVector<ScatterSamplePoint> points;

    void consider(const ScatterSamplePoint& point) {
        if ((seen % stride) == 0) {
            points.append(point);
            if (points.size() > limit) {
                QVector<ScatterSamplePoint> reduced;
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

double scaled_time_value(qint64 offsetMs, const QString& escala) {
    if (escala == QStringLiteral("milliseconds")) return static_cast<double>(offsetMs);
    if (escala == QStringLiteral("centiseconds")) return (static_cast<double>(offsetMs) / 1000.0) * 100.0;
    if (escala == QStringLiteral("minutes")) return (static_cast<double>(offsetMs) / 1000.0) / 60.0;
    if (escala == QStringLiteral("hours")) return (static_cast<double>(offsetMs) / 1000.0) / 3600.0;
    if (escala == QStringLiteral("days")) return (static_cast<double>(offsetMs) / 1000.0) / 86400.0;
    return static_cast<double>(offsetMs) / 1000.0;
}
}  // namespace

QVariant GraficoScatter::gerar() {
    try {
        const QString titulo = obter_texto("file_size_analysis", QStringLiteral("Análise de Tamanho de Arquivo"));
        constexpr double bytesPorMb = 1024.0 * 1024.0;
        const QString unknown = obter_texto("unknown", QStringLiteral("Desconhecido"));
        const QHash<QString, QString> mapa_ops = _obter_mapeamento_operacoes();
        const QHash<QString, QString> mapa_tipos = _obter_mapeamento_tipos();

        QVector<double> valores_mb;
        int invalid_size = 0;
        int invalid_timestamp = 0;
        qint64 valid_source_rows = 0;

        int outliers_removidos = 0;
        qint64 filtered_rows = 0;
        QDateTime data_min;
        QDateTime data_max;
        double min_y = std::numeric_limits<double>::max();
        double max_y = 0.0;
        QHash<QString, int> contagem_por_op;
        QHash<QString, ScatterSampler> samplers;

        const QString conn_name = QStringLiteral("file_size_scatter_conn_%1_%2")
                                      .arg(reinterpret_cast<quintptr>(this))
                                      .arg(QDateTime::currentMSecsSinceEpoch());
        bool ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para dispersão de tamanhos: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                const QString sql = QStringLiteral(
                    "SELECT tipo_operacao, tipo, timestamp, nome, size_b, size_kb, size_mb, size_gb, size_tb "
                    "FROM monitoramento WHERE timestamp IS NOT NULL");

                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(sql)) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar tamanhos para percentil: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const double bytes = bytes_from_query(query, 4, 5, 6, 7, 8);
                        if (bytes <= 0.0) {
                            ++invalid_size;
                            continue;
                        }
                        const QDateTime timestamp = parse_scatter_timestamp(query.value(2));
                        if (!timestamp.isValid()) {
                            ++invalid_timestamp;
                            continue;
                        }
                        valores_mb.append(bytes / bytesPorMb);
                        ++valid_source_rows;
                    }
                }

                if (ok && valores_mb.isEmpty()) {
                    db.close();
                }
            }
        }
        QSqlDatabase::removeDatabase(conn_name);
        if (!ok || valores_mb.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("no_size_data", QStringLiteral("Sem dados de tamanho disponíveis")));
        }

        const double limite_percentil = percentile(valores_mb, 0.99);
        outliers_removidos = valores_mb.size();

        const QString conn_name_stats = QStringLiteral("file_size_scatter_stats_%1_%2")
                                            .arg(reinterpret_cast<quintptr>(this))
                                            .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_stats);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para estatísticas de dispersão: %1").arg(db.lastError().text()),
                    true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                const QString sql = QStringLiteral(
                    "SELECT tipo_operacao, tipo, timestamp, nome, size_b, size_kb, size_mb, size_gb, size_tb "
                    "FROM monitoramento WHERE timestamp IS NOT NULL");
                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(sql)) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar estatísticas de dispersão: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const double bytes = bytes_from_query(query, 4, 5, 6, 7, 8);
                        if (bytes <= 0.0) continue;
                        const double tamanho_mb = bytes / bytesPorMb;
                        if (tamanho_mb > limite_percentil) continue;
                        const QDateTime timestamp = parse_scatter_timestamp(query.value(2));
                        if (!timestamp.isValid()) continue;

                        const QString operacao =
                            translated_value(mapa_ops, query.value(0).toString(), unknown);
                        if (!data_min.isValid() || timestamp < data_min) data_min = timestamp;
                        if (!data_max.isValid() || timestamp > data_max) data_max = timestamp;
                        min_y = std::min(min_y, tamanho_mb);
                        max_y = std::max(max_y, tamanho_mb);
                        contagem_por_op[operacao] += 1;
                        ++filtered_rows;
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_stats);
        if (!ok || filtered_rows <= 0 || !data_min.isValid() || !data_max.isValid()) {
            return _criar_grafico_sem_dados(titulo);
        }
        outliers_removidos -= static_cast<int>(filtered_rows);

        const double total_segundos = data_min.secsTo(data_max);
        const double total_minutos = total_segundos / 60.0;
        const double total_horas = total_minutos / 60.0;
        const double total_dias = total_horas / 24.0;

        QString escala = QStringLiteral("seconds");
        QString xlabel = obter_texto("time_seconds", QStringLiteral("Tempo (segundos)"));
        bool x_numerico = true;
        QString x_tick_suffix = QStringLiteral("s");

        if (filtered_rows > 1) {
            if (total_segundos < 60.0) {
                if (total_segundos < 1.0) {
                    escala = QStringLiteral("milliseconds");
                    xlabel = obter_texto("time_milliseconds", QStringLiteral("Tempo (milissegundos)"));
                    x_tick_suffix = QStringLiteral("ms");
                } else {
                    escala = QStringLiteral("centiseconds");
                    xlabel = obter_texto("time_centiseconds",
                                         QStringLiteral("Tempo (centésimos de segundo)"));
                    x_tick_suffix = QStringLiteral("cs");
                }
            } else if (total_minutos < 60.0) {
                escala = QStringLiteral("seconds");
                xlabel = obter_texto("time_seconds", QStringLiteral("Tempo (segundos)"));
                x_tick_suffix = QStringLiteral("s");
            } else if (total_horas < 24.0) {
                escala = QStringLiteral("minutes");
                xlabel = obter_texto("time_minutes", QStringLiteral("Tempo (minutos)"));
                x_tick_suffix = QStringLiteral("min");
            } else if (total_dias < 7.0) {
                escala = QStringLiteral("hours");
                xlabel = obter_texto("time_hours", QStringLiteral("Tempo (horas)"));
                x_tick_suffix = QStringLiteral("h");
            } else if (total_dias < 30.0) {
                escala = QStringLiteral("days");
                xlabel = obter_texto("time_days", QStringLiteral("Tempo (dias)"));
                x_tick_suffix = QStringLiteral("d");
            }
        }

        const bool y_log = (max_y / (min_y + 0.001)) > 100.0;

        QList<QString> ordem_preferida = {
            obter_texto("op_scanned", QStringLiteral("Escaneado")),
            obter_texto("op_moved", QStringLiteral("Movido")),
            obter_texto("op_added", QStringLiteral("Adicionado")),
            obter_texto("op_modified", QStringLiteral("Modificado")),
            obter_texto("op_deleted", QStringLiteral("Excluído")),
            obter_texto("op_renamed", QStringLiteral("Renomeado"))};

        QVariantList operation_order;
        for (const QString& op : ordem_preferida) {
            if (contagem_por_op.contains(op) && !operation_order.contains(op)) operation_order.append(op);
        }
        QList<QPair<QString, int>> restantes;
        for (auto it = contagem_por_op.constBegin(); it != contagem_por_op.constEnd(); ++it) {
            if (!operation_order.contains(it.key())) restantes.append({it.key(), it.value()});
        }
        std::sort(restantes.begin(), restantes.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) { return a.second > b.second; });
        for (const auto& it : restantes) operation_order.append(it.first);

        const int per_operation_limit =
            qBound(1200, kMaxFileSizeScatterPoints / qMax(1, operation_order.size()), kMaxFileSizeScatterPoints);
        for (const QVariant& opVar : operation_order) {
            ScatterSampler sampler;
            sampler.limit = per_operation_limit;
            sampler.points.reserve(qMin(per_operation_limit, contagem_por_op.value(opVar.toString(), 0)));
            samplers.insert(opVar.toString(), sampler);
        }

        const QString conn_name_points = QStringLiteral("file_size_scatter_points_%1_%2")
                                             .arg(reinterpret_cast<quintptr>(this))
                                             .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_points);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para pontos de dispersão: %1").arg(db.lastError().text()),
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
                        "SELECT tipo_operacao, tipo, timestamp, nome, size_b, size_kb, size_mb, size_gb, size_tb "
                        "FROM monitoramento WHERE timestamp IS NOT NULL"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar pontos de dispersão: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const double bytes = bytes_from_query(query, 4, 5, 6, 7, 8);
                        if (bytes <= 0.0) continue;
                        const double tamanho_mb = bytes / bytesPorMb;
                        if (tamanho_mb > limite_percentil) continue;
                        const QDateTime timestamp = parse_scatter_timestamp(query.value(2));
                        if (!timestamp.isValid()) continue;

                        ScatterSamplePoint point;
                        point.timestampMs = timestamp.toMSecsSinceEpoch();
                        point.sizeMb = tamanho_mb;
                        point.operacaoRaw = query.value(0).toString().trimmed();
                        point.tipoRaw = query.value(1).toString().trimmed();
                        point.operacao = translated_value(mapa_ops, point.operacaoRaw, unknown);
                        point.tipo = translated_value(mapa_tipos, point.tipoRaw, QString());
                        point.arquivo = query.value(3).toString().trimmed();
                        if (point.arquivo.isEmpty()) point.arquivo = QStringLiteral("-");
                        point.timestampText = timestamp.toString(Qt::ISODate);
                        samplers[point.operacao].consider(point);
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_points);
        if (!ok) {
            return _criar_grafico_sem_dados(titulo);
        }

        QVariantList pontos;
        pontos.reserve(qMin<qint64>(filtered_rows, kMaxFileSizeScatterPoints));
        const qint64 data_min_ms = data_min.toMSecsSinceEpoch();
        const double min_x = 0.0;
        double max_x = scaled_time_value(data_min.msecsTo(data_max), escala);
        if (max_x <= min_x) max_x = min_x + 1.0;
        for (const QVariant& opVar : operation_order) {
            const QString op = opVar.toString();
            const QVector<ScatterSamplePoint> sampled = samplers.value(op).points;
            for (const ScatterSamplePoint& point : sampled) {
                QVariantMap p;
                p.insert("x", scaled_time_value(point.timestampMs - data_min_ms, escala));
                p.insert("timestamp", point.timestampText);
                p.insert("y_mb", point.sizeMb);
                p.insert("tipo_operacao", point.operacao);
                p.insert("tipo_operacao_raw", point.operacaoRaw);
                p.insert("tipo", point.tipo);
                p.insert("tipo_raw", point.tipoRaw);
                p.insert("arquivo", point.arquivo);
                p.insert("color", cores_operacoes_.value(point.operacao, QStringLiteral("#333333")));
                pontos.append(p);
            }
        }

        QVariantMap op_colors;
        for (const QVariant& ov : operation_order) {
            const QString op = ov.toString();
            op_colors.insert(op, cores_operacoes_.value(op, QStringLiteral("#333333")));
        }

        QVariantMap out;
        out.insert("kind", "scatter");
        out.insert("title", titulo);
        out.insert("points", pontos);
        out.insert("x_label", xlabel);
        out.insert("y_label", obter_texto("file_size_mb", QStringLiteral("Tamanho do Arquivo (MB)")));
        out.insert("time_scale", escala);
        out.insert("x_tick_suffix", x_tick_suffix);
        out.insert("x_numeric", x_numerico);
        out.insert("x_min", min_x);
        out.insert("x_max", max_x);
        out.insert("y_min", min_y);
        out.insert("y_max", max_y);
        out.insert("operation_order", operation_order);
        out.insert("op_colors", op_colors);
        out.insert("y_log_scale", y_log);
        out.insert("outliers_removed", outliers_removidos);
        out.insert("invalid_size_rows", invalid_size);
        out.insert("invalid_timestamp_rows", invalid_timestamp);
        out.insert(QStringLiteral("grid_overlay"), true);
        out.insert(QStringLiteral("fast_point_cloud"), true);
        out.insert(QStringLiteral("source_points"), valid_source_rows);
        out.insert(QStringLiteral("display_points"), pontos.size());
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar scatter", true);
        return _criar_grafico_sem_dados(
            obter_texto("file_size_analysis", QStringLiteral("Análise de Tamanho de Arquivo")));
    }
}

}  // namespace source::data
