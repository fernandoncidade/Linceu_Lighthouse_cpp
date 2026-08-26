#include "gdc_02_adicionar_grid_cores.h"

#include <QGridLayout>
#include <QPushButton>

#include "source/ui/ui_11_DialogoCores.h"
#include "source/utils/LogManager.h"

namespace {

class BotaoCor : public QPushButton {
public:
    explicit BotaoCor(const QString& cor, int tamanho = 30)
        : QPushButton(), cor_(cor) {
        setFixedSize(tamanho, tamanho);
        setStyleSheet(QString("background-color: %1; border: 1px solid #888888;").arg(cor));
        setToolTip(cor);
    }

private:
    QString cor_;
};

}  // namespace

namespace source::ui::GerenciamentoDialogoCores {

void adicionar_grid_cores(source::ui::DialogoPaletaCores* self, QGridLayout* grid_layout,
                          const QStringList& lista_cores, int colunas) {
    try {
        int row = 0;
        int col = 0;
        for (const auto& cor_hex : lista_cores) {
            auto* botao = new BotaoCor(cor_hex);
            QObject::connect(botao, &QPushButton::clicked, self, [self, cor_hex]() {
                selecionar_cor(self, cor_hex);
            });
            grid_layout->addWidget(botao, row, col);

            ++col;
            if (col >= colunas) {
                col = 0;
                ++row;
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar grade de cores", true);
    }
}

}  // namespace source::ui::GerenciamentoDialogoCores
