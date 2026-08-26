#include "gst_15_GraficoDotplot.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVector>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
constexpr int kMaxDotplotPointsTotal = 18000;
constexpr int kMaxDotplotPointsPerType = 3000;

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

struct DotplotSamplePoint {
    QString tipo;
    QString tipoRaw;
    double sizeMb = 0.0;
    int index = 0;
};

struct DotplotSampler {
    int limit = kMaxDotplotPointsPerType;
    int stride = 1;
    int seen = 0;
    QVector<DotplotSamplePoint> points;

    void consider(const DotplotSamplePoint& point) {
        if ((seen % stride) == 0) {
            points.append(point);
            if (points.size() > limit) {
                QVector<DotplotSamplePoint> reduced;
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

QVariant GraficoDotplot::gerar() {
    try {
        const QString titulo = obter_texto("file_size_distribution", QStringLiteral("Distribuição de Tamanhos de Arquivo"));
        constexpr double bytesPorMb = 1024.0 * 1024.0;
        const QHash<QString, QString> mapa_tipos = _obter_mapeamento_tipos();
        QHash<QString, int> contagem_tipos;
        QHash<QString, QString> raw_por_tipo;

        const QString conn_name_counts = QStringLiteral("dotplot_counts_%1_%2")
                                             .arg(reinterpret_cast<quintptr>(this))
                                             .arg(QDateTime::currentMSecsSinceEpoch());
        bool ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_counts);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para dotplot: %1").arg(db.lastError().text()), true);
                ok = false;
            } else {
                QSqlQuery pragma(db);
                pragma.exec(QStringLiteral("PRAGMA query_only=ON"));
                pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
                pragma.exec(QStringLiteral("PRAGMA cache_size=-65536"));

                QSqlQuery query(db);
                query.setForwardOnly(true);
                if (!query.exec(QStringLiteral(
                        "SELECT tipo, size_b, size_kb, size_mb, size_gb, size_tb "
                        "FROM monitoramento"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar tipos para dotplot: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QString rawTipo = query.value(0).toString().trimmed();
                        if (rawTipo.isEmpty()) continue;
                        const double bytes = bytes_from_query(query, 1, 2, 3, 4, 5);
                        if (bytes <= 0.0) continue;
                        const QString tipo = mapa_tipos.value(rawTipo, rawTipo);
                        if (!raw_por_tipo.contains(tipo)) {
                            raw_por_tipo.insert(tipo, rawTipo);
                        }
                        contagem_tipos[tipo] += 1;
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_counts);
        if (!ok || contagem_tipos.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("no_size_data", QStringLiteral("Sem dados de tamanho disponíveis")));
        }

        QList<QPair<QString, int>> tipos_ordenados;
        tipos_ordenados.reserve(contagem_tipos.size());
        for (auto it = contagem_tipos.constBegin(); it != contagem_tipos.constEnd(); ++it) {
            tipos_ordenados.append({it.key(), it.value()});
        }
        std::sort(tipos_ordenados.begin(), tipos_ordenados.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                      return a.second > b.second;
                  });
        if (tipos_ordenados.size() > 8) tipos_ordenados = tipos_ordenados.mid(0, 8);

        QSet<QString> top_tipos;
        QVariantList tipos_labels;
        QVariantList tipos_raw_labels;
        for (const auto& p : tipos_ordenados) {
            top_tipos.insert(p.first);
            tipos_labels.append(p.first);
            tipos_raw_labels.append(raw_por_tipo.value(p.first, p.first));
        }

        QVector<double> valores;
        const QString conn_name_values = QStringLiteral("dotplot_values_%1_%2")
                                             .arg(reinterpret_cast<quintptr>(this))
                                             .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_values);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para percentil do dotplot: %1").arg(db.lastError().text()),
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
                        "SELECT tipo, size_b, size_kb, size_mb, size_gb, size_tb "
                        "FROM monitoramento"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar valores para dotplot: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QString rawTipo = query.value(0).toString().trimmed();
                        if (rawTipo.isEmpty()) continue;
                        const QString tipo = mapa_tipos.value(rawTipo, rawTipo);
                        if (!top_tipos.contains(tipo)) continue;
                        const double bytes = bytes_from_query(query, 1, 2, 3, 4, 5);
                        if (bytes <= 0.0) continue;
                        valores.append(bytes / bytesPorMb);
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_values);
        if (!ok || valores.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("insufficient_data_after_filtering",
                            QStringLiteral("Sem dados suficientes após filtragem")));
        }

        const double limite_superior = percentile(valores, 0.95);

        QVariantMap medias_por_tipo;
        QVariantMap medias_por_tipo_raw;
        QVariantMap contagem_por_tipo;
        QVariantMap contagem_por_tipo_raw;
        QVariantMap cores_por_tipo;
        QVariantMap cores_por_tipo_raw;
        QHash<QString, double> soma_tipo;
        QHash<QString, int> qtd_tipo;
        QHash<QString, DotplotSampler> samplers;
        const int per_type_limit =
            qBound(900, kMaxDotplotPointsTotal / qMax(1, tipos_labels.size()), kMaxDotplotPointsPerType);
        for (const QVariant& t : tipos_labels) {
            DotplotSampler sampler;
            sampler.limit = per_type_limit;
            sampler.points.reserve(qMin(per_type_limit, contagem_tipos.value(t.toString(), 0)));
            samplers.insert(t.toString(), sampler);
        }

        double min_y = std::numeric_limits<double>::max();
        double max_y = 0.0;
        const QString conn_name_points = QStringLiteral("dotplot_points_%1_%2")
                                             .arg(reinterpret_cast<quintptr>(this))
                                             .arg(QDateTime::currentMSecsSinceEpoch());
        ok = true;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn_name_points);
            db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY;QSQLITE_BUSY_TIMEOUT=5000"));
            db.setDatabaseName(db_path_);
            if (!db.open()) {
                source::utils::LogManager::get_logger().error(
                    QStringLiteral("Erro SQL ao abrir base para pontos do dotplot: %1").arg(db.lastError().text()),
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
                        "SELECT tipo, size_b, size_kb, size_mb, size_gb, size_tb "
                        "FROM monitoramento"))) {
                    source::utils::LogManager::get_logger().error(
                        QStringLiteral("Erro SQL ao consultar pontos para dotplot: %1").arg(query.lastError().text()),
                        true);
                    ok = false;
                } else {
                    while (query.next()) {
                        const QString rawTipo = query.value(0).toString().trimmed();
                        if (rawTipo.isEmpty()) continue;
                        const QString tipo = mapa_tipos.value(rawTipo, rawTipo);
                        if (!top_tipos.contains(tipo)) continue;
                        const double bytes = bytes_from_query(query, 1, 2, 3, 4, 5);
                        if (bytes <= 0.0) continue;
                        const double tamanho_mb = bytes / bytesPorMb;
                        if (tamanho_mb > limite_superior) continue;

                        const int index = qtd_tipo.value(tipo, 0);
                        soma_tipo[tipo] += tamanho_mb;
                        qtd_tipo[tipo] = index + 1;
                        min_y = std::min(min_y, tamanho_mb);
                        max_y = std::max(max_y, tamanho_mb);

                        samplers[tipo].consider(DotplotSamplePoint{tipo, rawTipo, tamanho_mb, index});
                    }
                }
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(conn_name_points);
        if (!ok || qtd_tipo.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("insufficient_data_after_filtering",
                            QStringLiteral("Sem dados suficientes após filtragem")));
        }

        const QList<QString> paleta = {
            QStringLiteral("#1f77b4"), QStringLiteral("#ff7f0e"), QStringLiteral("#2ca02c"),
            QStringLiteral("#9467bd"), QStringLiteral("#8c564b"), QStringLiteral("#7f7f7f"),
            QStringLiteral("#bcbd22"), QStringLiteral("#17becf")};

        for (const QVariant& t : tipos_labels) {
            const QString tipo = t.toString();
            const int qtd = qtd_tipo.value(tipo, 0);
            const double media = qtd > 0 ? soma_tipo.value(tipo, 0.0) / qtd : 0.0;
            const QString rawTipo = raw_por_tipo.value(tipo, tipo);
            medias_por_tipo.insert(tipo, media);
            medias_por_tipo_raw.insert(rawTipo, media);
            contagem_por_tipo.insert(tipo, qtd);
            contagem_por_tipo_raw.insert(rawTipo, qtd);
            const int cor_idx = tipos_labels.indexOf(t) % paleta.size();
            cores_por_tipo.insert(tipo, paleta.at(cor_idx));
            cores_por_tipo_raw.insert(rawTipo, paleta.at(cor_idx));
        }

        QVariantList pontos;
        pontos.reserve(qMin(valores.size(), kMaxDotplotPointsTotal));
        for (const QVariant& t : tipos_labels) {
            const QVector<DotplotSamplePoint> sampled = samplers.value(t.toString()).points;
            for (const DotplotSamplePoint& point : sampled) {
                QVariantMap p;
                p.insert("tipo", point.tipo);
                p.insert("tipo_raw", point.tipoRaw);
                p.insert("tamanho_mb", point.sizeMb);
                p.insert("index", point.index);
                pontos.append(p);
            }
        }

        const bool y_log = (max_y / (min_y + 0.001)) > 100.0;

        QVariantMap out;
        out.insert("kind", "dotplot");
        out.insert("title", titulo);
        out.insert("points", pontos);
        out.insert("types", tipos_labels);
        out.insert("types_raw", tipos_raw_labels);
        out.insert("means_by_type", medias_por_tipo);
        out.insert("means_by_type_raw", medias_por_tipo_raw);
        out.insert("counts_by_type", contagem_por_tipo);
        out.insert("counts_by_type_raw", contagem_por_tipo_raw);
        out.insert("type_colors", cores_por_tipo);
        out.insert("type_colors_raw", cores_por_tipo_raw);
        out.insert("x_label", obter_texto("file_types", QStringLiteral("Tipos de Arquivo")));
        out.insert("y_label", obter_texto("file_size_mb", QStringLiteral("Tamanho do Arquivo (MB)")));
        out.insert("y_min", min_y);
        out.insert("y_max", max_y);
        out.insert("y_log_scale", y_log);
        out.insert("outlier_limit_p95", limite_superior);
        out.insert(QStringLiteral("fast_point_cloud"), true);
        out.insert(QStringLiteral("source_points"), valores.size());
        out.insert(QStringLiteral("display_points"), pontos.size());
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar dotplot", true);
        return _criar_grafico_sem_dados(
            obter_texto("file_size_distribution", QStringLiteral("Distribuição de Tamanhos de Arquivo")));
    }
}

}  // namespace source::data
