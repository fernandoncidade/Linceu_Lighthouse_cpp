#include "gdc_08_atualizar_traducoes.h"

#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

void atualizar_traducoes(source::ui::DialogoPaletaCores* self) {
    try {
        QString titulo;
        if (!self->tipo_operacao_.isEmpty()) {
            titulo = self->get_text("select_color_for") + " " + self->get_text(self->tipo_operacao_);
        } else {
            titulo = self->get_text("select_color");
        }
        self->setWindowTitle(titulo);

        if (self->label_atual_ != nullptr) {
            self->label_atual_->setText(self->get_text("current") + ":");
        }
        if (self->label_novo_ != nullptr) {
            self->label_novo_->setText(self->get_text("new") + ":");
        }

        if (self->tab_widget_ != nullptr) {
            self->tab_widget_->setTabText(0, self->get_text("basics"));
            self->tab_widget_->setTabText(1, self->get_text("pastels"));
            self->tab_widget_->setTabText(2, self->get_text("vibrant"));
        }

        if (self->btn_avancado_ != nullptr) {
            self->btn_avancado_->setText(self->get_text("advanced_color_picker"));
        }
        if (self->btn_ok_ != nullptr) {
            self->btn_ok_->setText(self->get_text("ok"));
        }
        if (self->btn_cancelar_ != nullptr) {
            self->btn_cancelar_->setText(self->get_text("cancel"));
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar traduções do diálogo de cores", true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
