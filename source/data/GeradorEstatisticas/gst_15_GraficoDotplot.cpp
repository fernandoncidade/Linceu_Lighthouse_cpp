#include "gst_15_GraficoDotplot.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QSet>
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

QVariant GraficoDotplot::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("file_size_distribution", QStringLiteral("Distribuição de Tamanhos de Arquivo"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Registro {
            QString tipo;
            double tamanho_mb = 0.0;
        };

        QVector<Registro> registros;
        QHash<QString, int> contagem_tipos;
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString tipo = row.value("tipo").toString();
            if (tipo.trimmed().isEmpty()) continue;
            const double bytes = extrair_tamanho_bytes(row);
            if (bytes <= 0) continue;

            Registro r;
            r.tipo = tipo;
            r.tamanho_mb = bytes / (1024.0 * 1024.0);
            registros.append(r);
            contagem_tipos[tipo] += 1;
        }
        if (registros.isEmpty()) {
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
        for (const auto& p : tipos_ordenados) {
            top_tipos.insert(p.first);
            tipos_labels.append(p.first);
        }

        QVector<Registro> filtrados_top;
        filtrados_top.reserve(registros.size());
        QVector<double> valores;
        valores.reserve(registros.size());
        for (const Registro& r : registros) {
            if (!top_tipos.contains(r.tipo)) continue;
            filtrados_top.append(r);
            valores.append(r.tamanho_mb);
        }
        if (filtrados_top.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("insufficient_data_after_filtering",
                            QStringLiteral("Sem dados suficientes após filtragem")));
        }

        const double limite_superior = percentile(valores, 0.95);
        QVector<Registro> filtrados;
        filtrados.reserve(filtrados_top.size());
        for (const Registro& r : filtrados_top) {
            if (r.tamanho_mb <= limite_superior) filtrados.append(r);
        }
        if (filtrados.isEmpty()) {
            return _criar_grafico_sem_dados(
                obter_texto("insufficient_data_after_filtering",
                            QStringLiteral("Sem dados suficientes após filtragem")));
        }

        QVariantMap medias_por_tipo;
        QVariantMap contagem_por_tipo;
        QVariantMap cores_por_tipo;
        QHash<QString, double> soma_tipo;
        QHash<QString, int> qtd_tipo;
        for (const Registro& r : filtrados) {
            soma_tipo[r.tipo] += r.tamanho_mb;
            qtd_tipo[r.tipo] += 1;
        }

        const QList<QString> paleta = {
            QStringLiteral("#1f77b4"), QStringLiteral("#ff7f0e"), QStringLiteral("#2ca02c"),
            QStringLiteral("#9467bd"), QStringLiteral("#8c564b"), QStringLiteral("#7f7f7f"),
            QStringLiteral("#bcbd22"), QStringLiteral("#17becf")};

        for (const QVariant& t : tipos_labels) {
            const QString tipo = t.toString();
            const int qtd = qtd_tipo.value(tipo, 0);
            const double media = qtd > 0 ? soma_tipo.value(tipo, 0.0) / qtd : 0.0;
            medias_por_tipo.insert(tipo, media);
            contagem_por_tipo.insert(tipo, qtd);
            const int cor_idx = tipos_labels.indexOf(t) % paleta.size();
            cores_por_tipo.insert(tipo, paleta.at(cor_idx));
        }

        double min_y = std::numeric_limits<double>::max();
        double max_y = 0.0;
        QVariantList pontos;
        pontos.reserve(filtrados.size());
        QHash<QString, int> idx_por_tipo;
        for (const Registro& r : filtrados) {
            QVariantMap p;
            p.insert("tipo", r.tipo);
            p.insert("tamanho_mb", r.tamanho_mb);
            p.insert("index", idx_por_tipo.value(r.tipo, 0));
            idx_por_tipo[r.tipo] = idx_por_tipo.value(r.tipo, 0) + 1;
            pontos.append(p);

            min_y = std::min(min_y, r.tamanho_mb);
            max_y = std::max(max_y, r.tamanho_mb);
        }

        const bool y_log = (max_y / (min_y + 0.001)) > 100.0;

        QVariantMap out;
        out.insert("kind", "dotplot");
        out.insert("title", titulo);
        out.insert("points", pontos);
        out.insert("types", tipos_labels);
        out.insert("means_by_type", medias_por_tipo);
        out.insert("counts_by_type", contagem_por_tipo);
        out.insert("type_colors", cores_por_tipo);
        out.insert("x_label", obter_texto("file_types", QStringLiteral("Tipos de Arquivo")));
        out.insert("y_label", obter_texto("file_size_mb", QStringLiteral("Tamanho do Arquivo (MB)")));
        out.insert("y_min", min_y);
        out.insert("y_max", max_y);
        out.insert("y_log_scale", y_log);
        out.insert("outlier_limit_p95", limite_superior);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar dotplot", true);
        return _criar_grafico_sem_dados(
            obter_texto("file_size_distribution", QStringLiteral("Distribuição de Tamanhos de Arquivo")));
    }
}

}  // namespace source::data
