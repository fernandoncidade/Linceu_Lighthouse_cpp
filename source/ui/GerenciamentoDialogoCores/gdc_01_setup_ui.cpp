#include "gdc_01_setup_ui.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "gdc_02_adicionar_grid_cores.h"
#include "gdc_04_abrir_seletor_avancado.h"
#include "gdc_05_accept.h"
#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoDialogoCores {

void setup_ui(source::ui::DialogoPaletaCores* self) {
    try {
        auto* layout_principal = new QVBoxLayout(self);

        auto* layout_preview = new QHBoxLayout();
        self->preview_atual_ = new QFrame(self);
        self->preview_atual_->setFixedSize(50, 50);
        self->preview_atual_->setFrameStyle(QFrame::Box | QFrame::Plain);
        self->preview_atual_->setLineWidth(1);
        self->preview_atual_->setStyleSheet(QString("background-color: %1;").arg(self->cor_atual_.name()));

        self->preview_nova_ = new QFrame(self);
        self->preview_nova_->setFixedSize(50, 50);
        self->preview_nova_->setFrameStyle(QFrame::Box | QFrame::Plain);
        self->preview_nova_->setLineWidth(1);
        self->preview_nova_->setStyleSheet(QString("background-color: %1;").arg(self->cor_atual_.name()));

        self->label_atual_ = new QLabel(QString("%1:").arg(self->get_text("current")), self);
        self->label_novo_ = new QLabel(QString("%1:").arg(self->get_text("new")), self);
        layout_preview->addWidget(self->label_atual_);
        layout_preview->addWidget(self->preview_atual_);
        layout_preview->addSpacing(20);
        layout_preview->addWidget(self->label_novo_);
        layout_preview->addWidget(self->preview_nova_);
        layout_preview->addStretch(1);

        layout_principal->addLayout(layout_preview);

        self->tab_widget_ = new QTabWidget(self);

        auto* tab_basicas = new QWidget();
        auto* layout_basicas = new QVBoxLayout(tab_basicas);
        const QStringList cores_basicas = {
            "#000000", "#404040", "#808080", "#C0C0C0", "#FFFFFF", "#FF6200", "#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#00FFFF", "#FF00FF",
            "#800000", "#008000", "#000080", "#808000", "#008080", "#800080", "#FF8080", "#80FF80", "#8080FF", "#FFFF80", "#80FFFF", "#FF80FF",
            "#A52A2A", "#D2691E", "#CD853F", "#F4A460", "#8B4513", "#A0522D", "#B8860B", "#DAA520", "#BDB76B", "#6B8E23", "#556B2F", "#2E8B57",
            "#3CB371", "#20B2AA", "#5F9EA0", "#4682B4", "#4169E1", "#483D8B", "#6A5ACD", "#7B68EE", "#9370DB", "#4B0082", "#8A2BE2", "#9400D3",
            "#191970", "#003366", "#006400", "#2F4F4F", "#696969", "#708090", "#778899", "#8FBC8F", "#BC8F8F", "#CD5C5C", "#DC143C", "#B22222",
            "#FFA07A", "#D2B48C", "#7B3F00"
        };
        auto* grid_basicas = new QGridLayout();
        adicionar_grid_cores(self, grid_basicas, cores_basicas, 7);
        layout_basicas->addLayout(grid_basicas);
        self->tab_widget_->addTab(tab_basicas, self->get_text("basics"));

        auto* tab_pasteis = new QWidget();
        auto* layout_pasteis = new QVBoxLayout(tab_pasteis);
        const QStringList cores_pasteis = {
            "#FFE4E1", "#F0FFF0", "#E6E6FA", "#FFF8DC", "#FFFACD", "#FFE4B5", "#FFDAB9", "#FFC0CB", "#FFB6C1", "#FFE4C4", "#F5F5DC", "#F0FFFF",
            "#F5FFFA", "#F8F8FF", "#FFFFF0", "#F0F8FF", "#E0FFFF", "#F5F5F5", "#FFF0F5", "#FAF0E6", "#FDF5E6", "#FAEBD7", "#FFEFD5", "#FFEBCD",
            "#FFE4C4", "#FFDEAD", "#EEE8AA", "#F0E68C", "#E0EEE0", "#D8BFD8", "#DDA0DD", "#EE82EE", "#DA70D6", "#D8BFD8", "#E6E6FA", "#B0E0E6",
            "#ADD8E6", "#B0C4DE", "#AFEEEE", "#E0FFFF", "#E6C8FF", "#F5E6FF", "#F0E6FF", "#E6F0FF", "#E6FFF0", "#F0FFE6", "#FFE6F0", "#FFE6E6",
            "#E6FFE6", "#E6E6FF", "#FFE6FF", "#FFF5E6", "#F5FFE6", "#E6FFF5", "#F8F0FF", "#FFF0F8", "#F0FFF8", "#F8FFF0", "#E6F5FF", "#FFE6F5",
            "#FFE6C8", "#C8FFE6", "#E6C8FF"
        };
        auto* grid_pasteis = new QGridLayout();
        adicionar_grid_cores(self, grid_pasteis, cores_pasteis, 7);
        layout_pasteis->addLayout(grid_pasteis);
        self->tab_widget_->addTab(tab_pasteis, self->get_text("pastels"));

        auto* tab_vibrantes = new QWidget();
        auto* layout_vibrantes = new QVBoxLayout(tab_vibrantes);
        const QStringList cores_vibrantes = {
            "#FF0000", "#FF4500", "#FF6347", "#FF7F50", "#FF8C00", "#FFA500", "#FFD700", "#FFFF00", "#ADFF2F", "#7FFF00", "#00FF00", "#32CD32",
            "#00FA9A", "#00FFFF", "#1E90FF", "#0000FF", "#8A2BE2", "#FF00FF", "#C71585", "#FF1493", "#FF69B4", "#FF0066", "#FF3366", "#FF00CC",
            "#CC00FF", "#9900FF", "#6600FF", "#001AFF", "#00B3FF", "#FF33FF", "#0033FF", "#0099FF", "#00CCFF", "#00FFCC", "#00FF66", "#33FF00",
            "#99FF00", "#CCFF00", "#FFCC00", "#FF9900", "#FF6600", "#FF5000", "#FF0033", "#E100FF", "#FF1A00", "#FF00B3", "#B300FF", "#5500FF",
            "#00FFB3", "#00FF1A", "#B3FF00", "#FF5500", "#FF0055", "#AA00FF", "#007FFF", "#00FFAA", "#7FFF00", "#FFAA00", "#FF007F", "#3F00FF",
            "#00FF3F", "#FF3F00", "#FF3F33"
        };
        auto* grid_vibrantes = new QGridLayout();
        adicionar_grid_cores(self, grid_vibrantes, cores_vibrantes, 7);
        layout_vibrantes->addLayout(grid_vibrantes);
        self->tab_widget_->addTab(tab_vibrantes, self->get_text("vibrant"));

        layout_principal->addWidget(self->tab_widget_);

        self->btn_avancado_ = new QPushButton(self->get_text("advanced_color_picker"), self);
        QObject::connect(self->btn_avancado_, &QPushButton::clicked, self, [self]() {
            abrir_seletor_avancado(self);
        });
        layout_principal->addWidget(self->btn_avancado_);

        auto* layout_botoes = new QHBoxLayout();
        self->btn_ok_ = new QPushButton(self->get_text("ok"), self);
        QObject::connect(self->btn_ok_, &QPushButton::clicked, self, [self]() {
            aceitar(self);
        });
        self->btn_cancelar_ = new QPushButton(self->get_text("cancel"), self);
        QObject::connect(self->btn_cancelar_, &QPushButton::clicked, self, &DialogoPaletaCores::reject);

        layout_botoes->addStretch(1);
        layout_botoes->addWidget(self->btn_ok_);
        layout_botoes->addWidget(self->btn_cancelar_);
        layout_principal->addLayout(layout_botoes);

        self->setLayout(layout_principal);
        self->resize(400, 500);
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao configurar interface do diálogo de cores", true);
        throw;
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
