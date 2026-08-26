#pragma once

#include <QDialog>

class QPushButton;
class QTextBrowser;

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {

class AjudaDialog : public QDialog {
    Q_OBJECT
public:
    explicit AjudaDialog(QWidget* parent, QObject* interface_ref);

public slots:
    void on_idioma_alterado(const QString& novo_idioma);

private:
    QString t(const QString& key, const QString& fallback) const;
    void build_content();

private:
    QObject* parent_ref_;
    QObject* loc_;
    QTextBrowser* browser_;
    QPushButton* btn_ok_;
};

void exibir_ajuda(source::ui::GerenciadorMenusUI* self);

}
