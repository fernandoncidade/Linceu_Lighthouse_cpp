#include "gst_09_GraficoBoxplot.h"

#include <algorithm>
#include <limits>
#include <QSet>

#include <QHash>
#include <QList>
#include <QMap>
#include <QPair>

#include "source/utils/LogManager.h"

namespace source::data {

namespace {
QString categorizar_tamanho(double size_bytes) {
    if (size_bytes < 1024.0) return QStringLiteral("Muito Pequeno");
    if (size_bytes < 1024.0 * 1024.0) return QStringLiteral("Pequeno");
    if (size_bytes < 100.0 * 1024.0 * 1024.0) return QStringLiteral("Médio");
    if (size_bytes < 1024.0 * 1024.0 * 1024.0) return QStringLiteral("Grande");
    return QStringLiteral("Muito Grande");
}

struct SizeCandidate {
    QString nome;
    double bytes = -1.0;
    qint64 timestampMs = std::numeric_limits<qint64>::min();
};

double choose_candidate_bytes(const QVector<int>& indexes,
                              const QVector<SizeCandidate>& candidates,
                              const QString& nome,
                              qint64 targetTimestampMs) {
    if (indexes.isEmpty()) {
        return -1.0;
    }

    const QString trimmedName = nome.trimmed();
    bool found = false;
    double bestBytes = -1.0;
    qint64 bestDistance = std::numeric_limits<qint64>::max();

    const auto consider = [&](int candidateIndex, bool requireSameName) {
        const SizeCandidate& candidate = candidates.at(candidateIndex);
        if (candidate.bytes <= 0.0) {
            return;
        }

        const bool sameName = trimmedName.isEmpty() || candidate.nome == trimmedName;
        if (requireSameName && !sameName) {
            return;
        }

        qint64 distance = 0;
        if (targetTimestampMs != std::numeric_limits<qint64>::min() &&
            candidate.timestampMs != std::numeric_limits<qint64>::min()) {
            distance = std::llabs(candidate.timestampMs - targetTimestampMs);
        }

        if (!found || distance < bestDistance) {
            found = true;
            bestDistance = distance;
            bestBytes = candidate.bytes;
        }
    };

    for (int candidateIndex : indexes) {
        consider(candidateIndex, true);
    }
    if (found) {
        return bestBytes;
    }

    for (int candidateIndex : indexes) {
        consider(candidateIndex, false);
    }
    return bestBytes;
}
}  // namespace

QVariant GraficoBoxplot::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("size_distribution", QStringLiteral("Distribuição de Tamanhos"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVector<SizeCandidate> sizeCandidates;
        sizeCandidates.reserve(dados.size() * 2);
        QHash<QString, QVector<int>> sizeCandidatesByPath;
        QHash<QString, QVector<int>> sizeCandidatesByName;

        for (const QVariant& value : dados) {
            const QVariantMap row = value.toMap();
            const double bytes = extrair_tamanho_bytes(row);
            if (bytes <= 0.0) {
                continue;
            }

            const QString nome = row.value("nome").toString().trimmed();
            const QDateTime timestamp = extrair_timestamp(row);
            const int candidateIndex = sizeCandidates.size();
            sizeCandidates.append(SizeCandidate{
                nome,
                bytes,
                timestamp.isValid() ? timestamp.toMSecsSinceEpoch() : std::numeric_limits<qint64>::min()});

            const QString dirAtual = normalizar_caminho(row.value("dir_atual").toString(), QString());
            const QString dirAnterior = normalizar_caminho(row.value("dir_anterior").toString(), QString());
            if (!dirAtual.isEmpty()) {
                sizeCandidatesByPath[dirAtual].append(candidateIndex);
            }
            if (!dirAnterior.isEmpty() && dirAnterior != dirAtual) {
                sizeCandidatesByPath[dirAnterior].append(candidateIndex);
            }
            if (!nome.isEmpty()) {
                sizeCandidatesByName[nome].append(candidateIndex);
            }
        }

        QHash<QString, QVariantList> por_operacao;
        QHash<QString, QVariantList> por_tipo;
        QHash<QString, QVariantList> por_categoria;
        QHash<int, QVariantList> por_hora;
        QHash<QString, int> contagem_tipos_tamanho_valido;
        QHash<int, QMap<QDate, int>> eventos_por_hora_por_dia;

        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QDateTime dt = extrair_timestamp(row);
            if (dt.isValid()) {
                const int hora = dt.time().hour();
                eventos_por_hora_por_dia[hora][dt.date()] += 1;
            }

            double bytes = extrair_tamanho_bytes(row);
            if (bytes <= 0.0) {
                const QString nome = row.value("nome").toString().trimmed();
                const qint64 timestampMs = dt.isValid() ? dt.toMSecsSinceEpoch() : std::numeric_limits<qint64>::min();
                const QString dirAtual = normalizar_caminho(row.value("dir_atual").toString(), QString());
                const QString dirAnterior = normalizar_caminho(row.value("dir_anterior").toString(), QString());

                if (!dirAtual.isEmpty()) {
                    bytes = choose_candidate_bytes(sizeCandidatesByPath.value(dirAtual), sizeCandidates, nome,
                                                   timestampMs);
                }
                if (bytes <= 0.0 && !dirAnterior.isEmpty()) {
                    bytes = choose_candidate_bytes(sizeCandidatesByPath.value(dirAnterior), sizeCandidates, nome,
                                                   timestampMs);
                }
                if (bytes <= 0.0 && !nome.isEmpty()) {
                    bytes = choose_candidate_bytes(sizeCandidatesByName.value(nome), sizeCandidates, nome,
                                                   timestampMs);
                }
            }
            if (bytes <= 0) continue;
            const double mb = bytes / (1024.0 * 1024.0);

            const QString op = row.value("tipo_operacao").toString().trimmed();
            const QString tipo = row.value("tipo").toString().trimmed();
            if (dt.isValid()) {
                const int hora = dt.time().hour();
                QVariantList hora_list = por_hora.value(hora);
                hora_list.append(mb);
                por_hora.insert(hora, hora_list);
            }

            if (!op.isEmpty()) {
                QVariantList op_list = por_operacao.value(op);
                op_list.append(mb);
                por_operacao.insert(op, op_list);
            }

            if (!tipo.isEmpty()) {
                QVariantList tipo_list = por_tipo.value(tipo);
                tipo_list.append(mb);
                por_tipo.insert(tipo, tipo_list);
                contagem_tipos_tamanho_valido[tipo] += 1;
            }

            const QString categoria = categorizar_tamanho(bytes);
            QVariantList lista_cat = por_categoria.value(categoria);
            lista_cat.append(mb);
            por_categoria.insert(categoria, lista_cat);
        }

        QList<QPair<QString, int>> tipos_ordenados;
        tipos_ordenados.reserve(contagem_tipos_tamanho_valido.size());
        for (auto it = contagem_tipos_tamanho_valido.constBegin();
             it != contagem_tipos_tamanho_valido.constEnd(); ++it) {
            tipos_ordenados.append({it.key(), it.value()});
        }
        std::sort(tipos_ordenados.begin(), tipos_ordenados.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                      return a.second > b.second;
                  });
        if (tipos_ordenados.size() > 30) tipos_ordenados = tipos_ordenados.mid(0, 30);

