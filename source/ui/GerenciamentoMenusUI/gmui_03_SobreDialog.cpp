#include "gmui_03_SobreDialog.h"

#include <algorithm>
#include <QColor>
#include <QDesktopServices>
#include <QEvent>
#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSizePolicy>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>

#include "source/ui/ui_14_OpcoesSobre.h"
#include "source/utils/LogManager.h"

namespace {
QString get_text(QObject* loc, const QString& key, const QString& fallback = QString()) {
    QString text = fallback;
    if (loc != nullptr) {
        QString translated;
        if (QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, translated), Q_ARG(QString, key)) &&
            !translated.isEmpty() && translated != key) {
            text = translated;
        } else if (!fallback.isEmpty()) {
            text = fallback;
        }
    }
    return text;
}

QString build_license_doc_href(const QString& doc_id) {
    return QStringLiteral("license-doc://") + doc_id;
}

QString render_clickable_text_body(const QString& content, const QHash<QString, QString>& displayed_path_to_doc_id) {
    if (content.isEmpty()) {
        return QStringLiteral("<div style=\"white-space: pre-wrap;\"></div>");
    }

    struct LinkMatch {
        int start = 0;
        int length = 0;
        QString href;
        QString label;
    };

    QVector<LinkMatch> matches;

    static const QRegularExpression url_regex(QStringLiteral(R"(https?://[^\s<>"']+)"));
    const auto url_matches = url_regex.globalMatch(content);
    auto it_url = url_matches;
    while (it_url.hasNext()) {
        const auto match = it_url.next();
        const QString url = match.captured(0);
        LinkMatch link_match;
        link_match.start = match.capturedStart(0);
        link_match.length = match.capturedLength(0);
        link_match.href = url;
        link_match.label = url;
        matches.push_back(link_match);
    }

    for (auto it = displayed_path_to_doc_id.constBegin(); it != displayed_path_to_doc_id.constEnd(); ++it) {
        const QString& displayed_path = it.key();
        const QString& doc_id = it.value();
        int from = 0;
        while ((from = content.indexOf(displayed_path, from)) >= 0) {
            LinkMatch link_match;
            link_match.start = from;
            link_match.length = displayed_path.size();
            link_match.href = build_license_doc_href(doc_id);
            link_match.label = displayed_path;
            matches.push_back(link_match);
            from += displayed_path.size();
        }
    }

    std::sort(matches.begin(), matches.end(), [](const LinkMatch& lhs, const LinkMatch& rhs) {
        if (lhs.start != rhs.start) {
            return lhs.start < rhs.start;
        }
        return lhs.length > rhs.length;
    });

    QString html = QStringLiteral("<div style=\"white-space: pre-wrap;\">");
    int current_pos = 0;
    for (const LinkMatch& match : matches) {
        if (match.start < current_pos) {
            continue;
        }

        html += content.mid(current_pos, match.start - current_pos).toHtmlEscaped();
        html += QStringLiteral("<a href=\"") + match.href.toHtmlEscaped() + QStringLiteral("\">") +
                match.label.toHtmlEscaped() + QStringLiteral("</a>");
        current_pos = match.start + match.length;
    }

    html += content.mid(current_pos).toHtmlEscaped();
    html += QStringLiteral("</div>");
    return html;
}

QString render_clickable_text_document(const QString& content, const QHash<QString, QString>& displayed_path_to_doc_id) {
    return QStringLiteral("<html><body>%1</body></html>")
        .arg(render_clickable_text_body(content, displayed_path_to_doc_id));
}

QString montar_html_licencas(const QString& licencas,
                             const QString& site_oficial_text,
                             const QString& sites_licencas,
                             const QString& info_not_available_text,
                             const QHash<QString, QString>& displayed_path_to_doc_id) {
    QString texto_html = QStringLiteral("<html><body>");
    if (licencas.isEmpty()) {
        texto_html += QStringLiteral("<p>%1.</p>").arg(info_not_available_text.toHtmlEscaped());
    } else {
        texto_html += render_clickable_text_body(licencas, displayed_path_to_doc_id);
    }

    texto_html += QStringLiteral("<br><br><h3>") + site_oficial_text.toHtmlEscaped() + QStringLiteral("</h3><ul>");

    const QStringList sites = sites_licencas.split('\n', Qt::SkipEmptyParts);
    for (const QString& site : sites) {
        const QString site_limpo = site.trimmed();
        if (!site_limpo.isEmpty()) {
            texto_html += QStringLiteral("<li><a href=\"") + site_limpo.toHtmlEscaped() + QStringLiteral("\">") +
                          site_limpo.toHtmlEscaped() + QStringLiteral("</a></li>");
        }
    }

    texto_html += QStringLiteral("</ul></body></html>");
    return texto_html;
}

QString build_about_header_html(QObject* loc) {
    return QStringLiteral(
               "<div style=\"margin:0; padding:0;\">"
               "<span style=\"font-size:13pt; font-weight:600;\">LINCEU LIGHTHOUSE</span><br><br>"
               "<b>%1:</b> 2026.8.25.0<br>"
               "<b>%2:</b> Fernando Nillsson Cidade<br>"
               "<b>%3:</b> %4"
               "</div>")
        .arg(get_text(loc, "version", "Version").toHtmlEscaped(),
             get_text(loc, "authors", "Authors").toHtmlEscaped(),
             get_text(loc, "description", "Description").toHtmlEscaped(),
             get_text(loc, "description_text", "Application information").toHtmlEscaped());
}

struct AboutSearchMatch {
    QTextBrowser* browser = nullptr;
    int start = -1;
    int length = 0;
};

QVector<AboutSearchMatch> browser_search_matches(QTextBrowser* browser, const QString& query) {
    QVector<AboutSearchMatch> matches;
    if (browser == nullptr || browser->document() == nullptr || query.trimmed().isEmpty()) {
        return matches;
    }

    QTextCursor cursor(browser->document());
    while (true) {
        cursor = browser->document()->find(query.trimmed(), cursor);
        if (cursor.isNull()) {
            break;
        }
        matches.append({browser, cursor.selectionStart(), cursor.selectionEnd() - cursor.selectionStart()});
    }
    return matches;
}

QVector<AboutSearchMatch> collect_about_search_matches(QWidget* root, const QString& query, bool visible_only) {
    QVector<AboutSearchMatch> matches;
    if (root == nullptr || query.trimmed().isEmpty()) {
        return matches;
    }

    auto append_browser = [&](QTextBrowser* browser) {
        if (browser == nullptr || (visible_only && !browser->isVisibleTo(root))) {
            return;
        }
        matches += browser_search_matches(browser, query);
    };

    append_browser(qobject_cast<QTextBrowser*>(root));
    for (QTextBrowser* browser : root->findChildren<QTextBrowser*>()) {
        append_browser(browser);
    }

    return matches;
}

void apply_about_search_highlights(QWidget* root,
                                   const QString& query,
                                   QTextBrowser* current_browser,
                                   int current_start) {
    if (root == nullptr) {
        return;
    }

    const QString trimmed = query.trimmed();
    const QColor highlight = root->palette().color(QPalette::Highlight);
    const QColor highlighted_text = root->palette().color(QPalette::HighlightedText);

    auto apply_browser = [&](QTextBrowser* browser) {
        if (browser == nullptr) {
            return;
        }

        QList<QTextEdit::ExtraSelection> selections;
        const QVector<AboutSearchMatch> matches = browser_search_matches(browser, trimmed);
        for (const AboutSearchMatch& match : matches) {
            QTextCursor cursor(browser->document());
            cursor.setPosition(match.start);
            cursor.setPosition(match.start + match.length, QTextCursor::KeepAnchor);

            QColor background = highlight;
            background.setAlpha(browser == current_browser && match.start == current_start ? 190 : 95);

            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format.setBackground(background);
            if (browser == current_browser && match.start == current_start) {
                selection.format.setForeground(highlighted_text);
            }
            selections.append(selection);
        }
        browser->setExtraSelections(selections);
    };

    apply_browser(qobject_cast<QTextBrowser*>(root));
    for (QTextBrowser* browser : root->findChildren<QTextBrowser*>()) {
        apply_browser(browser);
    }
}

int find_about_search_match_index(const QVector<AboutSearchMatch>& matches, QTextBrowser* browser, int start) {
    for (int index = 0; index < matches.size(); ++index) {
        if (matches.at(index).browser == browser && matches.at(index).start == start) {
            return index;
        }
    }
    return -1;
}

void scroll_about_search_match_to_view(const AboutSearchMatch& match) {
    if (match.browser == nullptr || match.browser->document() == nullptr) {
        return;
    }

    QTextCursor cursor(match.browser->document());
    cursor.setPosition(match.start);
    cursor.setPosition(match.start + match.length, QTextCursor::KeepAnchor);
    match.browser->setTextCursor(cursor);
    match.browser->ensureCursorVisible();
}
}  // namespace

namespace source::ui::GerenciamentoMenusUI {

SobreDialog::SobreDialog(QWidget* parent,
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
                         const QString& avisos,
                         const QString& show_notices_text,
                         const QString& hide_notices_text,
                         const QString& privacy_policy,
                         const QString& show_privacy_policy_text,
                         const QString& hide_privacy_policy_text,
                         const QString& info_not_available_text,
                         const QString& release_notes,
                         const QString& show_release_notes_text,
                         const QString& hide_release_notes_text)
    : QDialog(parent),
      fixed_label_(nullptr),
      search_label_(nullptr),
      search_edit_(nullptr),
      search_previous_button_(nullptr),
      search_next_button_(nullptr),
      tabs_(nullptr),
      ok_button_(nullptr),
      history_browser_(nullptr),
      detalhes_browser_(nullptr),
      licencas_browser_(nullptr),
      avisos_browser_(nullptr),
      privacidade_browser_(nullptr),
      release_notes_browser_(nullptr),
      info_not_available_text_(info_not_available_text),
      search_current_browser_(nullptr),
      search_current_start_(-1),
      parent_ref_(interface_ref != nullptr ? interface_ref : parent),
      loc_(parent_ref_ != nullptr ? parent_ref_->property("loc").value<QObject*>() : nullptr) {
    try {
        setWindowTitle(titulo);
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                       Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        setModal(false);

        register_license_document(QStringLiteral("gnu-lgpl-v3"),
                                  source::ui::OpcoesSobre::GNU_LGPL_V3_LICENSE_PATH,
                                  QStringLiteral("GNU LESSER GENERAL PUBLIC LICENSE Version 3"),
                                  source::ui::OpcoesSobre::GNU_LGPL_V3_LICENSE_TEXT);

        auto* layout = new QVBoxLayout(this);
        layout->setSpacing(6);
        const int search_top_margin = std::max(0, fontMetrics().lineSpacing() - layout->spacing());

        auto* header_widget = new QWidget(this);
        auto* header_layout = new QVBoxLayout(header_widget);
        header_layout->setContentsMargins(0, 0, 0, 0);
        header_layout->setSpacing(0);

        fixed_label_ = new QLabel(texto_fixo, this);
        fixed_label_->setTextFormat(Qt::RichText);
        fixed_label_->setWordWrap(true);
        fixed_label_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        fixed_label_->setContentsMargins(0, 0, 0, 0);
        fixed_label_->setIndent(0);
        fixed_label_->setMargin(0);
        fixed_label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        header_layout->addWidget(fixed_label_);

        header_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout->addWidget(header_widget);

        auto* search_layout = new QGridLayout();
        search_layout->setContentsMargins(0, search_top_margin, 0, 6);
        search_layout->setHorizontalSpacing(8);
        search_layout->setColumnStretch(0, 0);
        search_layout->setColumnStretch(1, 1);

        search_label_ = new QLabel(this);
        search_label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        search_edit_ = new QLineEdit(this);
        search_edit_->setClearButtonEnabled(true);
        search_edit_->installEventFilter(this);
        search_previous_button_ = new QToolButton(this);
        search_next_button_ = new QToolButton(this);
        search_previous_button_->setArrowType(Qt::LeftArrow);
        search_next_button_->setArrowType(Qt::RightArrow);
        search_previous_button_->setAutoRaise(true);
        search_next_button_->setAutoRaise(true);
        search_previous_button_->setEnabled(false);
        search_next_button_->setEnabled(false);

        auto* search_controls = new QHBoxLayout();
        search_controls->setContentsMargins(0, 0, 0, 0);
        search_controls->setSpacing(4);
        search_controls->addWidget(search_edit_, 1);
        search_controls->addWidget(search_previous_button_);
        search_controls->addWidget(search_next_button_);
        search_layout->addWidget(search_label_, 0, 0);
        search_layout->addLayout(search_controls, 0, 1);
        layout->addLayout(search_layout);

        tabs_ = new QTabWidget(this);
        tabs_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        history_browser_ = new QTextBrowser(this);
        configure_browser(history_browser_);
        apply_text_or_placeholder(history_browser_, texto_history, info_not_available_text);
        tabs_->addTab(history_browser_, show_history_text.isEmpty() ? QStringLiteral("Show") : show_history_text);
        tab_labels_.push_back({show_history_text.isEmpty() ? QStringLiteral("Show") : show_history_text,
                               hide_history_text.isEmpty() ? QStringLiteral("Hide") : hide_history_text});

        detalhes_browser_ = new QTextBrowser(this);
        configure_browser(detalhes_browser_);
        apply_text_or_placeholder(detalhes_browser_, detalhes, info_not_available_text);
        tabs_->addTab(detalhes_browser_, show_details_text.isEmpty() ? QStringLiteral("Show") : show_details_text);
        tab_labels_.push_back({show_details_text.isEmpty() ? QStringLiteral("Show") : show_details_text,
                               hide_details_text.isEmpty() ? QStringLiteral("Hide") : hide_details_text});

        licencas_browser_ = new QTextBrowser(this);
        configure_browser(licencas_browser_);
        licencas_browser_->setHtml(montar_html_licencas(
            licencas, site_oficial_text, sites_licencas, info_not_available_text_, displayed_path_to_doc_id_));
        tabs_->addTab(licencas_browser_, show_licenses_text.isEmpty() ? QStringLiteral("Show") : show_licenses_text);
        tab_labels_.push_back({show_licenses_text.isEmpty() ? QStringLiteral("Show") : show_licenses_text,
                               hide_licenses_text.isEmpty() ? QStringLiteral("Hide") : hide_licenses_text});
        sites_licencas_ = sites_licencas;

        avisos_browser_ = new QTextBrowser(this);
        configure_browser(avisos_browser_);
        apply_text_or_placeholder(avisos_browser_, avisos, info_not_available_text);
        tabs_->addTab(avisos_browser_, show_notices_text.isEmpty() ? QStringLiteral("Show") : show_notices_text);
        tab_labels_.push_back({show_notices_text.isEmpty() ? QStringLiteral("Show") : show_notices_text,
                               hide_notices_text.isEmpty() ? QStringLiteral("Hide") : hide_notices_text});

        privacidade_browser_ = new QTextBrowser(this);
        configure_browser(privacidade_browser_);
        apply_text_or_placeholder(privacidade_browser_, privacy_policy, info_not_available_text);
        tabs_->addTab(privacidade_browser_,
                      show_privacy_policy_text.isEmpty() ? QStringLiteral("Show") : show_privacy_policy_text);
        tab_labels_.push_back({show_privacy_policy_text.isEmpty() ? QStringLiteral("Show") : show_privacy_policy_text,
                               hide_privacy_policy_text.isEmpty() ? QStringLiteral("Hide") : hide_privacy_policy_text});

        release_notes_browser_ = new QTextBrowser(this);
        configure_browser(release_notes_browser_);
        apply_text_or_placeholder(release_notes_browser_, release_notes, info_not_available_text);
        tabs_->addTab(release_notes_browser_,
                      show_release_notes_text.isEmpty() ? QStringLiteral("Show") : show_release_notes_text);
        tab_labels_.push_back({show_release_notes_text.isEmpty() ? QStringLiteral("Show") : show_release_notes_text,
                               hide_release_notes_text.isEmpty() ? QStringLiteral("Hide") : hide_release_notes_text});

        layout->addWidget(tabs_);

        auto* button_layout = new QHBoxLayout();
        ok_button_ = new QPushButton(ok_text, this);
        ok_button_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        ok_button_->setAutoDefault(false);
        ok_button_->setDefault(false);
        QObject::connect(ok_button_, &QPushButton::clicked, this, &QDialog::accept);
        button_layout->addStretch(1);
        button_layout->addWidget(ok_button_);
        layout->addLayout(button_layout);

        setMinimumSize(400, 200);

        QObject::connect(tabs_, &QTabWidget::currentChanged, this, &SobreDialog::update_tab_labels);
        QObject::connect(tabs_, &QTabWidget::currentChanged, this, [this](int) {
            reset_about_search_position();
            apply_about_search();
            update_about_search_buttons();
        });
        QObject::connect(search_edit_, &QLineEdit::textChanged, this, [this]() {
            reset_about_search_position();
            apply_about_search();
            update_about_search_buttons();
        });
        QObject::connect(search_edit_, &QLineEdit::returnPressed, this, [this]() {
            navigate_about_search(1);
        });
        QObject::connect(search_previous_button_, &QToolButton::clicked, this, [this]() {
            navigate_about_search(-1);
        });
        QObject::connect(search_next_button_, &QToolButton::clicked, this, [this]() {
            navigate_about_search(1);
        });
        update_search_texts();
        update_tab_labels(tabs_->currentIndex());

        if (loc_ != nullptr) {
            QObject::connect(loc_, SIGNAL(idioma_alterado(QString)), this, SLOT(on_idioma_alterado(QString)));
        }

        QTimer::singleShot(0, this, [this]() { refresh_fixed_label_geometry(); });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao criar dialog sobre", true);
    }
}

void SobreDialog::configure_browser(QTextBrowser* browser) {
    if (browser == nullptr) {
        return;
    }

    browser->setReadOnly(true);
    browser->setOpenLinks(false);
    browser->setOpenExternalLinks(false);
    QObject::connect(browser, &QTextBrowser::anchorClicked, this, &SobreDialog::handle_anchor_click);
}

void SobreDialog::apply_text_or_placeholder(QTextBrowser* browser, const QString& content, const QString& info_not_available_text) {
    if (browser == nullptr) {
        return;
    }

    if (!content.isEmpty()) {
        browser->setHtml(render_clickable_text_document(content, displayed_path_to_doc_id_));
    } else {
        browser->setHtml(QStringLiteral("<p>%1.</p>").arg(info_not_available_text.toHtmlEscaped()));
    }
}

void SobreDialog::register_license_document(const QString& doc_id,
                                            const QString& displayed_path,
                                            const QString& license_title,
                                            const QString& content) {
    if (doc_id.isEmpty() || displayed_path.isEmpty() || content.isEmpty()) {
        return;
    }

    displayed_path_to_doc_id_.insert(displayed_path, doc_id);
    license_document_titles_.insert(doc_id, license_title);
    license_document_contents_.insert(doc_id, content);
}

void SobreDialog::handle_anchor_click(const QUrl& url) {
    if (!url.isValid()) {
        return;
    }

    if (url.scheme() == QStringLiteral("license-doc")) {
        QString doc_id = url.host();
        if (doc_id.isEmpty()) {
            doc_id = url.path();
            if (doc_id.startsWith('/')) {
                doc_id.remove(0, 1);
            }
        }
        open_license_document(doc_id);
        return;
    }

    if (!QDesktopServices::openUrl(url)) {
        source::utils::LogManager::get_logger().error(
            QString("Erro ao abrir link externo no diálogo Sobre: %1").arg(url.toString()), true);
    }
}

void SobreDialog::open_license_document(const QString& doc_id) {
    const QString content = license_document_contents_.value(doc_id);
    if (content.isEmpty()) {
        source::utils::LogManager::get_logger().error(
            QString("Conteúdo da licença não disponível para o identificador '%1'").arg(doc_id), true);
        return;
    }

    const QString title = license_document_titles_.value(doc_id, doc_id);
    auto* dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                           Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    dialog->setModal(false);
    dialog->setWindowTitle(title);
    dialog->resize(960, 720);
    dialog->setMinimumSize(640, 420);

    auto* layout = new QVBoxLayout(dialog);
    auto* editor = new QPlainTextEdit(dialog);
    editor->setReadOnly(true);
    editor->setPlainText(content);
    layout->addWidget(editor);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

int SobreDialog::calculate_fixed_label_height(int available_width) const {
    if (fixed_label_ == nullptr) {
        return 0;
    }

    const int margin = fixed_label_->margin();
    const QMargins contents_margins = fixed_label_->contentsMargins();
    const int text_width =
        available_width - contents_margins.left() - contents_margins.right() - (margin * 2);

    if (text_width <= 0) {
        return fixed_label_->sizeHint().height();
    }

    QTextDocument document;
    document.setDefaultFont(fixed_label_->font());
    document.setDocumentMargin(0);
    document.setHtml(fixed_label_->text());
    document.setTextWidth(text_width);

    return qCeil(document.size().height()) + contents_margins.top() + contents_margins.bottom() + (margin * 2);
}

void SobreDialog::refresh_fixed_label_geometry() {
    if (fixed_label_ == nullptr) {
        return;
    }

    if (layout() != nullptr) {
        layout()->activate();
    }

    int available_width = 0;
    if (fixed_label_->parentWidget() != nullptr) {
        available_width = fixed_label_->parentWidget()->contentsRect().width();
    }
    if (available_width <= 0) {
        available_width = fixed_label_->width();
    }
    if (available_width <= 0) {
        available_width = contentsRect().width();
    }

    const int target_height = calculate_fixed_label_height(available_width);
    if (target_height <= 0) {
        return;
    }

    fixed_label_->setFixedHeight(target_height);
    fixed_label_->updateGeometry();

    if (fixed_label_->parentWidget() != nullptr && fixed_label_->parentWidget()->layout() != nullptr) {
        fixed_label_->parentWidget()->layout()->invalidate();
        fixed_label_->parentWidget()->layout()->activate();
    }

    if (layout() != nullptr) {
        layout()->invalidate();
        layout()->activate();
    }
}

void SobreDialog::on_idioma_alterado(const QString& novo_idioma) {
    try {
        if (loc_ == nullptr && parent_ref_ != nullptr) {
            loc_ = parent_ref_->property("loc").value<QObject*>();
        }

        const QString idioma = (loc_ != nullptr) ? loc_->property("idioma_atual").toString()
                                                 : (novo_idioma.isEmpty() ? QStringLiteral("en_US") : novo_idioma);

        const QString texto_sobre = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::ABOUT_TEXT_PT_BR
                                  : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::ABOUT_TEXT_ES_ES
                                  : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::ABOUT_TEXT_FR_FR
                                  : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::ABOUT_TEXT_IT_IT
                                  : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::ABOUT_TEXT_DE_DE
                                                                        : source::ui::OpcoesSobre::ABOUT_TEXT_EN_US;

        const QString texto_licenca = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::LICENSE_TEXT_PT_BR
                                    : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::LICENSE_TEXT_ES_ES
                                    : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::LICENSE_TEXT_FR_FR
                                    : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::LICENSE_TEXT_IT_IT
                                    : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::LICENSE_TEXT_DE_DE
                                                                          : source::ui::OpcoesSobre::LICENSE_TEXT_EN_US;

        const QString texto_aviso = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::NOTICE_TEXT_PT_BR
                                  : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::NOTICE_TEXT_ES_ES
                                  : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::NOTICE_TEXT_FR_FR
                                  : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::NOTICE_TEXT_IT_IT
                                  : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::NOTICE_TEXT_DE_DE
                                                                        : source::ui::OpcoesSobre::NOTICE_TEXT_EN_US;

        const QString texto_privacidade = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::PRIVACY_POLICY_PT_BR
                                       : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_ES_ES
                                       : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_FR_FR
                                       : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_IT_IT
                                       : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::PRIVACY_POLICY_DE_DE
                                                                             : source::ui::OpcoesSobre::PRIVACY_POLICY_EN_US;

        const QString texto_history = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::HISTORY_APP_PT_BR
                                    : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::HISTORY_APP_ES_ES
                                    : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::HISTORY_APP_FR_FR
                                    : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::HISTORY_APP_IT_IT
                                    : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::HISTORY_APP_DE_DE
                                                                          : source::ui::OpcoesSobre::HISTORY_APP_EN_US;

        const QString texto_release_notes = (idioma == QStringLiteral("pt_BR"))   ? source::ui::OpcoesSobre::RELEASE_NOTES_PT_BR
                                          : (idioma == QStringLiteral("es_ES")) ? source::ui::OpcoesSobre::RELEASE_NOTES_ES_ES
                                          : (idioma == QStringLiteral("fr_FR")) ? source::ui::OpcoesSobre::RELEASE_NOTES_FR_FR
                                          : (idioma == QStringLiteral("it_IT")) ? source::ui::OpcoesSobre::RELEASE_NOTES_IT_IT
                                          : (idioma == QStringLiteral("de_DE")) ? source::ui::OpcoesSobre::RELEASE_NOTES_DE_DE
                                                                                : source::ui::OpcoesSobre::RELEASE_NOTES_EN_US;

        if (loc_ != nullptr) {
            info_not_available_text_ =
                get_text(loc_,
                         "information_not_available",
                         info_not_available_text_.isEmpty() ? QStringLiteral("Information not available")
                                                            : info_not_available_text_);
        }

        if (loc_ != nullptr && fixed_label_ != nullptr) {
            fixed_label_->setText(build_about_header_html(loc_));
            setWindowTitle(get_text(loc_, "about", "About") + QStringLiteral(" - LINCEU LIGHTHOUSE"));
        }

        if (history_browser_ != nullptr) apply_text_or_placeholder(history_browser_, texto_history, info_not_available_text_);
        if (detalhes_browser_ != nullptr) apply_text_or_placeholder(detalhes_browser_, texto_sobre, info_not_available_text_);
        if (avisos_browser_ != nullptr) apply_text_or_placeholder(avisos_browser_, texto_aviso, info_not_available_text_);
        if (privacidade_browser_ != nullptr) {
            apply_text_or_placeholder(privacidade_browser_, texto_privacidade, info_not_available_text_);
        }
        if (release_notes_browser_ != nullptr) {
            apply_text_or_placeholder(release_notes_browser_, texto_release_notes, info_not_available_text_);
        }

        if (licencas_browser_ != nullptr) {
            const QString site_oficial = get_text(loc_, "site_oficial", "Official site");
            licencas_browser_->setHtml(montar_html_licencas(
                texto_licenca, site_oficial, sites_licencas_, info_not_available_text_, displayed_path_to_doc_id_));
        }

        if (loc_ != nullptr && tab_labels_.size() >= 6) {
            tab_labels_[0] = {get_text(loc_, "show_history", tab_labels_[0].first),
                              get_text(loc_, "hide_history", tab_labels_[0].second)};
            tab_labels_[1] = {get_text(loc_, "show_details", tab_labels_[1].first),
                              get_text(loc_, "hide_details", tab_labels_[1].second)};
            tab_labels_[2] = {get_text(loc_, "show_licenses", tab_labels_[2].first),
                              get_text(loc_, "hide_licenses", tab_labels_[2].second)};
            tab_labels_[3] = {get_text(loc_, "show_notices", tab_labels_[3].first),
                              get_text(loc_, "hide_notices", tab_labels_[3].second)};
            tab_labels_[4] = {get_text(loc_, "show_privacy_policy", tab_labels_[4].first),
                              get_text(loc_, "hide_privacy_policy", tab_labels_[4].second)};
            tab_labels_[5] = {get_text(loc_, "show_release_notes", tab_labels_[5].first),
                              get_text(loc_, "hide_release_notes", tab_labels_[5].second)};
        }

        if (tabs_ != nullptr) {
            update_tab_labels(tabs_->currentIndex());
        }
        update_search_texts();
        apply_about_search();
        update_about_search_buttons();

        if (ok_button_ != nullptr && loc_ != nullptr) {
            ok_button_->setText(get_text(loc_, "ok", "OK"));
        }

        refresh_fixed_label_geometry();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao aplicar tradução dinâmica no diálogo Sobre", true);
    }
}

bool SobreDialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == search_edit_ && event != nullptr && event->type() == QEvent::KeyPress) {
        auto* key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter) {
            navigate_about_search(key_event->modifiers().testFlag(Qt::ShiftModifier) ? -1 : 1);
            return true;
        }
    }

    return QDialog::eventFilter(watched, event);
}

void SobreDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    refresh_fixed_label_geometry();
}

void SobreDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    refresh_fixed_label_geometry();
}

void SobreDialog::update_tab_labels(int current_index) {
    try {
        if (tabs_ == nullptr) {
            return;
        }

        for (int i = 0; i < tabs_->count(); ++i) {
            const QString show_text = (i < tab_labels_.size()) ? tab_labels_[i].first : QStringLiteral("Show");
            const QString hide_text = (i < tab_labels_.size()) ? tab_labels_[i].second : QStringLiteral("Hide");
            tabs_->setTabText(i, (i == current_index) ? hide_text : show_text);
        }
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao atualizar rótulos das abas", true);
    }
}

void SobreDialog::update_search_texts() {
    if (search_label_ == nullptr || search_edit_ == nullptr ||
        search_previous_button_ == nullptr || search_next_button_ == nullptr) {
        return;
    }

    search_label_->setText(get_text(loc_, QStringLiteral("search_label"), QStringLiteral("Buscar:")));
    search_edit_->setPlaceholderText(get_text(
        loc_, QStringLiteral("about_search_placeholder"), QStringLiteral("Buscar texto na janela Sobre...")));
    search_previous_button_->setToolTip(get_text(
        loc_, QStringLiteral("about_search_previous"), QStringLiteral("Resultado anterior")));
    search_next_button_->setToolTip(get_text(
        loc_, QStringLiteral("about_search_next"), QStringLiteral("Próximo resultado")));
    search_previous_button_->setAccessibleName(search_previous_button_->toolTip());
    search_next_button_->setAccessibleName(search_next_button_->toolTip());
}

