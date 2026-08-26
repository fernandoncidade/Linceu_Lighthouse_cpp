#include "gst_08_GraficoLinha.h"

#include <algorithm>
#include <QMap>
#include <QSet>
#include <numeric>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoLinha::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("operations_by_day", QStringLiteral("Operações por Dia"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QMap<QString, QHash<QString, int>> por_dia;
        QSet<QString> operacoes_encontradas;
        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QDateTime dt = extrair_timestamp(row);
            if (!dt.isValid()) continue;
            const QString dia = dt.date().toString(Qt::ISODate);
            const QString op = row.value("tipo_operacao").toString().trimmed();
            if (op.isEmpty()) continue;
            por_dia[dia][op] += 1;
            operacoes_encontradas.insert(op);
        }

        if (por_dia.isEmpty()) return _criar_grafico_sem_dados(titulo);

        const QString op_added = obter_texto("op_added", QStringLiteral("Adicionado"));
        const QString op_scanned = obter_texto("op_scanned", QStringLiteral("Escaneado"));
        const QString op_deleted = obter_texto("op_deleted", QStringLiteral("Excluído"));
        const QString op_modified = obter_texto("op_modified", QStringLiteral("Modificado"));
        const QString op_moved = obter_texto("op_moved", QStringLiteral("Movido"));
        const QString op_renamed = obter_texto("op_renamed", QStringLiteral("Renomeado"));

        const QStringList ordem_preferida = {op_added, op_scanned, op_deleted, op_modified, op_moved, op_renamed};

        QStringList ordem_operacoes;
        ordem_operacoes.reserve(operacoes_encontradas.size());
        for (const QString& op : ordem_preferida) {
            if (operacoes_encontradas.contains(op)) ordem_operacoes.append(op);
        }
        QStringList restantes;
        for (const QString& op : operacoes_encontradas) {
            if (!ordem_operacoes.contains(op)) restantes.append(op);
        }
        std::sort(restantes.begin(), restantes.end(),
                  [](const QString& a, const QString& b) { return a.localeAwareCompare(b) < 0; });
        for (const QString& op : restantes) ordem_operacoes.append(op);

        QVariantList dias;
        QVariantList series;
        for (auto it = por_dia.constBegin(); it != por_dia.constEnd(); ++it) {
            dias.append(it.key());

            QVariantMap ops_map;
            for (const QString& op : ordem_operacoes) {
                const int valor = it.value().value(op, 0);
                if (valor > 0) ops_map.insert(op, valor);
            }

            QVariantMap item;
            item.insert("date", it.key());
            item.insert("operations", ops_map);
            item.insert("total", std::accumulate(it.value().constBegin(), it.value().constEnd(), 0, [](int s, int v) { return s + v; }));
            series.append(item);
        }

        const QList<QString> fallback_palette = {
            QStringLiteral("#0000ff"), QStringLiteral("#808080"), QStringLiteral("#ff0000"),
            QStringLiteral("#ff6200"), QStringLiteral("#ff00ff"), QStringLiteral("#00ff00")};
        QVariantMap op_colors;
        for (int i = 0; i < ordem_operacoes.size(); ++i) {
            const QString& op = ordem_operacoes.at(i);
            op_colors.insert(op, cores_operacoes_.value(op, fallback_palette.at(i % fallback_palette.size())));
        }

        QVariantList operation_order;
        for (const QString& op : ordem_operacoes) operation_order.append(op);

        QVariantMap out;
        out.insert("kind", "linha");
        out.insert("title", titulo);
        out.insert("labels", dias);
        out.insert("series", series);
        out.insert("op_colors", op_colors);
        out.insert("operation_order", operation_order);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico de linha", true);
        return _criar_grafico_sem_dados(obter_texto("operations_by_day", QStringLiteral("Operações por Dia")));
    }
}

}  // namespace source::data
