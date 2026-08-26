#include "gmui_21_on_traducoes_carregadas.h"

#include <QPointer>
#include <QTimer>

#include "gmui_04_criar_menu_principal.h"
#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

void on_traducoes_carregadas(source::ui::GerenciadorMenusUI* self, const QString&) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr) {
            return;
        }

        QPointer<QObject> iface(self->interface_obj());
        QTimer::singleShot(0, iface, [self, iface]() {
            if (iface == nullptr || self == nullptr || self->interface_obj() == nullptr) {
                return;
            }
            criar_menu_principal(self);
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir confirmação de conclusão da tradução", true);
    }
}

}
