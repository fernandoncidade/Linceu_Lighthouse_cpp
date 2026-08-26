#include "geui_03_limpar_referencia_dialog.h"

#include <QProgressDialog>
#include <QVariantList>
#include <QVariantHash>

#include "geui_31_worker_grafico.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoEstatisticasUI {

namespace {
constexpr auto kStatsGenerationIdProperty = "_estatisticas_generation_id";
constexpr auto kStatsPreparationFinishedProperty = "_estatisticas_preparacao_concluida";

qulonglong avancar_generation_id(QObject* iface) {
    if (iface == nullptr) {
        return 0;
    }

    qulonglong next = iface->property(kStatsGenerationIdProperty).toULongLong() + 1;
    if (next == 0) {
        next = 1;
    }
    iface->setProperty(kStatsGenerationIdProperty, QVariant::fromValue<qulonglong>(next));
    return next;
}
}

void limpar_referencia_dialog(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* interface_obj = self->interface_obj();
        avancar_generation_id(interface_obj);

        if (auto* worker = interface_obj->property("worker").value<GraficoWorker*>()) {
            QObject::disconnect(worker, nullptr, nullptr, nullptr);
            worker->parar();
            if (worker->isRunning()) {
                if (!worker->wait(3000)) {
                    worker->setParent(nullptr);
                    QObject::connect(worker, &QThread::finished, worker, &QObject::deleteLater,
                                     Qt::UniqueConnection);
                } else {
                    worker->deleteLater();
                }
            } else {
                worker->deleteLater();
            }
        }

        if (auto* progress = interface_obj->property("progress_dialog").value<QProgressDialog*>()) {
            progress->close();
            progress->deleteLater();
        }
        if (auto* progress_regen = interface_obj->property("progress_regeneracao").value<QProgressDialog*>()) {
            progress_regen->close();
            progress_regen->deleteLater();
        }

        const qulonglong gerador_ptr = interface_obj->property("_gerador_cpp_ptr").toULongLong();
        if (gerador_ptr != 0) {
            delete reinterpret_cast<source::data::GeradorEstatisticas*>(gerador_ptr);
            interface_obj->setProperty("_gerador_cpp_ptr", QVariant::fromValue<qulonglong>(0));
        }

        interface_obj->setProperty("dialog_estatisticas", QVariant());
        interface_obj->setProperty("checkboxes_graficos", QVariantHash());
        interface_obj->setProperty("graficos_dados", QVariantHash());
        interface_obj->setProperty("painel_selecao", QVariant());
        interface_obj->setProperty("splitter", QVariant());
        interface_obj->setProperty("painel_selecao_interno", QVariant());
        interface_obj->setProperty("btn_toggle_painel", QVariant());
        interface_obj->setProperty("checkbox_todos", QVariant());
        interface_obj->setProperty("checkboxes_layout", QVariant());
        interface_obj->setProperty("tab_widget", QVariant());
        interface_obj->setProperty("_estatisticas_container", QVariant());
        interface_obj->setProperty("_estatisticas_tab_widgets", QVariantHash());
        interface_obj->setProperty("_estatisticas_graficos_dados_por_idioma", QVariantHash());
        interface_obj->setProperty("_estatisticas_idioma_ativo", QVariant());
        interface_obj->setProperty("progress_dialog", QVariant());
        interface_obj->setProperty("progress_regeneracao", QVariant());
        interface_obj->setProperty("worker", QVariant());
        interface_obj->setProperty("fila_graficos", QVariantList());
        interface_obj->setProperty("graficos_para_regenerar", QVariantList());
        interface_obj->setProperty("_atualizando_idioma", false);
        interface_obj->setProperty("graficos_processados", 0);
        interface_obj->setProperty("total_graficos", 0);
        interface_obj->setProperty("indice_regeneracao_atual", 0);
        interface_obj->setProperty("gerando_grafico", false);
        interface_obj->setProperty(kStatsPreparationFinishedProperty, false);
        interface_obj->setProperty("_estatisticas_db_path", QVariant());
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao limpar referências do diálogo", true);
    }
}

}
