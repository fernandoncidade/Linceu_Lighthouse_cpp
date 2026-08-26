#include "gst_02_GraficoPizza.h"

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoPizza::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("operations_pie", QStringLiteral("Distribuição de Operações"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantMap out;
        out.insert("kind", "pizza");
        out.insert("title", titulo);
        const QVariantMap resumo = criar_resumo_por_coluna(dados, QStringLiteral("tipo_operacao"));
        const QVariantList labels = resumo.value("labels").toList();
        QVariantList colors;
        colors.reserve(labels.size());
        for (const QVariant& label_var : labels) {
            const QString label = label_var.toString();
            colors.append(cores_operacoes_.value(label, QStringLiteral("#333333")));
        }

        out.insert("labels", labels);
        out.insert("values", resumo.value("values"));
        out.insert("colors", colors);
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico de pizza", true);
        return _criar_grafico_sem_dados(
            obter_texto("operations_pie", QStringLiteral("Distribuição de Operações")));
    }
}

}  // namespace source::data
