#include "gdc_03_selecionar_cor.h"

#include <QColor>
#include <QFrame>

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

void selecionar_cor(source::ui::DialogoPaletaCores* self, const QString& cor_hex) {
    try {
        const QColor cor(cor_hex);
        if (self->preview_nova_ != nullptr) {
            self->preview_nova_->setStyleSheet(QString("background-color: %1;").arg(cor_hex));
        }
        self->cor_selecionada_ = cor;
    } catch (...) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao selecionar cor %1").arg(cor_hex), true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
