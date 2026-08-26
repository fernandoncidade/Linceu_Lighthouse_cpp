#include "gdc_06_traduzir_dialogo_cores.h"

#include <QGroupBox>
#include <QHash>
#include <QLabel>
#include <QPushButton>

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace {
QString normalizar(const QString& texto) {
    QString t = texto.trimmed();
    t.replace(":", "");
    t.replace("&", "");
    return t;
}
}

namespace source::ui::GerenciamentoDialogoCores {

void traduzir_dialogo_cores(source::ui::DialogoPaletaCores* self, QWidget* dialogo) {
    try {
        if (dialogo == nullptr) {
            dialogo = self;
        }

        const QHash<QString, QString> mapeamento = {
            {"Basic colors", self->get_text("basic_colors")},
            {"&Basic colors", self->get_text("basic_colors")},
            {"Custom colors", self->get_text("custom_colors")},
            {"&Custom colors", self->get_text("custom_colors")},
            {"Pick Screen Color", self->get_text("pick_screen_color")},
            {"&Pick Screen Color", self->get_text("pick_screen_color")},
            {"Add to Custom Colors", self->get_text("add_to_custom_colors")},
            {"&Add to Custom Colors", self->get_text("add_to_custom_colors")},
            {"Hue", self->get_text("hue")}, {"&Hue", self->get_text("hue")},
            {"Sat", self->get_text("sat")}, {"&Sat", self->get_text("sat")},
            {"Val", self->get_text("val")}, {"&Val", self->get_text("val")},
            {"Red", self->get_text("red")}, {"&Red", self->get_text("red")},
            {"Green", self->get_text("green")}, {"&Green", self->get_text("green")},
            {"Blue", self->get_text("blue")}, {"&Blue", self->get_text("blue")},
            {"HTML", self->get_text("html")}, {"&HTML", self->get_text("html")},
            {"OK", self->get_text("ok")}, {"&OK", self->get_text("ok")},
            {"Cancel", self->get_text("cancel")}, {"&Cancel", self->get_text("cancel")}
        };

        const auto labels = dialogo->findChildren<QLabel*>();
        for (auto* label : labels) {
            const QString original = label->text().trimmed();
            const QString norm = normalizar(original);
            for (auto it = mapeamento.begin(); it != mapeamento.end(); ++it) {
                if (normalizar(it.key()) == norm) {
                    label->setText(original.contains(':') ? (it.value() + ":") : it.value());
                    break;
                }
            }
        }

        const auto buttons = dialogo->findChildren<QPushButton*>();
        for (auto* button : buttons) {
            const QString original = button->text().trimmed();
            const QString norm = normalizar(original);
            for (auto it = mapeamento.begin(); it != mapeamento.end(); ++it) {
                if (normalizar(it.key()) == norm) {
                    button->setText(it.value());
                    break;
                }
            }
        }

        const auto groups = dialogo->findChildren<QGroupBox*>();
        for (auto* group : groups) {
            const QString original = group->title().trimmed();
            const QString norm = normalizar(original);
            for (auto it = mapeamento.begin(); it != mapeamento.end(); ++it) {
                if (normalizar(it.key()) == norm) {
                    group->setTitle(it.value());
                    break;
                }
            }
        }

        auto* htmlLabel = dialogo->findChild<QLabel*>("qt_colorname_label");
        if (htmlLabel != nullptr) {
            htmlLabel->setText(self->get_text("html") + ":");
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao traduzir diálogo de cores", true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
