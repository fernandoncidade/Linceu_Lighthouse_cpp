#include "gst_03_GraficoBarras.h"

#include "source/utils/LogManager.h"

namespace source::data {

QVariant GraficoBarras::gerar() {
    try {
        const auto dados = _obter_dados();
        const QString titulo = obter_texto("bar_chart", QStringLiteral("Top 30 Tipos de Arquivo"));
        if (dados.isEmpty()) return _criar_grafico_sem_dados(titulo);

        QVariantMap out;
        out.insert("kind", "barras");
        out.insert("title", titulo);
        const QVariantMap resumo = criar_resumo_por_coluna(dados, QStringLiteral("tipo"), 30);
        out.insert("labels", resumo.value("labels"));
        out.insert("values", resumo.value("values"));
        return out;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao gerar gráfico de barras", true);
        return _criar_grafico_sem_dados(QStringLiteral("Top 30 Tipos de Arquivo"));
    }
}

}  // namespace source::data
