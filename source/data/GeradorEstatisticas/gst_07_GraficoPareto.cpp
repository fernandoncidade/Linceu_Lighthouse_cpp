#include "gst_07_GraficoPareto.h"

#include <numeric>

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoPareto::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("pareto_analysis", QStringLiteral("Diagrama de Pareto"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        const QVariantMap resumo = criar_resumo_por_coluna(dados, QStringLiteral("tipo_operacao"));
        const QVariantList labels = resumo.value("labels").toList();
        const QVariantList values = resumo.value("values").toList();

        const double total = std::accumulate(values.begin(), values.end(), 0.0, [](double acc, const QVariant& v) {
            return acc + v.toDouble();
        });

        QVariantList cumulativa;
        double soma = 0.0;
        for (const QVariant& v : values) {
            soma += v.toDouble();
            cumulativa.append(total > 0.0 ? (soma / total) * 100.0 : 0.0);
        }

        const QList<QString> fallback_palette = {
            QStringLiteral("#ff0000"), QStringLiteral("#808080"), QStringLiteral("#0000ff"),
            QStringLiteral("#ff00ff"), QStringLiteral("#ff6200"), QStringLiteral("#00ff00")};
        QVariantList colors;
        colors.reserve(labels.size());
        for (int i = 0; i < labels.size(); ++i) {
            const QString op = labels.at(i).toString();
            const QString cor = cores_operacoes_.value(op, fallback_palette.at(i % fallback_palette.size()));
            colors.append(cor);
        }

        QVariantMap out;
        out.insert("kind", "pareto");
        out.insert("title", titulo);
        out.insert("labels", labels);
        out.insert("values", values);
        out.insert("cumulative", cumulativa);
        out.insert("colors", colors);
        out.insert("line_color", QStringLiteral("#ff0000"));
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico de Pareto", true);
        return _criar_grafico_sem_dados(obter_texto("pareto_analysis", QStringLiteral("Diagrama de Pareto")));
    }
}

}  // namespace source::data
