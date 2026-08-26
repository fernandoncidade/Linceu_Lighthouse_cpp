#pragma once

#include <QList>
#include <QVector>

class QHBoxLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QWidget;

namespace source::ui {

struct ButtonTranslationEntry {
    QLabel* label;
    QString translation_key;
};

class GerenciadorBotoesUI {
public:
    explicit GerenciadorBotoesUI(QWidget* parent);

    QPushButton* add_button_with_label(QVBoxLayout* layout, const QString& label_text, const QString& icon_name,
                                       const std::function<void()>& callback, const QString& icon_path = QString(),
                                       const QString& translation_key = QString());
    void update_buttons_text(QObject* loc);
    QPushButton* create_button();
    static void ajustar_largura_botao_texto(QPushButton* button, int extra_padding = 20);
    static void ajustar_largura_botoes_texto(const QList<QPushButton*>& buttons, int extra_padding = 20);

private:
    QWidget* parent_;
    QVector<ButtonTranslationEntry> buttons_data_;
};

}  // namespace source::ui
