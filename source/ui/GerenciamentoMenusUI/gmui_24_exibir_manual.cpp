#include "gmui_24_exibir_manual.h"

#include <QEvent>
#include <QKeyEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QMetaObject>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QSplitter>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "source/ui/ui_03_GerenciadorMenusUI.h"
#include "source/ui/ui_15_Manual.h"
#include "source/utils/LogManager.h"

namespace source::ui::GerenciamentoMenusUI {

namespace {
const QString kManualTopAnchor = QStringLiteral("manual_top");

QString get_text(QObject* loc, const QString& key, const QString& fallback) {
    QString text;
    if (loc != nullptr) {
        QMetaObject::invokeMethod(loc, "get_text", Q_RETURN_ARG(QString, text), Q_ARG(QString, key));
    }
    return text.isEmpty() || text == key ? fallback : text;
}

void registrar_dialogo_na_interface(QObject* iface, const char* property_name, QWidget* dialog) {
    if (iface == nullptr || property_name == nullptr || dialog == nullptr) {
        return;
    }

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(dialog, &QObject::destroyed, iface, [iface, property_name](QObject* dialog_destruido) {
        QObject* dialog_atual = iface->property(property_name).value<QObject*>();
        if (dialog_atual == dialog_destruido) {
            iface->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(nullptr)));
        }
    });
    iface->setProperty(property_name, QVariant::fromValue(static_cast<QObject*>(dialog)));
}
}  // namespace

ManualDialog::ManualDialog(QWidget* parent, QObject* interface_ref, const QString& titulo, const QString& lang)
    : QDialog(parent),
      search_label_(nullptr),
      search_edit_(nullptr),
      search_previous_button_(nullptr),
      search_next_button_(nullptr),
      toc_list_(nullptr),
      browser_(nullptr),
      btn_ok_(nullptr),
      current_lang_(lang),
      search_current_index_(-1),
      parent_ref_(interface_ref != nullptr ? interface_ref : parent),
      loc_(parent_ref_ != nullptr ? parent_ref_->property("loc").value<QObject*>() : nullptr) {
    try {
        setWindowTitle(titulo);
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                       Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint |
                       Qt::WindowMaximizeButtonHint);
        setModal(false);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(6, 6, 6, 6);
        layout->setSpacing(6);

        auto* search_layout = new QGridLayout();
        search_layout->setContentsMargins(0, 0, 0, 0);
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

        auto* splitter = new QSplitter(Qt::Horizontal, this);

        toc_list_ = new QListWidget(this);
        toc_list_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        splitter->addWidget(toc_list_);

        browser_ = new QTextBrowser(this);
        browser_->setOpenExternalLinks(true);
        browser_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        splitter->addWidget(browser_);

        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 3);
        splitter->setCollapsible(0, false);
        splitter->setCollapsible(1, false);
        splitter->setSizes({280, 820});

        layout->addWidget(splitter, 1);

        auto* footer_layout = new QHBoxLayout();
        footer_layout->addStretch();
        btn_ok_ = new QPushButton(tr("OK"), this);
        btn_ok_->setAutoDefault(false);
        btn_ok_->setDefault(false);
        QObject::connect(btn_ok_, &QPushButton::clicked, this, &QDialog::close);
        footer_layout->addWidget(btn_ok_);
        layout->addLayout(footer_layout);

        setSizeGripEnabled(true);
        setMinimumSize(700, 480);
        resize(1000, 720);

        if (current_lang_.isEmpty()) {
            current_lang_ = (loc_ != nullptr) ? loc_->property("idioma_atual").toString() : QStringLiteral("pt_BR");
            if (current_lang_.isEmpty()) {
                current_lang_ = QStringLiteral("pt_BR");
            }
        }

        build_manual_content(current_lang_);
        update_search_texts();

        if (loc_ != nullptr) {
            QObject::connect(loc_, SIGNAL(idioma_alterado(QString)), this, SLOT(on_idioma_alterado(QString)));
        }

        QObject::connect(search_edit_, &QLineEdit::textChanged, this, [this](const QString& text) {
            rebuild_search(text);
            update_search_buttons();
        });
        QObject::connect(search_edit_, &QLineEdit::returnPressed, this, [this]() {
            navigate_search(1);
            update_search_buttons();
        });
        QObject::connect(search_previous_button_, &QToolButton::clicked, this, [this]() {
            navigate_search(-1);
            update_search_buttons();
        });
        QObject::connect(search_next_button_, &QToolButton::clicked, this, [this]() {
            navigate_search(1);
            update_search_buttons();
        });

        QObject::connect(toc_list_, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            if (item == nullptr || browser_ == nullptr) {
                return;
            }

            const QString section_id = item->data(Qt::UserRole).toString();
            if (!section_id.isEmpty()) {
                browser_->scrollToAnchor(section_id);
            }
        });
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao inicializar ManualDialog", true);
    }
}

