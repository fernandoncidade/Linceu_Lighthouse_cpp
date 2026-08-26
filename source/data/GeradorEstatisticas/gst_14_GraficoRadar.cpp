#include "gst_14_GraficoRadar.h"

#include <algorithm>

#include <QSet>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoRadar::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("operations_by_file_type", QStringLiteral("Operações por Tipo de Arquivo"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantList linhas_validas;
        QHash<QString, int> contagem_tipos;
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString op = row.value("tipo_operacao").toString().trimmed();
            const QString tipo = row.value("tipo").toString().trimmed();
            if (op.isEmpty() || tipo.isEmpty()) continue;
            linhas_validas.append(row);
            contagem_tipos[tipo] += 1;
        }
        if (linhas_validas.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QPair<QString, int>> tipos_ordenados;
        tipos_ordenados.reserve(contagem_tipos.size());
        for (auto it = contagem_tipos.constBegin(); it != contagem_tipos.constEnd(); ++it) {
            tipos_ordenados.append({it.key(), it.value()});
        }
        std::sort(tipos_ordenados.begin(), tipos_ordenados.end(),
                  [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
                      return a.second > b.second;
                  });
        if (tipos_ordenados.size() > 10) tipos_ordenados = tipos_ordenados.mid(0, 10);

        QVariantList top_tipos_labels;
        QSet<QString> top_tipos;
        for (const auto& par : tipos_ordenados) {
            top_tipos.insert(par.first);
            top_tipos_labels.append(par.first);
        }

        QHash<QString, QHash<QString, int>> matriz;
        for (const QVariant& v : linhas_validas) {
            const QVariantMap row = v.toMap();
            const QString tipo = row.value("tipo").toString();
            if (!top_tipos.contains(tipo)) continue;
            const QString op = row.value("tipo_operacao").toString();
            matriz[op][tipo] += 1;
        }

        const QVariantList operacoes = {
            obter_texto("op_added", QStringLiteral("Adicionado")),
            obter_texto("op_deleted", QStringLiteral("Excluído")),
            obter_texto("op_renamed", QStringLiteral("Renomeado")),
            obter_texto("op_modified", QStringLiteral("Modificado")),
            obter_texto("op_moved", QStringLiteral("Movido")),
            obter_texto("op_scanned", QStringLiteral("Escaneado"))};

        QVariantList series;
        for (const QVariant& op_var : operacoes) {
            const QString op = op_var.toString();
            const QHash<QString, int> counts = matriz.value(op);
            QVariantList values_count;
            int total = 0;
            for (const QVariant& tipo_var : top_tipos_labels) {
                const int c = counts.value(tipo_var.toString(), 0);
                values_count.append(c);
                total += c;
            }

            QVariantList values_percent;
            for (const QVariant& c : values_count) {
                const double valor = c.toDouble();
                const double pct = total > 0 ? (valor / total) * 100.0 : 0.0;
                values_percent.append(pct);
            }

            QVariantMap s;
            s.insert("operacao", op);
            s.insert("values_count", values_count);
            s.insert("values_percent", values_percent);
            s.insert("color", cores_operacoes_.value(op, QStringLiteral("#333333")));
            series.append(s);
        }

        QVariantList types_short;
        for (const QVariant& t : top_tipos_labels) {
            const QString tipo = t.toString();
            types_short.append(tipo.size() < 15 ? tipo : tipo.left(12) + QStringLiteral("..."));
        }

        QVariantMap out;
        out.insert("kind", "radar");
        out.insert("title", titulo);
        out.insert("types", top_tipos_labels);
        out.insert("types_short", types_short);
        out.insert("series", series);
        out.insert("smooth_curve", false);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar radar", true);
        return _criar_grafico_sem_dados(QStringLiteral("Operações por Tipo de Arquivo"));
    }
}

}  // namespace source::data
