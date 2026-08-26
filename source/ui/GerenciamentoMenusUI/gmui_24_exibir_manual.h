#pragma once

#include <QDialog>
#include <QTextCursor>
#include <QVector>

class QLabel;
class QEvent;
class QListWidget;
class QLineEdit;
class QPushButton;
class QTextBrowser;
class QToolButton;

namespace source::ui { class GerenciadorMenusUI; }

namespace source::ui::GerenciamentoMenusUI {

class ManualDialog : public QDialog {
    Q_OBJECT
public:
    explicit ManualDialog(QWidget* parent, QObject* interface_ref, const QString& titulo, const QString& lang = QString());

public slots:
    void on_idioma_alterado(const QString& novo_idioma);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void build_manual_content(const QString& lang);
    void update_search_texts();
    void rebuild_search(const QString& text);
    void navigate_search(int step);
    void update_search_buttons();
    void update_search_highlights();

private:
    QLabel* search_label_;
    QLineEdit* search_edit_;
    QToolButton* search_previous_button_;
    QToolButton* search_next_button_;
    QListWidget* toc_list_;
    QTextBrowser* browser_;
    QPushButton* btn_ok_;
    QString current_lang_;
    QVector<QTextCursor> search_matches_;
    int search_current_index_;
    QObject* parent_ref_;
    QObject* loc_;
};

void exibir_manual(source::ui::GerenciadorMenusUI* self);

}