bool ManualDialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == search_edit_ && event != nullptr && event->type() == QEvent::KeyPress) {
        auto* key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == Qt::Key_Return || key_event->key() == Qt::Key_Enter) {
            navigate_search(key_event->modifiers().testFlag(Qt::ShiftModifier) ? -1 : 1);
            update_search_buttons();
            return true;
        }
    }

    return QDialog::eventFilter(watched, event);
}

void ManualDialog::on_idioma_alterado(const QString& novo_idioma) {
    try {
        current_lang_ = novo_idioma;
        setWindowTitle(source::ui::manual::get_manual_title(novo_idioma));
        if (btn_ok_ != nullptr) {
            btn_ok_->setText(tr("OK"));
        }
        build_manual_content(novo_idioma);
        update_search_texts();
    } catch (...) {
    }
}

void ManualDialog::build_manual_content(const QString& lang) {
    try {
        if (toc_list_ == nullptr || browser_ == nullptr) {
            return;
        }

        const auto [blocks, order] = source::ui::manual::get_manual_blocks(lang);
        Q_UNUSED(order)

        toc_list_->clear();
        QStringList html_parts;
        const QString link_color = browser_->palette().color(QPalette::Link).name();

        for (const auto& block : blocks) {
            if (block.kind == QStringLiteral("toc_item") && !block.section_id.isEmpty()) {
                auto* item = new QListWidgetItem(block.text, toc_list_);
                item->setData(Qt::UserRole, block.section_id);
            }
        }

        bool manual_title_rendered = false;
        for (const auto& block : blocks) {
            if (block.kind == QStringLiteral("bullet")) {
                html_parts << QStringLiteral("<ul><li>%1</li></ul>").arg(block.text);
            } else if (block.kind == QStringLiteral("toc_title")) {
                html_parts << QStringLiteral("<h3>%1</h3>").arg(block.text);
            } else if (block.kind == QStringLiteral("toc_item")) {
                html_parts << QStringLiteral("<p><a href=\"#%1\">%2</a></p>").arg(block.section_id, block.text);
            } else if (block.kind == QStringLiteral("section_title")) {
                html_parts << QStringLiteral(
                                  "<h2 id=\"%1\"><a href=\"#%2\" style=\"color: %3; text-decoration: none;\">%4</a></h2>")
                                  .arg(block.section_id, kManualTopAnchor, link_color, block.text);
            } else if (block.kind == QStringLiteral("detail_title")) {
                html_parts << QStringLiteral("<h4>%1</h4>").arg(block.text);
            } else if (block.kind == QStringLiteral("paragraph") || block.kind == QStringLiteral("line")) {
                if (!manual_title_rendered && block.kind == QStringLiteral("line")) {
                    html_parts << QStringLiteral("<p id=\"%1\"><a name=\"%1\"></a>%2</p>")
                                      .arg(kManualTopAnchor, block.text);
                    manual_title_rendered = true;
                } else {
                    html_parts << QStringLiteral("<p>%1</p>").arg(block.text);
                }
            } else if (block.kind == QStringLiteral("divider")) {
                html_parts << QStringLiteral("<hr />");
            } else if (block.kind == QStringLiteral("blank")) {
                html_parts << QString();
            }
        }

        browser_->setHtml(html_parts.join('\n'));
        rebuild_search(search_edit_ != nullptr ? search_edit_->text() : QString());
        update_search_buttons();
    } catch (...) {
    }
}

