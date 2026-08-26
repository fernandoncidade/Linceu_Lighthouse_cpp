#include "gfil_09_atualizar_interface.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include "source/tools/fil_01_Filtros.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace {
QString gt(source::tools::Filtros* self, const QString& key, const QString& fallback = QString()) {
    if (!self || !self->localizador()) return fallback;
    QString t = self->localizador()->get_text(key);
    return t.isEmpty() ? fallback : t;
}
}

namespace source::tools::GerenciamentoFiltros {

void atualizar_interface(source::tools::Filtros* self, const QString&) {
    try {
        if (!self) return;

        self->setWindowTitle(gt(self, "advanced_filters", self->windowTitle()));

        if (auto* gb = self->findChild<QGroupBox*>(QStringLiteral("grupo_operacao"))) {
            gb->setTitle(gt(self, "operation_filter", gb->title()));
        }
        if (auto* gb = self->findChild<QGroupBox*>(QStringLiteral("grupo_busca"))) {
            gb->setTitle(gt(self, "search", gb->title()));
        }
        if (auto* gb = self->findChild<QGroupBox*>(QStringLiteral("grupo_extensao"))) {
            gb->setTitle(gt(self, "extension_filter", gb->title()));
        }
        if (auto* gb = self->findChild<QGroupBox*>(QStringLiteral("grupo_data"))) {
            gb->setTitle(gt(self, "date_filter", gb->title()));
        }

        if (auto* label = self->findChild<QLabel*>(QStringLiteral("label_data_inicial"))) {
            label->setText(gt(self, "start_date", label->text()));
        }
        if (auto* label = self->findChild<QLabel*>(QStringLiteral("label_data_final"))) {
            label->setText(gt(self, "end_date", label->text()));
        }
        if (auto* label = self->findChild<QLabel*>(QStringLiteral("label_limpar_filtros"))) {
            label->setText(gt(self, "clear_filters", label->text()));
        }

        const QStringList opKeys = {"op_moved", "op_renamed", "op_added", "op_deleted", "op_modified", "op_scanned"};
        const QVariantMap map = self->property("checkboxes_operacao").toMap();
        for (const QString& key : opKeys) {
            auto* cb = qobject_cast<QCheckBox*>(map.value(key).value<QObject*>());
            if (cb) cb->setText(gt(self, key, cb->text()));
        }

        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_mover"))) {
            cb->setText(gt(self, "ignore_move_filter", cb->text()));
        }
        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_renomeados"))) {
            cb->setText(gt(self, "ignore_rename_filter", cb->text()));
        }
        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_adicionados"))) {
            cb->setText(gt(self, "ignore_add_filter", cb->text()));
        }
        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_excluidos"))) {
            cb->setText(gt(self, "ignore_delete_filter", cb->text()));
        }
        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_data_modificados"))) {
            cb->setText(gt(self, "ignore_modified_filter", cb->text()));
        }
        if (auto* cb = self->findChild<QCheckBox*>(QStringLiteral("ignorar_escaneados"))) {
            cb->setText(gt(self, "ignore_scanned_filter", cb->text()));
        }

        if (auto* botao = self->findChild<QPushButton*>(QStringLiteral("botao_limpar"))) {
            botao->setText(QString());
            botao->setFixedSize(botao->sizeHint());
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar interface de filtros", true);
    }
}

} // namespace source::tools::GerenciamentoFiltros
