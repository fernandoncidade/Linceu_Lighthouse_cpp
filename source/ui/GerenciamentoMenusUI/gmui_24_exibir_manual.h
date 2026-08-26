#pragma once

#include <QDialog>

class QListWidget;
class QPushButton;
class QTextBrowser;

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {

class ManualDialog : public QDialog {
    Q_OBJECT
public:
    explicit ManualDialog(QWidget* parent, QObject* interface_ref, const QString& titulo, const QString& lang = QString());

public slots:
    void on_idioma_alterado(const QString& novo_idioma);

private:
    void build_manual_content(const QString& lang);

private:
    QListWidget* toc_list_;
    QTextBrowser* browser_;
    QPushButton* btn_ok_;
    QString current_lang_;
    QObject* parent_ref_;
    QObject* loc_;
};

void exibir_manual(source::ui::GerenciadorMenusUI* self);

}