void ManualDialog::update_search_texts() {
    if (search_label_ == nullptr || search_edit_ == nullptr ||
        search_previous_button_ == nullptr || search_next_button_ == nullptr) {
        return;
    }

    search_label_->setText(get_text(loc_, QStringLiteral("search_label"), QStringLiteral("Buscar:")));
    search_edit_->setPlaceholderText(get_text(
        loc_, QStringLiteral("manual_search_placeholder"), QStringLiteral("Buscar texto no manual...")));
    search_previous_button_->setToolTip(get_text(
        loc_, QStringLiteral("manual_search_previous"), QStringLiteral("Resultado anterior")));
    search_next_button_->setToolTip(get_text(
        loc_, QStringLiteral("manual_search_next"), QStringLiteral("Próximo resultado")));
    search_previous_button_->setAccessibleName(search_previous_button_->toolTip());
    search_next_button_->setAccessibleName(search_next_button_->toolTip());
}

void ManualDialog::rebuild_search(const QString& text) {
    search_matches_.clear();
    search_current_index_ = -1;

    if (browser_ == nullptr || browser_->document() == nullptr || text.trimmed().isEmpty()) {
        update_search_highlights();
        return;
    }

    QTextCursor cursor(browser_->document());
    while (true) {
        cursor = browser_->document()->find(text.trimmed(), cursor);
        if (cursor.isNull()) {
            break;
        }
        search_matches_.append(cursor);
    }

    update_search_highlights();
}

void ManualDialog::navigate_search(int step) {
    if (browser_ == nullptr || search_matches_.isEmpty() || step == 0) {
        return;
    }

    if (search_current_index_ < 0) {
        search_current_index_ = step < 0 ? search_matches_.size() - 1 : 0;
    } else {
        search_current_index_ =
            (search_current_index_ + step + search_matches_.size()) % search_matches_.size();
    }

    const QTextCursor cursor = search_matches_.at(search_current_index_);
    update_search_highlights();
    browser_->setTextCursor(cursor);
    browser_->ensureCursorVisible();
}

void ManualDialog::update_search_buttons() {
    const bool enabled = !search_matches_.isEmpty();
    if (search_previous_button_ != nullptr) {
        search_previous_button_->setEnabled(enabled);
    }
    if (search_next_button_ != nullptr) {
        search_next_button_->setEnabled(enabled);
    }
}

void ManualDialog::update_search_highlights() {
    if (browser_ == nullptr) {
        return;
    }

    QList<QTextEdit::ExtraSelection> selections;
    const bool has_query = search_edit_ != nullptr && !search_edit_->text().trimmed().isEmpty();
    if (!has_query) {
        browser_->setExtraSelections(selections);
        return;
    }

    QColor highlight_color = browser_->palette().color(QPalette::Highlight);
    highlight_color.setAlpha(95);
    QColor current_color = browser_->palette().color(QPalette::Highlight);
    current_color.setAlpha(190);
    const QColor current_text_color = browser_->palette().color(QPalette::HighlightedText);

    for (int index = 0; index < search_matches_.size(); ++index) {
        QTextEdit::ExtraSelection selection;
        selection.cursor = search_matches_.at(index);
        selection.format.setBackground(index == search_current_index_ ? current_color : highlight_color);
        if (index == search_current_index_) {
            selection.format.setForeground(current_text_color);
        }
        selections.append(selection);
    }

    browser_->setExtraSelections(selections);
}

void exibir_manual(source::ui::GerenciadorMenusUI* self) {
    try {
        if (self == nullptr || self->interface_obj() == nullptr || self->loc_obj() == nullptr) {
            return;
        }

        QObject* iface = self->interface_obj();
        QObject* loc = self->loc_obj();

        QString lang = loc->property("idioma_atual").toString();
        if (lang.isEmpty()) {
            lang = QStringLiteral("pt_BR");
        }

        const QString titulo = source::ui::manual::get_manual_title(lang);
        auto* dialog = new ManualDialog(nullptr, iface, titulo, lang);

        registrar_dialogo_na_interface(iface, "_manual_dialog", dialog);
        dialog->show();
        dialog->raise();
        dialog->activateWindow();
    } catch (...) {
        source::utils::LogManager::get_logger().error("Erro ao exibir manual", true);
    }
}

}  // namespace source::ui::GerenciamentoMenusUI
