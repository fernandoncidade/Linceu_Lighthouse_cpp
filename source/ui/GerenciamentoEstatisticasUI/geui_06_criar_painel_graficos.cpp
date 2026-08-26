#include "geui_06_criar_painel_graficos.h"

#include <QStackedLayout>
#include <QTabWidget>
#include <QVariantHash>
#include <QWidget>

#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
constexpr auto kStatsTabWidgetsProperty = "_estatisticas_tab_widgets";
constexpr auto kStatsGraphDataByLanguageProperty = "_estatisticas_graficos_dados_por_idioma";
constexpr auto kStatsActiveLanguageProperty = "_estatisticas_idioma_ativo";
constexpr auto kStatsContainerProperty = "_estatisticas_container";

QStringList idiomas_estatisticas_suportados() {
    return {
        QStringLiteral("pt_BR"),
        QStringLiteral("en_US"),
        QStringLiteral("es_ES"),
        QStringLiteral("fr_FR"),
        QStringLiteral("it_IT"),
        QStringLiteral("de_DE")
    };
}

QString obter_idioma_atual(QObject* loc) {
    const QString idioma = (loc != nullptr) ? loc->property("idioma_atual").toString() : QString();
    return idioma.isEmpty() ? QStringLiteral("pt_BR") : idioma;
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

QWidget* criar_painel_graficos(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        auto* container = new QWidget();
        auto* stack = new QStackedLayout(container);
        stack->setContentsMargins(0, 0, 0, 0);
        stack->setSpacing(0);

        if (self && self->interface_obj()) {
            QObject* iface = self->interface_obj();
            const QString idioma = obter_idioma_atual(self->loc_obj());

            QVariantHash tabs_por_idioma;
            QVariantHash graficos_por_idioma;
            QTabWidget* aba_ativa = nullptr;

            for (const QString& idioma_suportado : idiomas_estatisticas_suportados()) {
                auto* tabs = new QTabWidget(container);
                stack->addWidget(tabs);
                tabs_por_idioma.insert(idioma_suportado, QVariant::fromValue(tabs));
                graficos_por_idioma.insert(idioma_suportado, QVariantHash());
                if (idioma_suportado == idioma) {
                    aba_ativa = tabs;
                }
            }

            if (aba_ativa == nullptr && !tabs_por_idioma.isEmpty()) {
                aba_ativa = tabs_por_idioma.constBegin().value().value<QTabWidget*>();
            }

            if (aba_ativa != nullptr) {
                stack->setCurrentWidget(aba_ativa);
            }

            iface->setProperty("tab_widget", QVariant::fromValue(aba_ativa));
            iface->setProperty(kStatsContainerProperty, QVariant::fromValue(container));
            iface->setProperty(kStatsTabWidgetsProperty, tabs_por_idioma);
            iface->setProperty(kStatsGraphDataByLanguageProperty, graficos_por_idioma);
            iface->setProperty(kStatsActiveLanguageProperty, idioma);
        }
        return container;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar painel de gráficos", true);
        return nullptr;
    }
}

}
