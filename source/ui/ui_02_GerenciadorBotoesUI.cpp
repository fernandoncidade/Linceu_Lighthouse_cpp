#include "ui_02_GerenciadorBotoesUI.h"

#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMetaObject>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "source/utils/IconUtils.h"
#include "source/utils/LogManager.h"

namespace source::ui {
namespace {

int calcular_largura_botao_texto(QPushButton* button, int extra_padding) {
    if (button == nullptr) {
        return 0;
    }

    const int padding = qMax(0, extra_padding);
    int largura = button->sizeHint().width();

    const QString texto = button->text().trimmed();
    if (!texto.isEmpty()) {
        largura = qMax(largura, button->fontMetrics().horizontalAdvance(texto) + padding);
    }

    if (!button->icon().isNull()) {
        largura = qMax(largura, button->fontMetrics().horizontalAdvance(texto) +
                                    button->iconSize().width() + padding);
    }

    return largura;
}

}

GerenciadorBotoesUI::GerenciadorBotoesUI(QWidget* parent) : parent_(parent) {}

QPushButton* GerenciadorBotoesUI::add_button_with_label(QVBoxLayout* layout, const QString& label_text,
                                                        const QString& icon_name,
                                                        const std::function<void()>& callback,
                                                        const QString&, const QString& translation_key) {
    try {
        auto* layout_h = new QHBoxLayout();
        auto* label = new QLabel(label_text, parent_);
        layout_h->addWidget(label);

        auto* button = create_button();
        const QString icon_file = source::utils::get_icon_path(icon_name);
        button->setIcon(QIcon(icon_file));
        QObject::connect(button, &QPushButton::clicked, parent_, callback);
        layout_h->addWidget(button);
        layout->addLayout(layout_h);

        ButtonTranslationEntry entry;
        entry.label = label;
        entry.translation_key = translation_key.isEmpty() ? label_text : translation_key;
        buttons_data_.push_back(entry);
        return button;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao adicionar botão", true);
        return nullptr;
    }
}

void GerenciadorBotoesUI::update_buttons_text(QObject* loc) {
    try {
        for (const auto& entry : buttons_data_) {
            QString translated;
            QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated),
                                      Q_ARG(QString, entry.translation_key));
            if (entry.label != nullptr) {
                entry.label->setText(translated);
            }
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar texto dos botões", true);
    }
}

QPushButton* GerenciadorBotoesUI::create_button() {
    try {
        auto* button = new QPushButton();
        const int width = 3 * button->fontMetrics().horizontalAdvance('m');
        button->setMinimumWidth(width);
        button->setMaximumWidth(width);
        button->setFont(QFont("Arial", 9));
        return button;
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar botão", true);
        return nullptr;
    }
}

void GerenciadorBotoesUI::ajustar_largura_botao_texto(QPushButton* button, int extra_padding) {
    try {
        if (button == nullptr) {
            return;
        }

        const int largura = calcular_largura_botao_texto(button, extra_padding);
        if (largura <= 0) {
            return;
        }

        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        button->setMinimumWidth(largura);
        button->setMaximumWidth(largura);
        button->adjustSize();
        button->updateGeometry();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao ajustar largura do botão de texto", true);
    }
}

void GerenciadorBotoesUI::ajustar_largura_botoes_texto(const QList<QPushButton*>& buttons, int extra_padding) {
    try {
        int largura_maxima = 0;
        for (auto* button : buttons) {
            largura_maxima = qMax(largura_maxima, calcular_largura_botao_texto(button, extra_padding));
        }

        if (largura_maxima <= 0) {
            return;
        }

        for (auto* button : buttons) {
            if (button == nullptr) {
                continue;
            }
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            button->setMinimumWidth(largura_maxima);
            button->setMaximumWidth(largura_maxima);
            button->adjustSize();
            button->updateGeometry();
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao ajustar largura dos botões de texto", true);
    }
}

}  // namespace source::ui
