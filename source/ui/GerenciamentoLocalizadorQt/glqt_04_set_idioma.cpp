#include "glqt_04_set_idioma.h"

#include <QApplication>

#include "glqt_01_inicializar_tradutor_metadados.h"
#include "glqt_03_carregar_tradutor.h"
#include "source/ui/ui_12_LocalizadorQt.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoLocalizadorQt {

void set_idioma(source::ui::LocalizadorQt* self, const QString& idioma) {
    try {
        if (self->property("_set_idioma_in_progress").toBool()) {
            return;
        }
        self->setProperty("_set_idioma_in_progress", true);

        if (!self->idiomas_suportados_.contains(idioma)) {
            source::utils::LogManager::get_logger().warning(
                QString("Idioma não suportado: %1").arg(idioma));
            self->setProperty("_set_idioma_in_progress", false);
            return;
        }

        if (idioma == self->idioma_atual_) {
            self->setProperty("_set_idioma_in_progress", false);
            return;
        }

        if (self->tradutor_metadados_ != nullptr) {
            inicializar_tradutor_metadados(self);
        }

        if (carregar_tradutor(self, idioma)) {
            self->idioma_atual_ = idioma;
            self->setProperty("idioma_atual", idioma);
            self->salvar_preferencia_idioma(idioma);
            emit self->idioma_alterado(idioma);
            emit self->traducoes_carregadas(idioma);
        }
        self->setProperty("_set_idioma_in_progress", false);
    } catch (...) {
        if (self != nullptr) {
            self->setProperty("_set_idioma_in_progress", false);
        }
        source::utils::LogManager::get_logger().error("Erro ao alterar idioma", true);
    }
}

}
