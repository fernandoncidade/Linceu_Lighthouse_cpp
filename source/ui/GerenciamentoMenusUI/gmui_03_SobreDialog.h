#pragma once

#include <QDialog>
#include <QHash>
#include <QVector>

class QLabel;
class QEvent;
class QLineEdit;
class QPushButton;
class QResizeEvent;
class QShowEvent;
class QTabWidget;
class QTextBrowser;
class QToolButton;
class QUrl;

namespace source::ui::GerenciamentoMenusUI {

class SobreDialog : public QDialog {
    Q_OBJECT
public:
    explicit SobreDialog(
        QWidget* parent,
        QObject* interface_ref,
        const QString& titulo,
        const QString& texto_fixo,
        const QString& texto_history,
        const QString& detalhes,
        const QString& licencas,
        const QString& sites_licencas,
        const QString& show_history_text,
        const QString& hide_history_text,
        const QString& show_details_text,
        const QString& hide_details_text,
        const QString& show_licenses_text,
        const QString& hide_licenses_text,
        const QString& ok_text,
        const QString& site_oficial_text,
        const QString& avisos = QString(),
        const QString& show_notices_text = QString(),
        const QString& hide_notices_text = QString(),
        const QString& privacy_policy = QString(),
        const QString& show_privacy_policy_text = QString(),
        const QString& hide_privacy_policy_text = QString(),
        const QString& info_not_available_text = QStringLiteral("Information not available"),
        const QString& release_notes = QString(),
        const QString& show_release_notes_text = QString(),
        const QString& hide_release_notes_text = QString());

public slots:
    void on_idioma_alterado(const QString& novo_idioma = QString());

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void configure_browser(QTextBrowser* browser);
    void apply_text_or_placeholder(QTextBrowser* browser, const QString& content, const QString& info_not_available_text);
    void register_license_document(const QString& doc_id,
                                   const QString& displayed_path,
                                   const QString& license_title,
                                   const QString& content);
    void handle_anchor_click(const QUrl& url);
    void open_license_document(const QString& doc_id);
    int calculate_fixed_label_height(int available_width) const;
    void refresh_fixed_label_geometry();
    void update_tab_labels(int current_index);
    void update_search_texts();
    void apply_about_search();
    void navigate_about_search(int step);
    void update_about_search_buttons();
    void reset_about_search_position();

private:
    QLabel* fixed_label_;
    QLabel* search_label_;
    QLineEdit* search_edit_;
    QToolButton* search_previous_button_;
    QToolButton* search_next_button_;
    QTabWidget* tabs_;
    QPushButton* ok_button_;
    QTextBrowser* history_browser_;
    QTextBrowser* detalhes_browser_;
    QTextBrowser* licencas_browser_;
    QTextBrowser* avisos_browser_;
    QTextBrowser* privacidade_browser_;
    QTextBrowser* release_notes_browser_;
    QString info_not_available_text_;
    QString sites_licencas_;
    QHash<QString, QString> displayed_path_to_doc_id_;
    QHash<QString, QString> license_document_titles_;
    QHash<QString, QString> license_document_contents_;
    QVector<QPair<QString, QString>> tab_labels_;
    QTextBrowser* search_current_browser_;
    int search_current_start_;
    QObject* parent_ref_;
    QObject* loc_;
};

}  // namespace source::ui::GerenciamentoMenusUI
