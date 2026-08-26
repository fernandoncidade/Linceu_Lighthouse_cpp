#include "geui_25_atualizar_dados_graficos_com_novos_titulos.h"

#include <QVariantMap>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kStatsGraphDataByLanguageProperty = "_estatisticas_graficos_dados_por_idioma";
constexpr auto kStatsActiveLanguageProperty = "_estatisticas_idioma_ativo";

QString obter_idioma_estatisticas(QObject* iface) {
    if (iface == nullptr) return QStringLiteral("pt_BR");
    const QString idioma = iface->property(kStatsActiveLanguageProperty).toString();
    return idioma.isEmpty() ? QStringLiteral("pt_BR") : idioma;
}

void sincronizar_graficos_idioma_ativo(QObject* iface, const QVariantHash& dados) {
    if (iface == nullptr) return;

    QVariantHash graficos_por_idioma = iface->property(kStatsGraphDataByLanguageProperty).toHash();
    graficos_por_idioma.insert(obter_idioma_estatisticas(iface), dados);
    iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_dados_graficos_com_novos_titulos(source::ui::GerenciadorEstatisticasUI* self, const QVariantList&, const QVariantHash& mapeamento_funcoes) {
    try {
        if (!self || !self->interface_obj()) return;
        QVariantHash dados = self->interface_obj()->property("graficos_dados").toHash();
        QVariantHash novos;

        for (auto it = dados.cbegin(); it != dados.cend(); ++it) {
            QVariantMap d = it.value().toMap();
            const QString func = d.value("func").toString();
            const QString novoTitulo = mapeamento_funcoes.value(func).toString();
            if (!novoTitulo.isEmpty()) {
                d.insert("titulo", novoTitulo);
                novos.insert(novoTitulo, d);
            } else {
                novos.insert(it.key(), d);
            }
        }

        self->interface_obj()->setProperty("graficos_dados", novos);
        sincronizar_graficos_idioma_ativo(self->interface_obj(), novos);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar dados dos gráficos", true);
    }
}

}
