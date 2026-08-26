#include "gst_12_GraficoScatter.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QVector>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
double percentile(QVector<double> values, const double q) {
    if (values.isEmpty()) return 0.0;
    std::sort(values.begin(), values.end());
    if (values.size() == 1) return values.first();

    const double idx = (values.size() - 1) * q;
    const int lo = static_cast<int>(std::floor(idx));
    const int hi = static_cast<int>(std::ceil(idx));
    const double frac = idx - lo;
    if (lo == hi) return values.at(lo);
    return values.at(lo) + (values.at(hi) - values.at(lo)) * frac;
}
}  // namespace

QVariant GraficoScatter::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("file_size_analysis", QStringLiteral("Análise de Tamanho de Arquivo"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Registro {
            QVariantMap row;
            QDateTime timestamp;
            double tamanho_mb = 0.0;
        };

        QVector<Registro> registros;
        registros.reserve(dados.size());
        int invalid_size = 0;
        int invalid_timestamp = 0;

        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const double bytes = extrair_tamanho_bytes(row);
            if (bytes <= 0) {
                invalid_size += 1;
                continue;
            }

            const QDateTime timestamp = extrair_timestamp(row);
            if (!timestamp.isValid()) {
                invalid_timestamp += 1;
                continue;
            }

            Registro r;
            r.row = row;
            r.timestamp = timestamp;
            r.tamanho_mb = bytes / (1024.0 * 1024.0);
            registros.append(r);
        }

        if (registros.isEmpty()) {
            return _criar_grafico_sem_dados(obter_texto("no_size_data", QStringLiteral("Sem dados de tamanho disponíveis")));
        }

        QVector<double> valores_mb;
        valores_mb.reserve(registros.size());
        for (const Registro& r : registros) valores_mb.append(r.tamanho_mb);

        const double limite_superior = percentile(valores_mb, 0.99);
        QVector<Registro> filtrados;
        filtrados.reserve(registros.size());
        for (const Registro& r : registros) {
            if (r.tamanho_mb <= limite_superior) filtrados.append(r);
        }

        const int outliers_removidos = registros.size() - filtrados.size();
        if (filtrados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QDateTime data_min = filtrados.first().timestamp;
        QDateTime data_max = data_min;
        for (const Registro& r : filtrados) {
            if (r.timestamp < data_min) data_min = r.timestamp;
            if (r.timestamp > data_max) data_max = r.timestamp;
        }

        const double total_segundos = data_min.secsTo(data_max);
        const double total_minutos = total_segundos / 60.0;
        const double total_horas = total_minutos / 60.0;
        const double total_dias = total_horas / 24.0;

        QString escala = QStringLiteral("seconds");
        QString xlabel = obter_texto("time_seconds", QStringLiteral("Tempo (segundos)"));
        bool x_numerico = true;
        QString x_tick_suffix = QStringLiteral("s");

        if (filtrados.size() > 1) {
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

        QVariantList pontos;
        pontos.reserve(filtrados.size());
        double min_y = std::numeric_limits<double>::max();
        double max_y = 0.0;
        double min_x = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        QHash<QString, int> contagem_por_op;

        for (const Registro& r : filtrados) {
            QVariantMap p;
            const qint64 offset_ms = data_min.msecsTo(r.timestamp);
            double x = offset_ms / 1000.0;  // default seconds
            if (escala == QStringLiteral("milliseconds")) {
                x = static_cast<double>(offset_ms);
            } else if (escala == QStringLiteral("centiseconds")) {
                x = (offset_ms / 1000.0) * 100.0;
            } else if (escala == QStringLiteral("seconds")) {
                x = offset_ms / 1000.0;
            } else if (escala == QStringLiteral("minutes")) {
                x = (offset_ms / 1000.0) / 60.0;
            } else if (escala == QStringLiteral("hours")) {
                x = (offset_ms / 1000.0) / 3600.0;
            } else if (escala == QStringLiteral("days")) {
                x = (offset_ms / 1000.0) / 86400.0;
            }
            p.insert("x", x);

            p.insert("timestamp", r.timestamp.toString(Qt::ISODate));
            p.insert("y_mb", r.tamanho_mb);
            QString operacao = r.row.value("tipo_operacao").toString().trimmed();
            if (operacao.isEmpty()) operacao = obter_texto("unknown", QStringLiteral("Desconhecido"));
            const QString tipo = r.row.value("tipo").toString().trimmed();
            QString arquivo = r.row.value("nome").toString().trimmed();
            if (arquivo.isEmpty()) arquivo = r.row.value("nome_anterior").toString().trimmed();
            p.insert("tipo_operacao", operacao);
            p.insert("tipo", tipo);
            p.insert("arquivo", arquivo);
            p.insert("color", cores_operacoes_.value(operacao, QStringLiteral("#333333")));
            pontos.append(p);

            min_y = std::min(min_y, r.tamanho_mb);
            max_y = std::max(max_y, r.tamanho_mb);
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            contagem_por_op[operacao] += 1;
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
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar scatter", true);
        return _criar_grafico_sem_dados(
            obter_texto("file_size_analysis", QStringLiteral("Análise de Tamanho de Arquivo")));
    }
}

}  // namespace source::data
