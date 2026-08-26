#include "geui_16_atualizar_graficos_sem_fechar.h"

#include <QDialog>
#include <QMetaObject>
#include <QProgressDialog>
#include <QTabWidget>

#include "geui_08_criar_lista_graficos.h"
#include "geui_09_criar_mapeamento_funcoes.h"
#include "geui_13_obter_estados_checkboxes.h"
#include "geui_17_regenerar_graficos_existentes.h"
#include "geui_23_atualizar_checkboxes_graficos.h"
#include "source/data/st_01_GeradorEstatisticas.h"
#include "source/ui/ui_06_GerenciadorEstatisticasUI.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key), Q_ARG(QString, fallback));
    }
    return text;
}
}

namespace source::ui::GerenciamentoEstatisticasUI {

void atualizar_graficos_sem_fechar(source::ui::GerenciadorEstatisticasUI* self) {
    try {
        if (!self || !self->interface_obj()) return;
        QObject* iface = self->interface_obj();
        if (iface->property("_atualizando_idioma").toBool()) return;
        if (iface->property("_trocando_idioma").toBool()) return;

        auto* dialog = iface->property("dialog_estatisticas").value<QDialog*>();
        if (!dialog || !dialog->isVisible()) return;

        dialog->setWindowTitle(get_text(self->loc_obj(), "statistics", "Statistics"));

        auto* tab_widget = iface->property("tab_widget").value<QTabWidget*>();
        const qulonglong gerador_ptr = iface->property("_gerador_cpp_ptr").toULongLong();
        if (!tab_widget || gerador_ptr == 0) {
            return;
        }

        auto* gerador = reinterpret_cast<source::data::GeradorEstatisticas*>(gerador_ptr);
        gerador->atualizar_textos_traduzidos();

        QVariantList graficos_atualizados = criar_lista_graficos(self);
        QHash<QString, bool> estados = obter_estados_checkboxes(self);
        QVariantHash mapeamento = criar_mapeamento_funcoes(self, graficos_atualizados);

        if (auto* progress_regeneracao = iface->property("progress_regeneracao").value<QProgressDialog*>()) {
            progress_regeneracao->close();
            progress_regeneracao->deleteLater();
            iface->setProperty("progress_regeneracao", QVariant());
        }

        regenerar_graficos_existentes(self, graficos_atualizados, mapeamento);
        atualizar_checkboxes_graficos(self, graficos_atualizados, estados, mapeamento);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar gráficos sem fechar", true);
    }
}

}