        QVariantMap by_type_top30;
        QVariantList top_types;
        for (const auto& par : tipos_ordenados) {
            top_types.append(par.first);
            by_type_top30.insert(par.first, por_tipo.value(par.first));
        }

        QVariantMap by_hour_of_day;
        QVariantList hour_order;
        const QString hour_format = obter_texto("hour_format", QStringLiteral("%1h"));
        for (int hora = 0; hora < 24; ++hora) {
            const QString hour_label = hour_format.arg(QString::number(hora));
            hour_order.append(hour_label);
            const QVariantList hour_values = por_hora.value(hora);
            if (!hour_values.isEmpty()) {
                by_hour_of_day.insert(hour_label, hour_values);
            }
        }

        QVariantList by_hour_daily_counts;
        by_hour_daily_counts.reserve(24);
        for (int hora = 0; hora < 24; ++hora) {
            QVariantMap entry;
            entry.insert("hour", hora);

            QVariantList counts;
            const QMap<QDate, int> mapa_dia = eventos_por_hora_por_dia.value(hora);
            for (auto it = mapa_dia.constBegin(); it != mapa_dia.constEnd(); ++it) {
                counts.append(it.value());
            }
            entry.insert("counts", counts);
            by_hour_daily_counts.append(entry);
        }

        QVariantMap by_operation;
        for (auto it = por_operacao.constBegin(); it != por_operacao.constEnd(); ++it) {
            by_operation.insert(it.key(), it.value());
        }

