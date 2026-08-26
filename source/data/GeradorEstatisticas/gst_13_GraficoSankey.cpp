#include "gst_13_GraficoSankey.h"

#include <algorithm>

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QPair>
#include <QSet>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoSankey::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("file_operations_flow", QStringLiteral("Fluxo de Operações em Arquivos"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        struct Registro {
            QString operacao;
            QString tipo;
            QDateTime timestamp;
        };

        QList<Registro> registros;

        for (const QVariant& v : dados) {
            const QVariantMap row = v.toMap();
            const QString op = row.value("tipo_operacao").toString().trimmed();
            const QString tipo = row.value("tipo").toString().trimmed();
            if (op.isEmpty() || tipo.isEmpty()) continue;
            registros.append({op, tipo, extrair_timestamp(row)});
        }
        if (registros.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QHash<QString, QList<Registro>> por_operacao;
        for (const Registro& r : registros) por_operacao[r.operacao].append(r);

        QHash<QString, QHash<QString, int>> fluxos;
        QHash<QString, int> total_por_operacao;
        QHash<QString, int> total_por_tipo;
        QSet<QString> operacoes_set;
        QSet<QString> tipos_set;

        for (auto it = por_operacao.constBegin(); it != por_operacao.constEnd(); ++it) {
            QList<Registro> lista = it.value();
            std::sort(lista.begin(), lista.end(), [](const Registro& a, const Registro& b) {
                if (a.timestamp.isValid() != b.timestamp.isValid()) return a.timestamp.isValid();
                if (a.timestamp.isValid() && b.timestamp.isValid() && a.timestamp != b.timestamp) {
                    return a.timestamp > b.timestamp;
                }
                if (a.tipo != b.tipo) return a.tipo.localeAwareCompare(b.tipo) < 0;
                return a.operacao.localeAwareCompare(b.operacao) < 0;
            });

            const int limite = qMin(20, lista.size());
            for (int i = 0; i < limite; ++i) {
                const Registro& r = lista.at(i);
                fluxos[r.operacao][r.tipo] += 1;
                total_por_operacao[r.operacao] += 1;
                total_por_tipo[r.tipo] += 1;
                operacoes_set.insert(r.operacao);
                tipos_set.insert(r.tipo);
            }
        }

        if (fluxos.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QList<QString> operacoes = operacoes_set.values();
        QList<QString> tipos = tipos_set.values();
        std::sort(operacoes.begin(), operacoes.end(), [&total_por_operacao](const QString& a, const QString& b) {
            const int va = total_por_operacao.value(a, 0);
            const int vb = total_por_operacao.value(b, 0);
            if (va != vb) return va > vb;
            return a.localeAwareCompare(b) < 0;
        });
        std::sort(tipos.begin(), tipos.end(), [&total_por_tipo](const QString& a, const QString& b) {
            const int va = total_por_tipo.value(a, 0);
            const int vb = total_por_tipo.value(b, 0);
            if (va != vb) return va > vb;
            return a.localeAwareCompare(b) < 0;
        });

        QVariantList op_labels;
        QVariantList tipo_labels;
        for (const QString& op : operacoes) op_labels.append(op);
        for (const QString& tipo : tipos) tipo_labels.append(tipo);

        QVariantList links;
        QVariantList link_colors;
        for (const QString& op : operacoes) {
            const auto mapa_tipos = fluxos.value(op);
            QList<QString> tipos_op = mapa_tipos.keys();
            std::sort(tipos_op.begin(), tipos_op.end(), [&mapa_tipos](const QString& a, const QString& b) {
                const int va = mapa_tipos.value(a, 0);
                const int vb = mapa_tipos.value(b, 0);
                if (va != vb) return va > vb;
                return a.localeAwareCompare(b) < 0;
            });

            for (const QString& tipo : tipos_op) {
                const int valor = mapa_tipos.value(tipo, 0);
                if (valor <= 0) continue;
                QVariantMap link;
                link.insert("source", op);
                link.insert("target", tipo);
                link.insert("value", valor);
                links.append(link);
                link_colors.append(cores_operacoes_.value(op, QStringLiteral("#666666")));
            }
        }

        static const QList<QString> tipo_palette = {
            QStringLiteral("#377eb8"), QStringLiteral("#4daf4a"), QStringLiteral("#e41a1c"),
            QStringLiteral("#984ea3"), QStringLiteral("#ff7f00"), QStringLiteral("#a65628"),
            QStringLiteral("#f781bf"), QStringLiteral("#999999"), QStringLiteral("#ffff33"),
            QStringLiteral("#a6cee3")};

        QVariantMap node_colors;
        int idx_tipo = 0;
        for (const QVariant& op : op_labels) {
            const QString label = op.toString();
            node_colors.insert(label, cores_operacoes_.value(label, QStringLiteral("#333333")));
        }
        for (const QVariant& tipo : tipo_labels) {
            const QString label = tipo.toString();
            node_colors.insert(label, tipo_palette.at(idx_tipo % tipo_palette.size()));
            idx_tipo += 1;
        }

        QVariantMap out;
        out.insert("kind", "sankey");
        out.insert("title", titulo);
        out.insert("subtitle", obter_texto("sankey_operations_subtitle",
                                           QStringLiteral("Últimos 20 arquivos/pastas por operação")));
        out.insert("operations", op_labels);
        out.insert("types", tipo_labels);
        out.insert("nodes", op_labels + tipo_labels);
        out.insert("node_colors", node_colors);
        out.insert("links", links);
        out.insert("link_colors", link_colors);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar sankey", true);
        return _criar_grafico_sem_dados(
            obter_texto("file_operations_flow", QStringLiteral("Fluxo de Operações em Arquivos")));
    }
}

}  // namespace source::data