void SobreDialog::apply_about_search() {
    if (tabs_ == nullptr) {
        return;
    }

    const QString query = search_edit_ != nullptr ? search_edit_->text() : QString();
    if (query.trimmed().isEmpty()) {
        reset_about_search_position();
    }

    apply_about_search_highlights(tabs_, query, search_current_browser_, search_current_start_);
}

void SobreDialog::navigate_about_search(int step) {
    if (tabs_ == nullptr || search_edit_ == nullptr || step == 0) {
        return;
    }

    QWidget* current_page = tabs_->currentWidget();
    const QString query = search_edit_->text().trimmed();
    const QVector<AboutSearchMatch> matches = collect_about_search_matches(current_page, query, true);
    if (matches.isEmpty()) {
        reset_about_search_position();
        apply_about_search();
        update_about_search_buttons();
        return;
    }

    int index = find_about_search_match_index(matches, search_current_browser_, search_current_start_);
    if (index < 0) {
        index = step < 0 ? matches.size() - 1 : 0;
    } else {
        index = (index + step + matches.size()) % matches.size();
    }

    const AboutSearchMatch match = matches.at(index);
    search_current_browser_ = match.browser;
    search_current_start_ = match.start;
    apply_about_search();
    scroll_about_search_match_to_view(match);
    update_about_search_buttons();
}

void SobreDialog::update_about_search_buttons() {
    const bool has_query = search_edit_ != nullptr && !search_edit_->text().trimmed().isEmpty();
    const bool has_matches =
        has_query && tabs_ != nullptr &&
        !collect_about_search_matches(tabs_->currentWidget(), search_edit_->text().trimmed(), true).isEmpty();

    if (search_previous_button_ != nullptr) {
        search_previous_button_->setEnabled(has_matches);
    }
    if (search_next_button_ != nullptr) {
        search_next_button_->setEnabled(has_matches);
    }
}

void SobreDialog::reset_about_search_position() {
    search_current_browser_ = nullptr;
    search_current_start_ = -1;
}

}  // namespace source::ui::GerenciamentoMenusUI