        const QString op_added = obter_texto("op_added", QStringLiteral("Adicionado"));
        const QString op_scanned = obter_texto("op_scanned", QStringLiteral("Escaneado"));
        const QString op_deleted = obter_texto("op_deleted", QStringLiteral("Excluído"));
        const QString op_modified = obter_texto("op_modified", QStringLiteral("Modificado"));
        const QString op_moved = obter_texto("op_moved", QStringLiteral("Movido"));
        const QString op_renamed = obter_texto("op_renamed", QStringLiteral("Renomeado"));
        const QStringList ordem_preferida_ops = {op_added, op_scanned, op_deleted, op_modified, op_moved, op_renamed};

        QSet<QString> ops_presentes;
        for (auto it = by_operation.constBegin(); it != by_operation.constEnd(); ++it) {
            if (!it.key().trimmed().isEmpty()) ops_presentes.insert(it.key().trimmed());
        }

        QStringList operation_order_list;
        operation_order_list.reserve(ops_presentes.size());
        for (const QString& op : ordem_preferida_ops) {
            if (ops_presentes.contains(op)) operation_order_list.append(op);
        }
        QStringList restantes_ops;
        for (const QString& op : ops_presentes) {
            if (!operation_order_list.contains(op)) restantes_ops.append(op);
        }
        std::sort(restantes_ops.begin(), restantes_ops.end(),
                  [](const QString& a, const QString& b) { return a.localeAwareCompare(b) < 0; });
        for (const QString& op : restantes_ops) operation_order_list.append(op);

        QVariantList operation_order;
        QVariantMap op_colors;
        const QList<QString> fallback_ops = {
            QStringLiteral("#0000ff"), QStringLiteral("#808080"), QStringLiteral("#ff0000"),
            QStringLiteral("#ff6200"), QStringLiteral("#ff00ff"), QStringLiteral("#00ff00")};
        for (int i = 0; i < operation_order_list.size(); ++i) {
            const QString& op = operation_order_list.at(i);
            operation_order.append(op);
            op_colors.insert(op, cores_operacoes_.value(op, fallback_ops.at(i % fallback_ops.size())));
        }

        const QString cat_muito_pequeno = obter_texto("very_small", QStringLiteral("Muito Pequeno"));
        const QString cat_pequeno = obter_texto("small", QStringLiteral("Pequeno"));
        const QString cat_medio = obter_texto("medium", QStringLiteral("Médio"));
        const QString cat_grande = obter_texto("large", QStringLiteral("Grande"));
        const QString cat_muito_grande = obter_texto("very_large", QStringLiteral("Muito Grande"));

        const QList<QPair<QString, QString>> cat_map = {
            {QStringLiteral("Muito Pequeno"), cat_muito_pequeno},
            {QStringLiteral("Pequeno"), cat_pequeno},
            {QStringLiteral("Médio"), cat_medio},
            {QStringLiteral("Grande"), cat_grande},
            {QStringLiteral("Muito Grande"), cat_muito_grande}};

        QVariantMap by_size_category;
        QVariantList category_order;
        for (const auto& it : cat_map) {
            by_size_category.insert(it.second, por_categoria.value(it.first));
            category_order.append(it.second);
        }

        QVariantMap out;
        out.insert("kind", "boxplot");
        out.insert("title", titulo);
        out.insert("by_operation", by_operation);
        out.insert("by_type", by_type_top30);
        out.insert("top_types", top_types);
        out.insert("by_hour_of_day", by_hour_of_day);
        out.insert("hour_order", hour_order);
        out.insert("by_hour_daily_counts", by_hour_daily_counts);
        out.insert("by_size_category", by_size_category);
        out.insert("category_order", category_order);
        out.insert("operation_order", operation_order);
        out.insert("op_colors", op_colors);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar boxplot", true);
        return _criar_grafico_sem_dados(obter_texto("size_distribution", QStringLiteral("Distribuição de Tamanhos")));
    }
}

}  // namespace source::data
